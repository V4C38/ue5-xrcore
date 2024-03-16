
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XRReplicatedPhysicsComponent.generated.h"


USTRUCT(BlueprintType)
struct FXRPhysicsReplicationData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Location;

	UPROPERTY(BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(BlueprintReadWrite)
	float ReplicationInterval;
};


class UXRReplicatedPhysicsComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnServerPhysicsReplicationStateChanged, UXRReplicatedPhysicsComponent*, Sender, bool, bServerIsReplicatingPhysics);
class UXRReplicatedPhysicsComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnServerReplicatedPhysicsData, UXRReplicatedPhysicsComponent*, Sender, FXRPhysicsReplicationData, ServerPhysicsData);

UCLASS( ClassGroup=(XRToolkit), meta=(BlueprintSpawnableComponent) )
class XR_TOOLKIT_API UXRReplicatedPhysicsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UXRReplicatedPhysicsComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Delegates for server physics replication state and data
	UPROPERTY(BlueprintAssignable, Category = "XRCore|Physics System|Delegates")
	FOnServerPhysicsReplicationStateChanged OnServerPhysicsReplicationStateChanged;
	UPROPERTY(BlueprintAssignable, Category = "XRCore|Physics System|Delegates")
	FOnServerReplicatedPhysicsData OnServerReplicatedPhysicsData;


	/**
	 * Enables or disables the server-side replication of the actor's transform to all clients.
	 * Also sets clients to interpolate to this server transform.
	 *
	 * @param bReplicatePhysics A boolean indicating whether to enable server-side replication.
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="XRCore|Physics System")
	void Server_SetServerReplicatePhysics(bool bReplicatePhysics);

	/**
	 * Checks if the server is currently replicating its transform to clients.
	 *
	 * @return Boolean value indicating whether server-side replication is enabled.
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Physics System")
	bool IsServerReplicatingPhysics() const;

	/**
	 * Find and Cache all UMeshComponents with the given tag - can be accessed with GetPhysicsMeshComponents.
	 * Does not set Simulate Physics. Use SetSimulatePhysics() for this. 
	 * @param InComponentTag Only components with this tag will be cached. 
	 **/
	UFUNCTION(BlueprintCallable, Category="XRCore|Physics System")
	void CachePhysicsMeshComponents(FName InComponentTag);

	/**
	 * Return all cached UMeshComponents that are used for Physics calculations.
	 * Can return nullptr.
	 * @return An array of mesh components.
	 **/
	UFUNCTION(BlueprintPure, Category="XRCore|Physics System")
	TArray<UMeshComponent*> GetPhysicsMeshComponents() const;
	
	/**
	 * Will set Simulate Physics on the Cached PhysicsMeshComponents.
	 * Not Replicated - Can be used to set this on individual clients or the server.  
	 * @param InSimulatePhysics Whether to enable or disable physics simulation.
	
	 **/
	UFUNCTION(BlueprintCallable, Category="XRCore|Physics System")
	void SetComponentsSimulatePhysics(bool InSimulatePhysics);


	/**
	 * Gets the latest server-side physics data for the owning actor.
	 *
	 * @return The latest physics replication data.
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Physics System")
	FXRPhysicsReplicationData GetServerPhysicsData() const;

	/**
	 * Disable the interpolation between the transforms received from the server on the client. This allows monitoring 
	 * and adjusting the replication interval values directly.
	 **/
	UPROPERTY(EditDefaultsOnly, Category = "XRCore|Physics System|Debug")
	bool bDisableClientInterpolation = false;

	/**
	 * Gets the dynamically adjusted replication interval.
	 *
	 * @return The current dynamic replication interval.
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Physics System")
	float GetDynamicReplicationInterval();

	/**
	 * The lowest update rate at which the server replicates the actors transform to all clients. In Seconds.
	 **/
	UPROPERTY(EditAnywhere, Category = "XRCore|Physics System")
	float ReplicationIntervalMax = 0.05f;
	float GetDefaultReplicationIntervalMax() const;
	/**
	 * The highest update rate at which the server replicates the actors transform to all clients. In Seconds.
	 **/
	UPROPERTY(EditAnywhere, Category = "XRCore|Physics System")
	float ReplicationIntervalMin = 0.02f;
	float GetDefaultReplicationIntervalMin() const;
	/**
	 * Whenever the Actors velocity is higher than this treshold, use the ShortestReplicationInterval
	 **/
	UPROPERTY(EditAnywhere, Category = "XRCore|Physics System")
	float VelocityThreshold = 350.0f;
	float GetDefaultVelocityThreshold() const;


protected:
	virtual void BeginPlay() override;

private:


	UFUNCTION(Category = "XRCore|Physics System")
	void ServerTick(float DeltaTime);
	UFUNCTION(Category = "XRCore|Physics System")
	void ClientTick(float DeltaTime);
	
	UFUNCTION(Server, Unreliable, Category="XRCore|Physics System")
	void Server_SetServerPhysicsData(FXRPhysicsReplicationData NewPhysicsData);

	UFUNCTION(Category = "XRCore|Physics System")
	void UpdateClientPhysicsData();

	UFUNCTION(BlueprintPure, Category = "XRCore|Physics System")
	float GetActorVelocity() const;
	
	float AccumulatedTime = 0.0f;
	float InterpolationAlpha = 0.0f;

	UPROPERTY()
	float DynamicReplicationInterval;

	UPROPERTY()
	FXRPhysicsReplicationData LastClientPhysicsData;
	UPROPERTY(ReplicatedUsing = OnRep_ServerPhysicsData)
	FXRPhysicsReplicationData ServerPhysicsData;
	UFUNCTION()
	void OnRep_ServerPhysicsData();

	UPROPERTY()
	TArray<UMeshComponent*> PhysicsMeshComponents = {};

	UPROPERTY(ReplicatedUsing = OnRep_ServerReplicatePhysics)
	bool bServerReplicatePhysics = false;
	UFUNCTION()
	void OnRep_ServerReplicatePhysics();

};

