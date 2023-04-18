// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "RouteSpline.generated.h"

UCLASS()
class DES310_PROJECT_API ARouteSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARouteSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetMaterial(int MaterialIndex);
	//virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere) USplineComponent* Spline;
	UPROPERTY(EditAnywhere) UStaticMesh* SplineMesh;
	UPROPERTY(EditAnywhere) TArray<UMaterial*> Materials;
	UPROPERTY(VisibleAnywhere) TArray<USplineMeshComponent*> Meshes;

	static int counter;
	
	FVector off = FVector(0.15,100,100);
	FVector Total;
	void CreateSpline();

};

