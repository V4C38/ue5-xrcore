
#include "XRInteractionSystemComponent.h"
#include "Net/UnrealNetwork.h"

UXRInteractionSystemComponent::UXRInteractionSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicated(true);
}

// =================================================================================================================================
// Interaction Execution
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
					Server_StopInteracting(CurrentInteractor, 0);
				}
			}

			// Cache Continuous XRInteraction 
			if (InInteractionComponent->GetIsContinuousInteraction())
			{
				ActiveInteractions.Add(InInteractionComponent);
				
				if (InXRInteractor)
				{
					InXRInteractor->Server_AddActiveInteractionComponent(InInteractionComponent);
					InXRInteractor->OnRequestStopXRInteraction.AddDynamic(this, &UXRInteractionSystemComponent::OnRequestStopXRInteraction);
				}
			}
			Multicast_StartInteraction(InInteractionComponent, InXRInteractor);
		}
	}
}

void UXRInteractionSystemComponent::Multicast_StartInteraction_Implementation(
	UXRInteractionComponent* InInteractionComponent, UXRInteractorComponent* InXRInteractor)
{
	InInteractionComponent->StartInteraction(InXRInteractor);
	OnInteractionStart.Broadcast(this, InInteractionComponent, InXRInteractor);
}
// =================================================================================================================================


// =================================================================================================================================
// Stop Interaction Execution
void UXRInteractionSystemComponent::Server_StopInteraction_Implementation(UXRInteractionComponent* InInteractionComponent, UXRInteractorComponent* InInteractor)
{
	if(InInteractionComponent)
	{
		// Remove Cached Continuous XRInteraction 
		if (InInteractionComponent->GetIsContinuousInteraction())
		{
			ActiveInteractions.Remove(InInteractionComponent);
			if(InInteractor)
			{
				InInteractor->Server_RemoveActiveInteractionComponent(InInteractionComponent);
			}
		}
		Multicast_StopInteraction(InInteractionComponent, InInteractor);
	}
}

void UXRInteractionSystemComponent::Multicast_StopInteraction_Implementation(
	UXRInteractionComponent* InInteractionComponent, UXRInteractorComponent* InXRInteractor)
{
	InInteractionComponent->EndInteraction(InXRInteractor);
	OnInteractionEnd.Broadcast(this, InInteractionComponent, InXRInteractor);
}
// =================================================================================================================================


// =================================================================================================================================
// Look for interactive objects
void UXRInteractionSystemComponent::Server_StartInteracting_Implementation(UXRInteractorComponent* InXRInteractor, int32 InExclusivePriority)
{
	if (InXRInteractor)
	{
		TArray<UXRInteractionComponent*> XRInteractions = {};
		// Get XRInteractionComponents from the closest Actor overlapping the XRInteractor
		if (!InExclusivePriority)
		{
			XRInteractions.Add(GetPrioritizedXRInteraction(InXRInteractor->GetGetClosestXRInteractions(0), InXRInteractor));
		}
		// Get XRInteractionComponents with only the specified Priority Setting
		else
		{
			XRInteractions = InXRInteractor->GetGetClosestXRInteractions(InExclusivePriority);
		}

		// Start Interaction on found / filtered Components 
		for (auto* PrioritizedInteraction : XRInteractions)
		{
			if (PrioritizedInteraction)
			{
				if (InXRInteractor->IsLaserInteractor())
				{
					if(PrioritizedInteraction->IsLaserInteractionEnabled())
					{
						Server_StartInteraction(PrioritizedInteraction, InXRInteractor);
					}
				}
				else
				{
					Server_StartInteraction(PrioritizedInteraction, InXRInteractor);
				}
			}
		}
	}
}
// =================================================================================================================================


// =================================================================================================================================
void UXRInteractionSystemComponent::Server_StopInteracting_Implementation(UXRInteractorComponent* InXRInteractor, int32 InExclusivePriority)
{
	if (InXRInteractor)
	{
		TArray<UXRInteractionComponent*> XRInteractionsToStop = {};
		// Cache only XRInteractions with the specified Priority
		// There is no support for a range - just a single value. Might result in having to call this function multiple times...
		if (InExclusivePriority)
		{
			for (auto* ActiveInteraction : InXRInteractor->GetActiveInteractionComponents())
			{
				if (ActiveInteraction)
				{
					if (ActiveInteraction->GetInteractionPriority() == InExclusivePriority)
					{
						XRInteractionsToStop.Add(ActiveInteraction);
					}
				}
			}
		}
		// Cache all XRInteractions
		else
		{
			XRInteractionsToStop = InXRInteractor->GetActiveInteractionComponents();
		}

		// Stop Cached Interactions
		for (auto* XRInteractionToStop : XRInteractionsToStop)
		{
			if (XRInteractionToStop)
			{
				Server_StopInteraction(XRInteractionToStop, InXRInteractor);
			}
		}
	}
}
// =================================================================================================================================


// =================================================================================================================================
// Utility
void UXRInteractionSystemComponent::Server_RestartAllActiveInteractions_Implementation()
{
	for (auto* ActiveInteraction : ActiveInteractions)
	{
		if (ActiveInteraction)
		{
			Multicast_StartInteraction(ActiveInteraction, ActiveInteraction->GetActiveInteractor());
		}
	}
}

void UXRInteractionSystemComponent::Server_StopAllActiveInteractions_Implementation()
{
	for (auto* ActiveInteraction : ActiveInteractions)
	{
		if (ActiveInteraction)
		{
			Multicast_StopInteraction(ActiveInteraction, ActiveInteraction->GetActiveInteractor());
		}
	}
}


TArray<UXRInteractionComponent*> UXRInteractionSystemComponent::GetActiveInteractions() const
{
	return ActiveInteractions;
}

UXRInteractionComponent* UXRInteractionSystemComponent::GetPrioritizedXRInteraction(
	TArray<UXRInteractionComponent*> InXRInteractions, UXRInteractorComponent* InXRInteractor) const
{
	UXRInteractionComponent* PrioritizedInteraction = nullptr;
	int HighestPriority = -1;
	
	for(auto* CurrentXRInteraction : InXRInteractions)
	{
		if (CurrentXRInteraction)
		{
			const int CurrentPriority = CurrentXRInteraction->GetInteractionPriority();
			if (CurrentPriority > HighestPriority)
			{
				// Active Interactions are only considered when explicitly enabled in the XRInteraction Config
				if (CurrentXRInteraction->GetIsInteractionActive())
				{
					UXRInteractorComponent* CurrentInteractor = CurrentXRInteraction->GetActiveInteractor();
					if (CurrentInteractor && InXRInteractor)
					{
						// TakeOver Mechanic 
						if (CurrentInteractor != InXRInteractor && CurrentXRInteraction->GetAllowTakeOver())
						{
							HighestPriority = CurrentPriority;
							PrioritizedInteraction = CurrentXRInteraction;
						}
					}
				}
				// InActive / Non-Continuous Interactions are always considered for prioritization
				else
				{
					HighestPriority = CurrentPriority;
					PrioritizedInteraction = CurrentXRInteraction;
				}
			}
		}
	}

	return PrioritizedInteraction;
}

void UXRInteractionSystemComponent::OnRequestStopXRInteraction(UXRInteractorComponent* Sender)
{
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, TEXT("Bound to Stop"));
	Server_StopInteracting(Sender, 0);
}
// =================================================================================================================================


// =================================================================================================================================
void UXRInteractionSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRInteractionSystemComponent, ActiveInteractions);
}
// =================================================================================================================================
