// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USpeherComponent;
class USkeletalMeshComponent;
class UBoxComponent;

UCLASS()
class KINGDOMMONSTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Rotate(float Deltatime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RotationRate = 45.f;

	bool Used;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* AttackHitbox;

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweepFrom, const FHitResult& SweepResults);

};
