#include "XRInteractionTrigger.h"
#include "XRInteractorComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"



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
	switch (TriggerType)
	{
		case EXRTriggerType::SingleHover:
			return;
		case EXRTriggerType::ToggleHover:
			return;
		case EXRTriggerType::HoverHold:
			return;
		case EXRTriggerType::Single:
			Super::StartInteraction(InInteractor);
			Server_SetTriggerState(!DefaultTriggerState);
			EndInteractionAfterTimer();
			break;
		case EXRTriggerType::Toggle:
			Super::StartInteraction(InInteractor);
			Server_SetTriggerState(!GetTriggerState());
			EndInteractionAfterTimer();
			break;
		case EXRTriggerType::Hold:
			Super::StartInteraction(InInteractor);
			Server_SetTriggerState(!DefaultTriggerState);
			break;
	}
}

void UXRInteractionTrigger::EndInteraction(UXRInteractorComponent* InInteractor)
{
	switch (TriggerType)
	{

		case EXRTriggerType::SingleHover:
			return;
		case EXRTriggerType::ToggleHover:
			return;
		case EXRTriggerType::HoverHold:
			return;
		case EXRTriggerType::Toggle:
			Super::EndInteraction(InInteractor);
			break;
		default:
			Super::EndInteraction(InInteractor);
			Server_SetTriggerState(DefaultTriggerState);
			break;
	}
}

void UXRInteractionTrigger::HoverInteraction(UXRInteractorComponent* InInteractor, bool bInHoverState)
{
	Super::HoverInteraction(InInteractor, bInHoverState);
	switch (TriggerType)
	{
	case EXRTriggerType::Single:
		return;
	case EXRTriggerType::Toggle:
		return;
	case EXRTriggerType::Hold:
		return;
	case EXRTriggerType::ToggleHover:
		if (bInHoverState)
		{
			Super::StartInteraction(InInteractor);
		}
		else
		{
			Super::EndInteraction(InInteractor);
		}
		Server_SetTriggerState(!GetTriggerState());
		break;

	default:
		if (bInHoverState)
		{
			Super::StartInteraction(InInteractor);
			Server_SetTriggerState(true);
		}
		else
		{
			Super::EndInteraction(InInteractor);
			Server_SetTriggerState(false);
		}
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
bool UXRInteractionTrigger::GetTriggerState()
{
	return bTriggerState;
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
