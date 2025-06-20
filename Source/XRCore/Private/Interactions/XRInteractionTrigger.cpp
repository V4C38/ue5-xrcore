#include "Interactions/XRInteractionTrigger.h"
#include "Interactions/XRInteractorComponent.h"

#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"



UXRInteractionTrigger::UXRInteractionTrigger()
{
	LaserBehavior = EXRLaserBehavior::Snap;
	MultiInteractorBehavior = EXRMultiInteractorBehavior::Enabled;
}

void UXRInteractionTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (GetTriggerState() != DefaultTriggerState)
	{
		Server_SetTriggerState(DefaultTriggerState, nullptr);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Interaction Events
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionTrigger::StartInteraction(UXRInteractorComponent* InInteractor)
{
	// Only Interact for the first Interactor (if multiple)
	if (GetActiveInteractors().Num() > 0)
	{
		Super::StartInteraction(InInteractor);
		return;
	}

	Super::StartInteraction(InInteractor);
	switch (TriggerBehavior)
	{
		case EXRInteractionTriggerBehavior::Trigger:
			Server_SetTriggerState(!DefaultTriggerState, InInteractor);
			EndInteractionAfterTimer();
			break;
		case EXRInteractionTriggerBehavior::Toggle:
			Server_SetTriggerState(!GetTriggerState(), InInteractor);
			EndInteractionAfterTimer();
			break;
		case EXRInteractionTriggerBehavior::Hold:
			Server_SetTriggerState(!DefaultTriggerState, InInteractor);
			break;
	}
}

void UXRInteractionTrigger::EndInteraction(UXRInteractorComponent* InInteractor)
{
	// Only set TriggerState when last Interactor stops interacting 
	if (GetActiveInteractors().Num() > 1)
	{
		Super::EndInteraction(InInteractor);
		return;
	}

	Super::EndInteraction(InInteractor);
	switch (TriggerBehavior)
	{
		case EXRInteractionTriggerBehavior::Trigger:
			Server_SetTriggerState(DefaultTriggerState, InInteractor);
			break;
		case EXRInteractionTriggerBehavior::Toggle:
			break;
		case EXRInteractionTriggerBehavior::Hold:
			Server_SetTriggerState(DefaultTriggerState, InInteractor);
			break;
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Trigger State
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionTrigger::SetTriggerState(bool InTriggerState, UXRInteractorComponent* InInteractor)
{
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		bTriggerState = InTriggerState;
		OnTriggerStateChanged.Broadcast(this, bTriggerState, InInteractor);
		return;
	}
	Server_SetTriggerState(InTriggerState, InInteractor);
}

void UXRInteractionTrigger::Server_SetTriggerState_Implementation(bool InTriggerState, UXRInteractorComponent* InInteractor)
{
	if (InTriggerState == bTriggerState)
	{
		return;
	}
	bTriggerState = InTriggerState;
	OnTriggerStateChanged.Broadcast(this, bTriggerState, InInteractor);
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		OnTriggerStateChanged.Broadcast(this, bTriggerState, InInteractor);
	}
}

bool UXRInteractionTrigger::GetTriggerState() const
{
	return bTriggerState;
}

void UXRInteractionTrigger::SetTriggerBehavior(EXRInteractionTriggerBehavior InTriggerBehavior)
{
	TriggerBehavior = InTriggerBehavior;
}

EXRInteractionTriggerBehavior UXRInteractionTrigger::GetTriggerBehavior() const
{
	return TriggerBehavior;
}
// Disable the component until the started timer completes.
void UXRInteractionTrigger::EndInteractionAfterTimer()
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}
	if (InteractionDuration > 0.0f)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.SetTimer(CooldownTimerHandle, this, &UXRInteractionTrigger::RequestInteractionTermination, InteractionDuration, false);
	}
}

void UXRInteractionTrigger::RequestInteractionTermination()
{
	if (GetActiveInteractors().Num() > 0)
	{
		for (auto Interactor : GetActiveInteractors())
		{
			Interactor->StopXRInteraction(this);
		}
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Replication
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionTrigger::OnRep_TriggerState()
{
	if (GetActiveInteractors().Num() > 0)
	{
		OnTriggerStateChanged.Broadcast(this, bTriggerState, GetActiveInteractors()[0]);
		return;
	}
	OnTriggerStateChanged.Broadcast(this, bTriggerState, nullptr);
}

void UXRInteractionTrigger::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRInteractionTrigger, bTriggerState);
}
