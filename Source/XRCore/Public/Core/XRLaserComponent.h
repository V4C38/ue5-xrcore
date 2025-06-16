#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "Core/XRCoreTypes.h"
#include "Interactions/XRInteractorComponent.h"

#include "XRLaserComponent.generated.h"

class UXRLaserComponent;
class UXRInteractorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXRLaserSpawned, UXRLaserComponent*, Sender, bool, SpawnResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXRLaserStateChanged, UXRLaserComponent*, Sender, bool, NewState);

// ================================================================================================================================================================
// Manages a XRLaser Actor (must implement IXRLaserInterface), interfaces with the InteractionSystem and UMG elements 
// ================================================================================================================================================================
UCLASS(Blueprintable, ClassGroup = (XRToolkit), meta = (BlueprintSpawnableComponent))
class XRCORE_API UXRLaserComponent : public USceneComponent, public IXRLaserInterface, public IXRInteractionInterface
{
	GENERATED_BODY()

public:	
	UXRLaserComponent();

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	* Specifies the XRLaser Actor that will be spawned at BeginPlay and maintained by this component.
	* Must implement the IXRLaserInterface.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XRCore|XRLaser")
	TSubclassOf<AActor> XRLaserClass;

	/**
	 * Set the HandType for this Laser.
	*/
	UPROPERTY(EditAnywhere, Category = "XRCore|XRLaser")
	EControllerHand XRControllerHand = EControllerHand::AnyHand;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// API
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	// XRLaser Interaface
	virtual void SetLaserActive_Implementation(bool bInState) override;
	virtual bool IsLaserActive_Implementation() const override;

	virtual UXRInteractorComponent* GetXRInteractor_Implementation() const override;
	virtual EXRLaserState GetLaserState_Implementation() override;

	virtual void SetControllerHand_Implementation(EControllerHand InXRControllerHand) override;
	virtual EControllerHand GetControllerHand_Implementation() const override;

	virtual AActor* GetXRLaserActor_Implementation() const;
	
	// XRInteraction Interaface
	virtual void StartInteractionByPriority_Implementation(int32 InPriority = 0, EXRInteractionPrioritySelection InPrioritySelectionCondition = EXRInteractionPrioritySelection::LowerEqual) override;
	virtual void StopInteractionByPriority_Implementation(int32 InPriority = 5, EXRInteractionPrioritySelection InPrioritySelectionCondition = EXRInteractionPrioritySelection::HigherEqual) override;
	virtual void StopAllInteractions_Implementation(UXRInteractorComponent* InInteractor) override;

	UPROPERTY(BlueprintAssignable, Category = "XRCore|XRLaser")
	FOnXRLaserSpawned OnXRLaserSpawned;
	UPROPERTY(BlueprintAssignable, Category = "XRCore|XRLaser")
	FOnXRLaserStateChanged OnXRLaserStateChanged;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_SetLaserActive(bool bInActive);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetLaserActive(bool bInActive);


	UFUNCTION(Server, Reliable)
	void Server_RequestInteraction(int32 InID, int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RequestInteraction(int32 InID, int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition);

	// Laser State
	UPROPERTY(ReplicatedUsing = OnRep_IsLaserActive)
	bool bIsLaserActive = false;
	UFUNCTION()
	void OnRep_IsLaserActive();

	// Laser Actor
	UFUNCTION()
	bool SpawnXRLaserActor();
	UFUNCTION()
	void OnRep_XRLaserActor();
	UPROPERTY(ReplicatedUsing = OnRep_XRLaserActor)
	AActor* XRLaserActor = nullptr;
};
