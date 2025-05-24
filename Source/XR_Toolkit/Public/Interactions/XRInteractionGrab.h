#pragma once

#include "CoreMinimal.h"

#include "Interactions/XRInteractionComponent.h"

#include "XRInteractionGrab.generated.h"

class UXRReplicatedPhysicsComponent;

// ================================================================================================================================================================
// Grab Interaction, physics or kinematics based
// ================================================================================================================================================================

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
    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction")
    bool bEnablePhysics = true;
    /**
    * Optionally specify additional components in the actor with this tag that should be Physics enabled and replicated.
    * Actor MUST have a UStaticMeshComponent as the RootComponent (this is cached even without the tag).
    */
    UPROPERTY(EditAnywhere, Category = "XRCore|Interaction")
    FName PhysicsTag = "XRPhysics";

    /**
    * Return the Component handling PhysicsReplication for this Interaction. If bEnablePhysics is true, this component will be spawned at BeginPlay().
    */
    UFUNCTION(BlueprintPure, Category = "XRCore|Interaction")
    UXRReplicatedPhysicsComponent* GetPhysicsReplicationComponent();

    /**
    * Does this XRInteractionGrab have Physics enabled. 
    */
    UFUNCTION(BlueprintPure, Category = "XRCore|Interaction")
    bool HasPhysicsEnabled() const;

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


    /**
    * This is used to ensure proper LateJoining for non-physics grab
    */
    UPROPERTY(ReplicatedUsing = OnRep_GrabActorTransform)
    FTransform GrabActorTransform;
    UFUNCTION()
    void OnRep_GrabActorTransform(); 

    UFUNCTION(Server, Reliable)
    void Server_UpdateGrabActorTransform();

    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:
    UFUNCTION()
    void InitializePhysics();
};
