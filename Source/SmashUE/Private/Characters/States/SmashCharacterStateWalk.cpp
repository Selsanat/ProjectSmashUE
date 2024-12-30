// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateWalk.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterStateID.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"

ESmashCharacterStateID USmashCharacterStateWalk::GetStateID()
{
	return ESmashCharacterStateID::Walk;
}

void USmashCharacterStateWalk::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	
	Character->PlayAnimMontage(WalkMontage);

	Character->InputMoveXFastEvent.AddDynamic(this, &USmashCharacterStateWalk::OnInputMoveXFast);
}

void USmashCharacterStateWalk::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
	

	Character->InputMoveXFastEvent.RemoveDynamic(this, &USmashCharacterStateWalk::OnInputMoveXFast);
}

void USmashCharacterStateWalk::StateTick(float DeltaTime)
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
			Character->AddMovementInput(FVector::ForwardVector, Character->GetOrientX());
		}
	}
	if (!Character->GetCharacterMovement()->IsMovingOnGround())
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Fall);
		return;
	}
}

void USmashCharacterStateWalk::OnInputMoveXFast(float InputMoveX)
{
	StateMachine->ChangeState(ESmashCharacterStateID::Run);
}
