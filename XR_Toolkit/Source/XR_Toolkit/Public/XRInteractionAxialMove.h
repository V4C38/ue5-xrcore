#pragma once

#include "CoreMinimal.h"
#include "XRInteractionComponent.h"
#include "XRInteractionAxialMove.generated.h"

UENUM(BlueprintType)
enum class EAxialMoveTarget : uint8
{
    OwningActor UMETA(DisplayName = "Owning Actor"),
    ThisComponent UMETA(DisplayName = "AxialMove Component"),
};

UCLASS(ClassGroup = (XRToolkit), meta = (BlueprintSpawnableComponent))

class XR_TOOLKIT_API UXRInteractionAxialMove : public UXRInteractionComponent
{
	GENERATED_BODY()


public:
    UXRInteractionAxialMove();
	virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartInteraction(UXRInteractorComponent* InInteractor) override;
	void EndInteraction(UXRInteractorComponent* InInteractor) override;


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
    * Movement Speed.
    */
    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction")
    float MovementSpeed = 1.0f;



    UPROPERTY(ReplicatedUsing = OnRep_AxialMoveResult)
    FTransform AxialMoveResult;

    UFUNCTION()
    void OnRep_AxialMoveResult();

    UPROPERTY(Replicated)
    FTransform Origin;

private:
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

};
