
#include "XRLaserComponent.h"
#include "XRInteractorComponent.h"
#include "Net/UnrealNetwork.h"


UXRLaserComponent::UXRLaserComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
	bAutoActivate = true;
}


void UXRLaserComponent::BeginPlay()
{
	Super::BeginPlay();

	ENetMode NetMode = GetWorld()->GetNetMode();
	if (NetMode == NM_DedicatedServer || NetMode == NM_ListenServer || NetMode == NM_Standalone)
	{
		bool SpawnResult = SpawnXRLaserActor();
		OnXRLaserSpawned.Broadcast(this, SpawnResult);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// XRLaser Interface
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRLaserComponent::SetLaserActive_Implementation()
{
	if (XRLaserActive)
	{
		if (XRLaserActor && XRLaserActor->GetClass()->ImplementsInterface(UXRLaserInterface::StaticClass()))
		{
			IXRLaserInterface::Execute_SetLaserActive(XRLaserActor);
		}
		GetLaserState_Implementation();
		return;
	}

	Server_SetLaserActive(true);
}

void UXRLaserComponent::SetLaserInactive_Implementation()
{
	if (!XRLaserActive)
	{
		if (XRLaserActor && XRLaserActor->GetClass()->ImplementsInterface(UXRLaserInterface::StaticClass()))
		{
			IXRLaserInterface::Execute_SetLaserInactive(XRLaserActor);
			CurrentLaserState = EXRLaserState::Inactive;
			OnXRLaserStateChanged.Broadcast(this, CurrentLaserState);
		}
		return;
	}

	Server_SetLaserActive(false);
}

bool UXRLaserComponent::IsLaserActive_Implementation() const
{
	if (XRLaserActor && XRLaserActor->GetClass()->ImplementsInterface(UXRLaserInterface::StaticClass()))
	{
		return IXRLaserInterface::Execute_IsLaserActive(XRLaserActor);
	}
	return false;
}

EXRLaserState UXRLaserComponent::GetLaserState_Implementation()
{
	EXRLaserState OutState = EXRLaserState::Inactive;
	if (XRLaserActor && XRLaserActor->GetClass()->ImplementsInterface(UXRLaserInterface::StaticClass()))
	{
		OutState =  IXRLaserInterface::Execute_GetLaserState(XRLaserActor);
	}
	if (OutState != CurrentLaserState)
	{
		CurrentLaserState = OutState;
		OnXRLaserStateChanged.Broadcast(this, CurrentLaserState);
	}
	return OutState;
}

void UXRLaserComponent::SetControllerHand_Implementation(EControllerHand InXRControllerHand)
{
	if (XRLaserActor && XRLaserActor->GetClass()->ImplementsInterface(UXRLaserInterface::StaticClass()))
	{
		IXRLaserInterface::Execute_SetControllerHand(XRLaserActor, XRControllerHand);
	}
}

EControllerHand UXRLaserComponent::GetControllerHand_Implementation() const
{
	return XRControllerHand;
}

AActor* UXRLaserComponent::GetXRLaserActor() const
{
	return XRLaserActor;
}

UXRInteractorComponent* UXRLaserComponent::GetXRInteractor_Implementation() const
{
	UXRInteractorComponent* OutXRInteractor = nullptr;
	if (XRLaserActor && XRLaserActor->GetClass()->ImplementsInterface(UXRLaserInterface::StaticClass()))
	{
		OutXRInteractor = IXRLaserInterface::Execute_GetXRInteractor(XRLaserActor);
	}
	return OutXRInteractor;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// XRInteraction Interface
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRLaserComponent::StartInteractionByPriority_Implementation(int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition)
{
	if (XRLaserActor && XRLaserActor->GetClass()->ImplementsInterface(UXRInteractionInterface::StaticClass()))
	{
		IXRInteractionInterface::Execute_StartInteractionByPriority(XRLaserActor, InPriority, InPrioritySelectionCondition);
	}
}
void UXRLaserComponent::StopInteractionByPriority_Implementation(int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition)
{
	if (XRLaserActor && XRLaserActor->GetClass()->ImplementsInterface(UXRInteractionInterface::StaticClass()))
	{
		IXRInteractionInterface::Execute_StopInteractionByPriority(XRLaserActor, InPriority, InPrioritySelectionCondition);
	}
}



// ------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UXRLaserComponent::SpawnXRLaserActor()
{
	if (!XRLaserClass)
	{
		return false;
	}
	if (!XRLaserClass->ImplementsInterface(UXRLaserInterface::StaticClass()))
	{
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);  
	XRLaserActor = GetWorld()->SpawnActor<AActor>(XRLaserClass, Location, Rotation, SpawnParams);

	if (!XRLaserActor)
	{
		return false;
	}

	XRLaserActor->SetReplicates(true);

	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		OnRep_XRLaserActor();
	}
	return true;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Replication
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRLaserComponent::Server_SetLaserActive_Implementation(bool bInActive)
{
	XRLaserActive = bInActive;
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		OnRep_XRLaserActive();
	}
}

void UXRLaserComponent::OnRep_XRLaserActive()
{
	if (XRLaserActive)
	{
		SetLaserActive_Implementation();
	}
	else
	{
		SetLaserInactive_Implementation();
	}
}

void UXRLaserComponent::OnRep_XRLaserActor()
{
	if (!XRLaserActor)
	{
		return;
	}
	if (XRLaserActor->GetClass()->ImplementsInterface(UXRLaserInterface::StaticClass()))
	{
		IXRLaserInterface::Execute_SetControllerHand(XRLaserActor, XRControllerHand);
		IXRLaserInterface::Execute_ProvideAttachmentRoot(XRLaserActor, this);
	}
}

void UXRLaserComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRLaserComponent, XRLaserActor);
	DOREPLIFETIME(UXRLaserComponent, XRLaserActive);
}
