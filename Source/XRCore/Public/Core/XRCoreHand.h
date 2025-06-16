#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Core/XRCoreHandComponent.h"

#include "XRCoreHand.generated.h"

class UXRInteractorComponent;
class UXRLaserComponent;

// ================================================================================================================================================================
// Replicated Hand Actor, attached to the MotionController and used for InteractionSystem and Input
// ================================================================================================================================================================
UCLASS()
class XRCORE_API AXRCoreHand : public AActor, public IXRCoreHandInterface
{
    GENERATED_BODY()

public:
    AXRCoreHand();
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintPure, Category = "XRCore|XRCoreHand")
    FXRCoreHandReplicationData GetReplicatedHandData() const;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------
    // API
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------
    virtual UXRInteractorComponent* GetXRInteractor_Implementation() const override;
    virtual UXRLaserComponent* GetXRLaser_Implementation() const override;
    virtual void SetControllerHand_Implementation(EControllerHand InControllerHand) override;
    virtual EControllerHand GetControllerHand_Implementation() const override;

    virtual void PrimaryInputAction_Implementation(float InAxisValue) override;
    virtual void SecondaryInputAction_Implementation(float InAxisValue) override;
    virtual void SetIsHandtrackingActive_Implementation(bool InIsActive) override;
    virtual bool IsHandtrackingActive_Implementation() const override;

    virtual APawn* GetOwningPawn_Implementation() const override;
    virtual void Client_UpdateXRCoreHandReplicationData_Implementation(const FXRCoreHandReplicationData& InXRCoreHandData) override;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Default components 
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------
    UPROPERTY(VisibleDefaultsOnly, Category = "XRCore|XRCoreHand")
    USceneComponent* RootSceneComponent;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    USceneComponent* MotionControllerRoot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    UXRInteractorComponent* XRInteractor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    UXRLaserComponent* XRLaserComponent;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Config
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------
    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    EControllerHand ControllerHand = EControllerHand::AnyHand;

    UPROPERTY()
    APawn* OwningPawn = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    bool bIsLocallyControlled = false;

protected:
    virtual void BeginPlay() override;

    UPROPERTY()
    FXRCoreHandReplicationData ReplicatedHandData;

    UPROPERTY(EditDefaultsOnly, Category = "XRCore|XRCoreHand", meta = (ClampMin = "0.0"))
    float InterpolationSpeed = 10.f;

    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    bool bIsHandtrackingActive = false;

    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    float LocallyControlled_PrimaryInputAxisValue = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    float LocallyControlled_SecondaryInputAxisValue = 0.0f;
};
