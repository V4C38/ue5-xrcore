
#include "XRInteractionSystemComponent.h"
#include "Net/UnrealNetwork.h"

UXRInteractionSystemComponent::UXRInteractionSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicated(true);
}

// Registers all XRInteractors found on the Owning Actor on component start.
void UXRInteractionSystemComponent::BeginPlay()
{
	InitiallyRegisterXRInteractors();
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Interaction Events
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

// Server: Starts interaction handling and sets owner for interaction authority.
void UXRInteractionSystemComponent::Server_StartInteraction_Implementation(
	UXRInteractionComponent* InInteractionComponent, UXRInteractorComponent* InXRInteractor)
{
	// Give Authority to the InteractionComponents Actor
	InInteractionComponent->GetOwner()->SetOwner(this->GetOwner());
	
	UXRInteractionComponent* PrioritizedInteraction = nullptr;
	int HighestPriority = 0;
	if(InInteractionComponent)
	{
		if (InInteractionComponent->IsActive())
		{
			UXRInteractorComponent* CurrentInteractor = InInteractionComponent->GetActiveInteractor();
			if (CurrentInteractor)
			{
				if (CurrentInteractor != InXRInteractor)
				{
					Server_StopInteraction(InInteractionComponent, CurrentInteractor);
				}
			}

			if (InXRInteractor)
			{
				InXRInteractor->Server_StartInteracting(InInteractionComponent);
			}
			// Cache Continuous XRInteraction 
			if (InInteractionComponent->IsContinuousInteraction())
			{
				ActiveInteractions.Add(InInteractionComponent);
			}
			Multicast_StartInteraction(InInteractionComponent, InXRInteractor);
		}
	}
}

// Multicast: Notifies all clients that an interaction has started.
void UXRInteractionSystemComponent::Multicast_StartInteraction_Implementation(
	UXRInteractionComponent* InInteractionComponent, UXRInteractorComponent* InXRInteractor)
{
	InInteractionComponent->StartInteraction(InXRInteractor);
	OnInteractionStart.Broadcast(this, InInteractionComponent, InXRInteractor);
}

// Bound for each Registered XRInteractor 
void UXRInteractionSystemComponent::OnInteractorRequestStartXRInteraction(UXRInteractorComponent* Sender, UXRInteractionComponent* InteractionToStart)
{
	if (Sender && InteractionToStart)
	{
		Server_StartInteraction(InteractionToStart, Sender);
	}
}


// Server: Stops interaction and updates server and client state.
void UXRInteractionSystemComponent::Server_StopInteraction_Implementation(UXRInteractionComponent* InInteractionComponent, UXRInteractorComponent* InInteractor)
{
	if(InInteractionComponent)
	{
		if (InInteractor)
		{
			InInteractor->Server_StopInteracting(InInteractionComponent);
		}
		// Remove Cached Continuous XRInteraction 
		ActiveInteractions.Remove(InInteractionComponent);
		Multicast_StopInteraction(InInteractionComponent, InInteractor);
	}
}

// Multicast: Notifies all clients that an interaction has ended.
void UXRInteractionSystemComponent::Multicast_StopInteraction_Implementation(
	UXRInteractionComponent* InInteractionComponent, UXRInteractorComponent* InXRInteractor)
{
	InInteractionComponent->EndInteraction(InXRInteractor);
	OnInteractionEnd.Broadcast(this, InInteractionComponent, InXRInteractor);
}

// Bound for each Registered XRInteractor 
void UXRInteractionSystemComponent::OnInteractorRequestStopXRInteraction(UXRInteractorComponent* Sender, UXRInteractionComponent* InteractionToStop)
{
	if (Sender && InteractionToStop)
	{
		Server_StopInteraction(InteractionToStop, Sender);
	}
}
// Bound for each Registered XRInteractor 
void UXRInteractionSystemComponent::OnInteractorRequestStopAllXRInteractions(UXRInteractorComponent* Sender)
{
	TArray<UXRInteractionComponent*> SenderActiveInteractions = Sender->GetActiveInteractions();
	for (UXRInteractionComponent* ActiveInteraction : SenderActiveInteractions)
	{
		Server_StopInteraction(ActiveInteraction, Sender);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Utility
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionSystemComponent::InitiallyRegisterXRInteractors()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// Iterate over all components and find XRInteractorComponents.
	TArray<UActorComponent*> Components;
	Owner->GetComponents(UXRInteractorComponent::StaticClass(), Components);
	for (UActorComponent* Component : Components)
	{
		UXRInteractorComponent* InteractorComponent = Cast<UXRInteractorComponent>(Component);
		if (InteractorComponent)
		{
			RegisterXRInteractor(InteractorComponent);
		}
	}
}

// Registers a single XRInteractor with the system.
void UXRInteractionSystemComponent::RegisterXRInteractor(UXRInteractorComponent* InInteractor) 
{
	if (InInteractor)
	{
		RegisteredXRInteractors.AddUnique(InInteractor);
		InInteractor->OnRequestStartXRInteraction.AddDynamic(this, &UXRInteractionSystemComponent::OnInteractorRequestStartXRInteraction);
		InInteractor->OnRequestStopXRInteraction.AddDynamic(this, &UXRInteractionSystemComponent::OnInteractorRequestStopXRInteraction);
		InInteractor->OnRequestStopAllXRInteractions.AddDynamic(this, &UXRInteractionSystemComponent::OnInteractorRequestStopAllXRInteractions);
	}
}
// Retrieves the list of currently registered XRInteractors.
TArray<UXRInteractorComponent*> UXRInteractionSystemComponent::GetRegisteredXRInteractors() const
{
	return RegisteredXRInteractors;
}

// Retrieves the list of currently active interactions.
TArray<UXRInteractionComponent*> UXRInteractionSystemComponent::GetActiveXRInteractions() const
{
	return ActiveInteractions;
}



// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRInteractionSystemComponent, ActiveInteractions);
}
// ------------------------------------------------------------------------------------------------------------------------------------------------------------