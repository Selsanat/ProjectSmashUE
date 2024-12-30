// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateRun.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterStateID.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"

ESmashCharacterStateID USmashCharacterStateRun::GetStateID()
{
	return ESmashCharacterStateID::Run;
}

void USmashCharacterStateRun::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	
	Character->PlayAnimMontage(RunMontage);
}

void USmashCharacterStateRun::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
	
}

void USmashCharacterStateRun::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);
	
	if (FMath::Abs(Character->GetInputJump()) > 0.1f)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Jump);
		return;
	}
	else
	{
		if (FMath::Abs(Character->GetInputMoveX()) < Character->GetInputMoveXThreshold())
		{
			StateMachine->ChangeState(ESmashCharacterStateID::Idle);
			return;
		}
		else
		{
			Character->SetOrientX(Character->GetInputMoveX());
			Character->AddMovementInput(FVector::ForwardVector, Character->GetInputMoveX());
		}
	}
	if (!Character->GetCharacterMovement()->IsMovingOnGround())
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Fall);
		return;
	}
}

