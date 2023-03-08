// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Vendor.h"
#include "Quest.h"
#include "GameFramework/Actor.h"
#include "SpaceStation.generated.h"

UCLASS()
class DES310_PROJECT_API ASpaceStation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpaceStation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(EditAnywhere)
	FString Name = "SpaceStation";

	UPROPERTY(EditAnywhere) USceneComponent* DummyRoot;
	UPROPERTY(EditAnywhere, Category = Camera) USpringArmComponent* CameraBoom;
	UPROPERTY(EditAnywhere, Category = Camera) UCameraComponent* Camera;
	
	UPROPERTY(EditAnywhere) UStaticMeshComponent* Mesh;

	UPROPERTY(Category = Rotation, EditAnywhere) FRotator RotationPerFrame;
	UPROPERTY(EditAnywhere) TSubclassOf<class AVendor> Vendor;
	// store casted actor from blueprint
	UPROPERTY(VisibleAnywhere) AVendor* VendorActor = nullptr;


	UPROPERTY(EditAnywhere, Category = Camera) float CameraDistance = 4000;

	// temporary 
	// quests will have to be created at runtime
	UPROPERTY(EditAnywhere)
	TSubclassOf<UQuest> QuestTemplate;
	UQuest* Quest;


	// keeps track if player is in this planet
	bool CurrentPlanet = false;


	UFUNCTION(BlueprintCallable)
	bool IsCurrentPlanet() { return CurrentPlanet; };
	UFUNCTION(BlueprintCallable)
	AVendor* GetVendor() { return VendorActor; };

	UFUNCTION(BlueprintCallable)
	UQuest* GetQuest() { return Quest; };
};
