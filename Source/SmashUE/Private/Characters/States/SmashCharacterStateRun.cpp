// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateRun.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterStateID.h"

ESmashCharacterStateID USmashCharacterStateRun::GetStateID()
{
	return ESmashCharacterStateID::Run;
}

void USmashCharacterStateRun::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Cyan,
		TEXT("Enter StateRun")
		);
	Character->PlayAnimMontage(RunMontage);
}

void USmashCharacterStateRun::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		TEXT("Exit StateRun")
		);
}

void USmashCharacterStateRun::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(
		-1,
		0.1f,
		FColor::Green,
		TEXT("Tick StateRun")
		);
	USkeletalMeshComponent* Mesh = Character->GetMesh();
	FVector StaticMesh_Location = Mesh->GetRelativeLocation();
	float HorizontalVelocity = Character->GetHorizontalVelocity();
	float Orient = FMath::Sign(Character->GetOrientX());
	HorizontalVelocity = FMath::Clamp((HorizontalVelocity+Acceleration * DeltaTime) * Orient, -RunSpeedMax, RunSpeedMax);
	Character->SetHorizontalVelocity(HorizontalVelocity);
	FVector Location = FVector(StaticMesh_Location.X + HorizontalVelocity, StaticMesh_Location.Y, StaticMesh_Location.Z);
	Mesh->SetRelativeLocation(Location);
}

