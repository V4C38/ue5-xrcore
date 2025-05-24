// Fill out your copyright notice in the Description page of Project Settings.


#include "Connections/XRConnectorHologram.h"
#include "Connections/XRConnectorComponent.h"

AXRConnectorHologram::AXRConnectorHologram()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComponent;
}

void AXRConnectorHologram::BeginPlay()
{
	Super::BeginPlay();
}

void AXRConnectorHologram::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AXRConnectorHologram::RemoveHologram()
{
	Destroy();
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// API
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void AXRConnectorHologram::ShowHologram_Implementation(UXRConnectorComponent* InConnector, UStaticMesh* InHologramMesh, float InHologramMeshScale)
{
	if (MeshComponent->GetStaticMesh() != InHologramMesh)
	{
		MeshComponent->SetStaticMesh(InHologramMesh);
		MeshComponent->SetWorldScale3D(FVector(InHologramMeshScale));
		HologramMeshScale = InHologramMeshScale;
	}
	if (GetWorld()->GetTimerManager().IsTimerActive(DestroyActorTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(DestroyActorTimer);
	}
}

void AXRConnectorHologram::HideHologram_Implementation(UXRConnectorComponent* InConnector)
{
	if (GetWorld()->GetTimerManager().IsTimerActive(DestroyActorTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(DestroyActorTimer);
	}
	GetWorld()->GetTimerManager().SetTimer(DestroyActorTimer, this, &AXRConnectorHologram::RemoveHologram, DestroyAfterHiddenSeconds, false);
}

void AXRConnectorHologram::SetHologramEnabled_Implementation(UXRConnectorComponent* InConnector, bool InState)
{
}

