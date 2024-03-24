// Fill out your copyright notice in the Description page of Project Settings.


#include "XRConnectorHologram.h"

AXRConnectorHologram::AXRConnectorHologram()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AXRConnectorHologram::BeginPlay()
{
	Super::BeginPlay();
	
}

void AXRConnectorHologram::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// API
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void AXRConnectorHologram::ShowHologram_Implementation(float HologramFadeDuration, UStaticMesh* HologramMesh, UMaterialInterface* HologramMaterial)
{

}

void AXRConnectorHologram::HideHologram_Implementation(float HologramFadeDuration)
{

}