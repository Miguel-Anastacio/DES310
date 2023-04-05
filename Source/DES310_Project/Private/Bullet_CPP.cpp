// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet_CPP.h"

#include "Components/StaticMeshComponent.h"

// Sets default values
ABullet_CPP::ABullet_CPP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Mesh"));
	SetRootComponent(BulletMesh);
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	

}

// Called when the game starts or when spawned
void ABullet_CPP::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABullet_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimeAlive += DeltaTime;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("%s"), *ProjectileMovement->Velocity.ToString()));

}

void ABullet_CPP::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Collided With " + OtherActor->GetName()));
	
}

