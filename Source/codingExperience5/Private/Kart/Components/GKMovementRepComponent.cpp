// Fill out your copyright notice in the Description page of Project Settings.

#include "GKMovementRepComponent.h"

#include "Net/UnrealNetwork.h"

UGKMovementRepComponent::UGKMovementRepComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicated(true);
}

void UGKMovementRepComponent::BeginPlay()
{
	Super::BeginPlay();

	GKMovementComponent = GetOwner()->FindComponentByClass<UGKMovementComponent>();
	if (GKMovementComponent == nullptr) UE_LOG(LogTemp, Error, TEXT("GKMovementComponent not found")) return;
}

void UGKMovementRepComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GKMovementComponent != nullptr) {

		if (GetOwnerRole() == ROLE_AutonomousProxy) {

			FSyncMove Move = GKMovementComponent->CreateMove(DeltaTime);
			GKMovementComponent->SimulateMove(Move);
			UnacknowledgedMoves.Add(Move);
			Server_SendMove(Move);
		}
		// we are the server and in control of the pawn
		if (GetOwnerRole() == ROLE_Authority && GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy) {

			FSyncMove Move = GKMovementComponent->CreateMove(DeltaTime);
			Server_SendMove(Move);
		}

		if (GetOwnerRole() == ROLE_SimulatedProxy) {

			GKMovementComponent->SimulateMove(ServerState.LastMove);
		}
	}
}

void UGKMovementRepComponent::OnRep_ServerState() {

	if (GKMovementComponent != nullptr) {

		GetOwner()->SetActorTransform(ServerState.Transform);
		GKMovementComponent->SetVelocity(ServerState.Velocity);

		ClearAcknowledgedMoves(ServerState.LastMove);

		for (const auto& Itr : UnacknowledgedMoves) {

			GKMovementComponent->SimulateMove(Itr);
		}
	}
}

void UGKMovementRepComponent::ClearAcknowledgedMoves(FSyncMove LastMove) {

	TArray<FSyncMove> NewMoves;

	for (const auto& Itr : UnacknowledgedMoves) {

		if (Itr.TimeStamp > LastMove.TimeStamp) {

			NewMoves.Add(Itr);
		}
	}

	UnacknowledgedMoves = NewMoves;
}

void UGKMovementRepComponent::Server_SendMove_Implementation(FSyncMove Move) {

	GKMovementComponent->SimulateMove(Move);

	ServerState.LastMove = Move;
	ServerState.Transform = GetOwner()->GetActorTransform();
	ServerState.Velocity = GKMovementComponent->GetVelocity();
}

bool UGKMovementRepComponent::Server_SendMove_Validate(FSyncMove Move) {

	return true;
}

void UGKMovementRepComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGKMovementRepComponent, ServerState);
}