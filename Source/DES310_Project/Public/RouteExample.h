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
#include "PathData.h"
#include "AudioManager.h"
#include "Enemy.h"
#include "Bullet_CPP.h"
/*#include "BaseState.h"
#include "FightingState.h"
#include "MovingState.h"
#include "SelectingState.h"
#include "OrbitingState.h"*/
#include "RandomEventsComponent.h"
#include "Sound/SoundCue.h"


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




// binding an event th when the state of the game changes
// this is the delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOrbitTransitionDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovingTransitionDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectingTransitionDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCheckpointTransitionDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeginOrbitTransitionDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatTransitionDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatOverTransitionDelegate);

// delegate to notify UI when user presses on a route
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPathClickedDelegate, UPathData*, CurrentPath);

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
	void GenerateDetails();
	void ClearRouteData();
	APath* CreateBasicCube(FTransform transform);
	APlanet* CreatePlanetMainRoute(FTransform transform);
	APlanet* CreatePlanet(FTransform transform, int i);
	
	void SwitchCamera();

	//State Variables
	/*BaseState* CurrentState;
	FightingState* FightingState;
	SelectingState* SelectingState;
	OrbitingState* OrbitingState;
	MovingState* MovingState;*/


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		URandomEventsComponent* EventsComponent;


	UPROPERTY(EditAnywhere,BlueprintReadWrite) AAudioManager* AudioManager;
	UPROPERTY(EditAnywhere,BlueprintReadWrite) FRotator TempAngler = FRotator(0,-30,0);
	
	//States for now will be do with just if statements but could possibly be deligated to their own classes
	bool MoveAlongPath(UPathData* PathData, float DeltaTime);
	void OrbitPlanet(UPathData* PathData, float DeltaTime);
	void SelectPath();

	UFUNCTION(BlueprintCallable)
		void TransitionToMap();

	int CameraIndex = 0;
	FVector positionOffset;
	

	UPROPERTY(EditAnywhere, Category = Skybox) UMaterial* SkyboxMaterial;
	UPROPERTY(EditAnywhere, Category = Skybox) UStaticMesh* SkyboxReference;
	
	UPROPERTY(BlueprintReadWrite) bool Temp;
	UPROPERTY(BlueprintReadWrite) bool Temp2;
	
	UPROPERTY() USplineComponent* SplineComponent1;
	UPROPERTY() USplineComponent* SplineComponent2;
	UPROPERTY() USplineComponent* SplineComponent3;

	UPROPERTY() USplineComponent* CameraSplineComponent1;
	UPROPERTY() USplineComponent* CameraSplineComponent2;
	UPROPERTY() USplineComponent* CameraSplineComponent3;

	UPROPERTY() USplineComponent* CurrentSpline;
	
	UPROPERTY(BlueprintReadOnly)
	APlanet* CurrentPlanet;

	APlayerController* PlayerController;
	ASpaceshipCharacter* SpaceshipCharacter;

	std::vector<int> PlanetIndex;
	
	UPROPERTY(EditAnywhere, Category = Camera) UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera) USpringArmComponent* CameraBoom;
	UPROPERTY(EditAnywhere, Category = Camera) float CameraTransitionSpeed = 5;
	UPROPERTY(EditAnywhere, Category = Camera) float CameraDistance = 20000;
	UPROPERTY(EditAnywhere, Category = Camera) float CameraLerpSpeed = 0.1;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Route) float PlayerMovementSpeed = 10;
	UPROPERTY(EditAnywhere, Category = Route) FVector2D PathStartEndPercent = FVector2D(0.1,0.9);
	UPROPERTY(EditAnywhere, Category = Route) int OverallPaths = 3;
	UPROPERTY(EditAnywhere, Category = Route) float PathHeightOffset = 20;
	UPROPERTY(EditAnywhere, Category = Route) float SinWaveAmplitude = 50;
	UPROPERTY(EditAnywhere, Category = Route) float PlanetScaling = 1;
	
	UPROPERTY(EditAnywhere, Category = BpActors) TArray<TSubclassOf<class APlanet>> PlanetBP;
	UPROPERTY(EditAnywhere, Category = BpActors) TArray<TSubclassOf<class APlanet>> SpaceStationBP;
	UPROPERTY(EditAnywhere, Category = BpActors) TSubclassOf<class APath> PathBP;

	UPROPERTY(EditAnywhere, Category = Poisson) FVector2D Dimensions = FVector2D(300, 300);
	UPROPERTY(EditAnywhere, Category = Poisson) float PointRadius = 60;
	UPROPERTY(EditAnywhere, Category = Poisson) float DisplayRadius = 60; // Can be Deleted
	UPROPERTY(EditAnywhere, Category = Poisson) int RejectionRate = 30;
	UPROPERTY(EditAnywhere, Category = Poisson) int PathsWanted = 6;
	UPROPERTY(EditAnywhere, Category = Poisson) int DetailsWanted = 6;
	UPROPERTY(EditAnywhere, Category = Poisson) int DetailRejectionRate = 10;
	UPROPERTY(EditAnywhere, Category = Poisson) float DetailMinDistance = 1000;


	
	UPROPERTY(EditAnywhere) float CombatTick = 5;
	UPROPERTY(EditAnywhere) float CombatChance= 30;
	float SuperTempTimer = 0;

	UPROPERTY(VisibleAnywhere) UPathData* RouteData;


	UPROPERTY(EditAnywhere) float RouteTickRate = 200000000;
	UPROPERTY(EditAnywhere) float CameraRate = 2;
	UPROPERTY(EditAnywhere) float SpinRate = 2;

	UPROPERTY(EditAnywhere, Category = Fight) FVector ppVec = FVector(0,150,0);
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Fight) FVector TempEnemyPosition = FVector(300, 600, 500);
	UPROPERTY(EditAnywhere, Category = Fight) float HomingStrength = 2000.f;

	
	TArray<TArray<APath*>> Hello;

	TArray<APath*> CubePath1;
	TArray<FVector2D> Path1;


	TArray<APath*> CubePath2;
	TArray<FVector2D> Path2;


	TArray<APath*> CubePath3;
	TArray<FVector2D> Path3;

	TArray<APlanet*> Planets;
	TArray<APlanet*> Details;


	PlayerStates PlayerState;
	PlayerStates PreviousState;

	UFUNCTION(BlueprintCallable)
		PlayerStates GetPlayerState() { return PlayerState; };

	float timer = 0;
	float cameraTimer = 0;
	float splineTimer = 0;
	float angle = 0;
	float randomSpinRate = 1;

	UStaticMesh* CubeMesh;
	UStaticMesh* SphereMesh;
	UFUNCTION(BlueprintCallable)
	void SwapState(PlayerStates State);


	UFUNCTION() void SwapToOrbiting();
	UFUNCTION() void BeginToOrbiting();
	UFUNCTION() void SwapToMoving();
	UFUNCTION() void SwapToSelecting();
	UFUNCTION() void SwapToCombat();
	
	UFUNCTION(BlueprintCallable) void GetPathSelected(UPathData* path);
	UFUNCTION(BlueprintCallable) void LeaveOrbit();
	
	
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable)FOrbitTransitionDelegate OrbitTransitionDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable) FBeginOrbitTransitionDelegate BeginOrbitTransitionDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable)	FMovingTransitionDelegate MovingTransitionDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable)	FSelectingTransitionDelegate SelectTransitionDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable)FPathClickedDelegate PathClickedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable)FCheckpointTransitionDelegate CheckpointTransitionDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable )FCombatTransitionDelegate CombatTransitionDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable )FCombatTransitionDelegate CombatOverTransitionDelegate;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		UCameraComponent* FightCamera;

	UPROPERTY() AEnemy* AEnemyActor; // Could be turned into a array and have multiple enemies
	UPROPERTY(EditAnywhere) TSubclassOf<class AEnemy> MyEnemy;

	
	UFUNCTION(BlueprintCallable)
	void StartGame();
	// used to hide the route when the player transitions to orbiting
	// and to display it when it chnages to selecting
	void ChangeVisibilityOfRoute(bool toHide);

	// used to make sure that we don't repeat planets on the same route
	std::vector<int> indexOfPlanetsInUse;

	void SetQuest();

	void FightScene(float DeltaTime);
	void CombatReset(ASpaceshipCharacter* Player);
};
