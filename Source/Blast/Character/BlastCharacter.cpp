

#include "BlastCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

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
	
	
}


void ABlastCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
void ABlastCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ACharacter::Jump);
	
	PlayerInputComponent->BindAxis("MoveForward",this,&ABlastCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&ABlastCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn",this,&ABlastCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp",this,&ABlastCharacter::LookUp);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement =true;
	
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

void ABlastCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



