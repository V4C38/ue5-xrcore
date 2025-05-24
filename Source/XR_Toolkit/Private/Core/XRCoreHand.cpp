#include "Core/XRCoreHand.h"
#include "Core/XRLaserComponent.h"
#include "Interactions/XRInteractorComponent.h"

AXRCoreHand::AXRCoreHand()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
    bReplicates = true;

    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    RootComponent = RootSceneComponent;

    MotionControllerRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MotionControllerRoot"));
    MotionControllerRoot->SetupAttachment(RootComponent);
    MotionControllerRoot->bEditableWhenInherited = true;

    XRInteractor = CreateDefaultSubobject<UXRInteractorComponent>(TEXT("XRInteractor"));
    XRInteractor->SetupAttachment(MotionControllerRoot);
    XRInteractor->bEditableWhenInherited = true;

    XRLaserComponent = CreateDefaultSubobject<UXRLaserComponent>(TEXT("XRLaserComponent"));
    XRLaserComponent->SetupAttachment(MotionControllerRoot);
    XRLaserComponent->bEditableWhenInherited = true;
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

    // Exclude locally controlling client as the HandActor is attached to the MotionController here
    if (!bIsLocallyControlled)
    {
        // Current transform of MotionControllerRoot
        FVector CurrentLoc = GetActorLocation();
        FQuat CurrentRot = GetActorQuat();

        // Target transform from replicated data
        FVector TargetLoc = ReplicatedHandData.Location;
        FQuat TargetRot = ReplicatedHandData.Rotation;

        // Smooth interpolation
        FVector NewLoc = FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaTime, InterpolationSpeed);
        FQuat NewRot = FQuat::Slerp(CurrentRot, TargetRot, DeltaTime * InterpolationSpeed);

        SetActorLocationAndRotation(NewLoc, NewRot);
    }
}

FXRCoreHandReplicationData AXRCoreHand::GetReplicatedHandData() const
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

void AXRCoreHand::Client_UpdateXRCoreHandReplicationData_Implementation(const FXRCoreHandReplicationData& InXRCoreHandData)
{
    if (!bIsLocallyControlled)
    {
        ReplicatedHandData = InXRCoreHandData;
    }
}