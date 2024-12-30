// CameraSettings.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CameraSettings.generated.h"

/**
 * UCameraSettings permet de stocker les tags et les paramètres de distance pour le zoom.
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "Camera Settings"))
class SMASHUE_API UCameraSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// Propriétés de distance
	UPROPERTY(Config, EditAnywhere, Category="Inputs")
	float DistanceBetweenTargetsMin = 300.f;

	UPROPERTY(Config, EditAnywhere, Category="Inputs")
	float DistanceBetweenTargetsMax = 1500.f;

	// Tags associés à la caméra
	UPROPERTY(Config, EditAnywhere, Category="Inputs")
	FString CameraMainTag = "CameraMain";

	UPROPERTY(Config, EditAnywhere, Category="Inputs")
	FString CameraDistanceMinTag = "CameraDistanceMin";

	UPROPERTY(Config, EditAnywhere, Category="Inputs")
	FString CameraDistanceMaxTag = "CameraDistanceMax";

	UPROPERTY(Config, EditAnywhere, Category="Inputs")
	FString CameraBoundsTag = "CameraBounds";

	UPROPERTY(Config, EditAnywhere, Category="Inputs")
	float SizeDampingFactor = 5.0f;
	UPROPERTY(Config, EditAnywhere, Category="Inputs")
	float PositionDampingFactor = 5.0f;
};
