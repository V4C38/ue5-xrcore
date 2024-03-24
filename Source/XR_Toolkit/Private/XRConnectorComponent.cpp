// Fill out your copyright notice in the Description page of Project Settings.


#include "XRConnectorComponent.h"
#include "XRConnectorSocket.h"
#include "XRConnectorHologram.h"
#include "XRToolsUtilityFunctions.h"
#include "XRInteractionGrab.h"
#include "Net/UnrealNetwork.h"

UXRConnectorComponent::UXRConnectorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = true;
	SetIsReplicatedByDefault(true);
}


void UXRConnectorComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeOverlapBindings();
	InitializeInteractionBindings();
}


// Rember to deinit connections on Destroy() of this Component


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Connection Logic
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UXRConnectorComponent::ConnectToSocket(UXRConnectorSocket* InSocket)
{
	if (!InSocket)
	{
		return false;
	}
	UXRConnectorSocket* AlreadyConnectedSocket = {};
	if (IsConnected(AlreadyConnectedSocket))
	{
		return false;
	}
	if (!InSocket->IsConnectionAllowed(this))
	{
		return false;
	}
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}
	Server_SetConnectedSocket(InSocket);
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		ConnectedSocket = InSocket;
		SetupConnection();
	}
	HideHologram(InSocket);
	return true;
}

bool UXRConnectorComponent::ConnectToClosestOverlappedSocket(UXRConnectorSocket*& OutSocket)
{
	OutSocket = nullptr;
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}
	TArray<AActor*> OverlappingActors = {};
	TArray<UXRConnectorSocket*> FoundSockets = {};
	Owner->GetOverlappingActors(OverlappingActors);
	for (auto OverlappingActor : OverlappingActors)
	{
		TArray<UXRConnectorSocket*> FoundFoundSocketsOnActor = {};
		OverlappingActor->GetComponents<UXRConnectorSocket>(FoundFoundSocketsOnActor);
		FoundSockets.Append(FoundFoundSocketsOnActor);
	}
	FVector OwnerLocation = Owner->GetActorLocation();
	float MinSqDistance = FLT_MAX;
	for (auto FoundSocket : FoundSockets)
	{
		float SocketDistance = FVector::DistSquared(FoundSocket->GetComponentLocation(), OwnerLocation);
		if (SocketDistance < MinSqDistance)
		{
			MinSqDistance = SocketDistance;
			OutSocket = FoundSocket;
		}
	}
	if (OutSocket)
	{
		ConnectToSocket(OutSocket);
		return true;
	}
	return false;
}

void UXRConnectorComponent::DisconnectFromSocket()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	Server_SetConnectedSocket(nullptr);
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		ConnectedSocket = nullptr;
		SetupConnection();
	}
}

void UXRConnectorComponent::SetupConnection()
{
	UXRConnectorSocket* CurrentlyConnectedSocket = {};
	if (IsConnected(CurrentlyConnectedSocket))
	{
		// New Connection
		if (CurrentlyConnectedSocket)
		{
			LocalCachedConnectedSocket = CurrentlyConnectedSocket;
			GetOwner()->AttachToComponent(CurrentlyConnectedSocket, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
			CurrentlyConnectedSocket->RegisterConnection(this);
			OnConnected.Broadcast(this, CurrentlyConnectedSocket);
			return;
		}
	}

	// Remove Exisiting Connection
	if (LocalCachedConnectedSocket.IsValid())
	{
		UXRConnectorSocket* DetachingSocket = LocalCachedConnectedSocket.Get();
		LocalCachedConnectedSocket = nullptr;
		GetOwner()->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
		DetachingSocket->DeregisterConnection(this);
		OnDisconnected.Broadcast(this, DetachingSocket);
		return;
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// API
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UXRConnectorComponent::IsConnected(UXRConnectorSocket*& OutConnectedSocket) const
{
	if (ConnectedSocket.IsValid())
	{
		OutConnectedSocket = ConnectedSocket.Get();
		return true;
	}
	return false;
}

FName UXRConnectorComponent::GetConnectorID() const
{
	return ConnectorID;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Overlap Logic
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRConnectorComponent::InitializeOverlapBindings()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	Owner->GetComponents<UPrimitiveComponent>(OwnerCollisions);

	for (auto Collider : OwnerCollisions)
	{
		Collider->OnComponentBeginOverlap.AddDynamic(this, &UXRConnectorComponent::OnOverlapBegin);
		Collider->OnComponentEndOverlap.AddDynamic(this, &UXRConnectorComponent::OnOverlapEnd);
	}
}

void UXRConnectorComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (UXRConnectorSocket* OverlappedSocket = Cast<UXRConnectorSocket>(OtherComp))
	{
		if (!OverlappedSockets.Contains(OverlappedSocket))
		{
			OverlappedSockets.Add(OverlappedSocket);
			ShowHologram(OverlappedSocket);
		}
	}
}

void UXRConnectorComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (UXRConnectorSocket* OverlappedSocket = Cast<UXRConnectorSocket>(OtherComp))
	{
		// Ensure an overlapped Socket is only removed when no Collider on the OwningActor is overlapping it anymore, not just the one that stopped overlapping
		for (auto Collider : OwnerCollisions)
		{
			if (Collider != OverlappedComponent)
			{
				TArray<UPrimitiveComponent*> OverlappedComponents = {};
				Collider->GetOverlappingComponents(OverlappedComponents);
				if (OverlappedComponents.Contains(OtherComp))
				{
					return;
				}
			}
		}
		OverlappedSockets.Remove(OverlappedSocket);
		HideHologram(OverlappedSocket);
	}
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Hologram
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRConnectorComponent::ShowHologram(UXRConnectorSocket* InSocket)
{
	if (!InSocket)
	{
		return;
	}
	if (!bShowConnectorHologram || !HologramMesh || ActiveHologramData.Contains(InSocket))
	{
		return;
	}

	FVector Location = InSocket->GetComponentLocation();
	FRotator Rotation = InSocket->GetComponentRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AXRConnectorHologram* SpawnedHologram = GetWorld()->SpawnActor<AXRConnectorHologram>(HologramClass, Location, Rotation, SpawnParams);
	if (SpawnedHologram)
	{
		ActiveHologramData.Add(InSocket, SpawnedHologram);
		SpawnedHologram->AttachToComponent(InSocket, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
		SpawnedHologram->ShowHologram(HologramFadeDuration, HologramMesh, HologramMaterial);
	}
}

void UXRConnectorComponent::HideHologram(UXRConnectorSocket* InSocket)
{
	if (!InSocket)
	{
		return;
	}
	AXRConnectorHologram** FoundHologram = ActiveHologramData.Find(InSocket);
	if (FoundHologram && *FoundHologram)
	{
		(*FoundHologram)->HideHologram(HologramFadeDuration);
		ActiveHologramData.Remove(InSocket);
	}
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Interaction Bindings
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRConnectorComponent::InitializeInteractionBindings()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	TArray<UXRInteractionGrab*> FoundGrabInteractions;
	Owner->GetComponents<UXRInteractionGrab>(FoundGrabInteractions);
	TArray<UXRInteractionComponent*> InteractionComponents;
	for (UXRInteractionGrab* GrabInteraction : FoundGrabInteractions)
	{
		// Because UXRInteractionGrab inherits from UXRInteractionComponent, we can safely cast
		UXRInteractionComponent* InteractionComponent = Cast<UXRInteractionComponent>(GrabInteraction);
		if (InteractionComponent)
		{
			InteractionComponents.Add(InteractionComponent);
		}
	}

	UXRInteractionComponent* HighestPriorityComponent = UXRToolsUtilityFunctions::GetXRInteractionByPriority(InteractionComponents, nullptr, 0, EXRInteractionPrioritySelection::LowerEqual,5);

	if (HighestPriorityComponent)
	{
		HighestPriorityComponent->OnInteractionStarted.AddDynamic(this, &UXRConnectorComponent::OnInteractionStarted);
		HighestPriorityComponent->OnInteractionEnded.AddDynamic(this, &UXRConnectorComponent::OnInteractionEnded);
	}
}

void UXRConnectorComponent::OnInteractionStarted(UXRInteractionComponent* Sender, UXRInteractorComponent* XRInteractorComponent)
{
	DisconnectFromSocket();
}

void UXRConnectorComponent::OnInteractionEnded(UXRInteractionComponent* Sender, UXRInteractorComponent* XRInteractorComponent)
{
	UXRConnectorSocket* OutConnectedSocket = {};
	ConnectToClosestOverlappedSocket(OutConnectedSocket);
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Replication
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRConnectorComponent::Server_SetConnectedSocket_Implementation(UXRConnectorSocket* InSocket)
{
	ConnectedSocket = InSocket;
}

void UXRConnectorComponent::OnRep_ConnectedSocket()
{
	SetupConnection();
}

void UXRConnectorComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRConnectorComponent, ConnectedSocket);
}
