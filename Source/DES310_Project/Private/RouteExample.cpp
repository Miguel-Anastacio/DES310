// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 10/02/23
//


#include "RouteExample.h"

#include <string>

#include "Kismet/KismetMathLibrary.h"
#include "SpaceshipCharacter.h"

#include "GameInstance_CPP.h"
#include "SpaceSkyBox.h"
#include "StatsComponent.h"
#include "Components/AudioComponent.h"


// Sets default values
ARouteExample::ARouteExample()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Give the Route a "root" so it can be moved about in the scene
	USceneComponent* root = this->CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	root->Mobility = EComponentMobility::Type::Movable;
	this->SetRootComponent(root);

	//Set up Camera Boom, giving it independent rotation
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->SetUsingAbsoluteScale(false);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetWorldRotation(FRotator(-90, 180, 0));
	
	//Set up camera and attach to boom
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Route Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	//Set up a separate camera for when a combat event takes place
	FightCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Fight Camera"));
	FightCamera->SetupAttachment(RootComponent);

	//Create Route Data to Store all necessary data about the current route
	RouteData = CreateDefaultSubobject<UPathData>(TEXT("Route Data"));
	//These hold the default data for the short and long route
	Route1Data = CreateDefaultSubobject<UPathData>(TEXT("Route1 Data"));
	Route2Data = CreateDefaultSubobject<UPathData>(TEXT("Route2 Data"));
	
	// Create Event Component
	EventsComponent = CreateDefaultSubobject<URandomEventsComponent>(TEXT("Events Component"));
	
}

// Called when the game starts or when spawned
void ARouteExample::BeginPlay()
{
	Super::BeginPlay();

	//Initialize Timers
	splineTimer = PathStartEndPercent.X; // Initialize Timer at Start point
	CombatTimer = 0;
	
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetShowMouseCursor(true); // We want a cursed to displayed for most of the game

	//Initialize Player
	SpaceshipCharacter = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)); // Create pointer reference to player
	SpaceshipCharacter->AudioManager = AudioManager;
	PlayerState = None; // Dont want to begin the main loop if its not been generated
	
	//Start the ambient music as soon as we start
	AudioManager->AmbientSoundComponent->Play();
	
	//Initialize Cameras
	CameraBoom->TargetArmLength = CameraBoom->TargetArmLength * this->GetActorScale().GetMax() * 2; 
	FightCamera->SetWorldLocation(FVector(0, 0, 3000.f));
	FightCamera->SetActive(false);

	
	//Initialize Splines to represent the routes 
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;
	
	Spline1 = GetWorld()->SpawnActor<ARouteSpline>(SplineBP,GetTransform(),SpawnParam);
	Spline1->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

	Spline2 = GetWorld()->SpawnActor<ARouteSpline>(SplineBP,GetTransform(),SpawnParam);
	Spline2->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	
	Spline3 = GetWorld()->SpawnActor<ARouteSpline>(SplineBP,GetTransform(),SpawnParam);
	Spline3->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

	//Set up Delegates, They are used to allow c++ funcs to call blueprint functionality 
	//is useful for telling UI when to enabled/disable ect..
	OrbitTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToOrbiting);
	BeginOrbitTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::BeginToOrbiting);
	CheckpointTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToOrbiting);
	MovingTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToMoving);
	SelectTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToSelecting);
	CombatTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToCombat);
	GameOverDelegate.AddUniqueDynamic(this, &ARouteExample::GameOver);
	PathClickedDelegate.AddUniqueDynamic(this, &ARouteExample::GetPathSelected);
	AudioManager->VictorySoundComponent->OnAudioFinished.AddUniqueDynamic(this, &ARouteExample::CallCombatOverDelegate);

}

// Called every frame
void ARouteExample::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	timer += DeltaTime;


	UGameInstance_CPP* GameInstance = Cast<UGameInstance_CPP>(UGameplayStatics::GetGameInstance(GetWorld())); // Get a reference to the game instance
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple,	FString::Printf(TEXT("State Num: %i"), (int)PlayerState));
	ASpaceshipCharacter* PlayerOBJ = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	//tODO do psudo after save stuff is in
	switch (PlayerState)
	{
	case PlayerStates::Moving:
		NavIncidentsTimer += DeltaTime; // Increase timer for events
		MoveAlongPath(RouteData, DeltaTime); // Update the players position
		PlayerOBJ->UpdatePlayerSpeed(DeltaTime); // Update Mini-game Speed
		CombatTimer += DeltaTime;
		if (NavIncidentsTimer > NavIncidentsCooldown) //Roll for an event after a certain time
		{
			if (EventsComponent->RollForEvent(RouteData->EventChance, DeltaTime, RouteData->StoryEventChance, RouteData->RandomEventChance)) // if it passes,swap to event
			{
				SwapState(Event);
			}
			else if (CombatTimer > CombatTick) // Roll for a combat event after a certain time
				{
					if (FMath::RandRange(0, 100) < RouteData->CombatEventChance) // if it passes then swap to combat
					{
						CombatTimer = 0;
						CombatTransitionDelegate.Broadcast();
						SwapState(Event);
					}	
				}
		}
		
		break;
	case PlayerStates::Orbiting: OrbitPlanet(RouteData, DeltaTime);
		// to do - save route data when route generated
		GameInstance->SaveGameData();
		break;
	case PlayerStates::Selecting: //SelectPath();
		break;
	case PlayerStates::Event:
		break;
	case PlayerStates::Fighting: FightScene(DeltaTime);
		break;
	default:
		break;
	}

}

//Create a Buoy 
APath* ARouteExample::CreateBasicCube(FTransform transform)
{
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;
	
	APath* MyNewActor = GetWorld()->SpawnActor<APath>(PathBP, transform, SpawnParam);
	MyNewActor->SetActorLocation(MyNewActor->GetActorLocation() - FVector(0,0,PathHeightOffset));
	MyNewActor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	
	return MyNewActor;
}

//Create A Randomly selected Planet to be on the main route
APlanet* ARouteExample::CreatePlanetMainRoute(FTransform transform)
{

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	int planetIndex =  0;
	bool foundUnique = false;
	if (indexOfPlanetsInUse.size() > 0)
	{
		while (!foundUnique)
		{
			planetIndex = FMath::RandRange(0, PlanetBP.Num() - 1);

			for (int i = 0; i < indexOfPlanetsInUse.size(); i++)
			{
				if (planetIndex == indexOfPlanetsInUse[i])
				{
					foundUnique = false;
					break;
				}
				else
				{
					foundUnique = true;
				}
			}
		}


	}


	if(planetIndex == PlanetBP.Num() - 1 || planetIndex == PlanetBP.Num() - 2) // Last 2 planets are starts so makes it so only one star is possible
	{
		indexOfPlanetsInUse.push_back(PlanetBP.Num() - 1);
		indexOfPlanetsInUse.push_back( PlanetBP.Num() - 2);
	}
	else
	{
		indexOfPlanetsInUse.push_back(planetIndex);
	}
	
	APlanet* APlanetActor = nullptr;
	// index 0 is the checkpoint always? TODO this means the first element in planets is ignored
	if(planetIndex == 0)
		APlanetActor = GetWorld()->SpawnActor<APlanet>(SpaceStationBP[planetIndex], transform, SpawnParams);
	else
		APlanetActor = GetWorld()->SpawnActor<APlanet>(PlanetBP[planetIndex], transform, SpawnParams);
		

	APlanetActor->Index = planetIndex;
	APlanetActor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	
	return APlanetActor;
}

//Create a specific Planet
APlanet* ARouteExample::CreatePlanet(FTransform transform, int i)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	i = FMath::Clamp(i,1,PlanetBP.Num() - 1);

	APlanet* APlanetActor;
	
	if(i == 0)
		APlanetActor = GetWorld()->SpawnActor<APlanet>(SpaceStationBP[i], transform, SpawnParams);
	else
		APlanetActor = GetWorld()->SpawnActor<APlanet>(PlanetBP[i], transform, SpawnParams);
		
	
	APlanetActor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);


	if(i == PlanetBP.Num() - 1 || i == PlanetBP.Num() - 2) // Last 2 planets are starts so makes it so only one star is possible
	{
		indexOfPlanetsInUse.push_back(PlanetBP.Num() - 1);
		indexOfPlanetsInUse.push_back( PlanetBP.Num() - 2);
	}
	else
	{
		indexOfPlanetsInUse.push_back(i);
	}

	APlanetActor->Index = i;
	APlanetActor->IsFirstPlanet = true;
	APlanetActor->SetPlanetIconUI();
	APlanetActor->Line2 = FText::FromString("You Are Here");
	APlanetActor->Line3 = FText::FromString(" ");
	
	return APlanetActor;
}

//Create a random detail from the list of available
ADetails* ARouteExample::CreateDetail(FTransform transform, int Index)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	
	Index = FMath::Clamp(Index,0,DetailBP.Num() - 1);

	ADetails* Detail = GetWorld()->SpawnActor<ADetails>(DetailBP[Index], transform, SpawnParams);
	Detail->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	Detail->Index = Index;
	
	return Detail;
}


void ARouteExample::GenerateImproved(int FirstPlanetID, FVector Offset)
{

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, FString::Printf(TEXT("Index = %i"), FirstPlanetID));
	
	do // Do while we have a valid route
	{
		
	ResetRoute();
	
	//Manually Add the Start and End planets to the possible routes
	FVector2D Max(FMath::RandRange(0.0,Dimensions.X), Dimensions.Y + PointRadius);
	FVector2D Min(FMath::RandRange(0.0,Dimensions.X), 0 - PointRadius);

	//Generate a random but evenly distributed number of points to represent the routes
	TArray<FVector2D> points = PoissonDiscSampling::PoissonDiscGenerator(PointRadius, FVector2D((int)Dimensions.X, (int)Dimensions.Y), RejectionRate);
	points.Add(Max);
	points.Add(Min);

	//Create an evenly distributed amount of connections between each point	
	TArray<Triangle> triangleList = DelaunayTriangulation::GenerateTriangulation(points);

	AStar astar;


	FVector2D min = points[0];	//Min = First Planet	
	FVector2D max = points[0];	//Max = Last Planet	
	float spaceStation = abs(points[0].X);	//spacestation = checkpoint

	int minID = 0;
	int maxID = 0;
	int spaceStationID = 0;

	int sign = 1;
	if(FMath::RandBool()) // pick weather the space station will be up or down
	{
		sign = -1;
	}
	
	for (int i = 0; i < points.Num(); i++) // pass all the vertex points to a-star
	{
		astar.AddPoint(points[i]);
	}

	//Loop Through all triangles in the list and add their connections to the a star points
	for (int i = 0; i < triangleList.Num(); i++)
	{
		TArray<FVector2D> Points;
		TArray<int> IDs;
		for(int j = 0; j < 3 ;j++)
		{
			Points.Add(triangleList[i].Vertex[j]);
			IDs.Add( astar.AddPoint(Points[j]));

			if (min.Y > Points[j].Y) //Check if this is the furthest left point
			{
				minID = IDs[j];

			}
			else if (max.Y <Points[j].Y) //Check if this is the furthest right point
			{
				maxID = IDs[j];
			}

			if (sign * abs(Points[j].X) > sign * spaceStation && Points[j].Y < Dimensions.Y * 0.8 && Points[j].Y > Dimensions.Y * 0.2) // 
			{
				/*if(IDs[j] != minID && IDs[j] != maxID) // We dont want the start or end to be a valid candidate for the space station
				{
					maxX = abs(Points[j].X);
					maxXID = IDs[j];
				}*/
				spaceStation = abs(Points[j].X);
				spaceStationID = IDs[j];
			}
		}
		
		//Connect the each point, since they are points of a triangle
		astar.ConnectPoints(IDs[0], IDs[1]);
		astar.ConnectPoints(IDs[1], IDs[2]);
		astar.ConnectPoints(IDs[2], IDs[0]);
		
	}
		

	astar.search(minID,maxID); // Find a route from Start to End planet
	astar.LockCurrentPath();
	Path1 = astar.path;
	
	astar.search(minID, spaceStationID); // Find a route to the start planet and space station
	astar.LockCurrentPath();
	Path2 = astar.path;

	astar.search(spaceStationID, maxID); // Find a route to the space station and end planet
	astar.LockCurrentPath();
	Path3 = astar.path;
	FVector2D SpaceStation = astar.begin.position;

	//Place buoys along path and se up the spline	
	CreatePath(Path1,StartToEndPath,Spline1->Spline, BuoysPercent);
	CreatePath(Path2,StartToStationPath,Spline2->Spline, BuoysPercent);
	CreatePath(Path3,StationToEndPath,Spline3->Spline, BuoysPercent);

	FTransform WorldTransform;
	WorldTransform = GetRootComponent()->GetComponentTransform();
		
	if(FirstPlanetID < 0) // Make sure we have a valid index for the planet
	{
		FirstPlanetID = FMath::RandRange(1,PlanetBP.Num());
	}
	
	//Spawn random planets at the generated positions
	FTransform SpawnTransform;
	if (Planets.Num() < 3)
	{
		SpawnTransform.SetRotation(FQuat4d(0, 0, 0, 1.f));
		SpawnTransform.SetScale3D(FVector(PlanetScaling, PlanetScaling, PlanetScaling));
		SpawnTransform.SetLocation(FVector(SpaceStation.X - Dimensions.X / 2, SpaceStation.Y - Dimensions.Y / 2, 0));
		Planets.Add(CreatePlanetMainRoute(SpawnTransform * WorldTransform));
		SpawnTransform.SetLocation(FVector(Min.X - Dimensions.X / 2, Min.Y - Dimensions.Y / 2 , 0));
		Planets.Add(CreatePlanet(SpawnTransform * WorldTransform,FirstPlanetID));
		SpawnTransform.SetLocation(FVector(Max.X - Dimensions.X / 2, Max.Y - Dimensions.Y / 2, 0));
		Planets.Add(CreatePlanetMainRoute(SpawnTransform * WorldTransform));
	}

	//Generate Details around the completed route	
	GenerateDetails();
	
	
	SpawnTransform *= WorldTransform;

	auto playerController = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (playerController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Player Moved")));

		playerController->SetActorLocation(FVector(SpawnTransform.GetLocation().X, SpawnTransform.GetLocation().Y, 1000)); // Initialize player position
	}

	//Set the correct text for each planet/stop	
	std::string Line = "Distance: ";
	int length = Spline2->Spline->GetSplineLength() / 20;
	Line += std::to_string(length);
	Line += " AU";
	Planets[0]->Line3 = FText::FromString(FString(Line.c_str()));

	Line = "Distance: ";
	length = Spline1->Spline->GetSplineLength() / 20;
	Line += std::to_string(length);
	Line += " AU";
	Planets[2]->Line3 = FText::FromString(FString(Line.c_str()));

	}
	while (!IsRouteGood()); // Check the route look correct / is valid
		
	// set quests
	SetQuest();
	
	//Create the splines that highlight on selection
	Spline1->CreateSpline(DetailScaling);
	Spline2->CreateSpline(DetailScaling);
	Spline3->CreateSpline(DetailScaling);

	//Hide the spline meshes until selection
	for(auto mesh : Spline1->Meshes)
	{
		mesh->SetVisibility(false);
	}
	for(auto mesh : Spline2->Meshes)
	{
		mesh->SetVisibility(false);
	}
	for(auto mesh : Spline3->Meshes)
	{
		mesh->SetVisibility(false);
	}

	// remove the high lighting
	Spline1->SetMaterial(1);
	Spline2->SetMaterial(1);
	Spline3->SetMaterial(1);


}

void ARouteExample::GenerateLoad(TArray<FVector> PlanetPositions, TArray<int> PlanetIDs)
{
	
	ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	
	do
	{
	
	ResetRoute();
	

	TArray<FVector2D> vect = PoissonDiscSampling::PoissonDiscGenerator(PointRadius, FVector2D((int)Dimensions.X, (int)Dimensions.Y), RejectionRate);
	FVector2D FirstPosition = FVector2D(PlanetPositions[0].X + Dimensions.X / 2,PlanetPositions[0].Y + Dimensions.Y / 2);
		FVector2D SSPosition = FVector2D(PlanetPositions[1].X + Dimensions.X / 2,PlanetPositions[1].Y + Dimensions.Y / 2);	
		FVector2D LastPosition = FVector2D(PlanetPositions[2].X + Dimensions.X / 2,PlanetPositions[2].Y + Dimensions.Y / 2);	

	vect.Add(FirstPosition); // First Planet
	vect.Add(SSPosition); // Space Station 
	vect.Add(LastPosition); // Last Planet		
		
	TArray<Triangle> triangleList = DelaunayTriangulation::GenerateTriangulation(vect);

	AStar astar;


	for (int i = 0; i < vect.Num(); i++) // pass all the vertex points to astar
	{
		astar.AddPoint(vect[i]);
	}
		
	int FirstID = astar.findPoint(FirstPosition);
	int LastID = astar.findPoint(SSPosition);
	int SSID = astar.findPoint(LastPosition);
	

	//Loop Through all triangles in the list and add their connections to the a star points
	for (int i = 0; i < triangleList.Num(); i++)
	{
		TArray<FVector2D> Points;
		TArray<int> IDs;
		for(int j = 0; j < 3 ;j++)
		{
			Points.Add(triangleList[i].Vertex[j]);
			IDs.Add( astar.AddPoint(Points[j]));
		}
		
		//Connect the each point, since they are points of a triangle
		astar.ConnectPoints(IDs[0], IDs[1]);
		astar.ConnectPoints(IDs[1], IDs[2]);
		astar.ConnectPoints(IDs[2], IDs[0]);
	
	}


	astar.search(FirstID,LastID); // Find a route from Start to End planet
	astar.LockCurrentPath();
	Path1 = astar.path;
	
	astar.search(LastID, SSID); // Find a route to the start planet and space station
	astar.LockCurrentPath();
	Path2 = astar.path;

	astar.search(SSID, LastID); // Find a route to the space station and end planet
	astar.LockCurrentPath();
	Path3 = astar.path;
	FVector2D SpaceStation = astar.begin.position;

	CreatePath(Path1,StartToEndPath,Spline1->Spline, BuoysPercent);
	CreatePath(Path2,StartToStationPath,Spline2->Spline, BuoysPercent);
	CreatePath(Path3,StationToEndPath,Spline3->Spline, BuoysPercent);

	FTransform WorldTransform;
	WorldTransform = GetRootComponent()->GetComponentTransform();
	

	FTransform SpawnTransfrom;
	if (Planets.Num() < 3)
	{
		SpawnTransfrom.SetRotation(FQuat4d(0, 0, 0, 1.f));
		SpawnTransfrom.SetScale3D(FVector(PlanetScaling, PlanetScaling, PlanetScaling));
		SpawnTransfrom.SetLocation(PlanetPositions[1]);
		Planets.Add(CreatePlanet(SpawnTransfrom * WorldTransform,PlanetIDs[1]));
		SpawnTransfrom.SetLocation(PlanetPositions[0]);
		Planets.Add(CreatePlanet(SpawnTransfrom * WorldTransform,PlanetIDs[0]));
		SpawnTransfrom.SetLocation(PlanetPositions[2]);
		Planets.Add(CreatePlanet(SpawnTransfrom * WorldTransform,PlanetIDs[2]));
	}

	GenerateDetails();
	

	SpawnTransfrom *= WorldTransform;

	auto playerController = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (playerController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Player Moved")));

		playerController->SetActorLocation(FVector(SpawnTransfrom.GetLocation().X, SpawnTransfrom.GetLocation().Y, 1000));
	}

	std::string Line = "Distance: ";
	int length = Spline2->Spline->GetSplineLength() / 20;
	Line += std::to_string(length);
	Line += " AU";
	Planets[0]->Line3 = FText::FromString(FString(Line.c_str()));

	Line = "Distance: ";
	length = Spline1->Spline->GetSplineLength() / 20;
	Line += std::to_string(length);
	Line += " AU";
	Planets[2]->Line3 = FText::FromString(FString(Line.c_str()));

	}
	while (!IsRouteGood());
		
	// set quests
	SetQuest();
		

	Spline1->CreateSpline(DetailScaling);
	Spline2->CreateSpline(DetailScaling);
	Spline3->CreateSpline(DetailScaling);

	for(auto mesh : Spline1->Meshes)
	{
		mesh->SetVisibility(false);
	}
	for(auto mesh : Spline2->Meshes)
	{
		mesh->SetVisibility(false);
	}
	for(auto mesh : Spline3->Meshes)
	{
		mesh->SetVisibility(false);
	}
}

//Create Bouys along the procedural path and fill the spline points
void ARouteExample::CreatePath(TArray<FVector2D>& Path, TArray<APath*>& PathMeshes,USplineComponent* SplineComponent, float PathPercentage = 50)
{
	FTransform WorldTransform;
	WorldTransform = GetRootComponent()->GetComponentTransform();
	
	FRotator Rotation;
	
	if( Path.Num() > 2)// Make the bouys point towards the destination
		Rotation = UKismetMathLibrary::FindLookAtRotation(FVector(Path[0].X,Path[0].Y,0),FVector(Path[Path.Num() - 1].X,Path[Path.Num() - 1].Y,0));

	float NumberOfPoints = Path.Num();
	float PointsWanted = NumberOfPoints * (PathPercentage / 100);
	float Step = NumberOfPoints/PointsWanted; // if we have 100 points but want only 10, then every 10 loops we spawn a bouy, so they are spread out evenly
	float Counter = 0;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, FString::Printf(TEXT("Max: %f"), NumberOfPoints));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, FString::Printf(TEXT("Wanted: %f"), PointsWanted));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, FString::Printf(TEXT("Step: %f"), Step));

	
	for (int i = 0; i < Path.Num(); i++)
	{
		FTransform SpawnTransfrom;
		SpawnTransfrom.SetRotation(Rotation.Quaternion());
		SpawnTransfrom.SetScale3D(FVector(10, 10, 10));
		SpawnTransfrom.SetLocation(FVector( Path[i].X - Dimensions.X / 2, Path[i].Y - Dimensions.Y / 2, UKismetMathLibrary::Sin(i) * SinWaveAmplitude));
		
		if(i >= Counter) 
		{
			auto path = CreateBasicCube(SpawnTransfrom * WorldTransform);
			path->SetActorScale3D(FVector(PlanetScaling/5, PlanetScaling/5, PlanetScaling/5)); //Bouys are always 5 time smaller than planets
			PathMeshes.Add(path);
			
			Counter += Step;
		}

		//We add all points to the spline so its not disconnected at the end sometimes
		SplineComponent->AddSplinePoint((SpawnTransfrom * WorldTransform).GetLocation(), ESplineCoordinateSpace::Type::World, true);
		SpawnTransfrom.AddToTranslation(FVector(0 ,50, 50));
		SpawnTransfrom *= WorldTransform;

	}
}

//Reset all procedural route related data
void ARouteExample::ResetRoute()
{
	Path1.Empty();
	Path2.Empty();
	Path3.Empty();

	CurrentSpline = NULL;
	CurrentPlanet = NULL;

	if(RouteData)
	{
		RouteData->Splines.Empty();
		RouteData->Stops.Empty();
		RouteData->Index = 0;
		RouteData->Max = 0;
	}

	Spline1->Spline->ClearSplinePoints();
	Spline2->Spline->ClearSplinePoints();
	Spline3->Spline->ClearSplinePoints();

	for(auto mesh : Spline1->Meshes)
	{
		mesh->DestroyComponent();
	}
	Spline1->Meshes.Empty();
	for(auto mesh : Spline2->Meshes)
	{
		mesh->DestroyComponent();
	}
	Spline2->Meshes.Empty();

	for(auto mesh : Spline3->Meshes)
	{
		mesh->DestroyComponent();
	}
	Spline3->Meshes.Empty();

	indexOfPlanetsInUse.clear();
	
	for (auto& Planet : Planets)
	{
		Planet->Destroy();
	}
	Planets.Empty();

	for (auto& CubePath : StartToEndPath)
	{
		CubePath->Destroy();
	}
	StartToEndPath.Empty();

	for (auto& CubePath : StartToStationPath)
	{
		CubePath->Destroy();
	}
	StartToStationPath.Empty();

	for (auto& CubePath : StationToEndPath)
	{
		CubePath->Destroy();
	}
	StationToEndPath.Empty();

	for (auto& CubePath : Details)
	{
		CubePath->Destroy();
	}
	Details.Empty();
	
	if (GetWorld())
		FlushPersistentDebugLines(GetWorld());
}

//Spawn Details around the generated route
void ARouteExample::GenerateDetails()
{
	
	for(int i = 0; i< DetailsWanted; i++) // repeat for how many details we want
	{
		int RandomIndex = FMath::RandRange(0,DetailBP.Num() - 1); // Get a random Detail Mesh
		FTransform transform(FVector(0,0,0));
		transform.SetScale3D(FVector(DetailScaling, DetailScaling, DetailScaling)); // Give appropriate size
		
		ADetails* Detail = CreateDetail(transform,RandomIndex);
		
		for(int j = 0; j < DetailRejectionRate; j++) // There is a possibility a detail cant be spawned, so only attempt a certain amount of times as to not make an infinite  loop
		{
			FVector Origin;
			FVector Radius; // Planets are uniformly sized so only need the radius of 1 dimension
			Detail->GetActorBounds(true,Origin,Radius);


			//DrawDebugBox(GetWorld(), FVector((Planets[0]->GetActorLocation().X - Planets[1]->GetActorLocation().X) * 0.25,0,0), FVector((Planets[0]->GetActorLocation().X - Planets[1]->GetActorLocation().X) * 0.75,(Planets[2]->GetActorLocation().Y - Planets[1]->GetActorLocation().Y) / 2,0), FColor::Purple, true, -1, 0, 10);

			//Get a random position inside the routes Bounding Box
			FVector RandomPosition = UKismetMathLibrary::RandomPointInBoundingBox(FVector((Planets[0]->GetActorLocation().X - Planets[1]->GetActorLocation().X) * 0.25,0,0),FVector((Planets[0]->GetActorLocation().X - Planets[1]->GetActorLocation().X) * 0.75,(Planets[2]->GetActorLocation().Y - Planets[1]->GetActorLocation().Y) / 2,0));

			//Find how close it is to each spline
			FVector ClosestPoint1 = Spline1->Spline->FindLocationClosestToWorldLocation(RandomPosition, ESplineCoordinateSpace::World);
			FVector ClosestPoint2 = Spline2->Spline->FindLocationClosestToWorldLocation(RandomPosition, ESplineCoordinateSpace::World);
			FVector ClosestPoint3 = Spline3->Spline->FindLocationClosestToWorldLocation(RandomPosition, ESplineCoordinateSpace::World);

			FColor Color = FColor::Green;

			//Check weather the new detail is too close to the existing route
			if(FVector::Distance(ClosestPoint1,RandomPosition) < DetailMinDistance + Radius.GetMax() || FVector::Distance(ClosestPoint2,RandomPosition)  < DetailMinDistance + Radius.GetMax() || FVector::Distance(ClosestPoint3,RandomPosition)  < DetailMinDistance + Radius.GetMax())
			{
				Color = FColor::Red;
				continue;
			}

			//Make sure its not spawning where the fight area is taking place
			if(FVector::Distance(FVector(0,0,0),RandomPosition) + 600 < DetailMinDistance + Radius.GetMax())
			{
				//continue;
			}

			//Check weather the new detail is too close to another detail
			bool failed = false;
			for(auto detail : Details)
			{
				if(FVector::Distance(RandomPosition,detail->GetActorLocation()) < DetailMinDistance + Radius.GetMax()){
					failed = true; 
				}
			}

			if(failed)
				continue;

			//Set up details position
			Detail->SetActorLocation(RandomPosition);
			Detail->StartingLocation = RandomPosition;

			//Details.Add(CreatePlanet(SpawnTransfrom * GetRootComponent()->GetComponentTransform(),RandomPlanetIndex));
			Details.Add(Detail);
			
			break;
		}
	}


}


void ARouteExample::ClearRouteData()
{

	
	
}

//Move along the current spline towards a planet, exit when close enough
bool ARouteExample::MoveAlongPath(UPathData* PathData , float DeltaTime)
{

	//Get The current Spline Track Position and Apply to the player
	float SplineLength = PathData->Splines[PathData->Index]->GetSplineLength();
	auto player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	splineTimer += DeltaTime * player->MovementSpeed / SplineLength;


	//Get where we are on the spline and lerp the player towards it
	float DistanceTraveled = FMath::Lerp(SplineLength, 0, splineTimer);
	FVector PlayerPosition = PathData->Splines[PathData->Index]->GetLocationAtDistanceAlongSpline(DistanceTraveled, ESplineCoordinateSpace::Type::World);
	FRotator PlayerRotation = PathData->Splines[PathData->Index]->GetRotationAtDistanceAlongSpline(DistanceTraveled, ESplineCoordinateSpace::Type::World);
	
	//Set player transform 
	UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->SetActorLocation(PlayerPosition);
	UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->SetActorRotation(PlayerRotation);
	AudioManager->ThrusterSoundComponent->SetWorldLocation(PlayerPosition); // move the thruster sound to follow the player
	
	if (splineTimer > PathStartEndPercent.Y) // if we are close enough to the end of the spline
	{
		splineTimer = PathStartEndPercent.X; // reset spline timer 
		PlayerController->SetViewTargetWithBlend(PathData->Stops[PathData->Index], CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear); // change to planet camera
		
		// with the checkpoint I would imagine it would be something like this
		if (PathData->Stops[PathData->Index]->IsCheckpoint) // if its a checkpoint we are at then there is different functionality
		{
			PathData->Stops[PathData->Index]->CurrentPlanet = true;
			CheckpointTransitionDelegate.Broadcast();
		}
		else // We reached a planet
		{
			// update the current planet
			// this bool in the planet class is used by the vendor UI
			PathData->Stops[PathData->Index]->CurrentPlanet = true;
			// reset events when we reach a planet
			EventsComponent->ResetEvents();
			OrbitTransitionDelegate.Broadcast();

			//Change Skybox color, but only at planets
			ASpaceSkyBox* SpaceSkyBox = Cast<ASpaceSkyBox>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpaceSkyBox::StaticClass())); 
			SpaceSkyBox->OffsetColor();


			// probably better if I put a function
			// to swap states like the route but...
			// reset the mini-game when you reach a planet
			player->EngineStatus = CRUISING;
			player->MovementSpeed = PlayerMovementSpeed;
			player->CurrentAcceleration = player->BaseAcceleration;
		}
		
	}

	return false; // The Movement is still in progress

}
void ARouteExample::OrbitPlanet(UPathData* PathData, float DeltaTime)
{
	
}

//Used to select path based of mouse position
//Is no longer in use but is probably worth keeping
void ARouteExample::SelectPath()
{
	for(int i =0; i < Planets.Num();i++)
	{
		Planets[i]->HideUI = false;
	}
	
	//SwapState(Selecting); TODO maybe not needed/ messes up previous state
	auto player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector LookPosition;
	FVector LookDirection;
	player->DeprojectMousePositionToWorld(LookPosition, LookDirection);

	
	float mouseX, mouseY;
	player->GetMousePosition(mouseX, mouseY);
	float Path1Distance = 10000000000000000; // TODO maybe do a distance test with the first element but i cba
	float Path2Distance = 10000000000000000;

	bool WhichPath = false; // TODO if we stick to 2 paths this is fine but could be improved

	for (auto path : StartToEndPath)
	{
		float distance = FVector::Distance(UKismetMathLibrary::FindClosestPointOnLine(path->GetActorLocation(), LookPosition, LookDirection), path->GetActorLocation());
		if (distance < Path1Distance)
		{
			Path1Distance = distance;
		}
	}

	for (auto path : StartToStationPath)
	{
		float distance = FVector::Distance(UKismetMathLibrary::FindClosestPointOnLine(path->GetActorLocation(), LookPosition, LookDirection), path->GetActorLocation());
		if (distance < Path2Distance)
		{
			Path2Distance = distance;
		}
	}

	for (auto path : StationToEndPath)
	{
		float distance = FVector::Distance(UKismetMathLibrary::FindClosestPointOnLine(path->GetActorLocation(), LookPosition, LookDirection), path->GetActorLocation());
		if (distance < Path2Distance)
		{
			Path2Distance = distance;
		}
	}


	if (Path1Distance < Path2Distance)
	{
		/*for (auto cube : StartToEndPath)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(4, 4, 4));
			}
		}
		for (auto cube : StartToStationPath)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(2, 2, 2));
			}
		}

		for (auto cube : StationToEndPath)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(2, 2, 2));
			}
		}*/

		/*
		Spline1->SetMaterial(0);
		Spline2->SetMaterial(1);
		Spline3->SetMaterial(1);*/
		
		CurrentSpline = Spline1->Spline;
		CurrentPlanet = Planets[2];
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Left Side")));

		/*for (auto cube : StartToStationPath)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(4, 4, 4));
			}
		}
		for (auto cube : StationToEndPath)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(4, 4, 4));
			}
		}

		for (auto cube : StartToEndPath)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(2, 2, 2));
			}
		}*/

		/*
		Spline1->SetMaterial(1);
		Spline2->SetMaterial(0);
		Spline3->SetMaterial(0);*/
		
		CurrentSpline = Spline2->Spline;
		CurrentPlanet = Planets[0];
		WhichPath = true;
	}
	


	
	ASpaceshipCharacter* Charac = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Charac->Selected;

	if (Charac->Selected)
	{

		for (auto cube : StartToStationPath)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(1, 1, 1));
			}
		}
		for (auto cube : StationToEndPath)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(1, 1, 1));
			}
		}

		for (auto cube : StartToEndPath)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(1, 1, 1));
			}
		}

		RouteData->Reset();


		Charac->Selected = false; // TODO change this back once the player clicks on the ui and move everything inside this if statement so scaling isnt changing all the time
		timer = 0;
		if (WhichPath)
		{
			RouteData->Splines.Add(Spline2->Spline);
			RouteData->Splines.Add(Spline3->Spline);
			RouteData->Stops.Add(Planets[0]);
			RouteData->Stops.Add(Planets[2]);
			RouteData->Max = RouteData->Splines.Num();
			RouteData->Index = 0;
			RouteData->RouteName = "Long Route";
			RouteData->AtFirstPlanet = false;
			//RouteData->CalculateLength();
		}
		else
		{
			RouteData->Splines.Add(Spline1->Spline);
			RouteData->Stops.Add(Planets[2]);
			RouteData->Max = RouteData->Splines.Num();
			RouteData->RouteName = "Short Route";
			RouteData->Index = 0;
			RouteData->StoryEventChance = 50;
			RouteData->AtFirstPlanet = false;
			//RouteData->CalculateLength();
		}

		PathClickedDelegate.Broadcast(RouteData);

	}

}

//Used to find where the planet is on screen, can play UI element based on where the planet was
//Is no longer in use as an alternative was found 
FVector2D ARouteExample::GetPlanetsOnScreenPosition(int Index)
{
	if(Index >= Planets.Num())
		return FVector2D(-100,-100);

	FVector4 Location = FVector4(Planets[Index]->GetActorLocation(),1);

	FMinimalViewInfo DesiredView;
	Camera->GetCameraView(0.001,DesiredView);

	FVector2D Size;
	GEngine->GameViewport->GetViewportSize(Size);

	FMatrix View;
	FMatrix Projection;
	FMatrix ViewProjection;
	UGameplayStatics::GetViewProjectionMatrix(DesiredView,View,Projection,ViewProjection);

	Location = View.TransformFVector4(Location);
	Location = Projection.TransformFVector4(Location);
	Location /= Location.W;
	Location.X *= 0.5;
	Location.Y *= -0.5;
	Location.X += 0.5;
	Location.Y += 0.5;
	Location.X *= -Size.X;
	Location.Y *= -Size.Y;
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, FString::Printf(TEXT("Screen Pos: %s"), *Location.ToString() ));
	return FVector2D (Location.X,Location.Y);
}

//Swap camera to route selection camera
void ARouteExample::TransitionToMap()
{
	PlayerController->SetViewTargetWithBlend(GetRootComponent()->GetAttachmentRootActor(), CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	SwapState(Selecting);
}

//Swap to orbiting the reached planet 
void ARouteExample::SwapToOrbiting()
{
	//Hide All other Route meshes 
	ChangeVisibilityOfRoute(true);

	//Show our current Planet only
	for (auto it : Planets)
	{
		if (it->CurrentPlanet)
		{
			CurrentPlanet = it;
			it->SetActorHiddenInGame(false);
			// see if player completed quest
			ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			player->WasQuestCompleted(it->Name);
		}
	}

	if(RouteData->AtFirstPlanet) // if we were at first planet and now reached a new planet, we are no longer at the first planet
	{
		RouteData->AtFirstPlanet = false;
	}
	else //Increment the index, used for what spline we travel along
	{
		RouteData->Index++;
	}
	
	SwapState(Orbiting);
	
}

//Use this when orbiting the first planet as to not mess up the routes
void ARouteExample::BeginToOrbiting()
{
	//Hide All other Route meshes 
	ChangeVisibilityOfRoute(true);

	//Show our current Planet only
	for (auto it : Planets)
	{
		if (it->CurrentPlanet)
		{
			CurrentPlanet = it;
			it->SetActorHiddenInGame(false);
			// see if player completed quest
			ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			player->WasQuestCompleted(it->Name);
		}
	}

	SwapState(Orbiting);
}

//initialize everything for swapping to moving
void ARouteExample::SwapToMoving()
{
	NavIncidentsTimer = 0.0f; // reset the event timer
	SwapState(Moving);
	ASpaceshipCharacter* Charac = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Charac->SetHidden(false); // enable visibility of the character
	Charac->TopDownCamera->SetActive(true); // enable their camera
	PlayerController->SetViewTargetWithBlend(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear); //swap to the player camera


	
	//Make the Camera Face the direction we are moving
	float SplineLength = RouteData->Splines[RouteData->Index]->GetSplineLength();
	FVector StartPoint = RouteData->Splines[RouteData->Index]->GetLocationAtDistanceAlongSpline(0, ESplineCoordinateSpace::Type::World);
	FVector EndPoint = RouteData->Splines[RouteData->Index]->GetLocationAtDistanceAlongSpline(SplineLength, ESplineCoordinateSpace::Type::World) - FVector(0,0,-50);
	
	Charac->CameraBoom->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(EndPoint, StartPoint) + CameraAngle);
}

//Set up for swapping to selecting
void ARouteExample::SwapToSelecting()
{
	ASpaceshipCharacter* Charac = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Charac->IsInSelectScreen = true;
	Camera->SetActive(true);
	PlayerController->SetViewTargetWithBlend(GetRootComponent()->GetAttachmentRootActor(), CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	SwapState(Selecting);

	// if leaving planet
	// put all planets as not being the current planet
	// this is not the best away to go about this
	// but this way we have to change less stuff
	
	//Make all the splines and planet available for selecting
	if (PreviousState == Orbiting)
	{
		for (auto it : Planets)
		{
			it->CurrentPlanet = false;
		}
		
		for(auto mesh : Spline1->Meshes)
		{
			mesh->SetVisibility(true);
		}
		for(auto mesh : Spline2->Meshes)
		{
			mesh->SetVisibility(true);
		}
		for(auto mesh : Spline3->Meshes)
		{
			mesh->SetVisibility(true);
		}
		
		ChangeVisibilityOfRoute(false);
	}

}

//When swapping to combat we dont want to get an immediate event afterwards, so reset event timer
void ARouteExample::SwapToCombat()
{
	NavIncidentsTimer = 0.0f;
}


//Simple test to make sure the route works
bool ARouteExample::IsRouteGood()
{

	//Make sure there is more than 0 so that a route is available
	//But also more than 3 so it at least looks good
	if(Spline1->Spline->GetNumberOfSplinePoints() <= 3)
		return false;

	if(Spline2->Spline->GetNumberOfSplinePoints() <= 3)
		return false;

	if(Spline3->Spline->GetNumberOfSplinePoints() <= 3)
		return false;


	return true; 
}

//Swap state is used to handle closing previous states properly
//It also keeps track of the previous state which is useful
void ARouteExample::SwapState(PlayerStates State)
{
	PreviousState = PlayerState;
	PlayerState = State;

	//if the previous state was moving then movement sounds should all stop
	if(PreviousState == Moving)
	{
		AudioManager->ThrusterSoundComponent->Stop();
		AudioManager->TurboSoundComponent->Stop();
		//AudioManager->AmbientSoundComponent->SetPaused(true);
	}
	else if (PlayerState == Moving)// if we now are moving then play thruster sound
	{
		AudioManager->ThrusterSoundComponent->Play();

		if (AudioManager->AmbientSoundComponent->bIsPaused)
			AudioManager->AmbientSoundComponent->SetPaused(false);
		else
			AudioManager->AmbientSoundComponent->Play();

	}
	
	if (PlayerState == Fighting) // Play combat music if we are fighting
	{
		AudioManager->BattleSoundComponent->Play();
	}
	else if(PreviousState == Fighting) // Stop combat music if we are leaving fighting
	{
		AudioManager->BattleSoundComponent->Stop();
	}

	
	if(PreviousState == Selecting) // disable all route meshes if we are no longer selecting
	{
		for(auto mesh : Spline1->Meshes)
		{
			mesh->SetVisibility(false);
		}
		for(auto mesh : Spline2->Meshes)
		{
			mesh->SetVisibility(false);
		}
		for(auto mesh : Spline3->Meshes)
		{
			mesh->SetVisibility(false);
		}

		for(int i =0; i < Planets.Num();i++)
		{
			Planets[i]->HideUI = true;
		}
	}

	if (PlayerState == Selecting) // Show UI Markers when selecting
	{
		for (int i = 0; i < Planets.Num(); i++)
		{
			Planets[i]->HideUI = false;
		}
	}
}

//BP function allowing it to be displayed in the UI
void ARouteExample::GetPathSelected(UPathData* path)
{
	path = RouteData;
}

//Dicide what action to take based of which planet we are leaving
void ARouteExample::LeaveOrbit()
{

	for (auto it : Planets)
	{
		it->CurrentPlanet = false;
	}
	ChangeVisibilityOfRoute(false);

	if(RouteData->ID == 0) // if we are on longer path, only show that paths buoys
	{
		for (auto it : StartToStationPath)
		{
			it->SetActorHiddenInGame(false);
		}

		for (auto it : StationToEndPath)
		{
			it->SetActorHiddenInGame(false);
		}

		for (auto it : StartToEndPath)
		{
			it->SetActorHiddenInGame(true);
		}
		
	}else if(RouteData->ID == 1)  // if we are on shorter path, only show that paths buoys
	{
		for (auto it : StartToStationPath)
		{
			it->SetActorHiddenInGame(true);
		}

		for (auto it : StationToEndPath)
		{
			it->SetActorHiddenInGame(true);
		}

		for (auto it : StartToEndPath)
		{
			it->SetActorHiddenInGame(false);
		}
	}

	//Three Conditions
	//Leaving Space Station - No need to select or generate route
	//Leaving Last Planet - Need Camera Transition and to regenerate a route
	//Leaving First Planet - No Route so need to generate and then select
	
	if(RouteData->Max == 0) // First Planet
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("First Planet")));
		SelectTransitionDelegate.Broadcast(); // if we are leaving first planet then go to selection
	}
	else if(RouteData->Max == RouteData->Index) // Last Planet
	{
		//If we are at the last planet we generate the route again before going to selection
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Last Planet")));
		GenerateImproved(Planets[2]->Index,FVector(0,0,0));
		SelectTransitionDelegate.Broadcast();
	}
	else // Space Station
	{
		//just go back to moving if leaving the space station, no need to select
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Spacestation")));
		MovingTransitionDelegate.Broadcast();
	}
	
}

//BP Function, set up splines and route data based on the button pressed
UPathData* ARouteExample::SelectRoute(bool WhichRoute)
{
	if(WhichRoute)
	{
		Spline1->SetMaterial(0); // 0 is off
		Spline2->SetMaterial(1); // 1 is highlighted
		Spline3->SetMaterial(1);
		
		CurrentSpline = Spline1->Spline;
		CurrentPlanet = Planets[2];
		SelectedPath = false;
		Route2Data->RouteName = "Route 2";
		Route2Data->AssignRouteValues();

		RouteData = Route2Data;
	}
	else
	{
		Spline1->SetMaterial(1);
		Spline2->SetMaterial(0);
		Spline3->SetMaterial(0);
		
		CurrentSpline = Spline2->Spline;
		CurrentPlanet = Planets[0];
		SelectedPath = true;

		Route1Data->RouteName = "Route 1";
		Route1Data->AssignRouteValues();
		RouteData = Route1Data;

	}

	return RouteData;


}

//Once a route has been confirmed selected, set up the route data and visibility 
void ARouteExample::FinalSelectRoute()
{

	RouteData->Reset();

	timer = 0;
	if (SelectedPath)
	{
		RouteData->Splines.Add(Spline2->Spline);
		RouteData->Splines.Add(Spline3->Spline);
		RouteData->Stops.Add(Planets[0]);
		RouteData->Stops.Add(Planets[2]);
		RouteData->Max = RouteData->Splines.Num();
		RouteData->Index = 0;
		RouteData->RouteName = "Long Route";
		RouteData->AtFirstPlanet = false;
		RouteData->ID = 0;

		for (auto it : StartToEndPath)
		{
			it->SetActorHiddenInGame(true);
		}

	}
	else
	{
		RouteData->Splines.Add(Spline1->Spline);
		RouteData->Stops.Add(Planets[2]);
		RouteData->Max = RouteData->Splines.Num();
		RouteData->RouteName = "Short Route";
		RouteData->Index = 0;
		RouteData->AtFirstPlanet = false;
		RouteData->ID = 0;
		
		for (auto it : StartToStationPath)
		{
			it->SetActorHiddenInGame(true);
		}

		for (auto it : StationToEndPath)
		{
			it->SetActorHiddenInGame(true);
		}
	}

	PathClickedDelegate.Broadcast(RouteData);
}

//Start Game is called after selecting a spaceship
void ARouteExample::StartGame()
{
	//First we Generate a route
	GenerateImproved(-1,FVector(0,0,0));

	//Set the player to be at the first planet
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(Planets[1], CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	Planets[1]->CurrentPlanet = true;
	CurrentPlanet = Planets[1];

	//Initialize player variables
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetShowMouseCursor(true);
	FightCamera->SetWorldLocation(FVector(0, 0, 3000.0));

	ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	player->AudioManager = AudioManager;
	

	Spline1->SetMaterial(1);
	Spline2->SetMaterial(1);
	Spline3->SetMaterial(1);
}

//This function either hides all route meshes or makes them all visible
void ARouteExample::ChangeVisibilityOfRoute(bool toHide)
{
	for (auto it : Planets)
	{
		it->SetActorHiddenInGame(toHide);
	}

	for (auto it : StartToEndPath)
	{
		it->SetActorHiddenInGame(toHide);
	}

	for (auto it : StartToStationPath)
	{
		it->SetActorHiddenInGame(toHide);
	}
	for (auto it : StationToEndPath)
	{
		it->SetActorHiddenInGame(toHide);
	}

	for (auto it : Details)
	{
		it->SetActorHiddenInGame(toHide);
	}

}

void ARouteExample::SetQuest()
{
	// On the starting planet a quest for the last one of the route?
	if(!Planets[1])
		return;

	if(!Planets[2])
		return;

	if (!Planets[1]->Quest)
		return;

	// on the first route 
	// just store the quest
	if (!LastQuestPreviousRoute)
	{
		LastQuestPreviousRoute = Planets[2]->Quest;
	}
	else
	{
		Planets[1]->Quest = LastQuestPreviousRoute;
	}

	Planets[1]->Quest->TargetName = Planets[2]->Name;
	
}

//Main combat loop
void ARouteExample::FightScene(float DeltaTime) {
	
	
	ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	FVector Origin;
	FVector Radius;
	player->GetActorBounds(true,Origin,Radius);
	
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	
	if (!FightCamera->IsActive())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Red, TEXT("Camera swap"));
		player->TopDownCamera->SetActive(false);
		Camera->SetActive(false);
		FightCamera->SetActive(true);
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this, CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
		player->SetActorLocation(FightCamera->GetComponentLocation() + FVector(600, -300, 0));

		//player->SetActorScale3D(FVector(2.f));


	}

	
	if (!AEnemyActor)
	{
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;
	
		AEnemyActor = GetWorld()->SpawnActor<AEnemy>(MyEnemy, GetTransform(), SpawnInfo);

		AEnemyActor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		FVector ActorSize, ActorOrigin;
		AEnemyActor->GetActorBounds(true, ActorOrigin, ActorSize, false);
		AEnemyActor->SetActorLocation(player->GetActorLocation() + TempEnemyPosition);
		AEnemyActor->SetActorRotation(FRotator(UKismetMathLibrary::FindLookAtRotation(player->GetActorLocation(), AEnemyActor->GetActorLocation())));

		AEnemyActor->SetPlayerLocation(player->GetActorLocation());
		AEnemyActor->SetEnemyLevel(player->StatsPlayerComponent->CurrentReputation);
		AEnemyActor->GetEnemyStats()->DamageTakenPerHit = player->StatsPlayerComponent->ATKPower / 10;
		
		player->StatsPlayerComponent->DamageTakenPerHit = AEnemyActor->GetEnemyStats()->ATKPower / 10;

		FVector Direction = FVector(player->GetActorLocation() - AEnemyActor->GetActorLocation());
		Direction.Z = 0;
		FRotator Rot = UKismetMathLibrary::FindLookAtRotation(AEnemyActor->GetActorLocation(), player->GetActorLocation());
			
		player->SetActorRotation(Rot);
	}

	AEnemyActor->SetActorLocation(player->GetActorLocation() + TempEnemyPosition + FVector(cos(timer) * 30,cos(timer) * 30,sin(timer) * 30));
	AEnemyActor->SetActorRotation(FRotator(UKismetMathLibrary::FindLookAtRotation(player->GetActorLocation(), AEnemyActor->GetActorLocation())));


	player->SetActorLocation(FightCamera->GetComponentLocation() + FVector(600, -300, 0) + FVector(cos(timer + 53.1) * 30,cos(timer+ 3.1) * 30,sin(timer+  543.1) * 30));
	player->SetActorRotation(FRotator(UKismetMathLibrary::FindLookAtRotation(AEnemyActor->GetActorLocation(), player->GetActorLocation())));

	if(player && !isDead)
	{
		player->Attack(DeltaTime,AEnemyActor);
	}

	if (AEnemyActor)
	{
		AEnemyActor->Attack();
	}
	player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!(player->Alive) && !isDead)
	{
		player->SetActorHiddenInGame(true);
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("Player is Dead"));
		//CombatReset();
		AudioManager->ExplosionSoundComponent->Play();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),AEnemyActor->ParticalSystem,player->GetActorLocation(),player->GetActorRotation(),player->GetActorScale() * 2);
		AudioManager->DefeatSoundComponent->SetWorldLocation(player->GetActorLocation());
		AudioManager->DefeatSoundComponent->Play();
		GameOverDelegate.Broadcast();
		//ResetCameraAfterCombat();

		isDead = true;
	}
	else if (!IsValid(AEnemyActor) && !isDead)
	{
		//CombatReset();
		SwapState(Event);
		AEnemyActor->ResetEnemy();
		AudioManager->VictorySoundComponent->SetWorldLocation(player->GetActorLocation());
		AudioManager->VictorySoundComponent->Play();
		AudioManager->ExplosionSoundComponent->Play();

		// player->TopDownCamera->SetActive(true);
		// UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this, CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	}

}


void ARouteExample::CombatReset() {

	//MovingTransitionDelegate.Broadcast();
	//SwapState(PreviousState);
	ASpaceshipCharacter* CurrentPlayer = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if(CurrentPlayer)
		CurrentPlayer->ResetCombat();
	
	AEnemyActor = nullptr;
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("Combat Reset"));
	SwapState(Event);
}

void ARouteExample::GameOver()
{

	ASpaceshipCharacter* CurrentPlayer = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if(CurrentPlayer)
		CurrentPlayer->ResetCombat();
	
	
	SwapState(Event);
}

void ARouteExample::CallCombatOverDelegate()
{
	CombatReset();
	CombatOverTransitionDelegate.Broadcast();
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("Audio has finished"));
}

void ARouteExample::ResetCameraAfterCombat()
{
	ASpaceshipCharacter* CurrentPlayer = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	FightCamera->SetActive(false);
	if (CurrentPlayer)
		CurrentPlayer->TopDownCamera->SetActive(true);

	PlayerController->SetViewTargetWithBlend(UGameplayStatics::GetPlayerCharacter(GetWorld(), CameraTransitionSpeed), 0, EViewTargetBlendFunction::VTBlend_Linear);
}
