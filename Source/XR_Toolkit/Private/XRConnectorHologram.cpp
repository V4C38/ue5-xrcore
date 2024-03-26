// Fill out your copyright notice in the Description page of Project Settings.


#include "XRConnectorHologram.h"
#include "XRConnectorComponent.h"

AXRConnectorHologram::AXRConnectorHologram()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
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
void AXRConnectorHologram::ShowHologram_Implementation(UXRConnectorComponent* InConnector, UStaticMesh* InHologramMesh)
{
}

void AXRConnectorHologram::HideHologram_Implementation(UXRConnectorComponent* InConnector)
{
}

void AXRConnectorHologram::SetHologramState_Implementation(UXRConnectorComponent* InConnector, bool InState)
{
}

