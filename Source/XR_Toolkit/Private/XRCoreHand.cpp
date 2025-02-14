#include "XRCoreHand.h"
#include "XRInteractorComponent.h"
#include "XRLaserComponent.h"

AXRCoreHand::AXRCoreHand()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
    bReplicates = true;

    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    RootComponent = RootSceneComponent;

    XRInteractor = CreateDefaultSubobject<UXRInteractorComponent>(TEXT("XRInteractor"));
    XRInteractor->SetupAttachment(RootSceneComponent);

    XRLaserComponent = CreateDefaultSubobject<UXRLaserComponent>(TEXT("XRLaserComponent"));
    XRLaserComponent->SetupAttachment(RootSceneComponent);
}

void AXRCoreHand::BeginPlay()
{
    Super::BeginPlay();

    if (XRLaserComponent && XRLaserComponent->GetClass()->ImplementsInterface(UXRLaserInterface::StaticClass()))
    {
        IXRLaserInterface::Execute_SetControllerHand(XRLaserComponent, ControllerHand);
    }
}

void AXRCoreHand::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// XRCoreHand Interface Implementations
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

UXRInteractorComponent* AXRCoreHand::GetXRInteractor_Implementation() const
{
    return XRInteractor;
}

UXRLaserComponent* AXRCoreHand::GetXRLaser_Implementation() const
{
    return XRLaserComponent;
}

void AXRCoreHand::SetControllerHand_Implementation(EControllerHand InControllerHand)
{
    ControllerHand = InControllerHand;
    if (XRLaserComponent && XRLaserComponent->GetClass()->ImplementsInterface(UXRLaserInterface::StaticClass()))
    {
        IXRLaserInterface::Execute_SetControllerHand(XRLaserComponent, ControllerHand);
    }
}

EControllerHand AXRCoreHand::GetControllerHand_Implementation() const
{
    return ControllerHand;
}

void AXRCoreHand::PrimaryInputAction_Implementation(float InAxisValue)
{
    // Handle primary input
}

void AXRCoreHand::SecondaryInputAction_Implementation(float InAxisValue)
{
    // Handle secondary input
}

void AXRCoreHand::SetIsHandtrackingActive_Implementation(bool InIsActive)
{
    bIsHandtrackingActive = InIsActive;
}

bool AXRCoreHand::IsHandtrackingActive_Implementation() const
{
    return bIsHandtrackingActive;
}

APawn* AXRCoreHand::GetOwningPawn_Implementation() const
{
    return OwningPawn;
}