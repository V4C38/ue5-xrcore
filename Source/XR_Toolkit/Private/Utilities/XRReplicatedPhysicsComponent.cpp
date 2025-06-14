#include "Utilities/XRReplicatedPhysicsComponent.h"
#include "Core/XRCoreSettings.h"
#include "Net/UnrealNetwork.h"

UXRReplicatedPhysicsComponent::UXRReplicatedPhysicsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = true;
	SetIsReplicatedByDefault(true);
}

void UXRReplicatedPhysicsComponent::BeginPlay()
{
	Super::BeginPlay();

	DefaultReplicationInterval = GetDefault<UXRCoreSettings>()->DefaultReplicationInterval;
	InteractedReplicationInterval = GetDefault<UXRCoreSettings>()->InteractedReplicationInterval;

	RegisterPhysicsMeshComponents(RegisterMeshComponentsWithTag);
	if (GetOwnerRole() == ROLE_Authority )
	{
		FXRPhysicsSnapshot NewSnapshot;
		NewSnapshot.ID = 1;
		NewSnapshot.Location = GetOwner()->GetActorLocation();
		NewSnapshot.Rotation = GetOwner()->GetActorRotation();
		NewSnapshot.bIsInteractedWith = false;

		Server_SetReplicatedSnapshot(NewSnapshot);
		SetSimulatePhysicsOnOwner(true);
	}
	
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UXRReplicatedPhysicsComponent::DelayedPhysicsSetup, 0.5f, false);
}

void UXRReplicatedPhysicsComponent::DelayedPhysicsSetup()
{
	if (bAutoActivate)
	{
		SetSimulatePhysicsOnOwner(GetOwnerRole() == ROLE_Authority);
	}
}

void UXRReplicatedPhysicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsActive())
	{
		return;
	}
	if (!GetOwner())
	{
		return;
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		ServerTick(DeltaTime);
	}
	else
	{
		ClientTick(DeltaTime);
	}
}

// -----------------------------------------------------------------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------------------------------------------------------------
void UXRReplicatedPhysicsComponent::OnRep_ReplicatedSnapshot()
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		ClientActiveSnapshot = ReplicatedSnapshot;
		GetOwner()->SetActorLocationAndRotation(ReplicatedSnapshot.Location, ReplicatedSnapshot.Rotation);
	}
}

FXRPhysicsSnapshot UXRReplicatedPhysicsComponent::GetLatestSnapshot() const
{
	return ReplicatedSnapshot;
}

// -----------------------------------------------------------------------------------------------------------------------------------
// Serverside 
// -----------------------------------------------------------------------------------------------------------------------------------
void UXRReplicatedPhysicsComponent::Server_ForceUpdate_Implementation()
{
	FXRPhysicsSnapshot NewSnapshot;
	NewSnapshot.ID = ReplicatedSnapshot.ID + 1;
	NewSnapshot.Location = GetOwner()->GetActorLocation();
	NewSnapshot.Rotation = GetOwner()->GetActorRotation();
	NewSnapshot.bIsInteractedWith = bIsInteractedWith;

	Server_SetReplicatedSnapshot(NewSnapshot);
}


void UXRReplicatedPhysicsComponent::ServerTick(float DeltaTime)
{
	if (!bPhysicsActive)
	{
		return;
	}

	// Do not replicate static objects
	if (GetActorVelocity() < 0.0001f && !bIsInteractedWith)
	{
		// Replicate only one time, when the object becomes static
		if (ReplicatedSnapshot.Location != GetOwner()->GetActorLocation())
		{
			FXRPhysicsSnapshot NewSnapshot;
			NewSnapshot.ID = ReplicatedSnapshot.ID + 1;
			NewSnapshot.Location = GetOwner()->GetActorLocation();
			NewSnapshot.Rotation = GetOwner()->GetActorRotation();
			NewSnapshot.bIsInteractedWith = false;

			Server_SetReplicatedSnapshot(NewSnapshot);
		}
		return;
	}

	float ReplicationInterval = ReplicatedSnapshot.bIsInteractedWith != 0 ? InteractedReplicationInterval : DefaultReplicationInterval;
	AccumulatedTime += DeltaTime;
	if (AccumulatedTime >= ReplicationInterval)
	{
		FXRPhysicsSnapshot NewSnapshot;
		NewSnapshot.ID = ReplicatedSnapshot.ID + 1;
		NewSnapshot.Location = GetOwner()->GetActorLocation();
		NewSnapshot.Rotation = GetOwner()->GetActorRotation();
		NewSnapshot.bIsInteractedWith = bIsInteractedWith;

		Server_SetReplicatedSnapshot(NewSnapshot);

		AccumulatedTime = 0.0f;
	}
}

void UXRReplicatedPhysicsComponent::Server_SetReplicatedSnapshot_Implementation(FXRPhysicsSnapshot InReplicatedSnapshot)
{
	ReplicatedSnapshot = InReplicatedSnapshot;
}

void UXRReplicatedPhysicsComponent::SetInteractedWith(bool bInInteracedWith)
{
	bIsInteractedWith = bInInteracedWith;
	Server_ForceUpdate();
}

bool UXRReplicatedPhysicsComponent::GetInteractedWith() const
{
	return bIsInteractedWith;
}

// -----------------------------------------------------------------------------------------------------------------------------------
// Client Side
// -----------------------------------------------------------------------------------------------------------------------------------
void UXRReplicatedPhysicsComponent::ClientTick(float DeltaTime)
{
	if (!bPhysicsActive)
	{
		return;
	}

	if (IsSequenceIDNewer(ReplicatedSnapshot.ID, ClientActiveSnapshot.ID))
	{
		ClientActiveSnapshot = ReplicatedSnapshot;
	}

	if (bDebugDisableClientInterpolation)
	{
		GetOwner()->SetActorLocationAndRotation(ReplicatedSnapshot.Location, ReplicatedSnapshot.Rotation);
		return;
	}
	float ReplicationInterval = ReplicatedSnapshot.bIsInteractedWith != 0 ? InteractedReplicationInterval : DefaultReplicationInterval;
	float SafeInterval = FMath::Max(ReplicationInterval * 2.0f, KINDA_SMALL_NUMBER);
	float InterpSpeed = 1.0f / SafeInterval;


	FVector TargetLocation = ClientActiveSnapshot.Location;
	FRotator TargetRotation = ClientActiveSnapshot.Rotation;

	FVector InterpLocation = FMath::VInterpTo(GetOwner()->GetActorLocation(), TargetLocation, DeltaTime, InterpSpeed);
	FRotator InterpRotation = FMath::RInterpTo(GetOwner()->GetActorRotation(), TargetRotation, DeltaTime, InterpSpeed);

	GetOwner()->SetActorLocationAndRotation(InterpLocation, InterpRotation);
}

void UXRReplicatedPhysicsComponent::OnRep_PhysicsActive()
{
	if (bPhysicsActive)
	{
		ClientActiveSnapshot = ReplicatedSnapshot;
		AccumulatedTime = 0.0f;
		Activate();
	}
	else
	{
		Deactivate();
	}
}

bool UXRReplicatedPhysicsComponent::IsSequenceIDNewer(uint32 InID1, uint32 InID2) const
{
	return int32(InID1 - InID2) > 0;
}

// -----------------------------------------------------------------------------------------------------------------------------------
// Colliders/Sim on Owner
// -----------------------------------------------------------------------------------------------------------------------------------

void UXRReplicatedPhysicsComponent::SetSimulatePhysicsOnOwner(bool InSimulatePhysics)
{
	for (auto* PhysicsMeshComponent : GetRegisteredMeshComponents())
	{
		PhysicsMeshComponent->SetSimulatePhysics(InSimulatePhysics);
	}
	bPhysicsActive = InSimulatePhysics;

	if (GetNetMode() == NM_Client || GetNetMode() == NM_Standalone)
	{
		if (bPhysicsActive)
		{
			ClientActiveSnapshot = ReplicatedSnapshot;
			AccumulatedTime = 0.0f;
			Activate();
		}
		else
		{
			Deactivate();
		}
	}
}

float UXRReplicatedPhysicsComponent::GetActorVelocity() const
{
	const AActor* Owner = GetOwner();
	return Owner ? Owner->GetVelocity().Size() : 0.f;
}

void UXRReplicatedPhysicsComponent::RegisterPhysicsMeshComponents(FName InComponentTag)
{
	TArray<UMeshComponent*> OutMeshComponents = {};

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// Add root component if it's a StaticMeshComponent.
	if (UStaticMeshComponent* RootStaticMesh = Cast<UStaticMeshComponent>(Owner->GetRootComponent()))
	{
		OutMeshComponents.AddUnique(RootStaticMesh);
	}

	if (!InComponentTag.IsNone())
	{	// Get all MeshComponents with the specified tag and add them to the list.
		TInlineComponentArray<UMeshComponent*> MeshComponents;
		Owner->GetComponents(MeshComponents);
		for (UMeshComponent* MeshComponent : MeshComponents)
		{
			if (MeshComponent && MeshComponent->ComponentHasTag(InComponentTag))
			{
				OutMeshComponents.AddUnique(MeshComponent);
			}
		}
	}

	RegisteredMeshComponents = OutMeshComponents;
}

TArray<UMeshComponent*> UXRReplicatedPhysicsComponent::GetRegisteredMeshComponents() const
{
	return RegisteredMeshComponents;
}


// -----------------------------------------------------------------------------------------------------------------------------------
// Lifetime Reps
// -----------------------------------------------------------------------------------------------------------------------------------
void UXRReplicatedPhysicsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRReplicatedPhysicsComponent, ReplicatedSnapshot); 
	DOREPLIFETIME(UXRReplicatedPhysicsComponent, bPhysicsActive);

}
