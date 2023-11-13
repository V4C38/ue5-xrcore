

#include "XRInteractorComponent.h"
#include "XRInteractionComponent.h"
#include "XRToolsUtilityFunctions.h"
#include "Net/UnrealNetwork.h"

UXRInteractorComponent::UXRInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetGenerateOverlapEvents(true);
	bAutoActivate = true;
	SetIsReplicated(true);
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
void UXRInteractorComponent::RequestStartXRInteraction(UXRInteractionComponent* InXRInteraction)
{
	if (InXRInteraction) // Start specified XRInteraction instead of searching for the next available one
	{
		OnRequestStartXRInteraction.Broadcast(this, InXRInteraction);
		return;
	}

	UXRInteractionComponent* InteractionToStart = nullptr;
	if (ActiveInteractionComponents.Num() > 0) // If already interacting, get Interacted Actor and start next available Interaction
	{
		AActor* CurrentInteractedActor = ActiveInteractionComponents[0]->GetOwner();
		if (CurrentInteractedActor)
		{
			InteractionToStart = UXRToolsUtilityFunctions::GetPrioritizedXRInteractionOnActor(CurrentInteractedActor, this);
		}
	}
	else // Search next available Interactive Actor and start highest priority Interaction
	{
		AActor* ClosestInteractiveActor = GetClosestXRInteractionActor();
		if (ClosestInteractiveActor)
		{
			InteractionToStart = UXRToolsUtilityFunctions::GetPrioritizedXRInteractionOnActor(ClosestInteractiveActor);
		}
	}
	// If we found an interaction to start, set the current actor and broadcast the event
	if (InteractionToStart)
	{
		// Broadcasting to XRInteractionSystemComponent (which will evaluate and start this interaction if valid)
		OnRequestStartXRInteraction.Broadcast(this, InteractionToStart);
	}
}

void UXRInteractorComponent::RequestStopXRInteraction(UXRInteractionComponent* InXRInteraction)
{
	if (InXRInteraction) // Stop specified XRInteraction instead of searching for the next available one
	{
		OnRequestStopXRInteraction.Broadcast(this, InXRInteraction);
		return;
	}

	UXRInteractionComponent* InteractionToStop = nullptr;
	if (ActiveInteractionComponents.Num() > 0) // If already interacting, get Interacted Actor and stop next available Interaction
	{
		InteractionToStop = UXRToolsUtilityFunctions::GetPrioritizedXRInteraction(ActiveInteractionComponents, nullptr, false, false);
	}
	if (InteractionToStop)
	{
		OnRequestStopXRInteraction.Broadcast(this, InteractionToStop);
	}
}

void UXRInteractorComponent::RequestStopAllXRInteractions()
{
	// Broadcasting to XRInteractionSystemComponent
	OnRequestStopAllXRInteractions.Broadcast(this);
}

// [Server] Implementation for starting interaction with a component, adds to active interactions if continuous
void UXRInteractorComponent::Server_StartInteracting_Implementation(UXRInteractionComponent* InInteractionComponent)
{
	if (!InInteractionComponent)
	{
		return;
	}
	// Ensuring we only add to active interactions for continuous interactions
	if (InInteractionComponent->IsContinuousInteraction())
	{
		ActiveInteractionComponents.AddUnique(InInteractionComponent);
	}
	Multicast_StartedInteracting_Implementation(InInteractionComponent);
}

void UXRInteractorComponent::Multicast_StartedInteracting_Implementation(UXRInteractionComponent* InteractionComponent)
{
	OnStartInteracting.Broadcast(this, InteractionComponent);
}


// [Server] Implementation for stopping interaction with a component, removes from active interactions if continuous
void UXRInteractorComponent::Server_StopInteracting_Implementation(UXRInteractionComponent* InInteractionComponent)
{
	if (InInteractionComponent)
	{
		ActiveInteractionComponents.Remove(InInteractionComponent);
	}
	Multicast_StoppedInteracting_Implementation(InInteractionComponent);
}

void UXRInteractorComponent::Multicast_StoppedInteracting_Implementation(UXRInteractionComponent* InteractionComponent)
{
	OnStopInteracting.Broadcast(this, InteractionComponent);
	TArray<AActor*> OverlappingActors;
	this->GetOverlappingActors(OverlappingActors);
	for (AActor* Actor : OverlappingActors)
	{
		HoverActor(Actor, true);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Utility
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

bool UXRInteractorComponent::CanInteract(UXRInteractionComponent*& OutPrioritizedXRInteraction) const
{
	AActor* ClosestInteractionActor = GetClosestXRInteractionActor();
	if (ClosestInteractionActor)
	{
		TArray<UXRInteractionComponent*> FoundXRInteractions = {};
		ClosestInteractionActor->GetComponents<UXRInteractionComponent>(FoundXRInteractions);
		OutPrioritizedXRInteraction = UXRToolsUtilityFunctions::GetPrioritizedXRInteraction(FoundXRInteractions);
		return true;
	}
	return false;
}

// Finds the closest actor that is interactable, based on distance and active interactions
AActor* UXRInteractorComponent::GetClosestXRInteractionActor() const
{
	TArray<AActor*> OverlappingActors = {};
	TArray<AActor*> InteractiveActors = {};
	GetOverlappingActors(OverlappingActors);
	AActor* PrioritizedActor = nullptr;
	float MinimumDistance = 0.0f;

	for (auto* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor)
		{
			TArray<UXRInteractionComponent*> FoundXRInteractions = {};
			OverlappingActor->GetComponents<UXRInteractionComponent>(FoundXRInteractions);
			bool AvailableInteraction = false;
			for (UXRInteractionComponent* FoundInteraction : FoundXRInteractions)
			{
				if (!FoundInteraction->IsInteractionActive() || FoundInteraction->GetAllowTakeOver())
				{
					AvailableInteraction = true;
					break;
				}
			}
			if (AvailableInteraction)
			{
				const float DistanceToActor = (this->GetComponentLocation() - OverlappingActor->GetActorLocation()).Size();
				if (PrioritizedActor)
				{
					if (DistanceToActor < MinimumDistance)
					{
						MinimumDistance = DistanceToActor;
						PrioritizedActor = OverlappingActor;
					}
				}
				else
				{
					MinimumDistance = DistanceToActor;
					PrioritizedActor = OverlappingActor;
				}
			}
		}
	}
	return PrioritizedActor;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Hovering
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractorComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}
	HoverActor(OtherActor, true);
}

void UXRInteractorComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}
	HoverActor(OtherActor, false);
}

void UXRInteractorComponent::HoverActor(AActor* OtherActor, bool bHoverState)
{
	TArray<UXRInteractionComponent*> TempInteractionComponents = {};
	OtherActor->GetComponents<UXRInteractionComponent>(TempInteractionComponents);
	for (UXRInteractionComponent* InteractionComp : TempInteractionComponents)
	{
		if (InteractionComp->IsActive())
		{
			if (!IsLaserInteractor() || IsLaserInteractor() && InteractionComp->IsLaserInteractionEnabled())
			{
				if (bHoverState)
				{
					OnHoverStateChanged.Broadcast(this, InteractionComp, true);
					HoveredInteractionComponents.AddUnique(InteractionComp);
					InteractionComp->HoverInteraction(this, true);
				}
				else
				{
					OnHoverStateChanged.Broadcast(this, InteractionComp, false);
					HoveredInteractionComponents.Remove(InteractionComp);
					InteractionComp->HoverInteraction(this, false);
				}
			}
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
	return ActiveInteractionComponents;
}

bool UXRInteractorComponent::IsLaserInteractor()
{
	return bIsLaserInteractor;
}

bool UXRInteractorComponent::IsLocallyControlled() const
{
	return bIsLocallyControlled;
}

UPhysicsConstraintComponent* UXRInteractorComponent::GetAssignedPhysicsConstraint() const
{
	return AssignedPhysicsConstraint;
}

void UXRInteractorComponent::SetAssignedPhysicsConstraint(UPhysicsConstraintComponent* InPhysicsConstraintComponent)
{
	AssignedPhysicsConstraint = InPhysicsConstraintComponent;
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