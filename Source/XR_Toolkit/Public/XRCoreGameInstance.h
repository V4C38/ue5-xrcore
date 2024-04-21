#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "XRCoreSettings.h"
#include "XRCoreNetDriver.h"
#include "XRCoreGameInstance.generated.h"


UCLASS()
class XR_TOOLKIT_API UXRCoreGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
};
