
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XRReplicatedPhysicsComponent.generated.h"


USTRUCT(BlueprintType)
struct FXRPhysicsSnapshot
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 ID = 0;

	UPROPERTY(BlueprintReadWrite)
	uint8 bIsInteractedWith = false;

	UPROPERTY(BlueprintReadWrite)
	FVector Location = {};

	UPROPERTY(BlueprintReadWrite)
	FRotator Rotation = {};
};


UCLASS( ClassGroup=(XRToolkit), meta=(BlueprintSpawnableComponent) )
class XR_TOOLKIT_API UXRReplicatedPhysicsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UXRReplicatedPhysicsComponent();

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// API
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Client: return the latest Snapshot that this Client received from the server. 
	 * Server: return the Snapshot the server is currently replicating to the clients. 
	 **/
	UFUNCTION(BlueprintPure, Category = "XRCore|Physics Replication")
	FXRPhysicsSnapshot GetLatestSnapshot() const;

	/**
	 * Marks the owner as InteractedWith, increasing/decreasing the replication interval. 
	 * Only Relevant on the Server.
	 * Set in XRCore Plugin Settings DefaultReplicationInterval and InteractedReplicationInterval
	 **/
	UFUNCTION(BlueprintCallable, Category = "XRCore|Physics Replication")
	void SetInteractedWith(bool bInInteracedWith);

	/**
	 * Is the owning actor marked as InteractedWith, increasing/decreasing the replication interval. 
	 **/
	UFUNCTION(BlueprintPure, Category = "XRCore|Physics Replication")
	bool GetInteractedWith() const;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Colliders/Sim on Owner
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Will set Simulate Physics on the Cached PhysicsMeshComponents.
	 * Not Replicated - Can be used to set this on individual clients or the server.
	 * @param InSimulatePhysics Whether to enable or disable physics simulation.
	 **/
	UFUNCTION(BlueprintCallable, Category = "XRCore|Physics Replication")
	void SetSimulatePhysicsOnOwner(bool InSimulatePhysics);

	/**
	 * Get the Owning Actors Velocity.
	 **/
	UFUNCTION(BlueprintPure, Category = "XRCore|Replicated Physics")
	float GetActorVelocity() const;

	/**
	 * Default tag used to initially register all MeshComponents on the owning actor. 
	 If the root component is a StaticMeshComponent, it will always be registered, even without any applied tag.
	 **/
	UPROPERTY(EditAnywhere, Category = "XRCore|Physics Replication")
	FName RegisterMeshComponentsWithTag = "";

	/**
	 * Find and Cache all UMeshComponents with the given tag - can be accessed with GetRegisteredMeshComponents.
	 * Does not set Simulate Physics. Use SetSimulatePhysics() for this.
	 * @param InComponentTag Only components with this tag will be cached.
	 **/
	UFUNCTION(BlueprintCallable, Category = "XRCore|Physics Replication")
	void RegisterPhysicsMeshComponents(FName InComponentTag);

	/**
	 * Return all cached UMeshComponents that are used for Physics calculations.
	 * Can return nullptr.
	 * @return An array of mesh components.
	 **/
	UFUNCTION(BlueprintPure, Category = "XRCore|Physics Replication")
	TArray<UMeshComponent*> GetRegisteredMeshComponents() const;

	/**
	 * Disable the interpolation between the transforms received from the server on the client. This allows monitoring
	 * and adjusting the replication interval values directly.
	 **/
	UPROPERTY(EditDefaultsOnly, Category = "XRCore|Physics Replication")
	bool bDebugDisableClientInterpolation = false;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void ServerTick(float DeltaTime);
	UFUNCTION()
	void ClientTick(float DeltaTime);

	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	UFUNCTION()
	void OnActivated(UActorComponent* Component, bool bReset);
	UFUNCTION()
	void OnDeactivated(UActorComponent* Component);

	UPROPERTY()
	bool bIsSimulatingPhysics = true;

	UFUNCTION()
	void DelayedPhysicsSetup();

private:
	UFUNCTION(Server, Reliable)
	void Server_SetCachedSnapshot(FXRPhysicsSnapshot InCachedSnapshot);

	float AccumulatedTime = 0.0f;
	float InterpolationAlpha = 0.0f;

	bool bIsInteractedWith = false;
	float DefaultReplicationInterval = 0.0f;
	float InteractedReplicationInterval = 0.0f;

	UPROPERTY(Replicated)
	FXRPhysicsSnapshot LatestSnapshot = {};
	UPROPERTY(ReplicatedUsing = OnRep_CachedSnapshot)
	FXRPhysicsSnapshot CachedSnapshot = {};


	FXRPhysicsSnapshot ClientActiveSnapshot = {};

	UFUNCTION()
	bool IsSequenceIDNewer(uint32 InID1, uint32 InID2) const;

	UFUNCTION()
	void OnRep_CachedSnapshot();

	UPROPERTY()
	TArray<UMeshComponent*> RegisteredMeshComponents = {};

};

