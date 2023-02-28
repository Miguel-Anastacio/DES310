// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Vendor.h"
#include "Quest.h"
#include "Planet.generated.h"


UCLASS()
class DES310_PROJECT_API APlanet : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere)
	FString Name = "Planet";

	UPROPERTY(EditAnywhere)
	USceneComponent* DummyRoot;

	UPROPERTY(EditAnywhere, Category = Camera)
	USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, Category = Camera)
	UCameraComponent* FocusPlanetCamera;

	/** Static Mesh Comp, Set In BP Default Properties */
	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PlanetMeshComponent;

	UPROPERTY(Category = Rotation, EditAnywhere)
	FRotator RotationPerFrame;
	
	/*
	UPROPERTY(EditAnywhere)
	USphereComponent* SphereCollisionComponent;
	*/

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AVendor> Vendor;
	// store casted actor from blueprint
	UPROPERTY(VisibleAnywhere)
	AVendor* VendorActor = nullptr;

	// temporary 
	// quests will have to be created at runtime
	UPROPERTY(EditAnywhere)
	TSubclassOf<UQuest> QuestTemplate;
	UQuest* Quest;



	// keeps track if player is in this planet
	bool CurrentPlanet = false;
	// Sets default values for this actor's properties
	APlanet();

	UFUNCTION(BlueprintCallable)
	bool IsCurrentPlanet() { return CurrentPlanet; };
	UFUNCTION(BlueprintCallable)
	AVendor* GetVendor() { return VendorActor; };

	UFUNCTION(BlueprintCallable)
	UQuest* GetQuest() { return Quest; };


	/*UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);*/


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
