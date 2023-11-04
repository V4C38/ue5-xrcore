#include "XRReplicatedPhysicsComponent.h"
#include "Net/UnrealNetwork.h"

UXRReplicatedPhysicsComponent::UXRReplicatedPhysicsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = true;
	SetIsReplicated(true);

}


void UXRReplicatedPhysicsComponent::BeginPlay()
{
	Super::BeginPlay();
	CachePhysicsMeshComponents("");
	UpdateClientPhysicsData();

	FString PluginConfigPath = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("XRToolkit"), TEXT("Config"), TEXT("DefaultXRToolkit.ini"));
	if (GConfig)
	{
		GConfig->GetFloat(TEXT("/Script/XRToolkit.XRReplicatedPhysicsComponent"), TEXT("ReplicationIntervalMax"), ReplicationIntervalMax, PluginConfigPath);
		GConfig->GetFloat(TEXT("/Script/XRToolkit.XRReplicatedPhysicsComponent"), TEXT("ReplicationIntervalMin"), ReplicationIntervalMin, PluginConfigPath);
		GConfig->GetFloat(TEXT("/Script/XRToolkit.XRReplicatedPhysicsComponent"), TEXT("VelocityThreshold"), VelocityThreshold, PluginConfigPath);
	}
}

void UXRReplicatedPhysicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner())
	{
		// Server: replicate PhysicsActors transform to clients
		if (GetOwnerRole() == ROLE_Authority && IsServerReplicatingPhysics())
		{
			ServerTick(DeltaTime);
		}
		// Client: Interpolate to ServerTransform
		else
		{
			ClientTick(DeltaTime);
		}
	}
}

// -----------------------------------------------------------------------------------------------------------------------------------
// Client Side
// -----------------------------------------------------------------------------------------------------------------------------------
void UXRReplicatedPhysicsComponent::ClientTick(float DeltaTime)
{
	if (bDisableClientInterpolation)
	{
		GetOwner()->SetActorLocationAndRotation(ServerPhysicsData.Location, ServerPhysicsData.Rotation);
	}
	else
	{
		float InterpSpeed = (1.0f / DynamicReplicationInterval);

		FVector CurrentLocation = GetOwner()->GetActorLocation();
		FVector TargetLocation = ServerPhysicsData.Location;

		FRotator CurrentRotation = GetOwner()->GetActorRotation();
		FRotator TargetRotation = ServerPhysicsData.Rotation;

		FVector InterpLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, InterpSpeed);
		FRotator InterpRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);

		GetOwner()->SetActorLocationAndRotation(InterpLocation, InterpRotation);
	}
}

void UXRReplicatedPhysicsComponent::OnRep_ServerReplicatePhysics()
{
	OnServerPhysicsReplicationStateChanged.Broadcast(this, IsServerReplicatingPhysics());
	SetComponentTickEnabled(IsServerReplicatingPhysics());

	// Disable Simulate Physics on Clients when Server is replicating
	if (GetOwnerRole() != ROLE_Authority)
	{
		SetComponentsSimulatePhysics(!IsServerReplicatingPhysics());
	}
}
void UXRReplicatedPhysicsComponent::OnRep_ServerPhysicsData()
{
	UpdateClientPhysicsData();
	OnServerReplicatedPhysicsData.Broadcast(this, ServerPhysicsData);
}

FXRPhysicsReplicationData UXRReplicatedPhysicsComponent::GetServerPhysicsData() const
{
	return ServerPhysicsData;
}


// -----------------------------------------------------------------------------------------------------------------------------------
// Serverside 
// -----------------------------------------------------------------------------------------------------------------------------------

void UXRReplicatedPhysicsComponent::ServerTick(float DeltaTime)
{
	// Adjust replication interval based on object velocity
	float Velocity = GetActorVelocity();
	if (Velocity < 0.0001f)
	{
		DynamicReplicationInterval = 0.0f;
	}
	else
	{
		float VelocityFactor = FMath::Clamp(Velocity / VelocityThreshold, 0.0f, 1.0f);
		DynamicReplicationInterval = FMath::Lerp(ReplicationIntervalMax, ReplicationIntervalMin, VelocityFactor);
	}
	AccumulatedTime += DeltaTime;
	if (AccumulatedTime >= DynamicReplicationInterval)
	{
		if (IsServerReplicatingPhysics())
		{
			FXRPhysicsReplicationData NewPhysicsData;
			NewPhysicsData.Location = GetOwner()->GetActorLocation();
			NewPhysicsData.Rotation = GetOwner()->GetActorRotation();
			NewPhysicsData.ReplicationInterval = DynamicReplicationInterval;

			Server_SetServerPhysicsData(NewPhysicsData);
		}
		AccumulatedTime = 0.0f;
	}
}

void UXRReplicatedPhysicsComponent::Server_SetServerReplicatePhysics_Implementation(bool bReplicatePhysics)
{
	UpdateClientPhysicsData();

	SetComponentsSimulatePhysics(true);
	SetComponentTickEnabled(bReplicatePhysics);
	bServerReplicatePhysics = bReplicatePhysics;

}

bool UXRReplicatedPhysicsComponent::IsServerReplicatingPhysics() const
{
	return bServerReplicatePhysics;
}

void UXRReplicatedPhysicsComponent::Server_SetServerPhysicsData_Implementation(FXRPhysicsReplicationData NewPhysicsData)
{
	ServerPhysicsData = NewPhysicsData;
}

float UXRReplicatedPhysicsComponent::GetDynamicReplicationInterval()
{
	return DynamicReplicationInterval;
}

void UXRReplicatedPhysicsComponent::CachePhysicsMeshComponents(FName InComponentTag)
{
	PhysicsMeshComponents.Empty();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// Add root component if it's a StaticMeshComponent.
	if (UStaticMeshComponent* RootStaticMesh = Cast<UStaticMeshComponent>(Owner->GetRootComponent()))
	{
		PhysicsMeshComponents.AddUnique(RootStaticMesh);
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
			PhysicsMeshComponents.AddUnique(MeshComponent);
		}
	}
}

TArray<UMeshComponent*> UXRReplicatedPhysicsComponent::GetPhysicsMeshComponents() const
{
	return PhysicsMeshComponents;
}

void UXRReplicatedPhysicsComponent::UpdateClientPhysicsData()
{
	FXRPhysicsReplicationData NewPhysicsData;
	NewPhysicsData.Location = GetOwner()->GetActorLocation();
	NewPhysicsData.Rotation = GetOwner()->GetActorRotation();
	NewPhysicsData.ReplicationInterval = ServerPhysicsData.ReplicationInterval;
	LastClientPhysicsData = NewPhysicsData;
}

void UXRReplicatedPhysicsComponent::SetComponentsSimulatePhysics(bool InSimulatePhysics)
{
	for (auto* PhysicsMeshComponent : GetPhysicsMeshComponents())
	{
		PhysicsMeshComponent->SetSimulatePhysics(InSimulatePhysics);
	}
}

float UXRReplicatedPhysicsComponent::GetActorVelocity() const
{
	if (!GetOwner()) return 0.0f;
	return GetOwner()->GetVelocity().Size();
}


// -----------------------------------------------------------------------------------------------------------------------------------
// Lifetime Reps
// -----------------------------------------------------------------------------------------------------------------------------------
void UXRReplicatedPhysicsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRReplicatedPhysicsComponent, ServerPhysicsData);
	DOREPLIFETIME(UXRReplicatedPhysicsComponent, bServerReplicatePhysics);
}

