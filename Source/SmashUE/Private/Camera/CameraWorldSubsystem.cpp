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

    InitCameraZoomParameters();
}

void UCameraWorldSubsystem::InitCameraZoomParameters()
{
    // Trouver CameraDistanceMin par tag et mettre à jour CameraZoomYMin en fonction de la position Y
    UCameraComponent* CameraDistanceMin = FindCameraByTag(TEXT("CameraDistanceMin"));
    if (CameraDistanceMin)
    {
        CameraZoomYMin = CameraDistanceMin->GetComponentLocation().Y;
    }

    // Trouver CameraDistanceMax par tag et mettre à jour CameraZoomYMax en fonction de la position Y
    UCameraComponent* CameraDistanceMax = FindCameraByTag(TEXT("CameraDistanceMax"));
    if (CameraDistanceMax)
    {
        CameraZoomYMax = CameraDistanceMax->GetComponentLocation().Y;
    }
}
void UCameraWorldSubsystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    TickUpdateCameraPosition(DeltaTime);
    TickUpdateCameraZoom(DeltaTime);
}

void UCameraWorldSubsystem::TickUpdateCameraZoom(float DeltaTime)
{
    if (!CameraMain) return;

    float GreatestDistanceBetweenTargets = CalculateGreatestDistanceBetweenTargets();

    // Calculer le pourcentage actuel de la distance
    float Percent = FMath::Clamp(
        (GreatestDistanceBetweenTargets - CameraZoomDistanceBetweenTargetsMin) /
        (CameraZoomDistanceBetweenTargetsMax - CameraZoomDistanceBetweenTargetsMin),
        0.0f,
        1.0f
    );

    // Mettre à jour la position Y de la caméra principale avec une interpolation Lerp
    float NewCameraY = FMath::Lerp(CameraZoomYMin, CameraZoomYMax, 1-Percent);
    FVector NewLocation = CameraMain->GetComponentLocation();
    NewLocation.Y = NewCameraY;

    GEngine->AddOnScreenDebugMessage(-1,200,FColor::Purple,FString::Printf(TEXT("Min %s"),*CameraMain->GetComponentLocation().ToString()));
    GEngine->AddOnScreenDebugMessage(-1,200,FColor::Yellow,FString::Printf(TEXT("Max %s"),*NewLocation.ToString()));
    CameraMain->SetWorldLocation(NewLocation);
}
float UCameraWorldSubsystem::CalculateGreatestDistanceBetweenTargets()
{
    float GreatestDistance = 0.0f;

    for (int32 i = 0; i < FollowTargets.Num(); ++i)
    {
        ICameraFollowTarget* TargetA = Cast<ICameraFollowTarget>(FollowTargets[i]);
        if (!TargetA || !TargetA->isFollowable()) continue;

        FVector PositionA = TargetA->GetFollowPosition();

        for (int32 j = i + 1; j < FollowTargets.Num(); ++j)
        {
            ICameraFollowTarget* TargetB = Cast<ICameraFollowTarget>(FollowTargets[j]);
            if (!TargetB || !TargetB->isFollowable()) continue;

            FVector PositionB = TargetB->GetFollowPosition();
            float Distance = FVector::Dist(PositionA, PositionB);

            if (Distance > GreatestDistance)
            {
                GreatestDistance = Distance;
            }
        }
    }

    return GreatestDistance;
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
