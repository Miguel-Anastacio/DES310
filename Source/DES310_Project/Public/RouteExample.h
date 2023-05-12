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
#include "RouteSpline.h"
#include "Components/SplineComponent.h"
#include "DelaunayTriangulation.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "PathData.h"
#include "AudioManager.h"
#include "Enemy.h"
#include "Details.h"
#include "Bullet_CPP.h"
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameOverDelegate);

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

	void CreatePath(TArray<FVector2D>& Path, TArray<APath*>& PathMeshes,USplineComponent* SplineComponent,float PathPercentage);

	void ClearRouteData();
	APath* CreateBasicCube(FTransform transform);
	APlanet* CreatePlanetMainRoute(FTransform transform);
	APlanet* CreatePlanet(FTransform transform, int i);
	ADetails* CreateDetail(FTransform transform, int Index);
	
	void SwitchCamera();

	//State Variables
	/*BaseState* CurrentState;
	FightingState* FightingState;
	SelectingState* SelectingState;
	OrbitingState* OrbitingState;
	MovingState* MovingState;*/


	//---Procedural Route---
	void Generate(); //Original Functions which isn't used, but can be used as reference
	void GenerateImproved(int FirstPlanetID, FVector Offset); //Using Poisson,Delaunay and A* We generate a procedural route with various parameters 
	void GenerateLoad(TArray<FVector> PlanetPositions, TArray<int> PlanetIDs); //Generates a route but using Pre-selected Planets, in order to generate a similar route from the loaded game
	void ResetRoute(); // Reset Everything to do with the Route Data and meshes
	void GenerateDetails(); // After Generating the Route, we run the Detail Generator which places astroids without colliding with the route

	
	UPROPERTY(EditAnywhere, Category = Poisson) float BuoysPercent = 50;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		URandomEventsComponent* EventsComponent;


	UPROPERTY(EditAnywhere,BlueprintReadWrite) AAudioManager* AudioManager;
	UPROPERTY(EditAnywhere,BlueprintReadWrite) FRotator CameraAngle = FRotator(0,-30,0);
	
	//States for now will be do with just if statements but could possibly be deligated to their own classes
	bool MoveAlongPath(UPathData* PathData, float DeltaTime);
	void OrbitPlanet(UPathData* PathData, float DeltaTime);
	void SelectPath();

	UFUNCTION(BlueprintCallable) FVector2D GetPlanetsOnScreenPosition(int Index);
	
	UFUNCTION(BlueprintCallable)
		void TransitionToMap();

	int CameraIndex = 0;
	FVector positionOffset;
	

	UPROPERTY(EditAnywhere, Category = Skybox) UMaterial* SkyboxMaterial;
	UPROPERTY(EditAnywhere, Category = Skybox) UStaticMesh* SkyboxReference;
	
	UPROPERTY(BlueprintReadWrite) bool Temp;
	UPROPERTY(BlueprintReadWrite) bool Temp2;
	

	UPROPERTY(EditAnywhere, Category = BpActors) TSubclassOf<class ARouteSpline> SplineBP;
	UPROPERTY(VisibleAnywhere) ARouteSpline* Spline1;
	UPROPERTY(VisibleAnywhere) ARouteSpline* Spline2;
	UPROPERTY(VisibleAnywhere) ARouteSpline* Spline3;

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
	UPROPERTY(EditAnywhere, Category = Route) float DetailScaling = 1;
	
	UPROPERTY(EditAnywhere, Category = BpActors) TArray<TSubclassOf<class APlanet>> PlanetBP;
	UPROPERTY(EditAnywhere, Category = BpActors) TArray<TSubclassOf<class ADetails>> DetailBP;
	UPROPERTY(EditAnywhere, Category = BpActors) TArray<TSubclassOf<class APlanet>> SpaceStationBP;
	UPROPERTY(EditAnywhere, Category = BpActors) TArray<TSubclassOf<class APlanet>> AsteroidClusters;
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


	UPROPERTY(VisibleAnywhere) UPathData* RouteData;
	UPROPERTY(VisibleAnywhere) UPathData* Route1Data;
	UPROPERTY(VisibleAnywhere) UPathData* Route2Data;




	UPROPERTY(EditAnywhere, Category = Fight) FVector ppVec = FVector(0,150,0);
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Fight) FVector TempEnemyPosition = FVector(300, 600, 500);
	UPROPERTY(EditAnywhere, Category = Fight) float HomingStrength = 2000.f;

	bool IsRouteGood();
	

	UPROPERTY() TArray<APath*> StartToEndPath;
	TArray<FVector2D> Path1;
	
	UPROPERTY() TArray<APath*> StartToStationPath;
	TArray<FVector2D> Path2;
	
	UPROPERTY() TArray<APath*> StationToEndPath;
	TArray<FVector2D> Path3;

	UPROPERTY() TArray<APlanet*> Planets;
	UPROPERTY() TArray<ADetails*> Details;

	// store the quest of the last planet of the previous route to set it as the quest of the first planet 
	// of the next route
	UQuest* LastQuestPreviousRoute = nullptr;


	//---Timers---
	float timer = 0;
	float cameraTimer = 0;
	float splineTimer = 0;

	float CombatTimer = 0;
	UPROPERTY(EditAnywhere) float RouteTickRate = 200000000;
	UPROPERTY(EditAnywhere) float CameraRate = 2;
	UPROPERTY(EditAnywhere) float SpinRate = 2;


	// minimum time that nothing can happen after something happens during navigation
	UPROPERTY(EditAnywhere)
	float NavIncidentsCooldown = 3.0f;
	float NavIncidentsTimer = 0.0f;


	
	//---States---

	//Variables to keep track of where the player is and was
	PlayerStates PlayerState;
	PlayerStates PreviousState;
	
	//Used to tie up previous states, depending on what state you came from
	//E.g stop playing combat if previous state was combat
	UFUNCTION(BlueprintCallable) void SwapState(PlayerStates State);
	
	//Used to prepare the next state, useful when you swap in from multiple different states
	UFUNCTION() void SwapToOrbiting();
	UFUNCTION() void BeginToOrbiting();
	UFUNCTION() void SwapToMoving();
	UFUNCTION() void SwapToSelecting();
	UFUNCTION() void SwapToCombat();

	//Blueprint Functionality
	UFUNCTION(BlueprintCallable)PlayerStates GetPlayerState() { return PlayerState; };
	UFUNCTION(BlueprintCallable)void SetPlayerState(PlayerStates State) { PlayerState = State; };



	
	UFUNCTION(BlueprintCallable) void GetPathSelected(UPathData* path);
	UFUNCTION(BlueprintCallable) void LeaveOrbit();
	UFUNCTION(BlueprintCallable) UPathData* SelectRoute(bool WhichRoute);
	UFUNCTION(BlueprintCallable) void FinalSelectRoute();
	bool SelectedPath = false;
	
	//---Delegates---
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable)FOrbitTransitionDelegate OrbitTransitionDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable) FBeginOrbitTransitionDelegate BeginOrbitTransitionDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable)	FMovingTransitionDelegate MovingTransitionDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable)	FSelectingTransitionDelegate SelectTransitionDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable)FPathClickedDelegate PathClickedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable)FCheckpointTransitionDelegate CheckpointTransitionDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable )FCombatTransitionDelegate CombatTransitionDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable )FCombatTransitionDelegate CombatOverTransitionDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Transitions", BlueprintCallable )FCombatTransitionDelegate GameOverDelegate;



	
	UFUNCTION(BlueprintCallable)
	void StartGame();
	// used to hide the route when the player transitions to orbiting
	// and to display it when it chnages to selecting
	void ChangeVisibilityOfRoute(bool toHide);

	// used to make sure that we don't repeat planets on the same route
	std::vector<int> indexOfPlanetsInUse;

	void SetQuest();

	//---Combat---
	UFUNCTION() void FightScene(float DeltaTime);
	UFUNCTION() void CombatReset();
	UFUNCTION() void CallCombatOverDelegate();
	UFUNCTION(BlueprintCallable) void ResetCameraAfterCombat();

	UPROPERTY(EditAnywhere) TSubclassOf<class AEnemy> MyEnemy;
	UPROPERTY(VisibleAnywhere, Category = Camera) UCameraComponent* FightCamera;
	UPROPERTY() AEnemy* AEnemyActor; // Could be turned into a array and have multiple enemies
};

