// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MainCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContetxt;
class UInputAction;
class AWeapon;
class UAnimMontage;


UENUM(BlueprintType)
enum class EMovementState : uint8 {
	MS_Normal UMETA(DisplayName = "Normal"),
	MS_Sprinting UMETA(DisplayName = "Sprinting")

};



UCLASS()
class KINGDOMMONSTER_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	virtual void Jump() override;
	virtual void Sprint();
	virtual void Run();
	virtual void Attack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputMappingContext* KingdomContext;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* MovementAction;


	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
		class UInputAction* JumpAction; 

	UPROPERTY(EditAnywhere, Category = Input)
		class UInputAction* SprintAction;


	UPROPERTY(EditAnywhere, Category = Input)
		class UInputAction* CombatAction;



	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UCameraComponent* CameraActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EMovementState MovementState;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsAttacking;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool IsAlive = true;
	//

	UPROPERTY(BlueprintReadOnly)
		AWeapon* EquippedWeapon;
	
	UPROPERTY(BlueprintReadOnly)
		float MovementSpeed;

	UPROPERTY(BlueprintReadOnly)
		bool bInAir;

	UPROPERTY(EditAnywhere)
		class UAnimMontage* CombatMontage;
	UFUNCTION()
		virtual void DamageBoxDetectOnOverlapBegin(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweepFrom, const FHitResult& SweepResults);

	
	UPROPERTY(BlueprintReadWrite)
		bool CanDetectDamageCollision;


	UPROPERTY(BlueprintReadWrite)
		float Health = 100.f; 

	void ApplyDamage();
	void RestartGame();

	
	void SetMovementSpeedAndAir();

	void EquipWeapon(AWeapon* WeaponActor);

	

	// speed control
	float RunningSpeed = 600.f;
	float SprintingSpeed = 950.f;


};
