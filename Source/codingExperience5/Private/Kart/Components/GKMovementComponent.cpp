// Fill out your copyright notice in the Description page of Project Settings.

#include "GKMovementComponent.h"
#include "Engine/World.h"

UGKMovementComponent::UGKMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Mass = 1000;
	MaxDrivingForce = 10000;
	MinimumTurningRadius = 10;
	DragCoefficient = 15;
	RollingResistenceCoefficient = 0.015;
}

FSyncMove UGKMovementComponent::CreateMove(float DeltaTime) {

	FSyncMove Move;
	Move.DeltaTime = DeltaTime;
	Move.SteeringThrow = SteeringThrow;
	Move.Throttle = Throttle;
	Move.TimeStamp = GetWorld()->TimeSeconds;

	return Move;
}

void UGKMovementComponent::SimulateMove(const FSyncMove& Move) {

	FVector Force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle;

	Force += GetAirResistence();
	Force += GetRollingResistence();

	FVector Acceleration = Force / Mass;

	Velocity += Acceleration * Move.DeltaTime;

	SetRotation(Move.DeltaTime, Move.SteeringThrow);

	SetLocationFromVelocity(Move.DeltaTime);
}

void UGKMovementComponent::SetRotation(const float &DeltaTime, float InSteeringThrow)
{
	float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MinimumTurningRadius * InSteeringThrow;
	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle);

	Velocity = RotationDelta.RotateVector(Velocity);

	GetOwner()->AddActorWorldRotation(RotationDelta, true);
}

void UGKMovementComponent::SetLocationFromVelocity(const float &DeltaTime)
{
	FVector Delta = Velocity * 100 * DeltaTime;

	FHitResult HitResult;
	GetOwner()->AddActorWorldOffset(Delta, true, &HitResult);

	if (HitResult.bBlockingHit) {

		Velocity = FVector::ZeroVector;
	}
}

FVector UGKMovementComponent::GetAirResistence() {

	return -Velocity.GetSafeNormal() * (Velocity.SizeSquared()) * DragCoefficient;
}

FVector UGKMovementComponent::GetRollingResistence() {

	// Convert from cm to m and to positive number
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;
	float NormalForce = Mass * AccelerationDueToGravity;

	return -Velocity.GetSafeNormal() * RollingResistenceCoefficient * NormalForce;
}