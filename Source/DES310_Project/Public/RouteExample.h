// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 10/02/23
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AStar.h"
#include "PoissonDiscSampling.h"
#include "Planet.h"
#include "Path.h"
#include "Components/SplineComponent.h"
#include "DelaunayTriangulation.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "BaseState.h"
#include "FightingState.h"
#include "MovingState.h"
#include "SelectingState.h"
#include "OrbitingState.h"
#include "RandomEventsComponent.h"
#include "RouteExample.generated.h"

UENUM(BlueprintType)
enum PlayerStates
{
	Moving = 0,
	Orbiting,
	Selecting,
	Fighting,
	Event
};

struct PathData
{
	TArray<USplineComponent*> Splines;
	TArray<APlanet*> Stops;
	int Index = 0;
	int Max = 0; // Could probably just use Stops.Num
	int EventChance = 10;
};

UCLASS()
class DES310_PROJECT_API ARouteExample : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARouteExample();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Generate();
	APath* CreateBasicCube(FTransform transform);
	APlanet* CreateBasicSphere(FTransform transform);
	TArray<FVector2D> SmoothLine(TArray<FVector2D>& vect);
	void SwitchCamera();

	//State Variables
	BaseState* CurrentState;
	FightingState* FightingState;
	SelectingState* SelectingState;
	OrbitingState* OrbitingState;
	MovingState* MovingState;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	URandomEventsComponent* EventsComponent;

	
	//States for now will be do with just if statements but could possibly be deligated to their own classes
	bool MoveAlongPath(PathData& PathData, float DeltaTime); 
	void OrbitPlanet(PathData& PathData, float DeltaTime);
	void SelectPath();

	UFUNCTION(BlueprintCallable)
		void TransitionToMap();

	int CameraIndex = 0;

	PathData RouteData;
	
	USplineComponent* SplineComponent1;
	USplineComponent* SplineComponent2;
	USplineComponent* SplineComponent3;

	USplineComponent* CurrentSpline;
	APlanet* CurrentPlanet;

	APlayerController* PlayerController;

	UPROPERTY(EditAnywhere, Category = Camera)
	UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, Category = Camera)
	USpringArmComponent* CameraBoom;


	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraTransitionSpeed = 5;

	
	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraDistance = 20000;

	float orbitTimer = 0;

	UPROPERTY(EditAnywhere)
	float PlayerTravelTime = 10;
	
	UPROPERTY(EditAnywhere)
		float SmoothFactor = 1;
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APlanet>> PlanetBP;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APath> PathBP;
	
	UPROPERTY(EditAnywhere)
	int RejectionRate = 30;
	
	UPROPERTY(EditAnywhere)
	int PathsWanted = 6;
	
	UPROPERTY(EditAnywhere)
	float PointRadius = 60;

	UPROPERTY(EditAnywhere)
	float DisplayRadius = 60;

	UPROPERTY(EditAnywhere)
	FVector2D Dimensions = FVector2D(300,300);
	
	UPROPERTY(EditAnywhere)
	float RouteTickRate = 2;

	UPROPERTY(EditAnywhere)
	float CameraRate = 2;

	UPROPERTY(EditAnywhere)
	float SpinRate = 2;

	UPROPERTY(EditAnywhere)
		AActor* cubeBP;

	TArray<APath*> CubePath1;
	TArray<FVector2D> Path1;
	FVector AveragePathPosition1;
	FVector2D AveragePathPosition12D; // TODO better names ;(
	
	TArray<APath*> CubePath2;
	TArray<FVector2D> Path2;
	FVector AveragePathPosition2;
	FVector2D AveragePathPosition22D;

	TArray<APath*> CubePath3;
	TArray<FVector2D> Path3;

	TArray<APlanet*> Planets;

	PlayerStates PlayerState;

	UFUNCTION(BlueprintCallable)
		PlayerStates GetPlayerState() { return PlayerState; };
	
	float timer = 0;
	float cameraTimer = 0;
	float splineTimer = 0;
	float angle = 0;
	float randomSpinRate = 1;

	UStaticMesh* CubeMesh;
	UStaticMesh* SphereMesh;

};
