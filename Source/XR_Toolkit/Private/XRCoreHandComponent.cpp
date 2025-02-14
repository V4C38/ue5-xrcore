 
#include "XRCoreHandComponent.h"
#include "XRCoreHand.h"
#include "Net/UnrealNetwork.h"

UXRCoreHandComponent::UXRCoreHandComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
	bAutoActivate = true;
}

void UXRCoreHandComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
	{
		Server_SpawnXRHand();
	}
}

AXRCoreHand* UXRCoreHandComponent::GetXRCoreHand() const
{
	return XRCoreHand;
}

void UXRCoreHandComponent::Server_SpawnXRHand_Implementation()
{
	if (XRCoreHand || !XRCoreHandClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetOwner()->GetInstigator();

	XRCoreHand = GetWorld()->SpawnActor<AXRCoreHand>(XRCoreHandClass, GetComponentTransform(), SpawnParams);

	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		OnRep_XRCoreHand();
	}
}

void UXRCoreHandComponent::OnRep_XRCoreHand()
{
	if (!XRCoreHand || !GetOwner())
	{
		return;
	}

	if (XRCoreHand->GetClass()->ImplementsInterface(UXRCoreHandInterface::StaticClass()))
	{
		IXRCoreHandInterface::Execute_SetControllerHand(XRCoreHand, ControllerHand);
	}

	OwningPawn = Cast<APawn>(GetOwner());
	XRCoreHand->OwningPawn = OwningPawn;
	if (OwningPawn && OwningPawn->HasAuthority())
	{
		XRCoreHand->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
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
	if (XRCoreHand)
	{
		IXRCoreHandInterface::Execute_PrimaryInputAction(XRCoreHand, InAxisValue);
	}
}

void UXRCoreHandComponent::SecondaryInputAction_Implementation(float InAxisValue)
{
	if (XRCoreHand)
	{
		IXRCoreHandInterface::Execute_SecondaryInputAction(XRCoreHand, InAxisValue);
	}
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