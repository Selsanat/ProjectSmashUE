// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SmashCharacterState.h"
#include "SmashCharacterStateJump.generated.h"


UCLASS(ClassGroup=(SmashCharacterState), meta=(BlueprintSpawnableComponent))
class SMASHUE_API USmashCharacterStateJump : public USmashCharacterState
{
	GENERATED_BODY()

public:
	virtual ESmashCharacterStateID GetStateID() override;


	virtual void StateEnter(ESmashCharacterStateID PreviousStateID) override;

	virtual void StateExit(ESmashCharacterStateID NextStateID) override;

	virtual void StateTick(float DeltaTime) override;

	float CurrentJumpTime;
protected:
	UPROPERTY(EditAnywhere)
	UAnimMontage* JumpMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* SecondJumpMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* SecondJumpBackwardMontage;

	UPROPERTY(EditAnywhere)
	float JumpWalkSpeed = 600.f;

	UPROPERTY(EditAnywhere)
	float JumpMaxheight = 25000.f;

	UPROPERTY(EditAnywhere)
	float JumpDuration = 1.f;

	UPROPERTY(EditAnywhere)
	float JumpAirControl = 0.2f;
};
