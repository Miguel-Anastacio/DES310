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
#include "Components/PointLightComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SlateWrapperTypes.h"
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

	UPROPERTY(EditAnywhere, Category = Light)
	UPointLightComponent* PointLight;

	/** Static Mesh Comp, Set In BP Default Properties */
	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PlanetMeshComponent;

	UPROPERTY(EditAnywhere, Category = Type)
	bool IsCheckpoint = false;

	UPROPERTY(BlueprintReadWrite)bool IsFirstPlanet = false;
	UPROPERTY(BlueprintReadWrite) FText Line1;
	UPROPERTY(BlueprintReadWrite) FText Line2;
	UPROPERTY(BlueprintReadWrite) FText Line3;
	UPROPERTY(BlueprintReadWrite) bool HideUI = true;
;
	
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


	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraDistance = 4000;

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


	int Index = -1;
	
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
