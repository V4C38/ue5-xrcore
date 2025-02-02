// Fill out your copyright notice in the Description page of Project Settings.


#include "XRConnectorComponent.h"
#include "XRConnectorSocket.h"
#include "XRConnectorHologram.h"
#include "XRToolsUtilityFunctions.h"
#include "XRInteractionGrab.h"
#include "XRReplicatedPhysicsComponent.h"
#include "Net/UnrealNetwork.h"

UXRConnectorComponent::UXRConnectorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bAutoActivate = true;
	SetIsReplicatedByDefault(true);

	// Assign default hologram class from settings if not already set.
	const UXRCoreSettings* DefaultSettings = GetDefault<UXRCoreSettings>();
	if (DefaultSettings && !HologramClass)
	{
		HologramClass = DefaultSettings->DefaultHologramClass.Get();
	}
	else
	{
		HologramClass = AXRConnectorHologram::StaticClass();
	}
}

void UXRConnectorComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeOverlapBindings();
	InitializeInteractionBindings();
	MinDistanceToConnectSquared = FMath::Square(MinDistanceToConnect);
}

void UXRConnectorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (ConnectedSocket.IsValid())
	{
		if (PreviouslyConnectedSocket.IsValid())
		{
			PreviouslyConnectedSocket.Get()->DeregisterConnection(this);
		}
		HideAllHolograms();
	}
}


void UXRConnectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// Interpolate to Connected Socket when establishing connection
	if (EstablishConnectionTimer.IsValid() && ConnectedSocket.IsValid())
	{
		float TimeLeft = GetWorld()->GetTimerManager().GetTimerRemaining(EstablishConnectionTimer);
		float Alpha = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, EstablishConnectionTime), FVector2D(1.0f, 0.0f), TimeLeft);

		FVector InterpolatedLocation = FMath::Lerp(Owner->GetActorLocation(), ConnectedSocket.Get()->GetComponentLocation(), Alpha);
		FQuat InterpolatedRotation = FMath::Lerp(Owner->GetActorQuat(), ConnectedSocket.Get()->GetComponentQuat(), Alpha);
		Owner->SetActorLocation(InterpolatedLocation);
		Owner->SetActorRotation(InterpolatedRotation);
	}

	// Update closest Sockets & Holograms
	LastOverlapUpdate += DeltaTime;
	if (LastOverlapUpdate > 0.25f)
	{
		float MinSqDistance = FLT_MAX;
		UXRConnectorSocket* NewClosestSocket = nullptr;
		TArray<TWeakObjectPtr<UXRConnectorSocket>> InvalidSockets = {};
		for (auto OverlappedSocket : OverlappedSockets)
		{
			if (!OverlappedSocket.IsValid())
			{
				InvalidSockets.Add(OverlappedSocket);
				continue;
			}
			if (OverlappedSocket.Get()->GetSocketState() == EXRConnectorSocketState::Disabled)
			{
				HideHologram(OverlappedSocket.Get());
				InvalidSockets.Add(OverlappedSocket);
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

		for (auto InvalidSocket : InvalidSockets)
		{
			OverlappedSockets.Remove(InvalidSocket);
		}
		LastOverlapUpdate = 0.0f;
	}
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Connection API
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
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}
	if (InSocket->IsConnectionAllowed(this))
	{
		Server_SetConnectedSocket(nullptr);
		if (GetWorld()->GetNetMode() == NM_Standalone)
		{
			InternalDetachFromSocket();
		}
	}
	Server_SetConnectedSocket(InSocket);
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		ConnectedSocket = InSocket;
		InternalRequestAttachToSocket();
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
		InternalDetachFromSocket();
	}
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Connection Logic
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRConnectorComponent::InternalRequestAttachToSocket()
{
	if (EstablishConnectionTime == 0.0f)
	{
		InternalAttachToSocket();
	}
	else
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(EstablishConnectionTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(EstablishConnectionTimer);
		}
		GetWorld()->GetTimerManager().SetTimer(EstablishConnectionTimer, this, &UXRConnectorComponent::InternalAttachToSocket, EstablishConnectionTime, false);
	}
}

void UXRConnectorComponent::InternalAttachToSocket() 
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	if (!ConnectedSocket.IsValid())
	{
		return;
	}
	UXRReplicatedPhysicsComponent* XRPhysicsComponent = GetOwner()->FindComponentByClass<UXRReplicatedPhysicsComponent>();
	if (XRPhysicsComponent)
	{
		XRPhysicsComponent->SetActive(false);
	}
	GetOwner()->AttachToComponent(ConnectedSocket.Get(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
	ConnectedSocket.Get()->RegisterConnection(this);
	OnConnected.Broadcast(this, ConnectedSocket.Get());
	PreviouslyConnectedSocket = ConnectedSocket.Get();
}

void UXRConnectorComponent::InternalDetachFromSocket()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	if (!PreviouslyConnectedSocket.IsValid())
	{
		return;
	}
	GetOwner()->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
	PreviouslyConnectedSocket.Get()->DeregisterConnection(this);
	OnDisconnected.Broadcast(this, PreviouslyConnectedSocket.Get());
	PreviouslyConnectedSocket = nullptr;

	UXRReplicatedPhysicsComponent* XRPhysicsComponent = GetOwner()->FindComponentByClass<UXRReplicatedPhysicsComponent>();
	if (XRPhysicsComponent)
	{
		XRPhysicsComponent->SetActive(true);
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
		UXRConnectorSocket* OutSocket = nullptr;
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
	if (!InSocket->IsHologramAllowed())
	{
		return;
	}

	// Only show the Hologram in Bound to Grab mode when the GrabInteraction is currently active
	if (bAutoBindToGrabInteraction)
	{
		if (!BoundGrabComponent)
		{
			return;
		}
		if (!BoundGrabComponent->IsInteractedWith())
		{
			return;
		}
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
	auto FoundInteractionComp = UXRToolsUtilityFunctions::GetXRInteractionByPriority(InteractionComponents, nullptr, 0, EXRInteractionPrioritySelection::LowerEqual,5);
	if (FoundInteractionComp)
	{
		FoundInteractionComp->OnInteractionStarted.AddDynamic(this, &UXRConnectorComponent::OnInteractionStarted);
		FoundInteractionComp->OnInteractionEnded.AddDynamic(this, &UXRConnectorComponent::OnInteractionEnded);
		BoundGrabComponent = Cast<UXRInteractionGrab>(FoundInteractionComp);
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
	if (ConnectedSocket.IsValid())
	{
		InternalRequestAttachToSocket();
	}
	else
	{
		InternalDetachFromSocket();
	}
}

void UXRConnectorComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRConnectorComponent, ConnectedSocket);
}
