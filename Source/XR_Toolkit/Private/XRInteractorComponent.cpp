

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

	/* Removed to always check if the Interactor is overlapping another Interaction
	
	UXRInteractionComponent* InteractionToStart = nullptr;

	if (ActiveInteractionComponents.Num() > 0) // If already interacting, get Interacted Actor and start next available Interaction
	{
		AActor* CurrentInteractedActor = ActiveInteractionComponents[0]->GetOwner();
		if (CurrentInteractedActor)
		{
			InteractionToStart = UXRToolsUtilityFunctions::GetXRInteractionOnActorByPriority(CurrentInteractedActor, this, InPriority, InPrioritySelectionCondition);
		}
	}
	else // Search next available Interactive Actor and get highest priority Interaction
	{
		AActor* ClosestInteractiveActor = GetClosestXRInteractionActor(InteractionToStart);
	}
	*/

	AActor* OverlappedActor = nullptr;
	UXRInteractionComponent* InteractionToStart = GetOverlappedXRInteraction(OverlappedActor);
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
	AActor* Owner = InteractionComponent->GetOwner();
	if (Owner)
	{
		if (IsAnyColliderOverlappingActor({}, Owner))
		{
			RequestHover(InteractionComponent, true);
		}
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Utility
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

bool UXRInteractorComponent::CanInteract(UXRInteractionComponent*& OutPrioritizedXRInteraction, AActor* InActor, int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition)
{
	if (!InActor)
	{
		AActor* OverlappedActor = nullptr;
		OutPrioritizedXRInteraction = GetOverlappedXRInteraction(OverlappedActor);
	}
	else
	{
		OutPrioritizedXRInteraction = UXRToolsUtilityFunctions::GetXRInteractionOnActorByPriority(InActor, this, InPriority, InPrioritySelectionCondition);
	}
	return OutPrioritizedXRInteraction != nullptr;
}

// Finds the highest priority XRInteraction in the overlapped components children
UXRInteractionComponent* UXRInteractorComponent::GetOverlappedXRInteraction(AActor*& OutOverlappedActor)
{
	TArray<UXRInteractionComponent*> FoundXRInteractions = {};
	float MinimumDistance = 0.0f;

	TArray<UPrimitiveComponent*> OverlappingComps;
	GetOverlappingComponents(OverlappingComps);


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
	// Get the prioritized one
	UXRInteractionComponent* OutXRInteraction = UXRToolsUtilityFunctions::GetXRInteractionByPriority(FoundXRInteractions, this, 0, EXRInteractionPrioritySelection::LowerEqual);
	OutOverlappedActor = nullptr;
	if (OutXRInteraction)
	{
		OutOverlappedActor = OutXRInteraction->GetOwner();
	}
	return OutXRInteraction;
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

bool UXRInteractorComponent::IsAnyColliderOverlappingActor(TArray<UPrimitiveComponent*> InCollidersToIgnore, AActor* InActor)
{
	TArray<UPrimitiveComponent*> RelevantColliders = AdditionalColliders;
	RelevantColliders.Add(this);
	for (auto IgnoreCollider : InCollidersToIgnore)
	{
		RelevantColliders.Remove(IgnoreCollider);
	}
	for (UPrimitiveComponent* Collider : RelevantColliders)
	{
		if (Collider->IsOverlappingActor(InActor))
		{
			return true;
		}
	}
	return false;
}

void UXRInteractorComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsAnyColliderOverlappingActor({OverlappedComp}, OtherActor))
	{
		UXRInteractionComponent* PrioritizedInteraction = nullptr;
		bool bCanInteract = CanInteract(PrioritizedInteraction, OtherActor, 0, EXRInteractionPrioritySelection::LowerEqual);
		if (!PrioritizedInteraction)
		{
			return;
		}
			RequestHover(PrioritizedInteraction, true);
	}
}

void UXRInteractorComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsAnyColliderOverlappingActor({OverlappedComp}, OtherActor))
	{
		TArray<UXRInteractionComponent*> FoundInteractions = {};
		if (UXRToolsUtilityFunctions::IsActorInteractive(OtherActor, FoundInteractions))
		{
			for (auto Interaction : FoundInteractions)
			{
				RequestHover(Interaction, false);
			}
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