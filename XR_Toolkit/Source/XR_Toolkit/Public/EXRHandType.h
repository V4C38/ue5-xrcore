#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EXRHandType.generated.h"

UENUM(BlueprintType)
enum class EXRHandType : uint8
{
	None UMETA(DisplayName="None"),
	Left UMETA(DisplayName="Left Hand"),
	Right UMETA(DisplayName="Right Hand"),
};
