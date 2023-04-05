// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Bullet_CPP.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "StatsComponent.h"
#include "Enemy.generated.h"

UCLASS()
class DES310_PROJECT_API AEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* CubeMesh;
	void SetPlayerLocation(FVector location) { PlayerLocation = location; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* EnemyCube;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStatsComponent* EnemyStats;

	UPROPERTY() ABullet_CPP* ABulletActor;
	UPROPERTY(EditAnywhere) TSubclassOf<class ABullet_CPP> MyBullet;
	UPROPERTY(VisibleAnywhere) TArray<ABullet_CPP*> BulletsFired;

	UPROPERTY(EditAnywhere, Category = Fight) float BulletSpawnOffset = -150.f;
	UPROPERTY(EditAnywhere, Category = Fight) float BulletSpeed = 1000.f;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(BlueprintReadOnly) float Health;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float InitialHealth;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MinLevelOffset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxLevelOffset;


	FVector PlayerLocation;

public:	
	// Called every frame
	void Attack();
	void ResetEnemy();
	UStatsComponent* GetEnemyStats() { return EnemyStats; };
	void SetEnemyLevel(int playerLevel);

	virtual void Tick(float DeltaTime) override;

};
