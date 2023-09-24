#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EXRInteractionGrabType.generated.h"

UENUM(BlueprintType)
enum class EXRInteractionGrabType : uint8
{
	Free UMETA(DisplayName="Free"),
	SnapRoot UMETA(DisplayName="Snap to Root"),
	SnapHand UMETA(DisplayName="Snap to Hand"),
	Custom UMETA(DisplayName="Custom"),
};
