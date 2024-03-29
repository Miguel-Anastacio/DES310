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
//#include "Components/WidgetComponent.h"
//#include "Components/SlateWrapperTypes.h"
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

	UPROPERTY(EditAnywhere, Category = Type) //Keep Track of the type for different functinality
	bool IsCheckpoint = false;

	UPROPERTY(BlueprintReadWrite) bool IsFirstPlanet = false;

	//Not Longer used but was used for the UI widget that show stats about the planet
	UPROPERTY(BlueprintReadWrite) FText Line1;
	UPROPERTY(BlueprintReadWrite) FText Line2;
	UPROPERTY(BlueprintReadWrite) FText Line3;
	UPROPERTY(BlueprintReadWrite) bool HideUI = true;

	UPROPERTY(Category = Rotation, EditAnywhere) // Controls the planet spinning
	FRotator RotationPerFrame;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AVendor> Vendor;
	// store casted actor from blueprint
	UPROPERTY(VisibleAnywhere)
	AVendor* VendorActor = nullptr;

	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraDistance = 4000;
	
	// UI icons for navigation
	// 0 - you are here 1 - checkpoint 2 - destination
	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> AllIcons;
	UPROPERTY(BlueprintReadWrite)
	UTexture2D* Icon = nullptr;

	UFUNCTION(BlueprintCallable)
		void SetPlanetIconUI();
	
	// keeps track if player is in this planet
	bool CurrentPlanet = false;
	// Sets default values for this actor's properties
	APlanet();

	UFUNCTION(BlueprintCallable)
	bool IsCurrentPlanet() { return CurrentPlanet; };

	UFUNCTION(BlueprintCallable)
	AVendor* GetVendor() { return VendorActor; };

	int Index = -1;
	
	UFUNCTION()
		void OnPlanetDestroyed(AActor* Act);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
