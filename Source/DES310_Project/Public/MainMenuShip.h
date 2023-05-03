// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainMenuShip.generated.h"

UCLASS()
class DES310_PROJECT_API AMainMenuShip : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainMenuShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float timer = 0;

	UPROPERTY(EditAnywhere) UStaticMeshComponent* Mesh1;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* Mesh2;

	UPROPERTY(EditAnywhere) FVector AxisSpeed = FVector(1,1,1);
	UPROPERTY(EditAnywhere) FVector AxisRadius = FVector(1,1,1);

	FVector StartingLocation;

};
