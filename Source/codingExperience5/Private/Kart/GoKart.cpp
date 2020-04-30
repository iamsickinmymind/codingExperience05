// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKart.h"

#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/GKMovementComponent.h"

#include "Net/UnrealNetwork.h"

#include "DrawDebugHelpers.h"

AGoKart::AGoKart()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(FName("BoxCollision"));
	RootComponent = BoxCollision;

	KartMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Mesh"));
	KartMesh->SetupAttachment(GetRootComponent());

	GKMovementComponent = CreateDefaultSubobject<UGKMovementComponent>(FName("MovementComponent"));
	if ensure(!(GKMovementComponent == nullptr)) return;

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

	if (GKMovementComponent != nullptr) {

		if (Role == ROLE_AutonomousProxy) {

			FSyncMove Move = GKMovementComponent->CreateMove(DeltaTime);
			GKMovementComponent->SimulateMove(Move);
			UnacknowledgedMoves.Add(Move);
			Server_SendMove(Move);
		}
		// we are the server and in control of the pawn
		if (Role == ROLE_Authority && GetRemoteRole() == ROLE_SimulatedProxy) {

			FSyncMove Move = GKMovementComponent->CreateMove(DeltaTime);
			Server_SendMove(Move);
		}

		if (Role == ROLE_SimulatedProxy) {

			GKMovementComponent->SimulateMove(ServerState.LastMove);
		}
	}
}

void AGoKart::OnRep_ServerState() {

	SetActorTransform(ServerState.Transform);
	GKMovementComponent->SetVelocity(ServerState.Velocity);

	ClearAcknowledgedMoves(ServerState.LastMove);

	for (const auto& Itr : UnacknowledgedMoves) {

		GKMovementComponent->SimulateMove(Itr);
	}
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

void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AGoKart::MoveRight);
}

void AGoKart::MoveForward(float Value) {

	GKMovementComponent->SetThrottle(Value);
}

void AGoKart::MoveRight(float Value) {

	GKMovementComponent->SetSteeringThrow(Value);
}

void AGoKart::Server_SendMove_Implementation(FSyncMove Move) {

	GKMovementComponent->SimulateMove(Move);

	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = GKMovementComponent->GetVelocity();
}

bool AGoKart::Server_SendMove_Validate(FSyncMove Move) {

	// TODO
	return true;
}

void AGoKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGoKart, ServerState);
}