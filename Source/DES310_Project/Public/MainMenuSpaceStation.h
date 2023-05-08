// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainMenuSpaceStation.generated.h"

UCLASS()
class DES310_PROJECT_API AMainMenuSpaceStation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainMenuSpaceStation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere) USceneComponent* Root;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* Mesh1;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* Mesh2;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* EmptyMesh;
	UPROPERTY(EditAnywhere) bool RandomizeDirection;
	
	UPROPERTY(EditAnywhere) FRotator Rotations;

};
