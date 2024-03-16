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
	 * The lowest update rate at which the server replicates the actors transform to all clients. In Seconds.
	 **/
	UPROPERTY(EditDefaultsOnly, Category = "Physics Replication")
	float DefaultReplicationIntervalMax = 0.05f;
	/**
	 * The highest update rate at which the server replicates the actors transform to all clients. In Seconds.
	 **/
	UPROPERTY(EditDefaultsOnly, Category = "Physics Replication")
	float DefaultReplicationIntervalMin = 0.02f;
	/**
	 * Whenever the Actors velocity is higher than this treshold, use the ShortestReplicationInterval
	 **/
	UPROPERTY(EditDefaultsOnly, Category = "Physics Replication")
	float DefaultVelocityThreshold = 350.0f;

};
