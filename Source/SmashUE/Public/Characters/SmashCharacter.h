﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SmashCharacter.generated.h"

struct FInputActionValue;
class USmashCharacterInputData;
class UInputMappingContext;
class USmashCharacterStateMachine;

UCLASS()
class SMASHUE_API ASmashCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Unreal Default
public:
	// Sets default values for this character's properties
	ASmashCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma endregion Unreal Default

#pragma region Orient

public :
	float GetOrientX() const;

	void SetOrientX(float NewOrientX);

	float GetHorizontalVelocity() const;

	void SetHorizontalVelocity(float NewHorizontalVelocity);

protected :
	UPROPERTY(BluePrintReadWrite)
	float OrientX = 1.f;
	float HorizontalVelocity = 0.f;
	void RotateMeshUsingOrientX() const;
	
#pragma endregion Orient

#pragma region State Machine

public:
	void CreateStateMachine();

	void InitStateMachine();

	void TickStateMachine(float DeltaTime) const;

protected:
	UPROPERTY(BlueprintReadOnly);
	TObjectPtr<USmashCharacterStateMachine> StateMachine;
	
#pragma endregion State Machine

#pragma region Input Data / Mapping Context
	
public:
	UPROPERTY()
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY()
	TObjectPtr<USmashCharacterInputData> InputData;

protected:
	void SetupMappingContextIntoController() const;
	
	#pragma endregion

#pragma region Input Move X

public:
	float GetInputMoveX() const;

protected:
	UPROPERTY()
	float InputMoveX = 0.f;

private:
	void BindInputMoveXAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent);

	void OnInputMoveX(const FInputActionValue& InputActionValue);


#pragma endregion
};
