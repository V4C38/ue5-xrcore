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

    UPROPERTY(BlueprintAssignable, Category = "XR Interaction|Trigger")
    FOnTriggerStateChanged OnTriggerStateChanged;

    /**
    * Sets the state of the Trigger. Replicated, only executed when called on Server.
    */
    UFUNCTION(BlueprintCallable, Category = "XR Interaction|Trigger")
    void SetTriggerState(bool InTriggerState);

    /**
    * Get the state of the Trigger. Replicated.
    */
    UFUNCTION(BlueprintPure, Category = "XR Interaction|Trigger")
    bool GetTriggerState();

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------

protected:
    /**
    * Sets the TriggerState to HoverState.
    */
    UPROPERTY(EditAnywhere, Category = "XR Interaction|Config")
    bool bTriggerOnHover = false;

    UPROPERTY(EditAnywhere, Category = "XR Interaction|Config")
    float CooldownDuration = 0.05f;

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
