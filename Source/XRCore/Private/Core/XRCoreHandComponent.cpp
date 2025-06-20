 
#include "Core/XRCoreHandComponent.h"
#include "Core/XRCoreHand.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

UXRCoreHandComponent::UXRCoreHandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
	bAutoActivate = true;
}

void UXRCoreHandComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickInterval(ReplicationInterval);

	if (GetOwner()->HasAuthority())
	{
		Server_SpawnXRHand();
	}
}
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Managed Hand Actor
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
AXRCoreHand* UXRCoreHandComponent::GetXRCoreHand() const
{
	return XRCoreHand;
}

void UXRCoreHandComponent::Server_SpawnXRHand_Implementation()
{
	if (IsValid(XRCoreHand) || !XRCoreHandClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetOwner()->GetInstigator();

	XRCoreHand = GetWorld()->SpawnActor<AXRCoreHand>(XRCoreHandClass, GetComponentTransform(), SpawnParams);
	if (XRCoreHand->GetClass()->ImplementsInterface(UXRCoreHandInterface::StaticClass()))
	{
		IXRCoreHandInterface::Execute_SetControllerHand(XRCoreHand, ControllerHand);
	}
	if (XRCoreHand)
	{
		XRCoreHand->SetReplicates(true);
	}

	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		OnRep_XRCoreHand();
	}
}

void UXRCoreHandComponent::OnRep_XRCoreHand()
{
	if (!XRCoreHand)
	{
		return;
	}

	OwningPawn = Cast<APawn>(GetOwner());
	if (!OwningPawn)
	{
		return;
	}

	bIsLocallyControlled = OwningPawn->IsLocallyControlled();
	XRCoreHand->OwningPawn = OwningPawn;
	XRCoreHand->bIsLocallyControlled = bIsLocallyControlled;
	if (XRCoreHand->GetClass()->ImplementsInterface(UXRCoreHandInterface::StaticClass()))
	{
		IXRCoreHandInterface::Execute_SetControllerHand(XRCoreHand, ControllerHand);
	}

	// If this is the controlling client, just attach the Hands root and enable ticking to start replicating to other clients
	if (bIsLocallyControlled)
	{
		if (XRCoreHand->MotionControllerRoot)
		{
			XRCoreHand->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
		PrimaryComponentTick.SetTickFunctionEnable(true);
	}	

}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Replication
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

// Tick only executes on the locally controlling client
void UXRCoreHandComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsLocallyControlled)
	{
		FXRCoreHandReplicationData HandData;
		HandData.Location = GetComponentLocation();
		HandData.Rotation = GetComponentQuat();
		HandData.PrimaryInputAxis = PrimaryInputAxisValue;
		HandData.SecondaryInputAxis = SecondaryInputAxisValue;

		Server_UpdateHandData(HandData);
	}
}

void UXRCoreHandComponent::Server_UpdateHandData_Implementation(FXRCoreHandReplicationData InXRCoreHandData)
{
	Multicast_UpdateHandData(InXRCoreHandData);
}

void UXRCoreHandComponent::Multicast_UpdateHandData_Implementation(FXRCoreHandReplicationData InXRCoreHandData)
{
	if (XRCoreHand)
	{
		IXRCoreHandInterface::Execute_Client_UpdateXRCoreHandReplicationData(XRCoreHand, InXRCoreHandData);
	}
}

void UXRCoreHandComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRCoreHandComponent, XRCoreHand);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// XRCoreHand Interface Implementations - Pass Through to XRCoreHand
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

UXRInteractorComponent* UXRCoreHandComponent::GetXRInteractor_Implementation() const
{
	return XRCoreHand ? IXRCoreHandInterface::Execute_GetXRInteractor(XRCoreHand) : nullptr;
}

UXRLaserComponent* UXRCoreHandComponent::GetXRLaser_Implementation() const
{
	return XRCoreHand ? IXRCoreHandInterface::Execute_GetXRLaser(XRCoreHand) : nullptr;
}

void UXRCoreHandComponent::SetControllerHand_Implementation(EControllerHand InControllerHand)
{
	ControllerHand = InControllerHand;

	if (XRCoreHand)
	{
		IXRCoreHandInterface::Execute_SetControllerHand(XRCoreHand, InControllerHand);
	}
}

EControllerHand UXRCoreHandComponent::GetControllerHand_Implementation() const
{
	return XRCoreHand ? IXRCoreHandInterface::Execute_GetControllerHand(XRCoreHand) : ControllerHand;
}

void UXRCoreHandComponent::PrimaryInputAction_Implementation(float InAxisValue)
{
	if (!bIsLocallyControlled || !XRCoreHand)
	{
		return;
	}
	PrimaryInputAxisValue = InAxisValue;
	IXRCoreHandInterface::Execute_PrimaryInputAction(XRCoreHand, InAxisValue);
}

void UXRCoreHandComponent::SecondaryInputAction_Implementation(float InAxisValue)
{
	if (!bIsLocallyControlled || !XRCoreHand)
	{
		return;
	}
	SecondaryInputAxisValue = InAxisValue;
	IXRCoreHandInterface::Execute_SecondaryInputAction(XRCoreHand, InAxisValue);
}

void UXRCoreHandComponent::SetIsHandtrackingActive_Implementation(bool InIsActive)
{
	if (XRCoreHand)
	{
		IXRCoreHandInterface::Execute_SetIsHandtrackingActive(XRCoreHand, InIsActive);
	}
}

bool UXRCoreHandComponent::IsHandtrackingActive_Implementation() const
{
	return XRCoreHand ? IXRCoreHandInterface::Execute_IsHandtrackingActive(XRCoreHand) : false;
}

APawn* UXRCoreHandComponent::GetOwningPawn_Implementation() const
{
	return OwningPawn;
}

void UXRCoreHandComponent::Client_UpdateXRCoreHandReplicationData_Implementation(const FXRCoreHandReplicationData& InXRCoreHandData)
{
	if (XRCoreHand)
	{
		IXRCoreHandInterface::Execute_Client_UpdateXRCoreHandReplicationData(XRCoreHand, InXRCoreHandData);
	}
}
