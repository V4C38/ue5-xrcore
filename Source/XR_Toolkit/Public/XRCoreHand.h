#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XRCoreHand.generated.h"

class UXRInteractorComponent;
class UXRLaserComponent;

UINTERFACE(MinimalAPI, BlueprintType)
class UXRCoreHandInterface : public UInterface
{
    GENERATED_BODY()
};

class IXRCoreHandInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
    UXRInteractorComponent* GetXRInteractor() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
    UXRLaserComponent* GetXRLaser() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
    void SetControllerHand(EControllerHand InControllerHand);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
    EControllerHand GetControllerHand() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
    void PrimaryInputAction(float InAxisValue);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
    void SecondaryInputAction(float InAxisValue);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
    void SetIsHandtrackingActive(bool InIsActive);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
    bool IsHandtrackingActive() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
    APawn* GetOwningPawn() const;
};

// -------------------------------------------------------------------------------------------------------------------------------------
// Hand used for Interacting with the world
// -------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class XR_TOOLKIT_API AXRCoreHand : public AActor, public IXRCoreHandInterface
{
    GENERATED_BODY()

public:
    AXRCoreHand();
    virtual void Tick(float DeltaTime) override;

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


    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    EControllerHand ControllerHand = EControllerHand::AnyHand;

    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    USceneComponent* RootSceneComponent;

    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    UXRInteractorComponent* XRInteractor;

    UPROPERTY(BlueprintReadWrite, Category = "XRCore|XRCoreHand")
    UXRLaserComponent* XRLaserComponent;

    UPROPERTY()
    bool bIsHandtrackingActive = false;

    UPROPERTY()
    APawn* OwningPawn = nullptr;

protected:
    virtual void BeginPlay() override;

};
