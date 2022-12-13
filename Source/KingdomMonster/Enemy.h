// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"


class USphereComponent;
class UBoxComponent;
class AMainCharacter;
class UAnimMontage;
class AAIcontroller;

UCLASS()
class KINGDOMMONSTER_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
		class USphereComponent* DetectSphere;

	UPROPERTY(EditAnywhere)
		class USphereComponent* CombatSphere;

	UPROPERTY(EditAnywhere)
		class UBoxComponent* AttackHitBox;

	UFUNCTION()
		virtual void SphereDetectOnOverlapBegin(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweepFrom, const FHitResult& SweepResults);

	UFUNCTION()
		virtual void SphereDetectOnOverlapEnd(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		virtual void CombatDetectOnOverlapBegin(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweepFrom, const FHitResult& SweepResults);

	UFUNCTION()
		virtual void CombatDetectOnOverlapEnd(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		virtual void AttackHitDetectOnOverlapBegin(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweepFrom, const FHitResult& SweepResults);

	class AMainCharacter* TargetChar;
	class AAIController* AI_Controller;

	void MoveToTarget();


	UPROPERTY(BlueprintReadOnly)
		bool IsAlive = true;

	bool bAttacking;

	UPROPERTY(EditAnywhere)
		float MovetoTargetRadius = 10.f;

	UPROPERTY(EditAnywhere)
		bool bTargetInAttackRange;

	float AttackDelayMin = 0.2f;
	float AttackDelayMax = 0.7f;

	bool CanDetectDamageCollision;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnded();

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere)
		class UAnimMontage* CombatMontage;

	float Healt = 100.f;
	void ApplyDamage();
	void DisposeEnemy();



};
