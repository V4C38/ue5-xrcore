// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/NoExportTypes.h"
#include "Engine/StaticMeshActor.h"
#include "TimerManager.h"
#include "XRConnectorComponent.generated.h"

class UXRConnectorComponent;
class UXRConnectorSocket;
class AXRConnectorHologram;
class UXRInteractionComponent;
class UXRInteractionGrab;
class UXRInteractorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnConnected, UXRConnectorComponent*, Sender, UXRConnectorSocket*, XRConnectorSocket);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDisconnected, UXRConnectorComponent*, Sender, UXRConnectorSocket*, XRConnectorSocket);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XR_TOOLKIT_API UXRConnectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UXRConnectorComponent();

	UPROPERTY(BlueprintAssignable, Category = "XRConnector")
	FOnConnected OnConnected;
	UPROPERTY(BlueprintAssignable, Category = "XRConnector")
	FOnDisconnected OnDisconnected;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// API
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/*
	* Connect to the specified Socket - Replicated.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRConnector")
	bool ConnectToSocket(UXRConnectorSocket* InSocket);

	/*
	* Attempt to connect to the closest Socket - Replicated.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRConnector")
	bool ConnectToClosestOverlappedSocket(UXRConnectorSocket*& OutSocket);

	/*
	* Disconnect from the currently connected Socket - Replicated.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRConnector")
	void DisconnectFromSocket();

	/*
	* Return all IDs this XRConnector is compatible with.
	*/
	UFUNCTION(BlueprintPure, Category = "XRConnector")
	FName GetConnectorID() const;

	/*
	* Return true if this Connector attached to a Socket. Also returns the Socket if connected.
	*/
	UFUNCTION(BlueprintPure, Category = "XRConnector")
	bool IsConnected(UXRConnectorSocket*& OutConnectedSocket) const;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Hologram
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/*
	* Shows the Hologram for a specific Socket.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRConnector")
	void ShowHologram(UXRConnectorSocket* InSocket);

	/*
	* Shows a Hologram for each overlapped Socket.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRConnector")
	void ShowAllAvailableHolograms();

	/*
	* Hides the Hologram for a specific Socket if it is visible.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRConnector")
	void HideHologram(UXRConnectorSocket* InSocket);

	/*
	* Hides all Holograms.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRConnector")
	void HideAllHolograms();

	/*
	* Set this Hologram to be Prioritized, affecting visual indicators only.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRConnector")
	void SetHologramState(UXRConnectorSocket* InSocket, bool IsPrioritized);



protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Server, Reliable, Category = "XRConnector")
	void Server_SetConnectedSocket(UXRConnectorSocket* InSocket);

	UFUNCTION()
	void SetupConnection();


	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/*
	* If true, will find the highest priority XRInteractionGrab on this Actor and bind to the OnInteractionStarted and  OnInteractionEnded events.
	* Will automatically ConnectToClosestOverlappedSocket when ending grab.
	*/
	UPROPERTY(Editanywhere, Category = "XRConnector")
	bool bAutoBindToGrabInteraction = false;

	/*
	* ID that determines compatiblility with XRConnectorSockets.
	*/
	UPROPERTY(Editanywhere, Category = "XRConnector")
	FName ConnectorID = "Default";

	/*
	* A connection will be established when the distance between the OwningActor and Socket is below this Treshold.
	* Note: The OwningActor must be within the Collision of the Socket for this to work.
	*/
	UPROPERTY(Editanywhere, Category = "XRConnector")
	float MinDistanceToConnect = 25.0;
	float MinDistanceToConnectSquared = 0.0f;

	/*
	* When enabled, shows a Hologram in the location of each Socket that the OwningActor is overlapping. Can also be triggered manually. 
	* This can cause performance issues, as a UStatickMeshActor is spawned for each hologram. 
	* DevNote: if this becomes a concern, an ObjectPool should be used instead of spawning Actors at runtime.
	*/
	UPROPERTY(Editanywhere, Category = "XRConnector|Hologram")
	bool bShowConnectorHologram = true;

	/*
	* Set the type of hologram that should be spawned.
	* This must implement the Interface IXRHologramInterface. See XRConnectorHologram as an example.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XRConnector|Hologram")
	TSubclassOf<AActor> HologramClass;

	/*
	* Define the StaticMesh that should be spawned as a Hologram in the location of available Sockets. 
	* Scale is determined by the OwningActor.
	*/
	UPROPERTY(Editanywhere, Category = "XRConnector|Hologram")
	UStaticMesh* HologramMesh = {};


private:	
	UPROPERTY(ReplicatedUsing = OnRep_ConnectedSocket)
	TWeakObjectPtr<UXRConnectorSocket> ConnectedSocket = {};
	TWeakObjectPtr<UXRConnectorSocket> LocalCachedConnectedSocket = {};

	UFUNCTION()
	void OnRep_ConnectedSocket();


	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Overlap Logic
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	float LastOverlapUpdate = 0.0f;
	void InitializeOverlapBindings();
	TArray<UPrimitiveComponent*> OwnerCollisions = {};
	TArray<TWeakObjectPtr<UXRConnectorSocket>> OverlappedSockets = {};
	TWeakObjectPtr<UXRConnectorSocket> ClosestSocket = {};
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Hologram
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	TMap<TWeakObjectPtr<UXRConnectorSocket>, TWeakObjectPtr<AActor>> AssignedHolograms = {};

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Interaction Mappings
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InitializeInteractionBindings();
	UXRInteractionGrab* BoundGrabComponent = nullptr;
	UFUNCTION()
	void OnInteractionStarted(UXRInteractionComponent* Sender, UXRInteractorComponent* XRInteractorComponent);
	UFUNCTION()
	void OnInteractionEnded(UXRInteractionComponent* Sender, UXRInteractorComponent* XRInteractorComponent);

};
