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

// Socket State
UENUM(BlueprintType)
enum class EXRHologramState : uint8
{
	Visible UMETA(DisplayName = "Visible"),
	Highlighted UMETA(DisplayName = "Highlighted"),
	Hidden UMETA(DisplayName = "Hidden"),
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
	* Setup the hologram for this connector. Requires a static mesh reference, scale can be overridden optionally.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRConnectorHologram")
	void InitHologram(const UXRConnectorComponent* InConnector, UStaticMesh* InHologramMesh, float InHologramMeshScale = 1.0f);
	
	/*
	* Set the display state of this hologram
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRConnectorHologram")
	void SetHologramState(EXRHologramState InState);
};