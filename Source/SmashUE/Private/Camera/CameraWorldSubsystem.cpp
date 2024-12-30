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
    CameraMain = FindCameraByTag(TEXT("CameraMain"));

    AActor* CameraBoundsActor = FindCameraBoundsActor();
    if (CameraBoundsActor != nullptr)
    {
        InitCameraBounds(CameraBoundsActor);
    }
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
    ClampPositionIntoCameraBounds();
}

FVector UCameraWorldSubsystem::CalculateAveragePositionBetweenTargets()
{
    FVector AveragePosition = FVector::ZeroVector;
    for (UObject* Target : FollowTargets)
    {
        ICameraFollowTarget* FollowTarget = Cast<ICameraFollowTarget>(Target);
        if (FollowTarget && FollowTarget->isFollowable())
        {
            AveragePosition += FollowTarget->GetFollowPosition();
        }
    }
    return FollowTargets.Num() > 0 ? AveragePosition / FollowTargets.Num() : AveragePosition;
}

UCameraComponent* UCameraWorldSubsystem::FindCameraByTag(const FName& Tag) const
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);
    return FoundActors.Num() > 0 ? FoundActors[0]->FindComponentByClass<UCameraComponent>() : nullptr;
}

AActor* UCameraWorldSubsystem::FindCameraBoundsActor()
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("CameraBounds"), FoundActors);
    return FoundActors.Num() > 0 ? FoundActors[0] : nullptr;
}

void UCameraWorldSubsystem::InitCameraBounds(AActor* CameraBoundsActor)
{
    FVector BoundsCenter;
    FVector BoundsExtents;
    CameraBoundsActor->GetActorBounds(false, BoundsCenter, BoundsExtents);

    CameraBoundsMin = FVector2D(BoundsCenter.X - BoundsExtents.X, BoundsCenter.Z - BoundsExtents.Z);
    CameraBoundsMax = FVector2D(BoundsCenter.X + BoundsExtents.X, BoundsCenter.Z + BoundsExtents.Z);
    CameraBoundsYProjectionCenter = BoundsCenter.Y;
}

void UCameraWorldSubsystem::ClampPositionIntoCameraBounds()
{
    FVector2D ViewportBoundsMin, ViewportBoundsMax;
    GetViewportBounds(ViewportBoundsMin, ViewportBoundsMax);

    FVector CameraLocation = CameraMain->GetComponentLocation();
    FVector VisibleSize = FVector(
        ViewportBoundsMax.X - ViewportBoundsMin.X,
        0,
        ViewportBoundsMax.Y - ViewportBoundsMin.Y
    );

    FVector2D HalfVisibleSize = FVector2D(VisibleSize.X / 2.0f, VisibleSize.Z / 2.0f);

    CameraLocation.X = FMath::Clamp(CameraLocation.X, 
        CameraBoundsMin.X + HalfVisibleSize.X, 
        CameraBoundsMax.X - HalfVisibleSize.X
    );

    CameraLocation.Z = FMath::Clamp(CameraLocation.Z, 
        CameraBoundsMin.Y + HalfVisibleSize.Y, 
        CameraBoundsMax.Y - HalfVisibleSize.Y
    );

    CameraMain->SetWorldLocation(CameraLocation);
}

void UCameraWorldSubsystem::GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax)
{
    UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
    if (!ViewportClient) return;

    FViewport* Viewport = ViewportClient->Viewport;
    if (!Viewport) return;

    FIntRect ViewRect(
        Viewport->GetInitialPositionXY(),
        Viewport->GetInitialPositionXY() + Viewport->GetSizeXY()
    );

    FIntRect ViewportRect = Viewport->CalculateViewExtents(CameraMain->AspectRatio, ViewRect);

    OutViewportBoundsMin = FVector2D(ViewportRect.Min);
    OutViewportBoundsMax = FVector2D(ViewportRect.Max);
}

FVector UCameraWorldSubsystem::CalculateWorldPositionFromViewportPosition(const FVector2D& ViewportPosition)
{
    if (!CameraMain) return FVector::Zero();

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController) return FVector::Zero();

    float YDistanceToCenter = CameraMain->GetComponentLocation().Y - CameraBoundsYProjectionCenter;

    FVector CameraWorldProjectDir;
    FVector WorldPosition;
    UGameplayStatics::DeprojectScreenToWorld(
        PlayerController,
        ViewportPosition,
        WorldPosition,
        CameraWorldProjectDir
    );
    WorldPosition += CameraWorldProjectDir * YDistanceToCenter;

    return WorldPosition;
}
