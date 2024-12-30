// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateIdle.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterStateID.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"

ESmashCharacterStateID USmashCharacterStateIdle::GetStateID()
{
	return ESmashCharacterStateID::Idle;
}

void USmashCharacterStateIdle::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	
	Character->PlayAnimMontage(IdleMontage);
	Character->SetHorizontalVelocity(0);
	Character->CanDoubleJump = true;
	Character->InputMoveXFastEvent.AddDynamic(this, &USmashCharacterStateIdle::OnInputMoveXFast);
}

void USmashCharacterStateIdle::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);



	Character->InputMoveXFastEvent.RemoveDynamic(this, &USmashCharacterStateIdle::OnInputMoveXFast);
}

void USmashCharacterStateIdle::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	
	if (FMath::Abs(Character->GetInputMoveX()) > Character->GetInputMoveXThreshold())
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Walk);
		return;
	}
	if (FMath::Abs(Character->GetInputJump()) > 0.1f)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Jump);
		return;
	}
	if (!Character->GetCharacterMovement()->IsMovingOnGround())
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Fall);
		return;
	}
}

void USmashCharacterStateIdle::OnInputMoveXFast(float InputMoveX)
{
	StateMachine->ChangeState(ESmashCharacterStateID::Run);
}

