
#pragma once

#include "CoreMinimal.h"
#include "FXRUserData.generated.h"

USTRUCT(BlueprintType) struct FFxrUserData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="XRToolkit|XRUserDataSystem")
	FString XRUserName;

	UPROPERTY(EditAnywhere, Category="XRToolkit|XRUserDataSystem")
	int32 XRAvatarConfig;

	FFxrUserData()
	{
		XRUserName = "Default Username";
		XRAvatarConfig = 1;
	}

};