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

    MotionControllerRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MotionControllerRoot"));
    MotionControllerRoot->SetupAttachment(RootComponent);

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

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tick - Interpolate on remote clients towards replicated transform data
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void AXRCoreHand::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsLocallyControlled)
    {
        // Current transform of MotionControllerRoot
        FVector CurrentLoc = MotionControllerRoot->GetComponentLocation();
        FQuat CurrentRot = MotionControllerRoot->GetComponentQuat();

        // Target transform from replicated data
        FVector TargetLoc = ReplicatedHandData.Location;
        FQuat TargetRot = ReplicatedHandData.Rotation;

        // Smooth interpolation
        FVector NewLoc = FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaTime, InterpolationSpeed);
        FQuat NewRot = FQuat::Slerp(CurrentRot, TargetRot, DeltaTime * InterpolationSpeed);

        MotionControllerRoot->SetWorldLocation(NewLoc);
        MotionControllerRoot->SetWorldRotation(NewRot);
    }
}

FXRCoreHandData AXRCoreHand::GetReplicatedHandData() const
{
    return ReplicatedHandData;
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
    LocallyControlled_PrimaryInputAxisValue = InAxisValue;
}

void AXRCoreHand::SecondaryInputAction_Implementation(float InAxisValue)
{
    LocallyControlled_SecondaryInputAxisValue = InAxisValue;
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

void AXRCoreHand::Client_UpdateXRCoreHandData_Implementation(const FXRCoreHandData& InXRCoreHandData)
{
    if (!bIsLocallyControlled)
    {
        ReplicatedHandData = InXRCoreHandData;
    }
}