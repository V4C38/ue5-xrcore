#include "Core/XRCoreSettings.h"

UXRCoreSettings::UXRCoreSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set defaults using absolute paths
	DefaultInteractionStartSound = TSoftObjectPtr<USoundBase>(FSoftObjectPath(TEXT("/XRCore/Assets/SFX/SC_Plop_01.SC_Plop_01")));
	DefaultInteractionEndSound = TSoftObjectPtr<USoundBase>(FSoftObjectPath(TEXT("/XRCore/Assets/SFX/SC_Plop_02.SC_Plop_02")));
	DefaultHighlightFadeCurve = TSoftObjectPtr<UCurveFloat>(FSoftObjectPath(TEXT("/XRCore/Assets/Curves/Curve_XRHighlightFade.Curve_XRHighlightFade")));
	DefaultHologramClass = TSoftClassPtr<AActor>(FSoftObjectPath(TEXT("/XRCore/Blueprints/BP_XRConnectorHologram")));
}

FName UXRCoreSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

FName UXRCoreSettings::GetSectionName() const
{
	return TEXT("XRCore");
}