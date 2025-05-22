// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "XRConnectorSocket.generated.h"

class UXRConnectorComponent;
class UXRConnectorSocket;


UENUM(BlueprintType)
enum class EXRConnectorSocketState : uint8
{
	Available UMETA(DisplayName = "Available"),
	Occupied UMETA(DisplayName = "Occupied"),
	Disabled UMETA(DisplayName = "Disabled"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSocketConnected, UXRConnectorSocket*, Sender, UXRConnectorComponent*, XRConnector);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSocketDisconnected, UXRConnectorSocket*, Sender, UXRConnectorComponent*, XRConnector);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XR_TOOLKIT_API UXRConnectorSocket : public USphereComponent
{
	GENERATED_BODY()

public:	
	UXRConnectorSocket();

	UPROPERTY(BlueprintAssignable, Category = "XRConnectorSocket")
	FOnSocketConnected OnSocketConnected;
	UPROPERTY(BlueprintAssignable, Category = "XRConnectorSocket")
	FOnSocketDisconnected OnSocketDisconnected;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// API
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/*
	* Return if this Socket currently enabled.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRConnectorSocket")
	void SetSocketState(EXRConnectorSocketState InSocketState);

	/*
	* Return the current state of this socket:
	* Enabled: Accepts new connections
	* Disabled: no new connections will be allowed
	*/
	UFUNCTION(BlueprintPure, Category = "XRConnectorSocket")
	EXRConnectorSocketState GetSocketState() const;

	/*
	* Return this sockets ID.
	*/
	UFUNCTION(BlueprintPure, Category = "XRConnectorSocket")
	FString GetSocketID() const;

	/*
	* Return all currently connected Components. 
	*/
	UFUNCTION(BlueprintPure, Category = "XRConnectorSocket")
	TArray<UXRConnectorComponent*> GetAttachedXRConnectors() const;

	/*
	* Return all XRConnectorIDs that can be connected to this Socket.
	*/
	UFUNCTION(BlueprintPure, Category = "XRConnectorSocket")
	TArray<FName> GetCompatibleIDs() const;

	/*
	* Determine if the requesting XRConnectorComponent is compatible with this socket and if this socket has available space for another attachment.
	*/
	UFUNCTION(BlueprintPure, Category = "XRConnectorSocket")
	bool IsConnectionAllowed(UXRConnectorComponent* InXRConnectorComponent) const;

	/*
	* Register a Connection.
	* Called by the XRConnectorComponent when it attaches to this Socket.
	*/
	UFUNCTION(Category = "XRConnectorSocket")
	void RegisterConnection(UXRConnectorComponent* InConnectorComponent);

	/*
	* Deregister a Connection.
	* Called by the XRConnectorComponent when it detaches from this Socket.
	*/
	UFUNCTION(Category = "XRConnectorSocket")
	void DeregisterConnection(UXRConnectorComponent* InConnectorComponent);

	/*
	* Can this socket have a hologram associated with it? (No if disabled or bCanHaveHologram is false)
	*/
	UFUNCTION(BlueprintPure, Category = "XRConnectorSocket")
	bool IsHologramAllowed() const;


protected:
	virtual void BeginPlay() override;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/*
	* Define an ID for this socket to handle multiple sockets on one Actor. Used for identifying sockets throughout sessions for Persistence / SaveGame data.
	*/
	UPROPERTY(EditAnywhere, Category = "XRConnectorSocket")
	FString ID = "ID";

	/*
	* Socket State:
	* Available: Accepts new connections
	* Occupied: Currently occupied by another ConnectorComponent
	* Disabled: No new connections will be allowed
	*/
	UPROPERTY(Editanywhere, Category = "XRConnectorSocket")
	EXRConnectorSocketState DefaultSocketState = EXRConnectorSocketState::Available;
	EXRConnectorSocketState SocketState = EXRConnectorSocketState::Available;

	/*
	* Define compatible XRConnectorSockets via a Name ID.
	* Leave empty to accept any XRConnector.
	*/
	UPROPERTY(Editanywhere, Category = "XRConnectorSocket")
	TArray<FName> CompatibleConnectorIDs = {};

	/*
	* Never allow a hologram to be shown for this socket if true.
	*/
	UPROPERTY(Editanywhere, Category = "XRConnectorSocket")
	bool bSupressHologram = false;

private:
	TArray<TWeakObjectPtr<UXRConnectorComponent>> AttachedXRConnectors = {};
		
};
