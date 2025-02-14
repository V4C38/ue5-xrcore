#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "XRCoreHandComponent.generated.h"

class AXRCoreHand;

// -------------------------------------------------------------------------------------------------------------------------------------
// Spawns, configures and manages an XRCoreHand Actor - to be parented to a MotionControllerComponent
// -------------------------------------------------------------------------------------------------------------------------------------
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XR_TOOLKIT_API UXRCoreHandComponent : public USceneComponent, public IXRCoreHandInterface
{
	GENERATED_BODY()

public:	
	UXRCoreHandComponent();

	/*
	* Return the XRCoreHand that is managed by this component.
	*/
	UFUNCTION(BlueprintPure, Category = "XRCore|XRCoreHand")
	AXRCoreHand* GetXRCoreHand() const;

	// XRCoreHand Interface Implementations
	virtual UXRInteractorComponent* GetXRInteractor_Implementation() const override;
	virtual UXRLaserComponent* GetXRLaser_Implementation() const override;
	virtual void SetControllerHand_Implementation(EControllerHand InControllerHand) override;
	virtual EControllerHand GetControllerHand_Implementation() const override;
	virtual void PrimaryInputAction_Implementation(float InAxisValue) override;
	virtual void SecondaryInputAction_Implementation(float InAxisValue) override;
	virtual void SetIsHandtrackingActive_Implementation(bool InIsActive) override;
	virtual bool IsHandtrackingActive_Implementation() const override;
	virtual APawn* GetOwningPawn_Implementation() const override;

protected:
	virtual void BeginPlay() override;

	/*
	* The HandType for this Hand.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "XRCore|XRCoreHand")
	EControllerHand ControllerHand = EControllerHand::AnyHand;

	/*
	* The subclass of XRCoreHand to be spawned by this component.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "XRCore|XRCoreHand")
	TSubclassOf<AXRCoreHand> XRCoreHandClass;

	UPROPERTY(ReplicatedUsing = OnRep_XRCoreHand)
	AXRCoreHand* XRCoreHand = nullptr;

	UFUNCTION(Server, Reliable)
	void Server_SpawnXRHand();
	UFUNCTION()
	void OnRep_XRCoreHand();

	UPROPERTY()
	APawn* OwningPawn = nullptr;
};
