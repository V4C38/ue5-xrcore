#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "XRLaserComponent.generated.h"

class UXRLaserComponent;
class UXRInteractorComponent;

UENUM(BlueprintType)
enum class EXRLaserState : uint8
{
	Inactive UMETA(DisplayName = "Inactive"),
	Seeking UMETA(DisplayName = "Seeking"),
	Hovering UMETA(DisplayName = "Hovering"),
	Interacting UMETA(DisplayName = "Interacting"),
};

UINTERFACE(MinimalAPI, BlueprintType)
class UXRLaserInterface : public UInterface
{
	GENERATED_BODY()
};

class IXRLaserInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	void SetLaserActive();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	void SetLaserInactive();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	bool IsLaserActive() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	EXRLaserState GetLaserState();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	UXRInteractorComponent* GetXRInteractor() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	void SetControllerHand(EControllerHand InXRControllerHand);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	EControllerHand GetControllerHand() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	void ProvideAttachmentRoot(USceneComponent* InComponent);
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXRLaserSpawned, UXRLaserComponent*, Sender, bool, SpawnResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXRLaserStateChanged, UXRLaserComponent*, Sender, EXRLaserState, NewState);

UCLASS(Blueprintable, ClassGroup = (XRToolkit), meta = (BlueprintSpawnableComponent))
class XR_TOOLKIT_API UXRLaserComponent : public USceneComponent, public IXRLaserInterface
{
	GENERATED_BODY()

public:	
	UXRLaserComponent();

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	* Specifies the XRLaser Actor that will be spawned at BeginPlay. 
	* This MUST implement the IXRLaserInterface to function.
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
	UPROPERTY(BlueprintAssignable, Category = "XRCore|XRLaser")
	FOnXRLaserSpawned OnXRLaserSpawned;
	UPROPERTY(BlueprintAssignable, Category = "XRCore|XRLaser")
	FOnXRLaserStateChanged OnXRLaserStateChanged;


	virtual void SetLaserActive_Implementation() override;
	virtual void SetLaserInactive_Implementation() override;

	virtual bool IsLaserActive_Implementation() const override;
	virtual UXRInteractorComponent* GetXRInteractor_Implementation() const override;
	virtual EXRLaserState GetLaserState_Implementation() override;

	virtual void SetControllerHand_Implementation(EControllerHand InXRControllerHand) override;
	virtual EControllerHand GetControllerHand_Implementation() const override;

	/**
	 * Returns the XRLaser Actor that is spawned at BeginPlay and managed by this component.
	*/
	UFUNCTION(BlueprintPure, Category = "XRCore|XRLaser")
	AActor* GetXRLaserActor() const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Blueprintcallable, Server, Reliable, Category = "XRCore|XRLaser")
	void Server_SetLaserActive(bool bInActive);
	UPROPERTY(ReplicatedUsing = OnRep_XRLaserActive)
	bool XRLaserActive = false;
	UFUNCTION()
	void OnRep_XRLaserActive();


	UPROPERTY(ReplicatedUsing = OnRep_XRLaserActor)
	AActor* XRLaserActor = nullptr;
	UFUNCTION()
	bool SpawnXRLaserActor();
	UFUNCTION()
	void OnRep_XRLaserActor();


	EXRLaserState CurrentLaserState = EXRLaserState::Inactive;
};
