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
	if (bUseOnHover)
	{
		bSnapXRLaserToActor = false;
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------

void UXRInteractionTrigger::StartInteraction(UXRInteractorComponent* InInteractor)
{
	if (bUseOnHover)
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
	if (bUseOnHover)
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
	if (!bUseOnHover)
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
		Deactivate();
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.SetTimer(CooldownTimerHandle, this, &UXRInteractionTrigger::EnableComponent, CooldownDuration, false);
	}
}

void UXRInteractionTrigger::EnableComponent()
{
	Activate();
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
