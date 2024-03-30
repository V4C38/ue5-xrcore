#include "XRReplicatedPhysicsComponent.h"
#include "XRCoreSettings.h"
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

	FString PluginConfigPath = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("XRToolkit"), TEXT("Config"), TEXT("DefaultXRToolkit.ini"));
	if (GConfig)
	{
		GConfig->GetFloat(TEXT("/Script/XRToolkit.XRReplicatedPhysicsComponent"), TEXT("DefaultReplicationInterval"), DefaultReplicationInterval, PluginConfigPath);
		GConfig->GetFloat(TEXT("/Script/XRToolkit.XRReplicatedPhysicsComponent"), TEXT("InteractedReplicationInterval"), InteractedReplicationInterval, PluginConfigPath);
	}

	RegisterPhysicsMeshComponents(RegisterMeshComponentsWithTag);
	if (GetOwnerRole() == ROLE_Authority )
	{
		FXRPhysicsSnapshot NewSnapshot;
		NewSnapshot.ID = 1;
		NewSnapshot.Location = GetOwner()->GetActorLocation();
		NewSnapshot.Rotation = GetOwner()->GetActorRotation();
		NewSnapshot.bIsInteractedWith = false;
		LatestSnapshot = NewSnapshot;
		Server_SetCachedSnapshot(NewSnapshot);
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

void UXRReplicatedPhysicsComponent::OnRegister()
{
	Super::OnRegister();

	OnComponentActivated.AddDynamic(this, &UXRReplicatedPhysicsComponent::OnActivated);
	OnComponentDeactivated.AddDynamic(this, &UXRReplicatedPhysicsComponent::OnDeactivated);
}

void UXRReplicatedPhysicsComponent::OnUnregister()
{
	OnComponentActivated.RemoveDynamic(this, &UXRReplicatedPhysicsComponent::OnActivated);
	OnComponentDeactivated.RemoveDynamic(this, &UXRReplicatedPhysicsComponent::OnDeactivated);

	Super::OnUnregister();
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
void UXRReplicatedPhysicsComponent::OnRep_CachedSnapshot()
{
	LatestSnapshot = CachedSnapshot;
	if (GetOwnerRole() != ROLE_Authority)
	{
		GetOwner()->SetActorLocationAndRotation(CachedSnapshot.Location, CachedSnapshot.Rotation);
	}
}

FXRPhysicsSnapshot UXRReplicatedPhysicsComponent::GetLatestSnapshot() const
{
	return LatestSnapshot;
}

void UXRReplicatedPhysicsComponent::OnActivated(UActorComponent* Component, bool bReset)
{
	SetSimulatePhysicsOnOwner(GetOwnerRole() == ROLE_Authority);
}

void UXRReplicatedPhysicsComponent::OnDeactivated(UActorComponent* Component)
{
	SetSimulatePhysicsOnOwner(false);
}

// -----------------------------------------------------------------------------------------------------------------------------------
// Serverside 
// -----------------------------------------------------------------------------------------------------------------------------------
void UXRReplicatedPhysicsComponent::ServerTick(float DeltaTime)
{
	if (!IsActive())
	{
		return;
	}

	// Do not replicate static objects
	if (GetActorVelocity() < 0.0001f && !bIsInteractedWith)
	{
		// Replicate only one time, when the object becomes static
		if (CachedSnapshot.Location != GetOwner()->GetActorLocation())
		{
			FXRPhysicsSnapshot NewSnapshot;
			NewSnapshot.ID = 0;
			NewSnapshot.Location = GetOwner()->GetActorLocation();
			NewSnapshot.Rotation = GetOwner()->GetActorRotation();
			NewSnapshot.bIsInteractedWith = false;
			Server_SetCachedSnapshot(NewSnapshot);
		}
		return;
	}

	float ReplicationInterval = LatestSnapshot.bIsInteractedWith != 0 ? InteractedReplicationInterval : DefaultReplicationInterval;
	AccumulatedTime += DeltaTime;
	if (AccumulatedTime >= ReplicationInterval)
	{
		FXRPhysicsSnapshot NewSnapshot;
		NewSnapshot.ID = LatestSnapshot.ID + 1;
		NewSnapshot.Location = GetOwner()->GetActorLocation();
		NewSnapshot.Rotation = GetOwner()->GetActorRotation();
		NewSnapshot.bIsInteractedWith = bIsInteractedWith;

		LatestSnapshot = NewSnapshot;
		AccumulatedTime = 0.0f;
	}
}

void UXRReplicatedPhysicsComponent::Server_SetCachedSnapshot_Implementation(FXRPhysicsSnapshot InCachedSnapshot)
{
	CachedSnapshot = InCachedSnapshot;
}

void UXRReplicatedPhysicsComponent::SetInteractedWith(bool bInInteracedWith)
{
	bIsInteractedWith = bInInteracedWith;
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
	if (bDebugDisableClientInterpolation)
	{
		GetOwner()->SetActorLocationAndRotation(LatestSnapshot.Location, LatestSnapshot.Rotation);
		return;
	}

	if (IsSequenceIDNewer(LatestSnapshot.ID, ClientActiveSnapshot.ID))
	{
		ClientActiveSnapshot = LatestSnapshot;
	}
	float ReplicationInterval = LatestSnapshot.bIsInteractedWith != 0 ? InteractedReplicationInterval : DefaultReplicationInterval;
	float InterpSpeed = (1.0f / (ReplicationInterval * 2.0f));

	FVector TargetLocation = ClientActiveSnapshot.Location;
	FRotator TargetRotation = ClientActiveSnapshot.Rotation;

	FVector InterpLocation = FMath::VInterpTo(GetOwner()->GetActorLocation(), TargetLocation, DeltaTime, InterpSpeed);
	FRotator InterpRotation = FMath::RInterpTo(GetOwner()->GetActorRotation(), TargetRotation, DeltaTime, InterpSpeed);

	GetOwner()->SetActorLocationAndRotation(InterpLocation, InterpRotation);

}

bool UXRReplicatedPhysicsComponent::IsSequenceIDNewer(uint32 InID1, uint32 InID2) const
{
	int32 delta = InID1 - InID2;

	// Check if InID1 is "ahead" of InID2, considering wrap-around.
	return (delta > 0) || (delta < -static_cast<int32>(UINT32_MAX / 2));
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
}

float UXRReplicatedPhysicsComponent::GetActorVelocity() const
{
	if (!GetOwner()) return 0.0f;
	return GetOwner()->GetVelocity().Size();
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

	if (InComponentTag.IsNone())
	{
		return;
	}

	// Get all MeshComponents with the specified tag and add them to the list.
	TInlineComponentArray<UMeshComponent*> MeshComponents;
	Owner->GetComponents(MeshComponents);
	for (UMeshComponent* MeshComponent : MeshComponents)
	{
		if (MeshComponent && MeshComponent->ComponentHasTag(InComponentTag))
		{
			OutMeshComponents.AddUnique(MeshComponent);
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
	DOREPLIFETIME(UXRReplicatedPhysicsComponent, LatestSnapshot);
	DOREPLIFETIME(UXRReplicatedPhysicsComponent, CachedSnapshot);
}
