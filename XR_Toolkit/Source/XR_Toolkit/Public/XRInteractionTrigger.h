#pragma once

#include "CoreMinimal.h"
#include "XRInteractionComponent.h"
#include "XRInteractionTrigger.generated.h"

class UXRInteractionTrigger;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTriggerStateChanged, UXRInteractionTrigger*, Sender, bool, State);

UCLASS(ClassGroup = (XRToolkit), meta = (BlueprintSpawnableComponent))
class XR_TOOLKIT_API UXRInteractionTrigger : public UXRInteractionComponent
{
	GENERATED_BODY()

public:
    UXRInteractionTrigger();
    virtual void BeginPlay() override;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------

    void StartInteraction(UXRInteractorComponent* InInteractor) override;
    void EndInteraction(UXRInteractorComponent* InInteractor) override;
    void HoverInteraction(UXRInteractorComponent* InInteractor, bool bInHoverState) override;

    UPROPERTY(BlueprintAssignable, Category = "XRToolkit|XR Interaction|Trigger")
    FOnTriggerStateChanged OnTriggerStateChanged;

    UFUNCTION(BlueprintCallable, Category = "XRToolkit|XR Interaction|Trigger")
    void SetTriggerState(bool InTriggerState);

    UFUNCTION(BlueprintPure, Category = "XRToolkit|XR Interaction|Trigger")
    bool GetTriggerState();

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------

protected:

    UPROPERTY(EditAnywhere, Category="XRToolkit|XR Interaction|Config")
    bool bResetAfterInteractionEnd = false;

    UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Config")
    bool bUseOnHover = false;

    UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Config")
    float CooldownDuration = 0.2f;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------

    UPROPERTY(ReplicatedUsing = OnRep_TriggerState)
    bool bTriggerState;

    UFUNCTION()
    void OnRep_TriggerState();

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------

private:
    UFUNCTION(BlueprintCallable, Category = "Cooldown")
    void RequestCooldown();
    FTimerHandle CooldownTimerHandle;

    UFUNCTION(BlueprintCallable, Category = "Cooldown")
    void EnableComponent();

};
