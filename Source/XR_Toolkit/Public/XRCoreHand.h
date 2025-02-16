#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XRCoreHandComponent.h"
#include "XRCoreHand.generated.h"

class UXRInteractorComponent;
class UXRLaserComponent;

// -------------------------------------------------------------------------------------------------------------------------------------
// Hand Actor representing the Motion Controller used for interacting with the world
// -------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class XR_TOOLKIT_API AXRCoreHand : public AActor, public IXRCoreHandInterface
{
    GENERATED_BODY()

public:
    AXRCoreHand();
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintPure, Category = "XRCore|XRCoreHand")
    FXRCoreHandData GetReplicatedHandData() const;


    // XRCoreHand Interface Implementations
    virtual UXRInteractorComponent* GetXRInteractor_Implementation() const override;
    virtual UXRLaserComponent* GetXRLaser_Implementation() const override;
    virtual void SetControllerHand_Implementation(EControllerHand InControllerHand) override;
    virtual EControllerHand GetControllerHand_Implementation() const override;
    virtual void PrimaryInputAction_Implementation(float InAxisValue) override;
    virtual void SecondaryInputAction_Implementation(float InAxisValue) override;
    virtual void SetIsHandtrackingActive_Implementation(bool InIsActive) override;
    virtual bool IsHandtrackingActive_Implementation() const override;
    virtual APawn* GetOwningPawn_Implementation() const override;
    virtual void Client_UpdateXRCoreHandData_Implementation(const FXRCoreHandData& InXRCoreHandData) override;


    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    EControllerHand ControllerHand = EControllerHand::AnyHand;

    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    USceneComponent* RootSceneComponent;

    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    USceneComponent* MotionControllerRoot;

    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    UXRInteractorComponent* XRInteractor;

    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    UXRLaserComponent* XRLaserComponent;

    UPROPERTY()
    APawn* OwningPawn = nullptr;
    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    bool bIsLocallyControlled = false;

protected:
    virtual void BeginPlay() override;

    UPROPERTY()
    FXRCoreHandData ReplicatedHandData;

    UPROPERTY(EditDefaultsOnly, Category = "XRCore|XRCoreHand")
    float InterpolationSpeed = 10.f;

    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    bool bIsHandtrackingActive = false;

    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    float LocallyControlled_PrimaryInputAxisValue = 0.0f;
    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    float LocallyControlled_SecondaryInputAxisValue = 0.0f;
};
