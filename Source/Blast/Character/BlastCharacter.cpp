

#include "BlastCharacter.h"

#include "Blast/BlastComponents/CombatComponent.h"
#include "Blast/Weapon/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ABlastCharacter::ABlastCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength=600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
	OverheadWidget->SetupAttachment(RootComponent);

	CombatCom = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatCom->SetIsReplicated(true);
	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABlastCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ABlastCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ABlastCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
void ABlastCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimOffset(DeltaTime);
}
void ABlastCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ABlastCharacter::BlastJump);
	
	PlayerInputComponent->BindAxis("MoveForward",this,&ABlastCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&ABlastCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn",this,&ABlastCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp",this,&ABlastCharacter::LookUp);

	PlayerInputComponent->BindAction("Equip",IE_Pressed,this,&ABlastCharacter::EquipButtonPressed);
	
	PlayerInputComponent->BindAction("Crouch",IE_Pressed,this,&ABlastCharacter::CrouchButtonPressed);

	PlayerInputComponent->BindAction("Aim",IE_Pressed,this,&ABlastCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim",IE_Released,this,&ABlastCharacter::AimButtomReleased);
	
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement =true;

	
}

void ABlastCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(CombatCom)
	{
		 CombatCom->Character=this;
	}

}


void ABlastCharacter::MoveForward(float Value)
{
	if(Controller!= nullptr && Value!=0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction,Value);
	}
}

void ABlastCharacter::MoveRight(float Value)
{
	if(Controller!= nullptr && Value!=0.f)
	{
		const FRotator YawRotation(0.f,Controller->GetControlRotation().Yaw,0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction,Value);
	}
}

void ABlastCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABlastCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ABlastCharacter::BlastJump()
{
	if(bIsCrouched)
	{
		UnCrouch();
	}
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]{Jump();}, 0.01, false);

	
}

void ABlastCharacter::EquipButtonPressed()
{
	if(CombatCom)
	{
		
		if(HasAuthority())
		{
			CombatCom->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
		
	}
}

void ABlastCharacter::CrouchButtonPressed()
{
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();	
	}
	
}

void ABlastCharacter::AimButtonPressed()
{
	if(CombatCom)
	{
		CombatCom->SetAiming(true);
	}
}

void ABlastCharacter::AimButtomReleased()
{
	if(CombatCom)
	{
		CombatCom->SetAiming(false);
	}
}

void ABlastCharacter::AimOffset(float DeltaTime)
{
	if(CombatCom && CombatCom->EquippedWeapon==nullptr) return;
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	
	if(Speed ==0.f && !bIsInAir)//Standing still, not jumping
	{
		FRotator CurrenAimRotation = FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator( CurrenAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		bUseControllerRotationYaw =false;
	}
	if(Speed >0.f || bIsInAir) // Running or jumping 
	{
		StartingAimRotation = FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw =true;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;
}

void ABlastCharacter::ServerEquipButtonPressed_Implementation()
{
	if(CombatCom)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s Equipped Weapon serverside"),*this->GetName())
		CombatCom->EquipWeapon(OverlappingWeapon);
	}
}

void ABlastCharacter::SetOverlapingWeapon(AWeapon* Weapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(false);
	}
	OverlappingWeapon = Weapon;
	if(IsLocallyControlled())
	{
		if(OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickUpWidget(true);
		}
	}
}



void ABlastCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(true);
	}
	if(LastWeapon)
	{
		LastWeapon->ShowPickUpWidget(false);
	}
}


bool ABlastCharacter::IsWeaponEquipped()
{
	return  (CombatCom && CombatCom->EquippedWeapon);
}

bool ABlastCharacter::IsAiming()
{
	return (CombatCom && CombatCom->bAiming);
}




