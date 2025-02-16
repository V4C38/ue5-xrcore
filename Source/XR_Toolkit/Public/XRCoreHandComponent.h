#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "XRCoreHandComponent.generated.h"

class AXRCoreHand;

// -------------------------------------------------------------------------------------------------------------------------------------
// Struct to hold all replicated data
// -------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FXRCoreHandData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FQuat Rotation = FQuat::Identity;

	UPROPERTY(BlueprintReadWrite)
	float PrimaryInputAxis = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float SecondaryInputAxis = 0.0f;
};


// -------------------------------------------------------------------------------------------------------------------------------------
// Hand Interface
// -------------------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, BlueprintType)
class UXRCoreHandInterface : public UInterface
{
	GENERATED_BODY()
};


class IXRCoreHandInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	UXRInteractorComponent* GetXRInteractor() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	UXRLaserComponent* GetXRLaser() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	void SetControllerHand(EControllerHand InControllerHand);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	EControllerHand GetControllerHand() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	void PrimaryInputAction(float InAxisValue);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	void SecondaryInputAction(float InAxisValue);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	void SetIsHandtrackingActive(bool InIsActive);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	bool IsHandtrackingActive() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	APawn* GetOwningPawn() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	void Client_UpdateXRCoreHandData(const FXRCoreHandData& InXRCoreHandData);

};


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

	/*
	* Interval at which the locally controlled MotionController replicates it`s data to other Clients
	*/
	UPROPERTY(EditDefaultsOnly, Category = "XRCore|XRCoreHand")
	float ReplicationInterval = 0.1f;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


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
	virtual void Client_UpdateXRCoreHandData_Implementation(const FXRCoreHandData& InXRCoreHandData) override;

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

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Movement Replication
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(Server, Unreliable)
	void Server_UpdateHandData(FXRCoreHandData InXRCoreHandData);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_UpdateHandData(FXRCoreHandData InXRCoreHandData);


	UFUNCTION(Server, Reliable)
	void Server_SpawnXRHand();
	UFUNCTION()
	void OnRep_XRCoreHand();

	UPROPERTY()
	APawn* OwningPawn = nullptr;
	UPROPERTY()
	bool bIsLocallyControlled = false;

	UPROPERTY()
	float PrimaryInputAxisValue = 0.0f;
	UPROPERTY()
	float SecondaryInputAxisValue = 0.0f;
};
