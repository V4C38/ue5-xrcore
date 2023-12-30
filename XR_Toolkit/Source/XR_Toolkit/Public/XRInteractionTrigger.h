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

    UPROPERTY(BlueprintAssignable, Category = "XRCore|Interaction")
    FOnTriggerStateChanged OnTriggerStateChanged;

    /**
    * Sets the state of the Trigger. Replicated, only executed when called on Server.
    */
    UFUNCTION(BlueprintCallable, Category = "XRCore|Interaction")
    void Server_SetTriggerState(bool InTriggerState);

    /**
    * Get the state of the Trigger. Replicated.
    */
    UFUNCTION(BlueprintPure, Category = "XRCore|Interaction")
    bool GetTriggerState();

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------

protected:
    /**
    * Sets the TriggerState to HoverState.
    */
    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction")
    bool bTriggerOnHover = false;

    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction")
    float CooldownDuration = 0.05f;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------

    UPROPERTY(ReplicatedUsing = OnRep_TriggerState)
    bool bTriggerState;

    UFUNCTION()
    void OnRep_TriggerState();

    UFUNCTION(BlueprintCallable, Category = "XRCore|Interaction")
    void RequestCooldown();
    UFUNCTION(BlueprintCallable, Category = "XRCore|Interaction")
    void EnableComponent();
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------

private:
    FTimerHandle CooldownTimerHandle;
};
