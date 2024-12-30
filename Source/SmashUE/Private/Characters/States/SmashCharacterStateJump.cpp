// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateJump.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"


ESmashCharacterStateID USmashCharacterStateJump::GetStateID()
{
	return ESmashCharacterStateID::Jump;
}

void USmashCharacterStateJump::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	
	CurrentJumpTime = 0.0f;
	float h = JumpMaxheight;
	float th = JumpDuration / 2;
	if (Character->CanDoubleJump)
	{
		Character->PlayAnimMontage(JumpMontage);
	}
	else
	{
		if (Character->GetInputMoveX() == Character->GetOrientX())
		{
			Character->PlayAnimMontage(SecondJumpMontage);
		}
		else
		{
			Character->PlayAnimMontage(SecondJumpBackwardMontage);
		}
	}
	Character->GetCharacterMovement()->AddImpulse(FVector::UpVector * (2 * h / th));
}

void USmashCharacterStateJump::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
	
}

void USmashCharacterStateJump::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);
	

	float h;
	float th;
	float g;
	if (Character->GetInputJump()<0.1f)
	{
		h = JumpMaxheight/5;
		th = JumpDuration/5/ 2;	
	}
	else
	{
		h = JumpMaxheight;
		th = JumpDuration / 2;
	}
	g = (-2 * h) / FMath::Pow(th, 2);
	FVector CharacterVelocity = Character->GetCharacterMovement()->Velocity;
	Character->GetCharacterMovement()->Velocity.X = FMath::Clamp(CharacterVelocity.X + (Character->GetInputMoveX() * JumpWalkSpeed * JumpAirControl), -JumpWalkSpeed, JumpWalkSpeed);
	FVector UpVector = CharacterVelocity+ FVector::UpVector * g * DeltaTime;
	Character->GetCharacterMovement()->AddImpulse(UpVector);
	
	if (CharacterVelocity.Z<0.1f)
	{
		Character->StopJumping();
		StateMachine->ChangeState(ESmashCharacterStateID::Fall);
		return;
	}
}