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
	void SetLaserActive(bool bInState);
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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	AActor* GetXRLaserActor() const;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXRLaserSpawned, UXRLaserComponent*, Sender, bool, SpawnResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXRLaserStateChanged, UXRLaserComponent*, Sender, bool, NewState);

UCLASS(Blueprintable, ClassGroup = (XRToolkit), meta = (BlueprintSpawnableComponent))
class XR_TOOLKIT_API UXRLaserComponent : public USceneComponent, public IXRLaserInterface, public IXRInteractionInterface
{
	GENERATED_BODY()

public:	
	UXRLaserComponent();

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	* Specifies the XRLaser Actor that will be spawned at BeginPlay. 
	* Must implement the IXRLaserInterface.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XRCore|XRLaser")
	TSubclassOf<AActor> XRLaserClass;

	/**
	 * Set the HandType for this Laser.
	*/
	UPROPERTY(EditAnywhere, Category = "XRCore|XRLaser")
	EControllerHand XRControllerHand = EControllerHand::AnyHand;


	UPROPERTY(BlueprintAssignable, Category = "XRCore|XRLaser")
	FOnXRLaserSpawned OnXRLaserSpawned;
	UPROPERTY(BlueprintAssignable, Category = "XRCore|XRLaser")
	FOnXRLaserStateChanged OnXRLaserStateChanged;


protected:
	virtual void BeginPlay() override;

	// XRLaser Interface
	virtual void SetLaserActive_Implementation(bool bInState) override;

	virtual bool IsLaserActive_Implementation() const override;
	virtual UXRInteractorComponent* GetXRInteractor_Implementation() const override;
	virtual EXRLaserState GetLaserState_Implementation() override;

	virtual void SetControllerHand_Implementation(EControllerHand InXRControllerHand) override;
	virtual EControllerHand GetControllerHand_Implementation() const override;

	virtual AActor* GetXRLaserActor_Implementation() const;


	UFUNCTION(Server, Reliable)
	void Server_SetLaserActive(bool bInActive);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetLaserActive(bool bInActive);


	// XRInteraction Interaface
	virtual void StartInteractionByPriority_Implementation(int32 InPriority = 0, EXRInteractionPrioritySelection InPrioritySelectionCondition = EXRInteractionPrioritySelection::LowerEqual) override;
	virtual void StopInteractionByPriority_Implementation(int32 InPriority = 5, EXRInteractionPrioritySelection InPrioritySelectionCondition = EXRInteractionPrioritySelection::HigherEqual) override;
	virtual void StopAllInteractions_Implementation(UXRInteractorComponent* InInteractor) override;

	UFUNCTION(Server, Reliable)
	void Server_RequestInteraction(int32 InID, int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RequestInteraction(int32 InID, int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition);



	UFUNCTION()
	bool SpawnXRLaserActor();
	UFUNCTION()
	void OnRep_XRLaserActor();
	UPROPERTY(ReplicatedUsing = OnRep_XRLaserActor)
	AActor* XRLaserActor = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_IsLaserActive)
	bool bIsLaserActive = false;
	UFUNCTION()
	void OnRep_IsLaserActive();
};
