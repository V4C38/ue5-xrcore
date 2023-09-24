#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EXRHandTrackingGesture.generated.h"

UENUM(BlueprintType)
enum class EXRHandTrackingGesture : uint8
{
	None UMETA(DisplayName="None"),
	Pinch UMETA(DisplayName="Pinch"),
	Pinch_Fist UMETA(DisplayName="Pinch Fist"),
	Fist UMETA(DisplayName="Fist"),
	Flat_Open UMETA(DisplayName="Flat Open")
};
