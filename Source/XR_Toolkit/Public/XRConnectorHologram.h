// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "XRConnectorHologram.generated.h"

class UXRConnectorComponent;


UINTERFACE(MinimalAPI, BlueprintType)
class UXRHologramInterface : public UInterface
{
	GENERATED_BODY()
};

class IXRHologramInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRConnectorHologram")
	void ShowHologram(UXRConnectorComponent* InConnector, UStaticMesh* InHologramMesh);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRConnectorHologram")
	void HideHologram(UXRConnectorComponent* InConnector);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRConnectorHologram")
	void SetHologramState(UXRConnectorComponent* InConnector, bool InState);
};

UCLASS()
class XR_TOOLKIT_API AXRConnectorHologram : public AActor, public IXRHologramInterface
{
	GENERATED_BODY()
	
public:	
	AXRConnectorHologram();
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// API
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void ShowHologram_Implementation(UXRConnectorComponent* InConnector, UStaticMesh* InHologramMesh) override;
    virtual void HideHologram_Implementation(UXRConnectorComponent* InConnector) override;
    virtual void SetHologramState_Implementation(UXRConnectorComponent* InConnector, bool InState) override;


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:	

};
