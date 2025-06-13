#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/StaticMeshActor.h"
#include "TimerManager.h"
#include "UObject/NoExportTypes.h"

#include "XRConnectorComponent.generated.h"

class UXRConnectorComponent;
class UXRConnectorSocket;
class AXRConnectorHologram;
class UXRInteractionComponent;
class UXRInteractionGrab;
class UXRInteractorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnConnected, UXRConnectorComponent*, Sender, UXRConnectorSocket*, XRConnectorSocket);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDisconnected, UXRConnectorComponent*, Sender, UXRConnectorSocket*, XRConnectorSocket);

// ================================================================================================================================================================
// Connects the owning Actor to an XRConnectionSocket 
// ================================================================================================================================================================
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
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "XRConnector")
	void Server_ConnectToSocket(UXRConnectorSocket* InSocket);

	/*
	* Attempt to connect to the closest Socket - Replicated.
	*/
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "XRConnector")
	void Server_ConnectToClosestOverlappedSocket();

	/*
	* Disconnect from the currently connected Socket - Replicated.
	*/
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "XRConnector")
	void Server_DisconnectFromSocket();

	/*
	* Return the closest of all currently overlapped sockets. 
	*/
	UFUNCTION(BlueprintPure, Category = "XRConnector")
	UXRConnectorSocket* GetClosestOverlappedSocket();

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
	* Shows a Hologram for each overlapped Socket.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRConnector")
	void ShowAllAvailableHolograms();

	/*
	* Hides all Holograms.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRConnector")
	void HideAllHolograms();

	/*
	* Set this Hologram to be Prioritized, affecting visual indicators only.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRConnector")
	void SetHologramState(UXRConnectorSocket* InSocket, EXRHologramState InState);


protected:
	virtual void BeginPlay() override; 
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/*
	* ID that determines compatiblility with XRConnectorSockets.
	*/
	UPROPERTY(EditAnywhere, Category = "XRConnector")
	FName ConnectorID = "Default";

	/*
	* A connection will be established when the distance between the OwningActor and Socket is below this Treshold.
	* Note: The OwningActor must be within the Collision of the Socket for this to work.
	*/
	UPROPERTY(Editanywhere, Category = "XRConnector", meta = (ClampMin = "0.0"))
	float MinDistanceToConnect = 10.0;
	float MinDistanceToConnectSquared = 0.0f;

	/*
	* If true, will find the highest priority XRInteractionGrab on this Actor and bind to the OnInteractionStarted and  OnInteractionEnded events.
	* Will automatically ConnectToClosestOverlappedSocket when ending grab.
	* Only shows holograms for this Connection while the GrabInteraction is active.
	*/
	UPROPERTY(EditAnywhere, Category = "XRConnector")
	bool bAutoBindToGrabInteraction = false;


	/*
	* When enabled, shows a Hologram in the location of each Socket that the OwningActor is overlapping. Can also be triggered manually. 
	* This can cause performance issues, as a UStatickMeshActor is spawned for each hologram.
	* DevNote: if this becomes a concern, an ObjectPool should be used instead of spawning Actors at runtime.
	*/
	UPROPERTY(EditAnywhere, Category = "XRConnector|Hologram")
	bool bShowConnectorHologram = true;


	/*
	* Set the type of hologram that should be spawned.
	* This must implement the Interface IXRHologramInterface. See BP_XRConnectorHologram as an example.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XRConnector|Hologram", meta = (MustImplement = "XRHologramInterface"))
	TSubclassOf<AActor> HologramClass;

	/*
	* Define the StaticMesh that should be spawned as a Hologram in the location of available Sockets.
	*/
	UPROPERTY(EditAnywhere, Category = "XRConnector|Hologram")
	UStaticMesh* HologramMesh;

	/*
	* Set the scale of the StaticMesh in the Hologram.
	*/
	UPROPERTY(EditAnywhere, Category = "XRConnector|Hologram", meta = (ClampMin = "0.0"))
	float HologramScale = 1.0;

	UPROPERTY(ReplicatedUsing = OnRep_ConnectedSocket)
	UXRConnectorSocket* ConnectedSocket = {};

	UFUNCTION()
	void OnRep_ConnectedSocket();

	UPROPERTY()
	TWeakObjectPtr<UXRConnectorSocket> PreviouslyConnectedSocket = {};

private:	

	// Overlap Logic
	void InitializeOverlapBindings();
	TArray<UPrimitiveComponent*> OwnerCollisions = {};
	TArray<TWeakObjectPtr<UXRConnectorSocket>> OverlappedSockets = {};
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Hologram
	TMap<TWeakObjectPtr<UXRConnectorSocket>, TWeakObjectPtr<AActor>> AssignedHolograms = {};

	// Interaction Mappings
	void InitializeInteractionBindings();
	UXRInteractionGrab* BoundGrabComponent = nullptr;

	// Time in seconds the connector will interpolate towards the socket when a connection is established.
	UPROPERTY(EditAnywhere, Category = "XRConnector", meta = (ClampMin = "0.0"))
	float EstablishConnectionTime = 0.5f;
	FTimerHandle EstablishConnectionTimer;


	UFUNCTION()
	void AttachToSocket();

	UFUNCTION()
	void DeferredAttachToSocket();

	UFUNCTION()
	void DetachFromSocket();

	UFUNCTION()
	void OnInteractionStarted(UXRInteractionComponent* Sender, UXRInteractorComponent* XRInteractorComponent);
	UFUNCTION()
	void OnInteractionEnded(UXRInteractionComponent* Sender, UXRInteractorComponent* XRInteractorComponent);

};
