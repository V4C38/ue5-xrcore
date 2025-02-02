
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
void UXRLaserComponent::SetLaserActive_Implementation(bool bInState)
{
	Server_SetLaserActive(bInState);
}

bool UXRLaserComponent::IsLaserActive_Implementation() const
{
	return bIsLaserActive;
}

EXRLaserState UXRLaserComponent::GetLaserState_Implementation()
{
	if (XRLaserActor && XRLaserActor->GetClass()->ImplementsInterface(UXRLaserInterface::StaticClass()))
	{
		return IXRLaserInterface::Execute_GetLaserState(XRLaserActor);
	}
	return EXRLaserState::Inactive;
}

void UXRLaserComponent::SetControllerHand_Implementation(EControllerHand InXRControllerHand)
{
	XRControllerHand = InXRControllerHand;
	if (XRLaserActor && XRLaserActor->GetClass()->ImplementsInterface(UXRLaserInterface::StaticClass()))
	{
		IXRLaserInterface::Execute_SetControllerHand(XRLaserActor, XRControllerHand);
	}
}

EControllerHand UXRLaserComponent::GetControllerHand_Implementation() const
{
	return XRControllerHand;
}

AActor* UXRLaserComponent::GetXRLaserActor_Implementation() const
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


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// XRInteraction Interface
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRLaserComponent::StartInteractionByPriority_Implementation(int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition)
{
	Server_RequestInteraction(0, InPriority, InPrioritySelectionCondition);
}

void UXRLaserComponent::StopInteractionByPriority_Implementation(int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition)
{
	Server_RequestInteraction(1, InPriority, InPrioritySelectionCondition);
}

void UXRLaserComponent::StopAllInteractions_Implementation(UXRInteractorComponent* InInteractor)
{
	Server_RequestInteraction(2, 0, EXRInteractionPrioritySelection::Equal);
}


void UXRLaserComponent::Server_RequestInteraction_Implementation(int32 InID, int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition)
{
	Multicast_RequestInteraction_Implementation(InID, InPriority, InPrioritySelectionCondition);
}

void UXRLaserComponent::Multicast_RequestInteraction_Implementation(int32 InID, int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition)
{
	if (InID == 0)
	{
		if (XRLaserActor && XRLaserActor->GetClass()->ImplementsInterface(UXRInteractionInterface::StaticClass()))
		{
			IXRInteractionInterface::Execute_StartInteractionByPriority(XRLaserActor, InPriority, InPrioritySelectionCondition);
		}
	}
	else if (InID == 1)
	{
		if (XRLaserActor && XRLaserActor->GetClass()->ImplementsInterface(UXRInteractionInterface::StaticClass()))
		{
			IXRInteractionInterface::Execute_StartInteractionByPriority(XRLaserActor, InPriority, InPrioritySelectionCondition);
		}

	}
	else if (InID == 2)
	{
		if (XRLaserActor && XRLaserActor->GetClass()->ImplementsInterface(UXRInteractionInterface::StaticClass()))
		{
			IXRInteractionInterface::Execute_StopAllInteractions(XRLaserActor, nullptr);
		}
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
	IXRLaserInterface::Execute_ProvideAttachmentRoot(XRLaserActor, this);
	IXRLaserInterface::Execute_SetControllerHand(XRLaserActor, XRControllerHand);
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
	bIsLaserActive = bInActive;
	Multicast_SetLaserActive(bInActive);
}

void UXRLaserComponent::Multicast_SetLaserActive_Implementation(bool bInActive)
{
	if (XRLaserActor && XRLaserActor->GetClass()->ImplementsInterface(UXRLaserInterface::StaticClass()))
	{
		IXRLaserInterface::Execute_SetLaserActive(XRLaserActor, bInActive);
	}
}

void UXRLaserComponent::OnRep_IsLaserActive()
{
	OnXRLaserStateChanged.Broadcast(this, bIsLaserActive);
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
	DOREPLIFETIME(UXRLaserComponent, bIsLaserActive);
}
