
#include "Connections/XRConnectorHologram.h"
#include "Connections/XRConnectorComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"

AXRConnectorHologram::AXRConnectorHologram()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	MeshComponent->SetGenerateOverlapEvents(false);
	SetActorEnableCollision(false);

	RootComponent = MeshComponent;

	bReplicates = false;
	SetReplicates(false);
	SetReplicatingMovement(false);
}

void AXRConnectorHologram::BeginPlay()
{
	Super::BeginPlay(); 
	if (DestroyActorTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DestroyActorTimer);
	}
}

void AXRConnectorHologram::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AXRConnectorHologram::DestroyHologram()
{
	if (!IsActorBeingDestroyed())
	{
		Destroy();
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// API
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void AXRConnectorHologram::InitHologram_Implementation(const UXRConnectorComponent* InConnector, UStaticMesh* InHologramMesh, float InHologramMeshScale)
{
	if (IsActorBeingDestroyed())
	{
		return;
	}

	if (MeshComponent->GetStaticMesh() != InHologramMesh)
	{
		HologramMeshScale = InHologramMeshScale;
		MeshComponent->SetStaticMesh(InHologramMesh);
		MeshComponent->SetWorldScale3D(FVector(InHologramMeshScale));
	}
}

void AXRConnectorHologram::SetHologramState_Implementation(EXRHologramState InState)
{
	switch (InState)
	{
	case EXRHologramState::Hidden:
		SetActorHiddenInGame(true);
		if (!GetWorld()->GetTimerManager().IsTimerActive(DestroyActorTimer))
		{
			GetWorld()->GetTimerManager().SetTimer(DestroyActorTimer, this, &AXRConnectorHologram::DestroyHologram, DestroyAfterHiddenSeconds, false);
		}
		break;

	default:
		if (GetWorld()->GetTimerManager().IsTimerActive(DestroyActorTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(DestroyActorTimer);
		}
		SetActorHiddenInGame(false);
		break;
	}

}