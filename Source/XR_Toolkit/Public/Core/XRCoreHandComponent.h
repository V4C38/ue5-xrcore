#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "Core/XRCoreTypes.h"

#include "XRCoreHandComponent.generated.h"

class AXRCoreHand;

// ================================================================================================================================================================
// Spawns, configures and manages an XRCoreHand Actor - to be parented to a MotionControllerComponent
// ================================================================================================================================================================
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XR_TOOLKIT_API UXRCoreHandComponent : public USceneComponent, public IXRCoreHandInterface
{
	GENERATED_BODY()

public:	
	UXRCoreHandComponent();

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// API
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual UXRInteractorComponent* GetXRInteractor_Implementation() const override;
	virtual UXRLaserComponent* GetXRLaser_Implementation() const override;
	virtual void SetControllerHand_Implementation(EControllerHand InControllerHand) override;
	virtual EControllerHand GetControllerHand_Implementation() const override;

	virtual void PrimaryInputAction_Implementation(float InAxisValue) override;
	virtual void SecondaryInputAction_Implementation(float InAxisValue) override;
	virtual void SetIsHandtrackingActive_Implementation(bool InIsActive) override;
	virtual bool IsHandtrackingActive_Implementation() const override;

	virtual APawn* GetOwningPawn_Implementation() const override;
	virtual void Client_UpdateXRCoreHandReplicationData_Implementation(const FXRCoreHandReplicationData& InXRCoreHandData) override;


	/*
	* Return the XRCoreHand that is managed by this component.
	*/
	UFUNCTION(BlueprintPure, Category = "XRCore|XRCoreHand")
	AXRCoreHand* GetXRCoreHand() const;

	/*
	* Interval at which the locally controlled MotionController replicates it`s data to other Clients.
	* Note: Irrelevant for NetMode Standalone.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "XRCore|XRCoreHand", meta = (ClampMin = "0.0"))
	float ReplicationInterval = 0.1f;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

	/*
	* The HandType for this Hand.
	* Note: XRCore only uses Left, Right and Any. Use XRCoreUtilityFunctions::SimplifyHandType() for convenience.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "XRCore|XRCoreHand")
	EControllerHand ControllerHand = EControllerHand::AnyHand;

	/*
	* The subclass of XRCoreHand to be spawned by this component.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "XRCore|XRCoreHand")
	TSubclassOf<AXRCoreHand> XRCoreHandClass;

	// Maintained instance of XRCoreHand for this component
	UPROPERTY(ReplicatedUsing = OnRep_XRCoreHand)
	AXRCoreHand* XRCoreHand = nullptr;

	UFUNCTION(Server, Reliable)
	void Server_SpawnXRHand();
	UFUNCTION()
	void OnRep_XRCoreHand();

	// Movement Replication
	UFUNCTION(Server, Unreliable)
	void Server_UpdateHandData(FXRCoreHandReplicationData InXRCoreHandData);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_UpdateHandData(FXRCoreHandReplicationData InXRCoreHandData);

	UPROPERTY()
	bool bIsLocallyControlled = false;

	UPROPERTY()
	APawn* OwningPawn = nullptr;
	UPROPERTY()
	float PrimaryInputAxisValue = 0.0f;
	UPROPERTY()
	float SecondaryInputAxisValue = 0.0f;
};
