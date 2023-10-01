

#include "XRInteractorComponent.h"
#include "XRInteractionComponent.h"
#include "Net/UnrealNetwork.h"

UXRInteractorComponent::UXRInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = true;
	ComponentTags.AddUnique("Interactor");
	SetIsReplicated(true);
}


EXRHandType UXRInteractorComponent::GetHandType() const
{
	return HandType;
}

void UXRInteractorComponent::Server_RequestStopXRInteraction_Implementation()
{
	OnRequestStopXRInteraction.Broadcast(this);
}

void UXRInteractorComponent::Server_SetHandType_Implementation(EXRHandType InHandType)
{
	HandType = InHandType;
}

bool UXRInteractorComponent::IsInteractionActive() const
{
	for (auto* ActiveInteraction : GetActiveInteractionComponents())
	{
		if (ActiveInteraction)
		{
			return true;
		}
	}
	return false;
}

TArray<UXRInteractionComponent*> UXRInteractorComponent::GetActiveInteractionComponents() const
{
	return ActiveInteractionComponents;
}

TArray<UXRInteractionComponent*> UXRInteractorComponent::GetGetClosestXRInteractions(int32 InExclusivePriority) const
{
	TArray<UXRInteractionComponent*> OutXRInteractions = {};
	TArray<AActor*> OverlappingActors = {};
	GetOverlappingActors(OverlappingActors);
	AActor* ClosestActor = nullptr;
	float MinimumDistance = 0.0f;
	
	
	for (auto* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor)
		{
			TArray<UXRInteractionComponent*> FoundXRInteractions = {};
			OverlappingActor->GetComponents(FoundXRInteractions);

			// When ExclusivePriority is set, remove elements that are not of the selected priority
			if(InExclusivePriority)
			{
				for (auto* FoundXRInteraction : FoundXRInteractions)
				{
					if (FoundXRInteraction)
					{
						if(FoundXRInteraction->GetInteractionPriority() != InExclusivePriority)
						{
							FoundXRInteractions.Remove(FoundXRInteraction);
						}
					}
				}
			}
			
			// Find Closest Actor 
			if (FoundXRInteractions.Num() > 0)
			{
				const float CurrentDistance = (this->GetComponentLocation() - OverlappingActor->GetActorLocation()).Size();
				if (ClosestActor)
				{
					if (CurrentDistance < MinimumDistance)
					{
						MinimumDistance = CurrentDistance;
						ClosestActor = OverlappingActor;
						OutXRInteractions = FoundXRInteractions;
					}
				}
				else
				{
					MinimumDistance = CurrentDistance;
					ClosestActor = OverlappingActor;
					OutXRInteractions = FoundXRInteractions;
				}
			}
		}
	}

	return OutXRInteractions;
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


void UXRInteractorComponent::InitializeComponent()
{
	Super::InitializeComponent();
	CacheIsLocallyControlled();
}

void UXRInteractorComponent::CacheIsLocallyControlled()
{
	AActor* Owner = GetOwner();
	APawn* TempOwningPawn = Cast<APawn>(Owner);
	if (TempOwningPawn)
	{
		bIsLocallyControlled = OwningPawn->IsLocallyControlled();
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

void UXRInteractorComponent::Server_AddActiveInteractionComponent_Implementation(UXRInteractionComponent* InInteractionComponent)
{
	if (InInteractionComponent)
	{
		ActiveInteractionComponents.AddUnique(InInteractionComponent);
	}
	Multicast_StartedInteracting_Implementation(InInteractionComponent);
}
void UXRInteractorComponent::Multicast_StoppedInteracting_Implementation(UXRInteractionComponent* InteractionComponent)
{
	OnStartInteracting.Broadcast(this, InteractionComponent);
}

void UXRInteractorComponent::Server_RemoveActiveInteractionComponent_Implementation(UXRInteractionComponent* InInteractionComponent)
{
	if (InInteractionComponent)
	{
		ActiveInteractionComponents.Remove(InInteractionComponent);
	}
	Multicast_StoppedInteracting_Implementation(InInteractionComponent);
}

void UXRInteractorComponent::Multicast_StartedInteracting_Implementation(UXRInteractionComponent* InteractionComponent)
{
	OnStopInteracting.Broadcast(this, InteractionComponent);
}

void UXRInteractorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRInteractorComponent, ActiveInteractionComponents);
	DOREPLIFETIME(UXRInteractorComponent, HandType);
}