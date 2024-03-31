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
	Enabled UMETA(DisplayName = "Enabled"),
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


protected:
	virtual void BeginPlay() override;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	/*
	* Socket State:
	* Enabled: Accepts new connections
	* Disabled: no new connections will be allowed
	*/
	UPROPERTY(Editanywhere, Category = "XRConnectorSocket")
	EXRConnectorSocketState SocketState = EXRConnectorSocketState::Enabled;

	/*
	* Define compatible XRConnectorSockets via a Name ID.
	*/
	UPROPERTY(Editanywhere, Category = "XRConnectorSocket")
	TArray<FName> CompatibleConnectorIDs = { "Default" };

	/*
	* Set how many XRConnectors are allowed to be connected simultaneously to this Socket.
	*/
	UPROPERTY(Editanywhere, Category = "XRConnectorSocket")
	int32 AllowedConnections = 1;


private:
	TArray<TWeakObjectPtr<UXRConnectorComponent>> AttachedXRConnectors = {};
		
};
