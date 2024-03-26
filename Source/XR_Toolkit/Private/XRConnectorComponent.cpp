// Fill out your copyright notice in the Description page of Project Settings.


#include "XRConnectorComponent.h"
#include "XRConnectorSocket.h"
#include "XRConnectorHologram.h"
#include "XRToolsUtilityFunctions.h"
#include "XRInteractionGrab.h"
#include "Net/UnrealNetwork.h"

UXRConnectorComponent::UXRConnectorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 0.25f;
	bAutoActivate = true;
	SetIsReplicatedByDefault(true);
	HologramClass = AXRConnectorHologram::StaticClass();
}


void UXRConnectorComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeOverlapBindings();
	InitializeInteractionBindings();
	MinDistanceToConnectSquared = FMath::Square(MinDistanceToConnect);
}
// Rember to deinit connections on Destroy() of this Component


void UXRConnectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	float MinSqDistance = FLT_MAX;
	UXRConnectorSocket* NewClosestSocket = nullptr;
	TArray<TWeakObjectPtr<UXRConnectorSocket>> RemoveSockets = {};
	for (auto OverlappedSocket : OverlappedSockets)
	{
		if (!OverlappedSocket.IsValid())
		{
			RemoveSockets.Add(OverlappedSocket);
			continue;
		}
		float DistanceSquared = FVector::DistSquared(Owner->GetActorLocation(), OverlappedSocket.Get()->GetComponentLocation());
		if (DistanceSquared < MinSqDistance && DistanceSquared <= MinDistanceToConnectSquared)
		{
			MinSqDistance = DistanceSquared;
			NewClosestSocket = OverlappedSocket.Get();
		}
	}

	if (ClosestSocket.IsValid() && ClosestSocket.Get() != NewClosestSocket)
	{
		if (ClosestSocket.IsValid())
		{
			SetHologramState(ClosestSocket.Get(), false);
		}
		if (NewClosestSocket)
		{
			SetHologramState(NewClosestSocket, true);
		}
		ClosestSocket = NewClosestSocket;
	}
	// If there was no closest socket before, but now we have one, set its state and update ClosestSocket
	else if (!ClosestSocket.IsValid() && NewClosestSocket)
	{
		SetHologramState(NewClosestSocket, true);
		ClosestSocket = NewClosestSocket;
	}

	for (auto Socket : RemoveSockets)
	{
		OverlappedSockets.Remove(Socket);
	}
}


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
	if (ClosestSocket.IsValid())
	{
		ConnectToSocket(ClosestSocket.Get());
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
		// Connect
		if (CurrentlyConnectedSocket)
		{
			LocalCachedConnectedSocket = CurrentlyConnectedSocket;
			GetOwner()->AttachToComponent(CurrentlyConnectedSocket, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
			CurrentlyConnectedSocket->RegisterConnection(this);
			OnConnected.Broadcast(this, CurrentlyConnectedSocket);
			SetComponentTickEnabled(false);
			return;
		}
	}

	// Disconnect
	if (LocalCachedConnectedSocket.IsValid())
	{
		UXRConnectorSocket* DetachingSocket = LocalCachedConnectedSocket.Get();
		LocalCachedConnectedSocket = nullptr;
		GetOwner()->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
		DetachingSocket->DeregisterConnection(this);
		OnDisconnected.Broadcast(this, DetachingSocket);
		SetComponentTickEnabled(true);
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
		if (!OverlappedSocket->IsConnectionAllowed(this))
		{
			return;
		}
		if (!OverlappedSockets.Contains(OverlappedSocket))
		{
			OverlappedSockets.Add(OverlappedSocket);
			ShowHologram(OverlappedSocket);
		}
		if (OverlappedSockets.Num() > 0 && !IsComponentTickEnabled())
		{
			SetComponentTickEnabled(true);
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
		if (OverlappedSockets.Num() == 0 && IsComponentTickEnabled())
		{
			SetComponentTickEnabled(false);
		}
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
	if (!bShowConnectorHologram || !HologramMesh)
	{
		return;
	}
	auto FoundHologram = AssignedHolograms.Find(InSocket);
	if (FoundHologram && FoundHologram->IsValid())
	{
		if (FoundHologram->Get()->Implements<UXRHologramInterface>())
		{
			IXRHologramInterface::Execute_ShowHologram(FoundHologram->Get(), this, HologramMesh);
		}
		return;
	}

	FVector Location = InSocket->GetComponentLocation();
	FRotator Rotation = InSocket->GetComponentRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* SpawnedHologram = GetWorld()->SpawnActor<AXRConnectorHologram>(HologramClass, Location, Rotation, SpawnParams);
	if (!SpawnedHologram)
	{
		return;
	}
	AssignedHolograms.Add(InSocket, SpawnedHologram);
	SpawnedHologram->AttachToComponent(InSocket, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
	if (SpawnedHologram->Implements<UXRHologramInterface>())
	{
		IXRHologramInterface::Execute_ShowHologram(SpawnedHologram, this, HologramMesh);
	}
}

void UXRConnectorComponent::ShowAllAvailableHolograms()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
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
	for (auto FoundSocket : FoundSockets)
	{
		if (!FoundSocket->IsConnectionAllowed(this))
		{
			continue;
		}
		if (!OverlappedSockets.Contains(FoundSocket))
		{
			OverlappedSockets.Add(FoundSocket);
		}
		ShowHologram(FoundSocket);
		if (OverlappedSockets.Num() > 0 && !IsComponentTickEnabled())
		{
			SetComponentTickEnabled(true);
		}
	}
}

void UXRConnectorComponent::HideAllHolograms()
{
	for (auto Socket : OverlappedSockets)
	{
		if (Socket.IsValid())
		{
			HideHologram(Socket.Get());
		}
	}
}

void UXRConnectorComponent::HideHologram(UXRConnectorSocket* InSocket)
{
	if (!InSocket)
	{
		return;
	}
	auto FoundHologram = AssignedHolograms.Find(InSocket);
	if (FoundHologram && FoundHologram->IsValid())
	{
		if (FoundHologram->Get()->Implements<UXRHologramInterface>())
		{
			IXRHologramInterface::Execute_HideHologram(FoundHologram->Get(), this);
		}
	}
	else
	{
		AssignedHolograms.Remove(InSocket);
	}
}

void UXRConnectorComponent::SetHologramState(UXRConnectorSocket* InSocket, bool IsPrioritized)
{
	if (!InSocket)
	{
		return;
	}
	auto FoundHologram = AssignedHolograms.Find(InSocket);
	if (FoundHologram && FoundHologram->IsValid())
	{
		auto Hologram = FoundHologram->Get();
		if (Hologram->Implements<UXRHologramInterface>())
		{
			IXRHologramInterface::Execute_SetHologramState(Hologram, this, IsPrioritized);
		}
	}
	else
	{
		AssignedHolograms.Remove(InSocket);
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
	ShowAllAvailableHolograms();
}

void UXRConnectorComponent::OnInteractionEnded(UXRInteractionComponent* Sender, UXRInteractorComponent* XRInteractorComponent)
{
	UXRConnectorSocket* OutConnectedSocket = {};
	ConnectToClosestOverlappedSocket(OutConnectedSocket);
	HideAllHolograms();
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
