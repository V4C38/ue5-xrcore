#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/NoExportTypes.h"

#include "Core/XRCoreTypes.h"

#include "XRCoreSettings.generated.h"


// ================================================================================================================================================================
// Default settings for XRCore
// ================================================================================================================================================================

UCLASS(config = XRCore, defaultconfig)
class XR_TOOLKIT_API UXRCoreSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
	UXRCoreSettings(const FObjectInitializer& ObjectInitializer);
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;

    /**
     * XR Standard to use. Specify whether if the default OpenXR plugin or MetaXR is used for this project.
     * This will impact the way MotionControllers and HandTracking is utilized as using the MetaXR plugin changes some of the rotations.
     */
    UPROPERTY(config, EditAnywhere)
    EXRStandard XRStandard = EXRStandard::OpenXR;

	/**
	 * The default sound to be played when an interaction is started.
	 * Can be overridden in any XRInteractionComponent.
	**/
	UPROPERTY(config, EditAnywhere, Category = "Defaults", meta = (AllowedClasses = "SoundWave,SoundCue"))
	TSoftObjectPtr<USoundBase> DefaultInteractionStartSound;

	/**
	 * The default sound to be played when an interaction is ended.
	 * Can be overridden in any XRInteractionComponent.
	**/
	UPROPERTY(config, EditAnywhere, Category = "Defaults", meta = (AllowedClasses = "SoundWave,SoundCue"))
	TSoftObjectPtr<USoundBase> DefaultInteractionEndSound;

	/**
	 * The default curve used for interpolation of one state of the XRHighlightComponent to another.
	 * Can be overridden in any XRInteractionComponent under Highlight or in a manually added XRHighlightComponent.
	**/
	UPROPERTY(config, EditAnywhere, Category = "Defaults", meta = (AllowedClasses = "CurveFloat"))
	TSoftObjectPtr<UCurveFloat> DefaultHighlightFadeCurve;

	/**
	 * The default Hologram class instantiated by the XRConnectorComponent when holograms are enabled.
	 * Can be overridden in any XRConnectorComponent. Class must implement the UXRHologramInterface.
	**/
	UPROPERTY(config, EditAnywhere, Category = "Defaults", meta = (AllowedClasses = "Actor"))
	TSoftClassPtr<AActor> DefaultHologramClass;

	/**
	 * The replication interval, in seconds, for sending snapshots from the server to all clients. 
	**/
	UPROPERTY(EditDefaultsOnly, Category = "Physics Replication", meta = (ClampMin = "0.0"))
	float DefaultReplicationInterval = 0.1f;

	/**
	 * The replication interval, in seconds, for when the Actor is currently interacted with.
	 **/
	UPROPERTY(EditDefaultsOnly, Category = "Physics Replication", meta = (ClampMin = "0.0"))
	float InteractedReplicationInterval = 0.01f;
};
