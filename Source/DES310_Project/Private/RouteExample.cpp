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

	USceneComponent* root = this->CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	root->Mobility = EComponentMobility::Type::Movable;
	this->SetRootComponent(root);

	CubeMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'")).Object;
	SphereMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'")).Object;

	//moved to begin play because it wasnt really working in constructor
	/*Spline1 = CreateDefaultSubobject<ARouteSpline>(TEXT("Spline Short Path 1"));
	Spline1->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	Spline2 = CreateDefaultSubobject<ARouteSpline>(TEXT("Spline Long Path 2"));
	Spline2->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	Spline3 = CreateDefaultSubobject<ARouteSpline>(TEXT("Spline Long Path 3"));
	Spline3->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);*/
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->SetUsingAbsoluteScale(false);

	CameraBoom->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Route Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	FightCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Fight Camera"));
	FightCamera->SetupAttachment(RootComponent);

	RouteData = CreateDefaultSubobject<UPathData>(TEXT("Route Data"));
	// just hold the values for the chances of each route
	Route1Data = CreateDefaultSubobject<UPathData>(TEXT("Route1 Data"));
	Route2Data = CreateDefaultSubobject<UPathData>(TEXT("Route2 Data"));
	
	CameraBoom->SetWorldRotation(FRotator(-90, 180, 0));

	PlayerState = PlayerStates::Selecting;

	// create event component
	EventsComponent = CreateDefaultSubobject<URandomEventsComponent>(TEXT("Events Component"));


}

// Called when the game starts or when spawned
void ARouteExample::BeginPlay()
{
	Super::BeginPlay();

	splineTimer = PathStartEndPercent.X;
	/*Generate();
	randomSpinRate = FMath::RandRange(1, 100);
	PlayerState = Selecting;*/
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this, CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetShowMouseCursor(true);

	SpaceshipCharacter = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	CameraBoom->TargetArmLength = CameraBoom->TargetArmLength * this->GetActorScale().Length(); // TODO change to use Highest x/y/z instead of the pythag
	FightCamera->SetWorldLocation(FVector(0, 0, 3000.f));
	FightCamera->SetActive(false);

	/*
	for (int i = 0; i < PlanetsBP.Num(); i++)
	{
		PlanetIndex.push_back();
	}*/

	SuperTempTimer = 0;
	
	AudioManager->AmbientSoundComponent->Play();
	ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	player->AudioManager = AudioManager;

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;
	
	Spline1 = GetWorld()->SpawnActor<ARouteSpline>(SplineBP,GetTransform(),SpawnParam);
	Spline1->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

	Spline2 = GetWorld()->SpawnActor<ARouteSpline>(SplineBP,GetTransform(),SpawnParam);
	Spline2->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	
	Spline3 = GetWorld()->SpawnActor<ARouteSpline>(SplineBP,GetTransform(),SpawnParam);
	Spline3->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

	
	OrbitTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToOrbiting);
	BeginOrbitTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::BeginToOrbiting);
	// transition to checkpoint is the same as to orbiting for now
	// the different delegate is just to bind different functionality in blueprint
	CheckpointTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToOrbiting);
	MovingTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToMoving);
	SelectTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToSelecting);
	CombatTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToCombat);
	GameOverDelegate.AddUniqueDynamic(this, &ARouteExample::CombatReset);


	PlayerState = Event;
	//PlayerState = Selecting;
	//SelectTransitionDelegate.Broadcast();
	PathClickedDelegate.AddUniqueDynamic(this, &ARouteExample::GetPathSelected);


	AudioManager->VictorySoundComponent->OnAudioFinished.AddUniqueDynamic(this, &ARouteExample::CallCombatOverDelegate);

}

// Called every frame
void ARouteExample::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	timer += DeltaTime;
	if (timer >= RouteTickRate)
	{
		timer = 0;
		//Generate();
	}

	//GetPlanetsOnScreenPosition(0);

	
	cameraTimer += DeltaTime;
	if (cameraTimer >= CameraRate)
	{
		cameraTimer = 0;
		SwitchCamera();
	}

	UGameInstance_CPP* GameInstance = Cast<UGameInstance_CPP>(UGameplayStatics::GetGameInstance(GetWorld()));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple,	FString::Printf(TEXT("State Num: %i"), (int)PlayerState));
	ASpaceshipCharacter* PlayerOBJ = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	FString StateName = "";
	switch (PlayerState)
	{
	case PlayerStates::Moving:
		NavIncidentsTimer += DeltaTime;
		StateName = "Moving";
		MoveAlongPath(RouteData, DeltaTime);
		PlayerOBJ->UpdatePlayerSpeed(DeltaTime);
		// passing the current path is cleaner
		// pass the values for now
		SuperTempTimer += DeltaTime;
		if (NavIncidentsTimer > NavIncidentsCooldown)
		{
			if (EventsComponent->RollForEvent(RouteData->EventChance, DeltaTime, RouteData->StoryEventChance, RouteData->RandomEventChance))
			{
				SwapState(Event);
			}
			else if (SuperTempTimer > CombatTick)
			{
				if (FMath::RandRange(0, 100) < RouteData->CombatEventChance)
				{
					SuperTempTimer = 0;
					CombatTransitionDelegate.Broadcast();
					SwapState(Event);
				}
			}
		}
		break;
	case PlayerStates::Orbiting: OrbitPlanet(RouteData, DeltaTime);
		StateName = "Orbiting";

		// to do - save route data when route generated
		GameInstance->SaveGameData();
		break;
	case PlayerStates::Selecting: //SelectPath();
		StateName = "Selecting";
		break;
	case PlayerStates::Event:
		StateName = "Event";
		break;
	case PlayerStates::Fighting: FightScene(DeltaTime);
		StateName = "Fighting";
		break;
	default:
		break;
	}

	/*if(RouteData->AtFirstPlanet)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple,	TEXT("True"));

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,	TEXT("False"));

	}*/
	/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Index: %i"), RouteData->Index));*/
}

//An example of how to use the route system
//instead of including this example just copy the code and remove the debug lines from them
//from there the points given out by A* can be used however you want

APath* ARouteExample::CreateBasicCube(FTransform transform)
{
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;
	
	APath* MyNewActor = GetWorld()->SpawnActor<APath>(PathBP, transform, SpawnParam);
	MyNewActor->SetActorLocation(MyNewActor->GetActorLocation() - FVector(0,0,PathHeightOffset));
	MyNewActor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	
	return MyNewActor;
}

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
	//if (PlanetIndex.Num() < 1)
	//{

	//	for (int i = 0; i < PlanetsBP.Num(); i++)
	//	{
	//		PlanetIndex.push_back(i);
	//	}

	//}


	//int numChosen = rand() % PlanetBP.Num();
	//int numSelected = arr[numChosen];

	//arr.RemoveAt(numChosen);


	//APlanet* APlanetActor = GetWorld()->SpawnActor<APlanet>(PlanetBP[FMath::RandRange(0, PlanetBP.Num() - 1)], transform, SpawnParams);

	APlanetActor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	
	return APlanetActor;
}

APlanet* ARouteExample::CreatePlanet(FTransform transform, int i)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	i = FMath::Clamp(i,1,PlanetBP.Num() - 1);
	
	APlanet* APlanetActor = GetWorld()->SpawnActor<APlanet>(PlanetBP[i], transform, SpawnParams);
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


ADetails* ARouteExample::CreateDetail(FTransform transform, int Index)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	
	Index = FMath::Clamp(Index,0,DetailBP.Num() - 1);

	ADetails* Detail = GetWorld()->SpawnActor<ADetails>(DetailBP[Index], transform, SpawnParams);
	Detail->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

	return Detail;

}


void ARouteExample::Generate()
{
	/*ResetRoute();

	FVector2D Max(Dimensions.X / 2, Dimensions.Y + PointRadius);
	FVector2D Min(Dimensions.X / 2, 0 - PointRadius);

	TArray<FVector2D> vect = PoissonDiscSampling::PoissonDiscGenerator(PointRadius, FVector2D((int)Dimensions.X, (int)Dimensions.Y), RejectionRate);
	vect.Add(Max);
	vect.Add(Min);

	//Probably could do this more efficiently... basically making it so in the engine the actual route isnt really far away from the route

	/*for(auto &point : vect)
	{
		point -= Dimensions / 2;
	}#1#

	TArray<Triangle> triangleList = DelaunayTriangulation::GenerateTriangulation(vect);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%i Positions"), vect.Num()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%i triangles "), triangleList.Num()));

	AStar astar;

	FVector2D min = vect[0];
	FVector2D max = vect[0];
	float maxX = abs(vect[0].X);

	int minID = 0;
	int maxID = 0;
	int maxXID = 0;

	int sign = 1;
	if(FMath::RandBool())
	{
		sign = -1;
	}
	
	
	for (int i = 0; i < vect.Num(); i++)
	{
		astar.AddPoint(vect[i]);
	}

	for (int i = 0; i < triangleList.Num(); i++)
	{
		FVector2D p1 = triangleList[i].Vertex[0];
		FVector2D p2 = triangleList[i].Vertex[1];
		FVector2D p3 = triangleList[i].Vertex[2];
		int id1 = astar.AddPoint(p1);
		int id2 = astar.AddPoint(p2);
		int id3 = astar.AddPoint(p3);


		astar.ConnectPoints(id1, id2);
		astar.ConnectPoints(id2, id3);
		astar.ConnectPoints(id3, id1);

		if (min.Y > p1.Y)
		{
			minID = id1;
		}
		else if (max.Y < p1.Y)
		{
			maxID = id1;
		}

		if (sign * abs(p1.X + 5) > sign * maxX && p1.Y < Dimensions.Y - 50 && p1.Y > 50)
		{
			maxX = abs(p1.X);
			maxXID = id1;
		}

		if (sign * abs(p2.X + 5) > sign * maxX && p2.Y <  Dimensions.Y - 50 && p2.Y > 50)
		{
			maxX = abs(p2.X);
			maxXID = id2;
		}

		if (sign * abs(p3.X + 5) > sign * maxX && p3.Y <  Dimensions.Y - 50 && p3.Y > 50)
		{
			maxX = abs(p3.X);
			maxXID = id3;
		}

		if (min.Y > p2.Y)
		{
			minID = id2;
		}
		else if (max.Y < p2.Y)
		{
			maxID = id2;
		}

		if (min.Y > p3.Y)
		{
			minID = id3;
		}
		else if (max.Y < p3.Y)
		{
			maxID = id3;
		}
	}

	for (auto point : astar.points)
	{
		if (FMath::RandBool())
		{
			point.blocked = true;
		}
	}


	FTransform WorldLocation;
	float scale = 1;
	if (GetRootComponent())
	{
		WorldLocation = GetRootComponent()->GetComponentTransform();
		FVector Scaling = GetRootComponent()->GetComponentScale();
		scale = Scaling.X + Scaling.Y + Scaling.Z;
		scale /= 3;
	}


	astar.search(minID, maxID);
	FVector2D checkPoint = astar.begin.position;
	for (int j = 0; j < astar.path.Num() - 1; j++)
	{
		int id = astar.findPoint(astar.path[j]);
		if (id != -1 && j != 0)
		{
			astar.points[id].blocked = true;
		}
	}
	Path1 = astar.path;
	astar.search(minID, maxXID);
	Path2 = astar.path;
	astar.search(maxXID, maxID);
	Path3 = astar.path;

	for (int j = 0; j < Path1.Num() - 1; j++)
	{
		int id = astar.findPoint(Path1[j]);
		if (id != -1 && j != 0)
		{
			astar.points[id].blocked = true;
		}
	}

	for (int j = 0; j < Path2.Num() - 1; j++)
	{
		int id = astar.findPoint(Path2[j]);
		if (id != -1 && j != 0)
		{
			astar.points[id].blocked = true;
		}
	}

	for (int j = 0; j < Path3.Num() - 1; j++)
	{
		int id = astar.findPoint(Path3[j]);
		if (id != -1 && j != 0)
		{
			astar.points[id].blocked = true;
		}
	}

	FRotator PathRotation;
	if( Path1.Num() > 2)// Might Be an unnessary check
		PathRotation = UKismetMathLibrary::FindLookAtRotation(FVector(Path1[0].X,Path1[0].Y,0),FVector(Path1[Path1.Num() - 1].X,Path1[Path1.Num() - 1].Y,0));

	for (int i = 0; i < Path1.Num(); i++)
	{
		FTransform SpawnTransfrom;
		SpawnTransfrom.SetRotation(PathRotation.Quaternion());
		SpawnTransfrom.SetScale3D(FVector(1, 1, 1));
		SpawnTransfrom.SetLocation(FVector( Path1[i].X - Dimensions.X / 2, Path1[i].Y - Dimensions.Y / 2, UKismetMathLibrary::Sin(i) * SinWaveAmplitude));


		if (i >= CubePath1.Num())
		{
			CubePath1.Add(CreateBasicCube(SpawnTransfrom * WorldLocation));
		}

		SplineComponent1->AddSplinePoint((SpawnTransfrom * WorldLocation).GetLocation(), ESplineCoordinateSpace::Type::World, true);
		SpawnTransfrom.AddToTranslation(FVector(0 ,50, 50));
		SpawnTransfrom *= WorldLocation;
		CameraSplineComponent1->AddSplinePoint(SpawnTransfrom.GetLocation(), ESplineCoordinateSpace::Type::World, true);
	}

	if( Path2.Num() > 2)// Might Be an unnessary check
		PathRotation = UKismetMathLibrary::FindLookAtRotation(FVector(Path2[0].X,Path2[0].Y,0),FVector(Path2[Path2.Num() - 1].X,Path2[Path2.Num() - 1].Y,0));
	
	for (int i = 0; i < Path2.Num(); i++)
	{
		FTransform SpawnTransfrom;
		SpawnTransfrom.SetRotation(PathRotation.Quaternion());
		SpawnTransfrom.SetScale3D(FVector(1, 1, 1));
		SpawnTransfrom.SetLocation(FVector( Path2[i].X - Dimensions.X / 2, Path2[i].Y - Dimensions.Y / 2, UKismetMathLibrary::Sin(i) * SinWaveAmplitude));


		if (i >= CubePath2.Num())
		{
			CubePath2.Add(CreateBasicCube(SpawnTransfrom * WorldLocation));
		}

		SplineComponent2->AddSplinePoint((SpawnTransfrom * WorldLocation).GetLocation(), ESplineCoordinateSpace::Type::World, true);
		SpawnTransfrom.AddToTranslation(FVector(0, 50, 50));
		SpawnTransfrom *= WorldLocation;
		CameraSplineComponent2->AddSplinePoint(SpawnTransfrom.GetLocation(), ESplineCoordinateSpace::Type::World, true);
	}

	if( Path3.Num() > 2)// Might Be an unnessary check
		PathRotation = UKismetMathLibrary::FindLookAtRotation(FVector(Path3[0].X,Path3[0].Y,0),FVector(Path3[Path3.Num() - 1].X,Path3[Path3.Num() - 1].Y,0));

	for (int i = 0; i < Path3.Num(); i++)
	{
		FTransform SpawnTransfrom;
		SpawnTransfrom.SetRotation(PathRotation.Quaternion());
		SpawnTransfrom.SetScale3D(FVector(1, 1, 1));
		SpawnTransfrom.SetLocation(FVector( Path3[i].X - Dimensions.X / 2, Path3[i].Y - Dimensions.Y / 2,UKismetMathLibrary::Sin(i) * SinWaveAmplitude));


		if (i >= CubePath3.Num())
		{
			CubePath3.Add(CreateBasicCube(SpawnTransfrom * WorldLocation));
		}

		SplineComponent3->AddSplinePoint((SpawnTransfrom * WorldLocation).GetLocation(), ESplineCoordinateSpace::Type::World, true);
		SpawnTransfrom.AddToTranslation(FVector(0, 50, 50));
		SpawnTransfrom *= WorldLocation;
		CameraSplineComponent3->AddSplinePoint(SpawnTransfrom.GetLocation(), ESplineCoordinateSpace::Type::World, true);
	}


	FTransform SpawnTransfrom;
	if (Planets.Num() < 3)
	{

		SpawnTransfrom.SetRotation(FQuat4d(0, 0, 0, 1.f));
		SpawnTransfrom.SetScale3D(FVector(PlanetScaling, PlanetScaling, PlanetScaling));
		SpawnTransfrom.SetLocation(FVector(astar.begin.position.X - Dimensions.X / 2, astar.begin.position.Y - Dimensions.Y / 2, 0));

		Planets.Add(CreatePlanetMainRoute(SpawnTransfrom * WorldLocation));
		SpawnTransfrom.SetLocation(FVector(astar.end.position.X - Dimensions.X / 2, astar.end.position.Y - Dimensions.Y / 2, 0));
		Planets.Add(CreatePlanetMainRoute(SpawnTransfrom * WorldLocation));
		SpawnTransfrom.SetLocation(FVector(checkPoint.X - Dimensions.X / 2, checkPoint.Y - Dimensions.Y / 2 , 0));
		Planets.Add(CreatePlanetMainRoute(SpawnTransfrom * WorldLocation));
	}

	GenerateDetails();
	
	//for (auto point : astar.points)
	//{
	//	if(point.blocked)
	//		continue;
	//	
	//	SpawnTransfrom.SetRotation(FQuat4d(0,0,0,1.f));
	//	SpawnTransfrom.SetScale3D(FVector(0.5,0.5,0.5));
	//	SpawnTransfrom.SetLocation(FVector(1,point.position.X - Dimensions.X/2,point.position.Y- Dimensions.Y/2));

	//	Planets.Add(CreateBasicSphere(SpawnTransfrom * WorldLocation));
	//	
	//}

	SpawnTransfrom *= WorldLocation;

	auto playerController = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (playerController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Player Moved")));

		playerController->SetActorLocation(FVector(SpawnTransfrom.GetLocation().X, SpawnTransfrom.GetLocation().Y, 1000));
	}


	// set quests
	SetQuest();


	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Spline Count: %i,%i,%i"),SplineComponent1->GetNumberOfSplinePoints(),SplineComponent2->GetNumberOfSplinePoints(),SplineComponent3->GetNumberOfSplinePoints()));
*/
}

void ARouteExample::GenerateImproved(int FirstPlanetID, FVector Offset)
{

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, FString::Printf(TEXT("Index = %i"), FirstPlanetID));
	
	ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	
	
	do
	{
	
	ResetRoute();
	
	//Manually Add the Start and End planets to the possible routes
	FVector2D Max(FMath::RandRange(0.0,Dimensions.X), Dimensions.Y + PointRadius);
	FVector2D Min(FMath::RandRange(0.0,Dimensions.X), 0 - PointRadius);

	TArray<FVector2D> vect = PoissonDiscSampling::PoissonDiscGenerator(PointRadius, FVector2D((int)Dimensions.X, (int)Dimensions.Y), RejectionRate);
	vect.Add(Max);
	vect.Add(Min);

	TArray<Triangle> triangleList = DelaunayTriangulation::GenerateTriangulation(vect);

	AStar astar;

	FVector2D min = vect[0];
	FVector2D max = vect[0];
	float maxX = abs(vect[0].X);

	int minID = 0;
	int maxID = 0;
	int maxXID = 0;

	int sign = 1;
	if(FMath::RandBool()) // pick weather the space station will be up or down
	{
		sign = -1;
	}
	
	for (int i = 0; i < vect.Num(); i++) // pass all the vertex points to astar
	{
		astar.AddPoint(vect[i]);
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

			if (sign * abs(Points[j].X) > sign * maxX && Points[j].Y < Dimensions.Y * 0.8 && Points[j].Y > Dimensions.Y * 0.2) // 
			{
				/*if(IDs[j] != minID && IDs[j] != maxID) // We dont want the start or end to be a valid candidate for the space station
				{
					maxX = abs(Points[j].X);
					maxXID = IDs[j];
				}*/
				maxX = abs(Points[j].X);
				maxXID = IDs[j];
			}
		}
		
		//Connect the each point, since they are points of a triangle
		astar.ConnectPoints(IDs[0], IDs[1]);
		astar.ConnectPoints(IDs[1], IDs[2]);
		astar.ConnectPoints(IDs[2], IDs[0]);
		
	}

	/*for (auto point : astar.points)
	{
		if (FMath::RandBool())
		{
			point.blocked = true;
		}
	}*/
	

	astar.search(minID,maxID); // Find a route from Start to End planet
	astar.LockCurrentPath();
	Path1 = astar.path;
	
	astar.search(minID, maxXID); // Find a route to the start planet and space station
	astar.LockCurrentPath();
	Path2 = astar.path;

	astar.search(maxXID, maxID); // Find a route to the space station and end planet
	astar.LockCurrentPath();
	Path3 = astar.path;
	FVector2D SpaceStation = astar.begin.position;

	CreatePath(Path1,CubePath1,Spline1->Spline, BuoysPercent);
	CreatePath(Path2,CubePath2,Spline2->Spline, BuoysPercent);
	CreatePath(Path3,CubePath3,Spline3->Spline, BuoysPercent);

	FTransform WorldTransform;
	WorldTransform = GetRootComponent()->GetComponentTransform();
	
	/////////////////////////////////////////////
	
	if(FirstPlanetID < 0)
	{
		FirstPlanetID = FMath::RandRange(1,PlanetBP.Num());
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, FString::Printf(TEXT("Index = %i"), FirstPlanetID));
	
	FTransform SpawnTransfrom;
	if (Planets.Num() < 3)
	{
		SpawnTransfrom.SetRotation(FQuat4d(0, 0, 0, 1.f));
		SpawnTransfrom.SetScale3D(FVector(PlanetScaling, PlanetScaling, PlanetScaling));
		SpawnTransfrom.SetLocation(FVector(SpaceStation.X - Dimensions.X / 2, SpaceStation.Y - Dimensions.Y / 2, 0));
		Planets.Add(CreatePlanetMainRoute(SpawnTransfrom * WorldTransform));
		SpawnTransfrom.SetLocation(FVector(Min.X - Dimensions.X / 2, Min.Y - Dimensions.Y / 2 , 0));
		Planets.Add(CreatePlanet(SpawnTransfrom * WorldTransform,FirstPlanetID));
		SpawnTransfrom.SetLocation(FVector(Max.X - Dimensions.X / 2, Max.Y - Dimensions.Y / 2, 0));
		Planets.Add(CreatePlanetMainRoute(SpawnTransfrom * WorldTransform));
	}

	GenerateDetails();
	
	//for (auto point : astar.points)
	//{
	//	if(point.blocked)
	//		continue;
	//	
	//	SpawnTransfrom.SetRotation(FQuat4d(0,0,0,1.f));
	//	SpawnTransfrom.SetScale3D(FVector(0.5,0.5,0.5));
	//	SpawnTransfrom.SetLocation(FVector(1,point.position.X - Dimensions.X/2,point.position.Y- Dimensions.Y/2));

	//	Planets.Add(CreateBasicSphere(SpawnTransfrom * WorldLocation));
	//	
	//}


	
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
		

	Spline1->CreateSpline();
	Spline2->CreateSpline();
	Spline3->CreateSpline();

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

void ARouteExample::CreatePath(TArray<FVector2D>& Path, TArray<APath*>& PathMeshes,USplineComponent* SplineComponent, float PathPercentage = 50)
{
	FTransform WorldTransform;
	WorldTransform = GetRootComponent()->GetComponentTransform();
	
	FRotator Rotation;
	
	if( Path.Num() > 2)// Might Be an unnessary check
		Rotation = UKismetMathLibrary::FindLookAtRotation(FVector(Path[0].X,Path[0].Y,0),FVector(Path[Path.Num() - 1].X,Path[Path.Num() - 1].Y,0));

	float NumberOfPoints = Path.Num();
	float PointsWanted = NumberOfPoints * (PathPercentage / 100);
	float Step = NumberOfPoints/PointsWanted;
	float Counter = 0;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, FString::Printf(TEXT("Max: %f"), NumberOfPoints));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, FString::Printf(TEXT("Wanted: %f"), PointsWanted));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, FString::Printf(TEXT("Step: %f"), Step));

	
	for (int i = 0; i < Path.Num(); i++)
	{
		if(i >= Counter)
		{
			FTransform SpawnTransfrom;
			SpawnTransfrom.SetRotation(Rotation.Quaternion());
			SpawnTransfrom.SetScale3D(FVector(10, 10, 10));
			SpawnTransfrom.SetLocation(FVector( Path[i].X - Dimensions.X / 2, Path[i].Y - Dimensions.Y / 2, UKismetMathLibrary::Sin(i) * SinWaveAmplitude));

			auto path = CreateBasicCube(SpawnTransfrom * WorldTransform);
			path->SetActorScale3D(FVector(PlanetScaling/5, PlanetScaling/5, PlanetScaling/5));
			PathMeshes.Add(path);
		
			SplineComponent->AddSplinePoint((SpawnTransfrom * WorldTransform).GetLocation(), ESplineCoordinateSpace::Type::World, true);
			SpawnTransfrom.AddToTranslation(FVector(0 ,50, 50));
			SpawnTransfrom *= WorldTransform;

			Counter += Step;
		}
	}
}

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

	for (auto& CubePath : CubePath1)
	{
		CubePath->Destroy();
	}
	CubePath1.Empty();

	for (auto& CubePath : CubePath2)
	{
		CubePath->Destroy();
	}
	CubePath2.Empty();

	for (auto& CubePath : CubePath3)
	{
		CubePath->Destroy();
	}
	CubePath3.Empty();

	for (auto& CubePath : Details)
	{
		CubePath->Destroy();
	}
	Details.Empty();
	
	if (GetWorld())
		FlushPersistentDebugLines(GetWorld());
}

void ARouteExample::GenerateDetails()
{
	bool Success = false;
	
	for(int i = 0; i< DetailsWanted; i++)
	{
		int RandomIndex = FMath::RandRange(0,DetailBP.Num() - 1);
		ADetails* Detail = CreateDetail(FTransform(FVector(0,0,0)),RandomIndex);
		
		for(int j = 0; j < DetailRejectionRate; j++)
		{
			
			FVector Origin;
			FVector Radius; // Planets are uniformly sized so only need the raidus of 1 dimension
			Detail->GetActorBounds(true,Origin,Radius);


			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, FString::Printf(TEXT("Planet Dimensions %s"), *Radius.ToString()));

	
			//DrawDebugBox(GetWorld(), FVector((Planets[0]->GetActorLocation().X - Planets[1]->GetActorLocation().X) * 0.25,0,0), FVector((Planets[0]->GetActorLocation().X - Planets[1]->GetActorLocation().X) * 0.75,(Planets[2]->GetActorLocation().Y - Planets[1]->GetActorLocation().Y) / 2,0), FColor::Purple, true, -1, 0, 10);

			FVector RandomPosition = UKismetMathLibrary::RandomPointInBoundingBox(FVector((Planets[0]->GetActorLocation().X - Planets[1]->GetActorLocation().X) * 0.25,0,0),FVector((Planets[0]->GetActorLocation().X - Planets[1]->GetActorLocation().X) * 0.75,(Planets[2]->GetActorLocation().Y - Planets[1]->GetActorLocation().Y) / 2,0));
		
			FVector ClosestPoint1 = Spline1->Spline->FindLocationClosestToWorldLocation(RandomPosition, ESplineCoordinateSpace::World);
			FVector ClosestPoint2 = Spline2->Spline->FindLocationClosestToWorldLocation(RandomPosition, ESplineCoordinateSpace::World);
			FVector ClosestPoint3 = Spline3->Spline->FindLocationClosestToWorldLocation(RandomPosition, ESplineCoordinateSpace::World);

	
			FColor Color = FColor::Green;

			//Check weather the new detail is too close to the existing route
			if(FVector::Distance(ClosestPoint1,RandomPosition) < DetailMinDistance + Radius.GetMax() || FVector::Distance(ClosestPoint2,RandomPosition)  < DetailMinDistance + Radius.GetMax() || FVector::Distance(ClosestPoint3,RandomPosition)  < DetailMinDistance + Radius.GetMax())
			{
				Color = FColor::Red;
				/*
				DrawDebugLine(GetWorld(),ClosestPoint1,RandomPosition,Color, true,-1,0,50);
				DrawDebugLine(GetWorld(),ClosestPoint2,RandomPosition, Color, true,-1,0,50);
				DrawDebugLine(GetWorld(),ClosestPoint3,RandomPosition, Color, true,-1,0,50);
				*/

				continue;
			}

			/*DrawDebugLine(GetWorld(),ClosestPoint1,RandomPosition,Color, true,-1,0,50);
			DrawDebugLine(GetWorld(),ClosestPoint2,RandomPosition, Color, true,-1,0,50);
			DrawDebugLine(GetWorld(),ClosestPoint3,RandomPosition, Color, true,-1,0,50);*/

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
			
			Detail->SetActorScale3D(FVector(DetailScaling, DetailScaling, DetailScaling));
			Detail->SetActorLocation(RandomPosition);

			//Details.Add(CreatePlanet(SpawnTransfrom * GetRootComponent()->GetComponentTransform(),RandomPlanetIndex));
			Details.Add(Detail);
			Success = true;

			break;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, FString::Printf(TEXT("%i Details"), Details.Num()));

}


void ARouteExample::ClearRouteData()
{

	
	
}

void  ARouteExample::SwitchCamera()
{

	/*if(CameraIndex >= Planets.Num())
	{
		CameraIndex = 0;
	}

	UGameplayStatics::GetPlayerController(GetWorld(),0)->SetViewTargetWithBlend(Planets[CameraIndex],CameraTransitionSpeed,EViewTargetBlendFunction::VTBlend_Linear);
	CameraIndex++;*/
}

bool ARouteExample::MoveAlongPath(UPathData* PathData , float DeltaTime)
{

	//Get The current Spline Track Position and Apply to the player
	float SplineLength = PathData->Splines[PathData->Index]->GetSplineLength();
	auto player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	splineTimer += DeltaTime * player->MovementSpeed / SplineLength;

	/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple,  FString::Printf( TEXT("Spline Length %f"),SplineLength));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple,  FString::Printf( TEXT("Travel Time %f"),PlayerTravelTime));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple,  FString::Printf( TEXT("Spline Timer %f"),splineTimer));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple,  FString::Printf( TEXT("Spline Count %i"),PathData->Splines[PathData->Index]->GetNumberOfSplinePoints()));*/
	
	float DistanceTraveled = FMath::Lerp(SplineLength, 0, splineTimer);
	FVector PlayerPosition = PathData->Splines[PathData->Index]->GetLocationAtDistanceAlongSpline(DistanceTraveled, ESplineCoordinateSpace::Type::World);
	FRotator PlayerRotation = PathData->Splines[PathData->Index]->GetRotationAtDistanceAlongSpline(DistanceTraveled, ESplineCoordinateSpace::Type::World);

	//PlayerRotation = FRotator(PlayerRotation.Pitch, PlayerRotation.Yaw, 180);
	
	//Make the Camera Face the direction we are moving
	/*
	ASpaceshipCharacter* Charac = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	

	FVector StartPoint = Charac->TopDownCamera->GetComponentLocation();
	FVector EndPoint = Charac->GetActorLocation();

	FRotator CameraRotation = UKismetMathLibrary::FindLookAtRotation(StartPoint,EndPoint);
	Charac->TopDownCamera->SetWorldRotation(FQuat::Slerp(CameraRotation.Quaternion(), Charac->GetActorRotation().Quaternion(), CameraLerpSpeed * DeltaTime));
	*/

	
	/*StartPoint =Charac->TopDownCamera->GetComponentLocation();
	EndPoint = RouteData->Splines[RouteData->Index]->GetLocationAtDistanceAlongSpline(SplineLength, ESplineCoordinateSpace::Type::World);

	FRotator PlanetRotation = UKismetMathLibrary::FindLookAtRotation(EndPoint,StartPoint);

	FRotator NewRotation = FQuat::Slerp(CameraRotation.Quaternion(), PlanetRotation.Quaternion(), 0.5).Rotator();

	Charac->CameraBoom->SetWorldRotation(FQuat::Slerp(NewRotation.Quaternion(), Charac->GetActorRotation().Quaternion(), CameraLerpSpeed * DeltaTime));*/
	

	
	//TODO gives the spaceship a "Hover Feel" but not working currently
	/*
	FVector Offset;
	Offset.X = UKismetMathLibrary::Sin(timer);
	Offset.Y = UKismetMathLibrary::Cos(timer);
	Offset.Z = UKismetMathLibrary::Cos(timer * 2);
	*/
	//PlayerPosition += Offset * 500;

	UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->SetActorLocation(PlayerPosition);
	UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->SetActorRotation(PlayerRotation);
	AudioManager->ThrusterSoundComponent->SetWorldLocation(PlayerPosition);

	/* // TODO F key scaling stuff, can probably be removed for the final build
	if(Temp)
	{
		Temp = false;
		PlayerController->SetViewTargetWithBlend(this,CameraTransitionSpeed,EViewTargetBlendFunction::VTBlend_Linear);
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->SetActorScale3D(FVector(10,10,10));
	}

	if(Temp2)
	{
		Temp2 = false;
		PlayerController->SetViewTargetWithBlend(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0),CameraTransitionSpeed,EViewTargetBlendFunction::VTBlend_Linear);
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->SetActorScale3D(FVector(1,1,1));

	}
	*/

	
	if (splineTimer > PathStartEndPercent.Y)
	{
		splineTimer = PathStartEndPercent.X;
		PlayerController->SetViewTargetWithBlend(PathData->Stops[PathData->Index], CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
		// with the checkpoint I would imagine it would be something like this
		
		if (PathData->Stops[PathData->Index]->IsCheckpoint)
		{
			PathData->Stops[PathData->Index]->CurrentPlanet = true;
			CheckpointTransitionDelegate.Broadcast();
		}
		else
		{
			// update the current planet
			// this bool in the planet class is used by the vendor UI
			PathData->Stops[PathData->Index]->CurrentPlanet = true;
			// reset events when we reach a planet
			EventsComponent->ResetEvents();
			OrbitTransitionDelegate.Broadcast();
		}
		
	}

	return false; // The Movement is still in progress

}
void ARouteExample::OrbitPlanet(UPathData* PathData, float DeltaTime)
{
	
}
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

	for (auto path : CubePath1)
	{
		float distance = FVector::Distance(UKismetMathLibrary::FindClosestPointOnLine(path->GetActorLocation(), LookPosition, LookDirection), path->GetActorLocation());
		if (distance < Path1Distance)
		{
			Path1Distance = distance;
		}
	}

	for (auto path : CubePath2)
	{
		float distance = FVector::Distance(UKismetMathLibrary::FindClosestPointOnLine(path->GetActorLocation(), LookPosition, LookDirection), path->GetActorLocation());
		if (distance < Path2Distance)
		{
			Path2Distance = distance;
		}
	}

	for (auto path : CubePath3)
	{
		float distance = FVector::Distance(UKismetMathLibrary::FindClosestPointOnLine(path->GetActorLocation(), LookPosition, LookDirection), path->GetActorLocation());
		if (distance < Path2Distance)
		{
			Path2Distance = distance;
		}
	}


	if (Path1Distance < Path2Distance)
	{
		/*for (auto cube : CubePath1)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(4, 4, 4));
			}
		}
		for (auto cube : CubePath2)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(2, 2, 2));
			}
		}

		for (auto cube : CubePath3)
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

		/*for (auto cube : CubePath2)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(4, 4, 4));
			}
		}
		for (auto cube : CubePath3)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(4, 4, 4));
			}
		}

		for (auto cube : CubePath1)
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

		for (auto cube : CubePath2)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(1, 1, 1));
			}
		}
		for (auto cube : CubePath3)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(1, 1, 1));
			}
		}

		for (auto cube : CubePath1)
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

void ARouteExample::TransitionToMap()
{
	PlayerController->SetViewTargetWithBlend(GetRootComponent()->GetAttachmentRootActor(), CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	SwapState(Selecting);
}

void ARouteExample::SwapToOrbiting()
{


	//TODO change skybox color;
	ASpaceSkyBox* SpaceSkyBox = Cast<ASpaceSkyBox>(UGameplayStatics::GetActorOfClass(GetWorld(),ASpaceSkyBox::StaticClass()));
	SpaceSkyBox->OffsetColor();
	
	// it would probably look better 
	// if we made all other planets and the path invisible when we are in a planet
	// either do this or make the UI more opaque
	ChangeVisibilityOfRoute(true);

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

	if(RouteData->AtFirstPlanet)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, FString::Printf(TEXT("huh: %i"), RouteData->Index ));
		RouteData->AtFirstPlanet = false;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, FString::Printf(TEXT("AAAAAAAAAAAAAA: %i"), RouteData->Index ));

		RouteData->Index++;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, FString::Printf(TEXT("BBBBBBBBBB: %i"), RouteData->Index ));

	}
	
	SwapState(Orbiting);



}

//Use this when orbiting the first planet as to not mess up the routes
void ARouteExample::BeginToOrbiting()
{
	// it would probably look better 
	// if we made all other planets and the path invisible when we are in a planet
	// either do this or make the UI more opaque
	ChangeVisibilityOfRoute(true);

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

void ARouteExample::SwapToMoving()
{
	NavIncidentsTimer = 0.0f;
	SwapState(Moving);
	ASpaceshipCharacter* Charac = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Charac->SetHidden(false);
	Charac->TopDownCamera->SetActive(true);
	PlayerController->SetViewTargetWithBlend(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);


	
	//Make the Camera Face the direction we are moving
	float SplineLength = RouteData->Splines[RouteData->Index]->GetSplineLength();
	FVector StartPoint = RouteData->Splines[RouteData->Index]->GetLocationAtDistanceAlongSpline(0, ESplineCoordinateSpace::Type::World);
	FVector EndPoint = RouteData->Splines[RouteData->Index]->GetLocationAtDistanceAlongSpline(SplineLength, ESplineCoordinateSpace::Type::World) - FVector(0,0,-50);
	
	Charac->CameraBoom->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(EndPoint, StartPoint) + TempAngler);
	//Charac->CameraBoom->AddWorldRotation(FRotator(0,0,15));
	
	
	/*if (RouteData->Index >= RouteData->Max)
	{
		//TODO probably make a functions for reseting everyting
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, FString::Printf(TEXT("AAAAAAAAAAAAAAAAAAAAAA: %i"), RouteData->Index ));

		
		RouteData->Reset();

		SplineComponent1->ClearSplinePoints();
		SplineComponent2->ClearSplinePoints();
		SplineComponent3->ClearSplinePoints();

		CameraSplineComponent1->ClearSplinePoints();
		CameraSplineComponent2->ClearSplinePoints();
		CameraSplineComponent3->ClearSplinePoints();

		Generate();

		PlayerController->SetViewTargetWithBlend(GetRootComponent()->GetAttachmentRootActor(), CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);

		SelectTransitionDelegate.Broadcast();

	}
	else
	{
		PlayerController->SetViewTargetWithBlend(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	}*/

}

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

void ARouteExample::SwapToCombat()
{
	NavIncidentsTimer = 0.0f;
}


bool ARouteExample::IsRouteGood()
{

	if(Spline1->Spline->GetNumberOfSplinePoints() <= 3)
		return false;

	if(Spline2->Spline->GetNumberOfSplinePoints() <= 3)
		return false;

	if(Spline3->Spline->GetNumberOfSplinePoints() <= 3)
		return false;


	return true;
}

void ARouteExample::SwapState(PlayerStates State)
{
	PreviousState = PlayerState;
	PlayerState = State;

	if(PreviousState == Moving)
	{
		AudioManager->ThrusterSoundComponent->Stop();
		AudioManager->TurboSoundComponent->Stop();
	}
	else if (PlayerState == Moving)
	{
		AudioManager->ThrusterSoundComponent->Play();

	}

	if (PlayerState == Fighting)
	{
		AudioManager->BattleSoundComponent->Play();
	}
	else if(PreviousState == Fighting)
	{
		AudioManager->BattleSoundComponent->Stop();
	}

	if (PlayerState == Moving)
	{
		if (AudioManager->AmbientSoundComponent->bIsPaused)
			AudioManager->AmbientSoundComponent->SetPaused(false);
		else
			AudioManager->AmbientSoundComponent->Play();
	}
	else if(PreviousState == Moving)
	{
		AudioManager->AmbientSoundComponent->SetPaused(true);
	}

	if(PreviousState == Selecting)
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

	if (PlayerState == Selecting)
	{
		for (int i = 0; i < Planets.Num(); i++)
		{
			Planets[i]->HideUI = false;
		}
	}
}

void ARouteExample::GetPathSelected(UPathData* path)
{
	path = RouteData;
}

void ARouteExample::LeaveOrbit()
{
	/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Max: %i"), RouteData->Max));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Index: %i"), RouteData->Index));*/


	//Three Conditions
	//Leaving Space Station - No need to select or generate route
	//Leaving Last Planet - Need Camera Transition and to regrenerate a route
	//Leaving First Planet - No Route so need to generate and then select


	for (auto it : Planets)
	{
		it->CurrentPlanet = false;
	}

	ChangeVisibilityOfRoute(false);

	if(RouteData->ID == 0)
	{
		for (auto it : CubePath2)
		{
			it->SetActorHiddenInGame(true);
		}

		for (auto it : CubePath3)
		{
			it->SetActorHiddenInGame(false);
		}
		
	}else if(RouteData->ID == 1)
	{
		for (auto it : CubePath1)
		{
			it->SetActorHiddenInGame(true);
		}
	}
	
	
	if(RouteData->Max == 0) // First Planet
	{

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("First Planet")));

		SelectTransitionDelegate.Broadcast();
	}
	else if(RouteData->Max == RouteData->Index) // Last Planet
	{

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Last Planet")));
		//UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this, CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA: %i"), RouteData->Max));
		GenerateImproved(Planets[1]->Index,FVector(0,0,0));

		SelectTransitionDelegate.Broadcast();
	}
	else // Space Station
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Spacestation")));
		MovingTransitionDelegate.Broadcast();
	}
	
}

UPathData* ARouteExample::SelectRoute(bool WhichRoute)
{
	if(WhichRoute)
	{
		Spline1->SetMaterial(0);
		Spline2->SetMaterial(1);
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

		for (auto it : CubePath1)
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
		
		for (auto it : CubePath2)
		{
			it->SetActorHiddenInGame(true);
		}

		for (auto it : CubePath3)
		{
			it->SetActorHiddenInGame(true);
		}
	}

	PathClickedDelegate.Broadcast(RouteData);
}

void ARouteExample::StartGame()
{
	GenerateImproved(-1,FVector(0,0,0));
	randomSpinRate = FMath::RandRange(1, 100);

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(Planets[1], CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	Planets[1]->CurrentPlanet = true;
	CurrentPlanet = Planets[1];

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetShowMouseCursor(true);

	FightCamera->SetWorldLocation(FVector(0, 0, 3000.0));

	ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	player->AudioManager = AudioManager;
	

	Spline1->SetMaterial(1);
	Spline2->SetMaterial(1);
	Spline3->SetMaterial(1);
}

void ARouteExample::ChangeVisibilityOfRoute(bool toHide)
{
	for (auto it : Planets)
	{
		it->SetActorHiddenInGame(toHide);
	}

	for (auto it : CubePath1)
	{
		it->SetActorHiddenInGame(toHide);
	}

	for (auto it : CubePath2)
	{
		it->SetActorHiddenInGame(toHide);
	}
	for (auto it : CubePath3)
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
		AEnemyActor->GetActorBounds(false, ActorOrigin, ActorSize, false);
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

	if(player)
	{
		player->Attack(DeltaTime,AEnemyActor);
	}

	if (AEnemyActor)
	{
		AEnemyActor->Attack();
	}
	player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!(player->Alive))
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("Player is Dead"));
		//CombatReset();
		GameOverDelegate.Broadcast();
		ResetCameraAfterCombat();
	}
	else if (!IsValid(AEnemyActor))
	{
		//CombatReset();
		SwapState(Event);
		AudioManager->VictorySoundComponent->SetWorldLocation(player->GetActorLocation());
		AudioManager->VictorySoundComponent->Play();

		// player->TopDownCamera->SetActive(true);
		// UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this, CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	}

}


void ARouteExample::CombatReset() {

	//MovingTransitionDelegate.Broadcast();
	//SwapState(PreviousState);
	ASpaceshipCharacter* CurrentPlayer = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	AEnemyActor->ResetEnemy();
;
	if(CurrentPlayer)
		CurrentPlayer->ResetCombat();
	
	AEnemyActor = nullptr;
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("Combat Reset"));
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
