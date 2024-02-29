#include "XRInteractionTrigger.h"
#include "XRInteractorComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"



UXRInteractionTrigger::UXRInteractionTrigger()
{
	LaserBehavior = EXRLaserBehavior::Snap;
	MultiInteractorBehavior = EXRMultiInteractorBehavior::Allow;
}

void UXRInteractionTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (GetTriggerState() != DefaultTriggerState)
	{
		Server_SetTriggerState(DefaultTriggerState);
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
			Server_SetTriggerState(!DefaultTriggerState);
			EndInteractionAfterTimer();
			break;
		case EXRInteractionTriggerBehavior::Toggle:
			Server_SetTriggerState(!GetTriggerState());
			EndInteractionAfterTimer();
			break;
		case EXRInteractionTriggerBehavior::Hold:
			Server_SetTriggerState(!DefaultTriggerState);
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
			Server_SetTriggerState(DefaultTriggerState);
			break;
		case EXRInteractionTriggerBehavior::Toggle:
			break;
		case EXRInteractionTriggerBehavior::Hold:
			Server_SetTriggerState(DefaultTriggerState);
			break;
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Trigger State
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionTrigger::Server_SetTriggerState_Implementation(bool InTriggerState)
{
	if (InTriggerState == bTriggerState)
	{
		return;
	}
	bTriggerState = InTriggerState;
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		OnTriggerStateChanged.Broadcast(this, bTriggerState);
	}
}
bool UXRInteractionTrigger::GetTriggerState() const
{
	return bTriggerState;
}

EXRInteractionTriggerBehavior UXRInteractionTrigger::GetTriggerBehavior() const
{
	return TriggerBehavior;
}
// Disable the component until the started timer completes.
void UXRInteractionTrigger::EndInteractionAfterTimer()
{
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
// Editor UI
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionTrigger::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UXRInteractionTrigger, TriggerBehavior))
	{
		bInteractionDurationVisible = (TriggerBehavior != EXRInteractionTriggerBehavior::Hold);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Replication
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionTrigger::OnRep_TriggerState()
{
	OnTriggerStateChanged.Broadcast(this, bTriggerState);
}

void UXRInteractionTrigger::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRInteractionTrigger, bTriggerState);
}
