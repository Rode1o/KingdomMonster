// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "AIModule/Classes/AIController.h"
#include "MainCharacter.h"
#include "Animation/AnimInstance.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DetectSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	DetectSphere->SetupAttachment(GetRootComponent());
	DetectSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatCollision"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	AttackHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	AttackHitBox->SetupAttachment(GetMesh(), TEXT("HitboxSocket"));






}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AI_Controller = Cast<AAIController>(GetController());

	DetectSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::SphereDetectOnOverlapBegin);
	DetectSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::SphereDetectOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatDetectOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatDetectOnOverlapEnd);


	AttackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackHitDetectOnOverlapBegin);


	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::SphereDetectOnOverlapBegin(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweepFrom, const FHitResult& SweepResults)
{
	if (!IsAlive)
		return;

	AMainCharacter* MyChar = Cast<AMainCharacter>(OtherActor);
	if (MyChar) {
		TargetChar = MyChar;
		MoveToTarget();
	}
}

void AEnemy::SphereDetectOnOverlapEnd(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsAlive)
		return;

	AMainCharacter* MyChar = Cast<AMainCharacter>(OtherActor);
	if (MyChar) {
		TargetChar = nullptr;
		if (AI_Controller)
			AI_Controller->StopMovement();
	}

}

void AEnemy::CombatDetectOnOverlapBegin(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweepFrom, const FHitResult& SweepResults)
{
	if (!IsAlive)
		return;

	AMainCharacter* Mychar = Cast<AMainCharacter>(OtherActor);

	if (Mychar) {

		bTargetInAttackRange = true;

		float AttackDelay = FMath::RandRange(AttackDelayMin, AttackDelayMax);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackDelay);

	}


}

void AEnemy::CombatDetectOnOverlapEnd(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsAlive)
		return;

	AMainCharacter* Mychar = Cast<AMainCharacter>(OtherActor);
	if (Mychar) {
		bTargetInAttackRange = false;
	}
}

void AEnemy::AttackHitDetectOnOverlapBegin(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweepFrom, const FHitResult& SweepResults)
{

	if (CanDetectDamageCollision) {
		AMainCharacter* MyChar = Cast<AMainCharacter>(OtherActor);

		if (MyChar) {
			CanDetectDamageCollision = false;

			MyChar->ApplyDamage();


		}

	}



}

void AEnemy::MoveToTarget()
{
	if (bAttacking)
		return;

	if (AI_Controller) {
		FNavPathSharedPtr NavPath;
		FAIMoveRequest MoveRequest;

		MoveRequest.SetGoalActor(TargetChar);
		MoveRequest.SetAcceptanceRadius(MovetoTargetRadius);
		AI_Controller->MoveTo(MoveRequest, &NavPath);
	}


}

void AEnemy::Attack()
{
	if (!IsAlive)
		return;

	if (bAttacking)
		return;

	bAttacking = true;
	CanDetectDamageCollision = true;

	if (AI_Controller) {
		AI_Controller->StopMovement();
	}

	auto AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage) {

		AnimInstance->Montage_Play(CombatMontage, 1.15F);
		AnimInstance->Montage_JumpToSection(TEXT("Attack"), CombatMontage);


	}
}

void AEnemy::AttackEnded()
{

	bAttacking = false;
	CanDetectDamageCollision = false;

	if (TargetChar) {
		if (bTargetInAttackRange) {

			float AttackDelay = FMath::RandRange(AttackDelayMin, AttackDelayMax);
			GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackDelay);

		}
		else {
			MoveToTarget();
		}
	}
	else {
		

		bTargetInAttackRange = false;


	}
}

void AEnemy::ApplyDamage()
{


	Healt -= 20.f;
	if (Healt <= 0 ) {

		if (AI_Controller)
			AI_Controller->StopMovement();

		IsAlive = false;
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::DisposeEnemy, 2.f);

	}


}

void AEnemy::DisposeEnemy()
{
	Destroy();
}

