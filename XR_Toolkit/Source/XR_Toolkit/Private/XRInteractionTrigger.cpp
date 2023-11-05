#include "XRInteractionTrigger.h"
#include "Net/UnrealNetwork.h"



UXRInteractionTrigger::UXRInteractionTrigger()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = true;
	SetIsReplicated(true);

	// Set default config values for this Interaction Type
	bSnapXRLaserToActor = true; 
	InteractionPriority = 2;
}

void UXRInteractionTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (bTriggerOnHover)
	{
		bSnapXRLaserToActor = false;
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Interaction Events
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionTrigger::StartInteraction(UXRInteractorComponent* InInteractor)
{
	if (bTriggerOnHover)
	{
		return;
	}
	Super::StartInteraction(InInteractor);
	SetTriggerState(true);
	if (!IsContinuousInteraction())
	{
		RequestCooldown();
	}
}

void UXRInteractionTrigger::EndInteraction(UXRInteractorComponent* InInteractor)
{
	if (bTriggerOnHover)
	{
		return;
	}
	Super::EndInteraction(InInteractor);
	RequestCooldown();
	SetTriggerState(false);
}

void UXRInteractionTrigger::HoverInteraction(UXRInteractorComponent* InInteractor, bool bInHoverState)
{
	Super::HoverInteraction(InInteractor, bInHoverState);
	if (!bTriggerOnHover)
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
	if (GetOwner()->HasAuthority())
	{
		if (InTriggerState == bTriggerState)
		{
			return;
		}
		bTriggerState = InTriggerState;
	}
}
bool UXRInteractionTrigger::GetTriggerState()
{
	return bTriggerState;
}

// Disable the component until the started timer completes.
void UXRInteractionTrigger::RequestCooldown()
{
	if (CooldownDuration > 0.0f)
	{
		SetActive(false, false);
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.SetTimer(CooldownTimerHandle, this, &UXRInteractionTrigger::EnableComponent, CooldownDuration, false);
	}
}

void UXRInteractionTrigger::EnableComponent()
{
	SetActive(true, false);
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
