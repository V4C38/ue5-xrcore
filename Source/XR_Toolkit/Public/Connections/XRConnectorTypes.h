#pragma once

#include "CoreMinimal.h"

#include "XRConnectorTypes.generated.h"

class UXRConnectorComponent;
class UStaticMesh;

// ================================================================================================================================================================
// Types and Interfaces for the XRCore Connection System
// ================================================================================================================================================================

// Socket State
UENUM(BlueprintType)
enum class EXRConnectorSocketState : uint8
{
	Available UMETA(DisplayName = "Available"),
	Occupied UMETA(DisplayName = "Occupied"),
	Disabled UMETA(DisplayName = "Disabled"),
};

// Hologram Interface
UINTERFACE(MinimalAPI, BlueprintType)
class UXRHologramInterface : public UInterface
{
	GENERATED_BODY()
};

class IXRHologramInterface
{
	GENERATED_BODY()

public:	
	/*
	* Show / Enable the hologram for this connector. Requires a static mesh reference, scale can be overridden optionally.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRConnectorHologram")
	void ShowHologram(UXRConnectorComponent* InConnector, UStaticMesh* InHologramMesh, float InHologramMeshScale = 1.0f);

	/*
	* Hide the hologram for this connector.
	* Note: Currently the AActor is destroyed after DestroyAfterHiddenSeconds duration. 
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRConnectorHologram")
	void HideHologram(UXRConnectorComponent* InConnector);

	/*
	* Enable/Disable the hologram for this connector.
	* Note: Disabling a Hologram that is Occupied will retain the Occupied state but disable (iE. hide it)
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRConnectorHologram")
	void SetHologramEnabled(UXRConnectorComponent* InConnector, bool InState);
};