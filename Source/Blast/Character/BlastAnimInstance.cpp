// Fill out your copyright notice in the Description page of Project Settings.


#include "BlastAnimInstance.h"

#include "BlastCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBlastAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	BlastCharacter = Cast<ABlastCharacter>(TryGetPawnOwner());
}

void UBlastAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(BlastCharacter ==nullptr)
	{
		BlastCharacter = Cast<ABlastCharacter>(TryGetPawnOwner());
	}
	if(BlastCharacter ==nullptr) return;

	FVector Velocity = BlastCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();
	
	bIsInAir = BlastCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = BlastCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = BlastCharacter->IsWeaponEquipped();
	bIsCrouched = BlastCharacter->bIsCrouched;
	
	bAiming = BlastCharacter->IsAiming();
}
