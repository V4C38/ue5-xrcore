#include "XRInteractionTrigger.h"
#include "Net/UnrealNetwork.h"



UXRInteractionTrigger::UXRInteractionTrigger()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = true;
	SetIsReplicated(true);
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

void UXRInteractionTrigger::StartInteraction(UXRInteractorComponent* InInteractor)
{
	if (bTriggerOnHover)
	{
		return;
	}
	Super::StartInteraction(InInteractor);
	SetTriggerState(!bTriggerState);
	if (!GetIsContinuousInteraction())
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
	if (bResetAfterInteractionEnd)
	{
		SetTriggerState(!bTriggerState);
	}
}

void UXRInteractionTrigger::HoverInteraction(UXRInteractorComponent* InInteractor, bool bInHoverState)
{
	Super::HoverInteraction(InInteractor, bInHoverState);
	if (!bTriggerOnHover)
	{
		return;
	}
	SetTriggerState(bInHoverState);
}
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

void UXRInteractionTrigger::SetTriggerState(bool InTriggerState)
{
	if (GetOwner()->HasAuthority())
	{
		if (InTriggerState == bTriggerState)
		{
			return;
		}
		bTriggerState = !bTriggerState;
	}
}
bool UXRInteractionTrigger::GetTriggerState()
{
	return bTriggerState;
}

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
