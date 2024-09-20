

#include "XRInteractorComponent.h"
#include "XRInteractionComponent.h"
#include "XRToolsUtilityFunctions.h"
#include "Net/UnrealNetwork.h"

UXRInteractorComponent::UXRInteractorComponent()
{
	SphereRadius = 1.0f;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
	bAutoActivate = true;
}

void UXRInteractorComponent::InitializeComponent()
{
	Super::InitializeComponent();
	CacheIsLocallyControlled();
}

void UXRInteractorComponent::BeginPlay()
{
	Super::BeginPlay();
	OnComponentBeginOverlap.AddDynamic(this, &UXRInteractorComponent::OnOverlapBegin);
	OnComponentEndOverlap.AddDynamic(this, &UXRInteractorComponent::OnOverlapEnd);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Interaction Events
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractorComponent::StartXRInteractionByPriority(int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition)
{
	TArray<AActor*> OverlappedActors = {};
	UXRInteractionComponent* InteractionToStart = UXRToolsUtilityFunctions::GetXRInteractionByPriority(GetOverlappedXRInteractions(), this);
	if (InteractionToStart)
	{
		StartXRInteraction(InteractionToStart);
	}
}

void UXRInteractorComponent::StartXRInteraction(UXRInteractionComponent* InInteractionComponent)
{
	if (!InInteractionComponent)
	{
		return;
	}
	if (!InInteractionComponent->IsActive())
	{
		return;
	}

	// Stop other Interaction if TakeOver, return if Blocked, Start Interaction if Allowed
	auto ActiveInteractors = InInteractionComponent->GetActiveInteractors();
	if (ActiveInteractors.Num() > 0)
	{
		switch (InInteractionComponent->GetMultiInteractorBehavior())
		{
			case EXRMultiInteractorBehavior::TakeOver:
				for (auto Interactor : ActiveInteractors)
				{
					if (Interactor != this)
					{
						Interactor->Server_TerminateInteraction(InInteractionComponent);
					}
				}
				break;
			default:
				break;
		}
	}
	Server_ExecuteInteraction(InInteractionComponent);
}

// [Server] Implementation for starting interaction with a component, adds to active interactions and sets the Owner of the Interacted Actor to this Components Owner (to grant Authority)
void UXRInteractorComponent::Server_ExecuteInteraction_Implementation(UXRInteractionComponent* InInteractionComponent)
{
	if (!InInteractionComponent)
	{
		return;
	}
	if (InInteractionComponent->GetOwner() && GetOwner())
	{
		InInteractionComponent->GetOwner()->SetOwner(GetOwner());
	}
	ActiveInteractionComponents.AddUnique(InInteractionComponent);
	Multicast_ExecuteInteraction(InInteractionComponent);
}

void UXRInteractorComponent::Multicast_ExecuteInteraction_Implementation(UXRInteractionComponent* InteractionComponent)
{
	if (!InteractionComponent)
	{
		return;
	}
	InteractionComponent->StartInteraction(this);
	OnStartedInteracting.Broadcast(this, InteractionComponent);
	HoveredInteractionComponents.Remove(InteractionComponent);
}


void UXRInteractorComponent::StopXRInteractionByPriority(int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition)
{
	UXRInteractionComponent* InteractionToStop = nullptr;
	if (ActiveInteractionComponents.Num() > 0)
	{
		// Provide nullptr instead of this interactor to ensure Interactions that this Interactor is active on are not discarded
		InteractionToStop = UXRToolsUtilityFunctions::GetXRInteractionByPriority(GetActiveInteractions(), nullptr, InPriority, InPrioritySelectionCondition);
	}
	if (InteractionToStop)
	{
		StopXRInteraction(InteractionToStop);
	}

}

void UXRInteractorComponent::StopAllXRInteractions()
{
	auto ActiveInteractions = GetActiveInteractions();
	for (UXRInteractionComponent* ActiveInteraction : ActiveInteractions) {
		Server_TerminateInteraction(ActiveInteraction);
	}
}

void UXRInteractorComponent::StopXRInteraction(UXRInteractionComponent* InXRInteraction)
{
	if (!InXRInteraction)
	{
		return;
	}
	Server_TerminateInteraction(InXRInteraction);
}

// [Server] Implementation for stopping interaction with a component, removes from active interactions if continuous
void UXRInteractorComponent::Server_TerminateInteraction_Implementation(UXRInteractionComponent* InInteractionComponent)
{
	if (!InInteractionComponent)
	{
		return;
	}
	ActiveInteractionComponents.Remove(InInteractionComponent);
	Multicast_TerminateInteraction(InInteractionComponent);
}

void UXRInteractorComponent::Multicast_TerminateInteraction_Implementation(UXRInteractionComponent* InteractionComponent)
{
	if (!InteractionComponent)
	{
		return;
	}
	InteractionComponent->EndInteraction(this);
	OnStoppedInteracting.Broadcast(this, InteractionComponent);

	// Restart Highlight after Interaction End (if hovering)
	if (GetOverlappedXRInteractions().Contains(InteractionComponent))
	{
		RequestHover(InteractionComponent, true);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Utility
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UXRInteractorComponent::CanInteract(UXRInteractionComponent*& OutPrioritizedXRInteraction, int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition)
{
	OutPrioritizedXRInteraction = UXRToolsUtilityFunctions::GetXRInteractionByPriority(GetOverlappedXRInteractions(), this);
	return OutPrioritizedXRInteraction != nullptr;
}


TArray<UXRInteractionComponent*> UXRInteractorComponent::GetOverlappedXRInteractions() const
{
	TArray<UXRInteractionComponent*> FoundXRInteractions = {};

	TArray<UPrimitiveComponent*> OverlappingComps = {};
	GetOverlappingComponents(OverlappingComps);
	for (auto Collider : AdditionalColliders)
	{
		TArray<UPrimitiveComponent*> AdditionalOverlappingComps = {};
		Collider->GetOverlappingComponents(AdditionalOverlappingComps);
		for (auto AdditionalOverlappingComp : AdditionalOverlappingComps)
		{
			OverlappingComps.AddUnique(AdditionalOverlappingComp);
		}
	}

	for (UPrimitiveComponent* OverlappingComponent : OverlappingComps)
	{
		// check if any of the child components are XRInteractions 
		TArray<USceneComponent*> ChildComponents;
		OverlappingComponent->GetChildrenComponents(true, ChildComponents);
		for (USceneComponent* ChildComponent : ChildComponents)
		{
			UXRInteractionComponent* FoundXRInteraction = Cast<UXRInteractionComponent>(ChildComponent);
			if (FoundXRInteraction)
			{
				FoundXRInteractions.Add(FoundXRInteraction);
			}
		}
	}
	return FoundXRInteractions;
}

TArray<AActor*> UXRInteractorComponent::GetAllOverlappingActors() const
{
	TArray<AActor*> OverlappingActors = {};
	TArray<AActor*> TempActors = {};
	GetOverlappingActors(OverlappingActors);

	for (UPrimitiveComponent* AdditionalCollider : AdditionalColliders)
	{
		if (AdditionalCollider)
		{
			TempActors.Empty();
			AdditionalCollider->GetOverlappingActors(TempActors);
			for (AActor* Actor : TempActors)
			{
				if (Actor && !OverlappingActors.Contains(Actor))
				{
					OverlappingActors.AddUnique(Actor);
				}
			}
		}
	}
	return OverlappingActors;
}


TArray<UXRInteractionComponent*> UXRInteractorComponent::GetChildXRInteractionComponents(UPrimitiveComponent* InComponent)
{
	TArray<UXRInteractionComponent*> FoundXRInteractions = {};
	TArray<USceneComponent*> ChildComponents;
	InComponent->GetChildrenComponents(true, ChildComponents);
	for (USceneComponent* ChildComponent : ChildComponents)
	{
		UXRInteractionComponent* FoundXRInteraction = Cast<UXRInteractionComponent>(ChildComponent);
		if (FoundXRInteraction)
		{
			FoundXRInteractions.Add(FoundXRInteraction);
		}
	}
	return FoundXRInteractions;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Collisions
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractorComponent::SetAdditionalColliders(TArray<UPrimitiveComponent*> InColliders)
{
	if (AdditionalColliders.Num())
	{
		for (UPrimitiveComponent* Collider : AdditionalColliders)
		{
			if (Collider)
			{
				Collider->OnComponentBeginOverlap.RemoveDynamic(this, &UXRInteractorComponent::OnOverlapBegin);
				Collider->OnComponentEndOverlap.RemoveDynamic(this, &UXRInteractorComponent::OnOverlapEnd);
			}
		}
	}
	AdditionalColliders = InColliders;
	for (UPrimitiveComponent* Collider : AdditionalColliders)
	{
		if (Collider)
		{
			Collider->OnComponentBeginOverlap.AddDynamic(this, &UXRInteractorComponent::OnOverlapBegin);
			Collider->OnComponentEndOverlap.AddDynamic(this, &UXRInteractorComponent::OnOverlapEnd);
		}
	}
}

TArray<UPrimitiveComponent*> UXRInteractorComponent::GetAdditionalColliders() const
{
	return AdditionalColliders;
}

bool UXRInteractorComponent::IsAnyColliderOverlappingComponent(UPrimitiveComponent* InComponent, bool IgnoreSelf)
{
	TArray<UPrimitiveComponent*> RelevantColliders = AdditionalColliders;
	if (!IgnoreSelf) { RelevantColliders.Add(this); }
	for (UPrimitiveComponent* Collider : RelevantColliders)
	{
		if (Collider->IsOverlappingComponent(InComponent))
		{
			return true;
		}
	}
	return false;
}

void UXRInteractorComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsAnyColliderOverlappingComponent(OtherComp, true))
	{
		UXRInteractionComponent* PrioritizedInteraction = UXRToolsUtilityFunctions::GetXRInteractionByPriority(GetChildXRInteractionComponents(OtherComp), this, 0, EXRInteractionPrioritySelection::LowerEqual);
		if (PrioritizedInteraction)
		{
			RequestHover(PrioritizedInteraction, true);
		}
	}
}

void UXRInteractorComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsAnyColliderOverlappingComponent(OtherComp, true))
	{
		for (auto Interaction : GetChildXRInteractionComponents(OtherComp))
		{
			RequestHover(Interaction, false);
		}
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Hovering
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractorComponent::RequestHover(UXRInteractionComponent* InInteraction, bool bInHoverState)
{
	if (!InInteraction)
	{
		return;
	}
	if (bInHoverState)
	{
		if (!HoveredInteractionComponents.Contains(InInteraction))
		{
			HoveredInteractionComponents.Add(InInteraction);
			InInteraction->HoverInteraction(this, true);
			OnHoverStateChanged.Broadcast(this, InInteraction, true);
		}
	}
	if (!bInHoverState)
	{
		if (HoveredInteractionComponents.Contains(InInteraction))
		{
			HoveredInteractionComponents.Remove(InInteraction);
			InInteraction->HoverInteraction(this, false);
			OnHoverStateChanged.Broadcast(this, InInteraction, false);
		}
	}
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Config
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UXRInteractorComponent::IsInteracting() const
{
	return !ActiveInteractionComponents.IsEmpty();
}

void UXRInteractorComponent::Server_SetXRControllerHand_Implementation(EControllerHand InXRControllerHand)
{
	XRControllerHand = InXRControllerHand;
}

EControllerHand UXRInteractorComponent::GetXRControllerHand() const
{
	return XRControllerHand;
}

TArray<UXRInteractionComponent*> UXRInteractorComponent::GetActiveInteractions() const
{
	TArray<UXRInteractionComponent*> OutInteractions = {};
	for (auto ActiveInteraction : ActiveInteractionComponents)
	{
		if (ActiveInteraction.IsValid())
		{
			OutInteractions.AddUnique(ActiveInteraction.Get());
		}
	}
	return OutInteractions;
}

bool UXRInteractorComponent::IsLaserInteractor()
{
	return bIsLaserInteractor;
}

bool UXRInteractorComponent::IsLocallyControlled() const
{
	return bIsLocallyControlled;
}

UPhysicsConstraintComponent* UXRInteractorComponent::GetPhysicsConstraint() const
{
	return PhysicsConstraint;
}

void UXRInteractorComponent::SetPhysicsConstraint(UPhysicsConstraintComponent* InPhysicsConstraintComponent)
{
	PhysicsConstraint = InPhysicsConstraintComponent;
}

void UXRInteractorComponent::CacheIsLocallyControlled()
{
	AActor* Owner = GetOwner();
	APawn* TempOwningPawn = Cast<APawn>(Owner);
	if (TempOwningPawn)
	{
		bIsLocallyControlled = TempOwningPawn->IsLocallyControlled();
	}
}

void UXRInteractorComponent::SetOwningPawn(APawn* InOwningPawn)
{
	OwningPawn = InOwningPawn;
}

APawn* UXRInteractorComponent::GetOwningPawn() const
{
	return OwningPawn;
}

void UXRInteractorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRInteractorComponent, ActiveInteractionComponents);
	DOREPLIFETIME(UXRInteractorComponent, XRControllerHand);
}