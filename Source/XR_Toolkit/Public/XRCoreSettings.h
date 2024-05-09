#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DeveloperSettings.h"
#include "XRCoreSettings.generated.h"


UENUM(BlueprintType)
enum class EXRStandard : uint8
{
	OpenXR UMETA(DisplayName = "OpenXR"),
	MetaXR UMETA(DisplayName = "MetaXR"),
};

/**
 * Settings for XRCore
 */
UCLASS(config = XRCore, defaultconfig)
class XR_TOOLKIT_API UXRCoreSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    /**
     * XR Standard to use. Specify whether if the default OpenXR plugin or MetaXR is used for this project.
     * This will impact the way MotionControllers and HandTracking is utilized as using the MetaXR plugin changes some of the rotations.
     */
    UPROPERTY(config, EditAnywhere)
    EXRStandard XRStandard = EXRStandard::OpenXR;

	/**
	 * This will automatically assign the XRCoreNetDriver as the NetDriver.
	 * Allows to effectively call RPCs from any Client without authority.
	 * Ignores the authority check for RPCs when an actor has the IXRBypassAuthorityCheck Interface applied. 
	 */
	UPROPERTY(config, EditAnywhere)
	bool UseCustomNetDriver = true;

	/**
	 * The replication interval, in seconds, for sending snapshots from the server to all clients. 
	**/
	UPROPERTY(EditDefaultsOnly, Category = "Physics Replication")
	float DefaultReplicationInterval = 0.1f;

	/**
	 * The replication interval, in seconds, for when the Actor is currently interacted with.
	 **/
	UPROPERTY(EditDefaultsOnly, Category = "Physics Replication")
	float InteractedReplicationInterval = 0.01f;
};
