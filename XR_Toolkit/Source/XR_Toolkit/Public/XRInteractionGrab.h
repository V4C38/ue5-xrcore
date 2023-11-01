#pragma once

#include "CoreMinimal.h"
#include "XRInteractionComponent.h"
#include "XRInteractionGrab.generated.h"

class UXRReplicatedPhysicsComponent;

UCLASS(ClassGroup = (XRToolkit), meta = (BlueprintSpawnableComponent))
class XR_TOOLKIT_API UXRInteractionGrab : public UXRInteractionComponent
{
	GENERATED_BODY()

public:
    UXRInteractionGrab();
    virtual void BeginPlay() override;

    void StartInteraction(UXRInteractorComponent* InInteractor) override;
    void EndInteraction(UXRInteractorComponent* InInteractor) override;


    /**
    * Enable Replicated Physics. 
    * Actor MUST have a UStaticMeshComponent as the RootComponent. 
    */
    UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Config")
    bool bEnablePhysics = true;
    /**
    * Enable Replicated Physics.
    * Actor MUST have a UStaticMeshComponent as the RootComponent.
    */
    UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Config")
    FName PhysicsTag = "XRPhysics";


protected:
    UPROPERTY()
    UXRReplicatedPhysicsComponent* XRReplicatedPhysicsComponent = nullptr;

    UFUNCTION()
    void AttachOwningActorToXRInteractor(UXRInteractorComponent* InInteractor);

    UFUNCTION()
    void DetachOwningActorFromXRInteractor();

    UFUNCTION()
    void PhysicsGrab(UXRInteractorComponent* InInteractor);
    UFUNCTION()
    void PhysicsUngrab(UXRInteractorComponent* InInteractor);

private:
    UFUNCTION()
    void InitializePhysics();
};
