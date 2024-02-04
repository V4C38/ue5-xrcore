#pragma once

#include "CoreMinimal.h"
#include "XRInteractionComponent.h"
#include "XRInteractionAxialMove.generated.h"

class UXRInteractionAxialMove;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMovementUpdate, UXRInteractionAxialMove*, Sender, float, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementLimitReached, UXRInteractionAxialMove*, Sender);

UENUM(BlueprintType)
enum class EAxialMoveTarget : uint8
{
    OwningActor UMETA(DisplayName = "Owning Actor"),
    ThisComponent UMETA(DisplayName = "AxialMove Component"),
};

UCLASS(ClassGroup = (XRCore), meta = (BlueprintSpawnableComponent))

class XRCORE_API UXRInteractionAxialMove : public UXRInteractionComponent
{
	GENERATED_BODY()


public:
    UXRInteractionAxialMove();
	virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartInteraction(UXRInteractorComponent* InInteractor) override;
	void EndInteraction(UXRInteractorComponent* InInteractor) override;

    UPROPERTY(BlueprintAssignable, Category = "XRCore|Interaction")
    FOnMovementUpdate OnMovementUpdate;
    UPROPERTY(BlueprintAssignable, Category = "XRCore|Interaction")
    FOnMovementLimitReached OnMovementLimitReached;

    UFUNCTION(BlueprintPure, Category = "XRCore|Interaction")
    float GetMovementProgress() const;

protected:
    /**
    * Determine whether to move the Actor owning this component, or this component only.
    */
    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction")
    EAxialMoveTarget ObjectToMove = EAxialMoveTarget::OwningActor;

    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|AxisConstraints")
    bool bConstrainX = false;

    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|AxisConstraints")
    bool bConstrainY = false;

    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|AxisConstraints")
    bool bConstrainZ = false;

    /**
    * The maximum Distance allowed to travel from the origin location.
    */
    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction")
    float DistanceLimit = 1.0f;
    /**
    * Movement Speed. A Value of 0.0 will disable movemnt interpolation.
    */
    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction")
    float MovementSpeed = 0.0f;



    UPROPERTY(ReplicatedUsing = OnRep_AxialMoveResult)
    FTransform AxialMoveResult = FTransform();

    UFUNCTION()
    void OnRep_AxialMoveResult();

    UPROPERTY(Replicated)
    FTransform RootTransform = FTransform();

    UPROPERTY()
    FTransform InteractorOrigin = FTransform();

protected:
   

private:
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

};
