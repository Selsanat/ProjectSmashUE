// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraWorldSubsystem.h"

#include "Camera/CameraComponent.h"
#include "Camera/CameraFollowTarget.h"
#include "Kismet/GameplayStatics.h"

void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	CameraMain = FindCameraByTag(TEXT("CameraMain"));
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickUpdateCameraPosition(DeltaTime);
}

void UCameraWorldSubsystem::AddFollowTarget(UObject* FollowTarget)
{
	FollowTargets.Add(FollowTarget);
}

void UCameraWorldSubsystem::RemoveFollowTarget(UObject* FollowTarget)
{
	FollowTargets.Remove(FollowTarget);
}

void UCameraWorldSubsystem::TickUpdateCameraPosition(float DeltaTime)
{
	FVector ComputedLocation = CalculateAveragePositionBetweenTargets();
	FVector NewLocation = FVector(ComputedLocation.X, 1920, ComputedLocation.Z);
	CameraMain->SetWorldLocation(NewLocation);
}

FVector UCameraWorldSubsystem::CalculateAveragePositionBetweenTargets()
{
	FVector AveragePosition = FVector::ZeroVector;
	for (int i = 0; i < FollowTargets.Num(); i++)
	{
		ICameraFollowTarget* FollowTarget = Cast<ICameraFollowTarget>(FollowTargets[i]);
		if (FollowTarget!=nullptr)
		{
			if (FollowTarget->isFollowable())
			{
				AveragePosition += FollowTarget->GetFollowPosition();
			}
		}
	}
	return AveragePosition/FollowTargets.Num();
}

UCameraComponent* UCameraWorldSubsystem::FindCameraByTag(const FName& Tag) const
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag,FoundActors);
	return FoundActors[0]->FindComponentByClass<UCameraComponent>();
}

