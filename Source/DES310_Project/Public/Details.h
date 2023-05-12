// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PointLightComponent.h"
#include "Details.generated.h"

UCLASS()
class DES310_PROJECT_API ADetails : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADetails();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere) UPointLightComponent* PointLight;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* PlanetMeshComponent;

	UPROPERTY(Category = Rotation, EditAnywhere) FRotator RotationPerFrame;
	
	// variables to do floating movement in the asteroids
	float timer = 0;
	UPROPERTY(EditAnywhere) FVector AxisSpeed = FVector(1, 1, 1);
	UPROPERTY(EditAnywhere) FVector AxisRadius = FVector(1, 1, 1);
	FVector StartingLocation;

	int Index = -1;
};
