// Fill out your copyright notice in the Description page of Project Settings.


#include "BlastAnimInstance.h"

#include "BlastCharacter.h"
#include "Blast/Weapon/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	EquippedWeapon = BlastCharacter->GetEquippedWeapon();
	bIsCrouched = BlastCharacter->bIsCrouched;
	TurningInPlace = BlastCharacter->GetTurningInPlace();
	bAiming = BlastCharacter->IsAiming();

	//Offset Yaw for Strafing
	FRotator AimRotation = BlastCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Velocity);
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 6.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlastCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation,CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f);
	Lean= FMath::Clamp(Interp,-90.f,90.f);

	AO_Yaw = BlastCharacter->GetAO_Yaw();
	AO_Pitch = BlastCharacter->GetAO_Pitch();

	if(bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlastCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);

		FVector OutPosition;
		FRotator OutRotation;
		BlastCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation );
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if(BlastCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("Hand_R"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(),RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlastCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaSeconds, 30.f);
		}
	}
		
}
