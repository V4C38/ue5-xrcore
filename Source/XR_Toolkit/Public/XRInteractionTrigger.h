#pragma once

#include "CoreMinimal.h"
#include "XRInteractionComponent.h"
#include "XRInteractionTrigger.generated.h"

class UXRInteractionTrigger;

UENUM(BlueprintType)
enum class EXRTriggerType : uint8
{
    Single UMETA(DisplayName = "Single Activation"),
    SingleHover UMETA(DisplayName = "Single Trigger using hover"),
    Toggle UMETA(DisplayName = "Toggle - Single Activation"),
    ToggleHover UMETA(DisplayName = "Toggle using hover"),
    Hold UMETA(DisplayName = "Hold"),
    HoverHold UMETA(DisplayName = "Hold Interaction using hover"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTriggerStateChanged, UXRInteractionTrigger*, Sender, bool, TriggerState);

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
    * Get the state of the Trigger. Replicated.
    */
    UFUNCTION(BlueprintPure, Category = "XRCore|Interaction")
    bool GetTriggerState();

    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction")
    bool DefaultTriggerState = false;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------

protected:
    /**
    * Trigger Behaviors.
    */
    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction")
    EXRTriggerType TriggerType = EXRTriggerType::Hold;


    UFUNCTION(Server, Reliable)
    void Server_SetTriggerState(bool InTriggerState);

    /**
    * If this is a OneShot Trigger, the Interaction will be terminated after this duration (in seconds).
    */
    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction", meta = (EditCondition = "bInteractionDurationVisible"))
    float InteractionDuration = 0.1f;
    UPROPERTY()
    bool bInteractionDurationVisible = false;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

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
