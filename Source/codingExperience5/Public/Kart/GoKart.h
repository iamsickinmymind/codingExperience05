// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

USTRUCT(BlueprintType)
struct FSyncMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Throttle;

	UPROPERTY()
	float SteeringThrow;

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float TimeStamp;
};

USTRUCT(BlueprintType)
struct FSyncState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FSyncMove LastMove;
};

UCLASS()
class CODINGEXPERIENCE5_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetLocationFromVelocity(const float &DeltaTime);
	void SetRotation(const float &DeltaTime, float InSteeringThrow);

	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FSyncMove Move);

	FVector GetAirResistence();
	FVector GetRollingResistence();

	UFUNCTION()
	void OnRep_ServerState();

private:

	void SimulateMove(const FSyncMove& Move);
	FSyncMove CreateMove(float DeltaTime);
	void ClearAcknowledgedMoves(FSyncMove LastMove);

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USkeletalMeshComponent* KartMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* BoxCollision = nullptr;

	// Mass in kg
	UPROPERTY(EditAnywhere, Category = "Movement")
	float Mass;

	// Max force applied to car when max throttle in Newtons
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxDrivingForce;

	// Minimum radius of the car turning circle at full lock
	// Meters
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MinimumTurningRadius;

	// Higher means more drag
	// Drag in Kg / m
	UPROPERTY(EditAnywhere, Category = "Movement")
	float DragCoefficient;

	// Rolling resistence
	// Higher is higher resistence
	// Resistence in Kg / m
	UPROPERTY(EditAnywhere, Category = "Movement")
	float RollingResistenceCoefficient;

private:

	UPROPERTY(ReplicatedUsing=OnRep_ServerState)
	FSyncState ServerState;

	FVector Velocity;
	float SteeringThrow;
	float Throttle;

	TArray<FSyncMove> UnacknowledgedMoves;
};
