// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"

#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if(!HasAuthority()) return;
	
	APawn* InstigaterPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket =  GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	
	if(MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		// From muzzle flash socket to hit location from TraceUnderCrosshairs
		FVector ToTarget =  HitTarget - SocketTransform.GetLocation();
		UE_LOG(LogTemp, Warning, TEXT("ToTarget : %g, %g, %g"), ToTarget.X ,ToTarget.Y ,ToTarget.Z);
		
		FRotator TargetRotation = ToTarget.Rotation();
		UE_LOG(LogTemp, Warning, TEXT("TargetRotation : Yaw:%g, Pitch:%g"), TargetRotation.Yaw ,TargetRotation.Pitch );
		
		if(ProjectileClass && InstigaterPawn)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = GetOwner();
			SpawnParameters.Instigator = InstigaterPawn;
			UWorld* World = GetWorld();
			
			if(World)
			{
				World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParameters
				);
			}
		}
	}
}
