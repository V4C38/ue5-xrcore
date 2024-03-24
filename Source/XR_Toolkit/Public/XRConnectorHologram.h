// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XRConnectorHologram.generated.h"

UCLASS()
class XR_TOOLKIT_API AXRConnectorHologram : public AActor
{
	GENERATED_BODY()
	
public:	
	AXRConnectorHologram();

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// API
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "XRConnectorHologram")
	void ShowHologram(float HologramFadeDuration, UStaticMesh* HologramMesh, UMaterialInterface* HologramMaterial);
	virtual void ShowHologram_Implementation(float HologramFadeDuration, UStaticMesh* HologramMesh, UMaterialInterface* HologramMaterial);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "XRConnectorHologram")
	void HideHologram(float HologramFadeDuration);
	virtual void HideHologram_Implementation(float HologramFadeDuration);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:	

};
