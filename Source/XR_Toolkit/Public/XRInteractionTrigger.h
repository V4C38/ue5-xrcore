#pragma once

#include "CoreMinimal.h"
#include "XRInteractionComponent.h"
#include "XRInteractionTrigger.generated.h"

class UXRInteractionTrigger;
class UXRInteractorComponent;

UENUM(BlueprintType)
enum class EXRInteractionTriggerBehavior : uint8
{
    Trigger UMETA(DisplayName = "Trigger"),
    Toggle UMETA(DisplayName = "Toggle"),
    Hold UMETA(DisplayName = "Hold"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTriggerStateChanged, UXRInteractionTrigger*, Sender, bool, TriggerState, UXRInteractorComponent*, Interactor);

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

    UPROPERTY(BlueprintAssignable, Category = "XRCore|Interaction")
    FOnTriggerStateChanged OnTriggerStateChanged;

    /**
    * Set the state of the Trigger. Replicated if called with authority.
    * @param InInteractor - Optional. Will be propagated to the FOnTriggerStateChanged Delegate as Interactor.
    */
    UFUNCTION(BlueprintCallable, Category = "XRCore|Interaction")
    void SetTriggerState(bool InTriggerState, UXRInteractorComponent* InInteractor);

    /**
    * Get the state of the Trigger.
    */
    UFUNCTION(BlueprintPure, Category = "XRCore|Interaction")
    bool GetTriggerState() const;

    /**
    * Set the Triggers Behavior.
    * Not Replicated - set this on all Clients/Server manually as it is assumed that the OwningActor does not have authority.
    * Trigger: Single Interaction that ends and returns to DefaultState after InteractionDuration
    * Toggle: Single Interaction that ends after InteractionDuration
    * Hold: Interaction returns to DefaultState after the last XRInteractor stops interacting
    */
    UFUNCTION(BlueprintCallable, Category = "XRCore|Interaction")
    void SetTriggerBehavior(EXRInteractionTriggerBehavior InTriggerBehavior);
    /**
    * Get the Triggers Behavior
    * Trigger: Single Interaction that ends and returns to DefaultState after InteractionDuration
    * Toggle: Single Interaction that ends after InteractionDuration
    * Hold: Interaction returns to DefaultState after the last XRInteractor stops interacting
    */
    UFUNCTION(BlueprintPure, Category = "XRCore|Interaction")
    EXRInteractionTriggerBehavior GetTriggerBehavior() const;

    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction")
    bool DefaultTriggerState = false;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------

protected:
    /**
    * Trigger Behavior
    * Trigger: Single Interaction that ends and returns to DefaultState after InteractionDuration
    * Toggle: Single Interaction that ends after InteractionDuration
    * Hold: Interaction returns to DefaultState after the last XRInteractor stops interacting
    */
    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction")
    EXRInteractionTriggerBehavior TriggerBehavior = EXRInteractionTriggerBehavior::Trigger;

    UFUNCTION(Server, Reliable)
    void Server_SetTriggerState(bool InTriggerState, UXRInteractorComponent* InInteractor);

    /**
    * If this is a OneShot Trigger, the Interaction will be terminated after this duration (in seconds).
    */
    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction")
    float InteractionDuration = 0.25f;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------

    UPROPERTY(ReplicatedUsing = OnRep_TriggerState)
    bool bTriggerState;

    UFUNCTION()
    void OnRep_TriggerState();

    UFUNCTION(BlueprintCallable, Category = "XRCore|Interaction")
    void EndInteractionAfterTimer();
    UFUNCTION(BlueprintCallable, Category = "XRCore|Interaction")
    void RequestInteractionTermination();
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------

private:
    FTimerHandle CooldownTimerHandle;
};
