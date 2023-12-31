#include "XRInteractionTrigger.h"
#include "XRInteractorComponent.h"
#include "Net/UnrealNetwork.h"



UXRInteractionTrigger::UXRInteractionTrigger()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = true;
	SetIsReplicated(true);

	InteractionPriority = 2;
	LaserBehavior = EXRLaserBehavior::Snap;
}

void UXRInteractionTrigger::BeginPlay()
{
	Super::BeginPlay();
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Interaction Events
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionTrigger::StartInteraction(UXRInteractorComponent* InInteractor)
{
	if (TriggerType == EXRTriggerType::HoverOneShot || TriggerType == EXRTriggerType::HoverHold)
	{
		return;
	}
	Super::StartInteraction(InInteractor);
	if (TriggerType == EXRTriggerType::OneShot)
	{
		EndInteractionTimer();
	}
	SetTriggerState(!GetTriggerState());
}

void UXRInteractionTrigger::EndInteraction(UXRInteractorComponent* InInteractor)
{
	if (TriggerType == EXRTriggerType::HoverOneShot || TriggerType == EXRTriggerType::HoverHold)
	{
		return;
	}
	Super::EndInteraction(InInteractor);
	SetTriggerState(false);
}

void UXRInteractionTrigger::HoverInteraction(UXRInteractorComponent* InInteractor, bool bInHoverState)
{
	Super::HoverInteraction(InInteractor, bInHoverState);
	if (TriggerType == EXRTriggerType::OneShot || TriggerType == EXRTriggerType::Hold)
	{
		return;
	}
	if (bInHoverState)
	{
		Super::StartInteraction(InInteractor);
		SetTriggerState(true);
	}
	else
	{
		Super::EndInteraction(InInteractor);
		SetTriggerState(false);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Trigger State
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionTrigger::SetTriggerState(bool InTriggerState)
{
	// For Standalone
	bTriggerState = InTriggerState;
	if (InTriggerState == bTriggerState)
	{
		return;
	}
	Server_SetTriggerState(InTriggerState);
}

void UXRInteractionTrigger::Server_SetTriggerState_Implementation(bool InTriggerState)
{
	if (InTriggerState == bTriggerState)
	{
		return;
	}
	bTriggerState = InTriggerState;
}
bool UXRInteractionTrigger::GetTriggerState()
{
	return bTriggerState;
}

// Disable the component until the started timer completes.
void UXRInteractionTrigger::EndInteractionTimer()
{
	if (InteractionDuration > 0.0f)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.SetTimer(CooldownTimerHandle, this, &UXRInteractionTrigger::RequestInteractionTermination, InteractionDuration, false);
	}
}

void UXRInteractionTrigger::RequestInteractionTermination()
{
	if (GetActiveInteractor())
	{
		GetActiveInteractor()->StopXRInteraction(this);
	}
}

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
