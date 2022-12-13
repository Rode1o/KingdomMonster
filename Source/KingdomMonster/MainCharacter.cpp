// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/InputComponent.h"
#include "GameFrameWork/PlayerController.h"
#include "Weapon.h"
#include "Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"



// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	CameraActor = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraActor"));
	CameraActor->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraActor->bUsePawnControlRotation = false;


	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// add character movement settings

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 550.f;
	GetCharacterMovement()->AirControl = 0.2f;

}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetMovementSpeedAndAir();

}

void AMainCharacter::Jump()
{

	if (!IsAlive)
		return;
	if (bIsAttacking)
		return;
	Super::Jump();
}

void AMainCharacter::Sprint()
{

	if (!IsAlive)
		return;
	if (GetCharacterMovement()->Velocity.Size() == 0)
		return;

	MovementState = EMovementState::MS_Sprinting;
	GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;

}

void AMainCharacter::Run()
{
	if (!IsAlive)
		return;
	if (GetCharacterMovement()->Velocity.Size() == 0)
		return;

	MovementState = EMovementState::MS_Normal;
	GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
}

void AMainCharacter::Attack()
{
	if (!IsAlive)
		return;
	if (EquippedWeapon) {

		UAnimInstance* MyAnim = GetMesh()->GetAnimInstance();
		if (MyAnim && CombatMontage) {
			MyAnim->Montage_Play(CombatMontage, 2.5f);
			
			float RandAnim = FMath::RandRange(0, 1);

			if (RandAnim > 0) {
				MyAnim->Montage_JumpToSection(TEXT("Attack_1"));
			}
			else {
				MyAnim->Montage_JumpToSection(TEXT("Attack_2"));
			}

			CanDetectDamageCollision = true;
		}
	}

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(KingdomContext, 0);
		}
	}
	
}

void AMainCharacter::Move(const FInputActionValue& value)
{

	if (!IsAlive)
		return;

	if (!bIsAttacking){
	const FVector2D MovementVector = value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);

	}

}

void AMainCharacter::Look(const FInputActionValue& value)
{
	const FVector2D LooksAxisVector = value.Get<FVector2D>();

	AddControllerPitchInput(LooksAxisVector.Y);
	AddControllerYawInput(LooksAxisVector.X);
}

// Called every frame


// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMainCharacter::Jump);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMainCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMainCharacter::Run);
		EnhancedInputComponent->BindAction(CombatAction, ETriggerEvent::Started, this, &AMainCharacter::Attack);
	}



}

void AMainCharacter::SetMovementSpeedAndAir()
{
	auto CharSpeed = GetVelocity();
	auto LateralSpeed = FVector(CharSpeed.X, CharSpeed.Y, 0.0f);

	MovementSpeed = LateralSpeed.Size();

	bInAir = GetMovementComponent()->IsFalling();
}

void AMainCharacter::EquipWeapon(AWeapon* WeaponActor)
{

	if (EquippedWeapon != nullptr)
		return;

	WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RightHandSocket"));
	WeaponActor->Used = true;
	EquippedWeapon = WeaponActor;

	// add overlap for hitbox

	EquippedWeapon->AttackHitbox->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::DamageBoxDetectOnOverlapBegin);


}

void AMainCharacter::ApplyDamage()
{
	if (!IsAlive)
		return;

	Health -= 10.f;

	if (Health <= 0.f) {

		IsAlive = false;
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AMainCharacter::RestartGame, 3.f);


	}

}

void AMainCharacter::RestartGame()
{

	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);

}




void AMainCharacter::DamageBoxDetectOnOverlapBegin(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweepFrom, const FHitResult& SweepResults)
{


	if (CanDetectDamageCollision) {

		AEnemy* Enemy = Cast<AEnemy>(OtherActor);

		if (Enemy) {
			CanDetectDamageCollision = false;

			Enemy->ApplyDamage();
		}


	}



}
