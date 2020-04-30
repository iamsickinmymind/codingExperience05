// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GKMovementComponent.generated.h"

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

/*
CUSTOM MOVEMENT COMPONENT
*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CODINGEXPERIENCE5_API UGKMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UGKMovementComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SimulateMove(const FSyncMove& Move);
	FSyncMove CreateMove(float DeltaTime);

	FORCEINLINE FVector GetVelocity() const { return Velocity; };
	FORCEINLINE void SetVelocity(FVector NewVelocity) { Velocity = NewVelocity; };
	FORCEINLINE void SetThrottle(float NewValue) { Throttle = NewValue; };
	FORCEINLINE void SetSteeringThrow(float NewValue) { SteeringThrow = NewValue; };

protected:

	virtual void BeginPlay() override;

private:

	FVector GetAirResistence();
	FVector GetRollingResistence();
	void SetLocationFromVelocity(const float &DeltaTime);
	void SetRotation(const float &DeltaTime, float InSteeringThrow);

protected:

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

	FVector Velocity;
	float SteeringThrow;
	float Throttle;
};