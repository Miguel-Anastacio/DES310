// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpaceSkyBox.generated.h"

UCLASS()
class DES310_PROJECT_API ASpaceSkyBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpaceSkyBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OffsetColor();
	UFUNCTION(BlueprintCallable) void SetColor(float Hue);

	UPROPERTY(EditAnywhere, Category = Skybox) UMaterialInterface* Material;
	UPROPERTY(EditAnywhere, Category = Skybox) UStaticMeshComponent* SkyMesh;
	UPROPERTY(EditAnywhere, Category = Skybox) UMaterialInstanceDynamic* DynamicMaterial;
	
	UPROPERTY(EditAnywhere, Category = Skybox) bool RandomizeSkyBox;
	UPROPERTY(EditAnywhere, Category = Skybox) bool RandomizeEverything;
	UPROPERTY(EditAnywhere, Category = Skybox) bool Blend;
	UPROPERTY(EditAnywhere, Category = Skybox) float Offset = 1;
	float CurrentHue = 0;
	float TargetHue = 0;

};
