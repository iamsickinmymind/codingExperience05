// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKart.h"

#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Net/UnrealNetwork.h"

#include "DrawDebugHelpers.h"

AGoKart::AGoKart()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(FName("BoxCollision")); 
	RootComponent = BoxCollision;

	KartMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Mesh"));
	KartMesh->SetupAttachment(GetRootComponent());

	Mass = 1000;
	MaxDrivingForce = 10000;
	MinimumTurningRadius = 10;
	DragCoefficient = 15;
	RollingResistenceCoefficient = 0.015;

	NetUpdateFrequency = 25.f;
	SetReplicates(true);
}

void AGoKart::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_AutonomousProxy) {

		FSyncMove Move = CreateMove(DeltaTime);
		SimulateMove(Move);
		UnacknowledgedMoves.Add(Move);
		Server_SendMove(Move);
	}
	// we are the server and in control of the pawn
	if (Role == ROLE_Authority && GetRemoteRole() == ROLE_SimulatedProxy) {

		FSyncMove Move = CreateMove(DeltaTime);
		Server_SendMove(Move);
	}

	if (Role == ROLE_SimulatedProxy) {

		SimulateMove(ServerState.LastMove);
	}
}

void AGoKart::OnRep_ServerState() {

	SetActorTransform(ServerState.Transform);
	Velocity = ServerState.Velocity;

	ClearAcknowledgedMoves(ServerState.LastMove);

	for (const auto& Itr : UnacknowledgedMoves)	{

		SimulateMove(Itr);
	}
}

void AGoKart::SimulateMove(const FSyncMove& Move) {

	FVector Force = GetActorForwardVector() * MaxDrivingForce * Move.Throttle;

	Force += GetAirResistence();
	Force += GetRollingResistence();

	FVector Acceleration = Force / Mass;

	Velocity += Acceleration * Move.DeltaTime;

	SetRotation(Move.DeltaTime, Move.SteeringThrow);

	SetLocationFromVelocity(Move.DeltaTime);
}

FSyncMove AGoKart::CreateMove(float DeltaTime) {

	FSyncMove Move;
	Move.DeltaTime = DeltaTime;
	Move.SteeringThrow = SteeringThrow;
	Move.Throttle = Throttle;
	Move.TimeStamp = GetWorld()->TimeSeconds;

	return Move;
}

void AGoKart::ClearAcknowledgedMoves(FSyncMove LastMove) {

	TArray<FSyncMove> NewMoves;

	for (const auto& Itr : UnacknowledgedMoves) {

		if (Itr.TimeStamp > LastMove.TimeStamp) {

			NewMoves.Add(Itr);
		}
	}

	UnacknowledgedMoves = NewMoves;
}

void AGoKart::SetRotation(const float &DeltaTime, float InSteeringThrow)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MinimumTurningRadius * InSteeringThrow;
	FQuat RotationDelta(GetActorUpVector(), RotationAngle);

	Velocity = RotationDelta.RotateVector(Velocity);

	AddActorWorldRotation(RotationDelta, true);
}

void AGoKart::SetLocationFromVelocity(const float &DeltaTime)
{
	FVector Delta = Velocity * 100 * DeltaTime;

	FHitResult HitResult;
	AddActorWorldOffset(Delta, true, &HitResult);

	if (HitResult.bBlockingHit) {

		Velocity = FVector::ZeroVector;
	}
}

FVector AGoKart::GetAirResistence() {

	return -Velocity.GetSafeNormal() * (Velocity.SizeSquared()) * DragCoefficient;
}

FVector AGoKart::GetRollingResistence() {

	// Convert from cm to m and to positive number
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;
	float NormalForce = Mass * AccelerationDueToGravity;

	return -Velocity.GetSafeNormal() * RollingResistenceCoefficient * NormalForce;
}

void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AGoKart::MoveRight);
}

void AGoKart::MoveForward(float Value) {

	Throttle = Value;
}

void AGoKart::MoveRight(float Value) {

	SteeringThrow = Value;
}

void AGoKart::Server_SendMove_Implementation (FSyncMove Move) {

	SimulateMove(Move);

	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = Velocity;
}

bool AGoKart::Server_SendMove_Validate(FSyncMove Move) {

	// TODO
	return true;
}

void AGoKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGoKart, ServerState);
}