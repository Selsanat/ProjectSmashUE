// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateFall.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"


ESmashCharacterStateID USmashCharacterStateFall::GetStateID()
{
	return ESmashCharacterStateID::Fall;
}

void USmashCharacterStateFall::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	
	Character->PlayAnimMontage(FallMontage);
	Character->GetCharacterMovement()->Velocity.Z = 0;
	
}

void USmashCharacterStateFall::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
}

void USmashCharacterStateFall::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);
	FVector CharacterVelocity = Character->GetCharacterMovement()->Velocity;
	Character->GetCharacterMovement()->Velocity.X = FMath::Clamp(CharacterVelocity.X + Character->GetInputMoveX() * FallHorizontalMoveSpeed * FallAirControl, -FallHorizontalMoveSpeed, FallHorizontalMoveSpeed);
	FVector UpVector = CharacterVelocity+ FVector::UpVector * FallGravityScale* DeltaTime;
	Character->GetCharacterMovement()->AddImpulse(UpVector);

	if(Character->GetMovementComponent()->IsMovingOnGround())
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Idle);
	}
	if(Character->GetInputJump()>0.1f && Character->CanDoubleJump)
	{
		Character->CanDoubleJump = false;
		StateMachine->ChangeState(ESmashCharacterStateID::Jump);
	}
}
