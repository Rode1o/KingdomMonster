// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "MainCharacter.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = SphereCollision;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	AttackHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	AttackHitbox->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnOverlapBegin);
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Rotate(DeltaTime);

}

void AWeapon::Rotate(float Deltatime)
{
	if (Used)
		return;

	auto Rotation = GetActorRotation();
	Rotation.Yaw += Deltatime * RotationRate;
	SetActorRotation(Rotation);
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweepFrom, const FHitResult& SweepResults)
{

	if (OtherActor->ActorHasTag("Player")) {

		AMainCharacter* PlayerCharacter = Cast<AMainCharacter>(OtherActor);

		if (PlayerCharacter) {
			PlayerCharacter->EquipWeapon(this);
		}
	}


}

