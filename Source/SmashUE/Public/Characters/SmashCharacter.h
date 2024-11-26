// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraFollowTarget.h"
#include "GameFramework/Character.h"
#include "SmashCharacter.generated.h"

struct FInputActionValue;
class USmashCharacterInputData;
class UInputMappingContext;
class USmashCharacterStateMachine;

UCLASS()
class SMASHUE_API ASmashCharacter : public ACharacter, public ICameraFollowTarget
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
	float GetInputJump() const;
	float GetInputMoveXThreshold();

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

	UPROPERTY()
	float InputMoveXThreshold = 0.f;

protected:
	void SetupMappingContextIntoController() const;
	
	#pragma endregion

#pragma region Input Move X

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputMoveXEvent, float, InputMoveX);
public:
	float GetInputMoveX() const;

	UPROPERTY()
	FInputMoveXEvent InputMoveXFastEvent;

	UPROPERTY()
	bool CanDoubleJump = true;
	
protected:
	UPROPERTY()
	float InputMoveX = 0.f;

	UPROPERTY()
	float InputJump = 0.f;

private:

	void BindInputMoveXAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent);

	void OnInputMoveX(const FInputActionValue& InputActionValue);

	void OnInputMoveXFast(const FInputActionValue& InputActionValue);

	void OnInputJump(const FInputActionValue& InputActionValue);

public:
	virtual FVector GetFollowPosition() override;
	virtual bool isFollowable() override;

private:
#pragma endregion
};
