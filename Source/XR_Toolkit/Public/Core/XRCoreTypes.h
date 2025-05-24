#pragma once

#include "CoreMinimal.h"

#include "XRCoreTypes.generated.h"

// ================================================================================================================================================================
// Types and Interfaces used throughout core components
// ================================================================================================================================================================

UENUM(BlueprintType)
enum class EXRStandard : uint8
{
	OpenXR UMETA(DisplayName = "OpenXR"),
	MetaXR UMETA(DisplayName = "MetaXR"),
};

USTRUCT(BlueprintType)
struct FLocationAndRotation
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XRCore|Utilities")
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XRCore|Utilities")
	FRotator Rotation;

	FLocationAndRotation()
		: Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator) {
	}
};

// -------------------------------------------------------------------------------------------------------------------------------------
// Hand
// -------------------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ESimplifiedControllerHand : uint8
{
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
	Any UMETA(DisplayName = "Any Hand"),
};

USTRUCT(BlueprintType)
struct FXRCoreHandReplicationData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FQuat Rotation = FQuat::Identity;

	// Input data (for example trigger or pinch amount), used for animation on remotes
	UPROPERTY(BlueprintReadWrite)
	float PrimaryInputAxis = 0.0f;

	// Input data (for example grip), used for animation on remotes
	UPROPERTY(BlueprintReadWrite)
	float SecondaryInputAxis = 0.0f;
};

UINTERFACE(MinimalAPI, BlueprintType)
class UXRCoreHandInterface : public UInterface
{
	GENERATED_BODY()
};

class IXRCoreHandInterface
{
	GENERATED_BODY()

public:

	/**
	 * Get this Hands XRInteractorComponent. (This is always expected to be valid)
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	UXRInteractorComponent* GetXRInteractor() const;

	/**
	 * Get this Hands XRLaserComponent. (This is always expected to be valid)
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	UXRLaserComponent* GetXRLaser() const;

	/**
	 * Set this Hands ControllerHand.
	 * Note: XRCore only uses Left, Right and Any unless you specify other types.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	void SetControllerHand(EControllerHand InControllerHand);

	/**
	 * Set this Hands ControllerHand.
	 * Note: XRCore only uses Left, Right and Any unless you specify other types.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	EControllerHand GetControllerHand() const;

	/**
	 * Propagate the current InputAxis value to the Hand actor to process.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	void PrimaryInputAction(float InAxisValue);

	/**
	 * Propagate the current InputAxis value to the Hand actor to process.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	void SecondaryInputAction(float InAxisValue);

	/**
	 * Set HandTracking active / inactive for the Hand actor to process this state.
	 * Note: HandTracking is currently not fully supported by XRCore but all OpenXR values are passed to the Hand actor regardless.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	void SetIsHandtrackingActive(bool InIsActive);

	/**
	 * Retrieve if HandTracking is currently active on this Hand actor.
	 * Note: HandTracking is currently not fully supported by XRCore but all OpenXR values are passed to the Hand actor regardless.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	bool IsHandtrackingActive() const;

	/**
	 * Get the pawn owning this Hand actor.
	 * Note: destroying the pawn will NOT remove the Hand actor automatically!
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	APawn* GetOwningPawn() const;


	/**
	 * Set the transform and input data for this Hand actor. This will cause the server to replicate it to all other clients.
	 * Note: irrelevant for NetMode Standalone.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRCoreHand")
	void Client_UpdateXRCoreHandReplicationData(const FXRCoreHandReplicationData& InXRCoreHandReplicationData);
};


// -------------------------------------------------------------------------------------------------------------------------------------
// Laser
// -------------------------------------------------------------------------------------------------------------------------------------
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
	/**
	 * Enables, disables this laser.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	void SetLaserActive(bool bInState);

	/**
	 * Get the current activation state of this laser.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	bool IsLaserActive() const;

	/**
	 * Propagate the current InputAxis value to the Hand actor to process.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")

	/**
	 * Get the laser state with more information than active/inactive.
	 * Seeking: not hovering over an interaction
	 * Hovering: hovering an interaction that is enabled and available
	 * Interacting: XRLaser actors XRInteractor is currently interacting
	*/
	EXRLaserState GetLaserState();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")

	/**
	 * Get this laser actors Interactor. (This is expected to be valid if using the XRCoreLaser actor)
	*/
	UXRInteractorComponent* GetXRInteractor() const;

	/**
	 * Set the ControllerHand associated with this laser. XRCore generally only uses Left, Right, Any.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	void SetControllerHand(EControllerHand InXRControllerHand);	
	
	/**
	 * Get the ControllerHand associated with this laser. XRCore generally only uses Left, Right, Any.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	EControllerHand GetControllerHand() const;

	/**
	* Set the component that both the XRLaserComponent and XRLaser (actor) will attach to (forward is also forward for the laser) 
	* Note: in the default XRCore implementation, this is a SceneComponent on the XRHand actor
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	void ProvideAttachmentRoot(USceneComponent* InComponent);

	/**
	* Only relevant for XRLaserComponent: return the XRLaser actor maintained by this component.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRLaser")
	AActor* GetXRLaserActor() const;
};
