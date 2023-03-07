// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 10/02/23
//


#include "RouteExample.h"

#include "DetailLayoutBuilder.h"
#include "Kismet/KismetMathLibrary.h"
#include "SpaceshipCharacter.h"
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


	
	SplineComponent1 = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Short Path 1"));
	SplineComponent1->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	SplineComponent1->ClearSplinePoints();
	SplineComponent1->bDrawDebug = true;

	SplineComponent2 = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Long Path 2"));
	SplineComponent2->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	SplineComponent2->ClearSplinePoints();
	SplineComponent2->bDrawDebug = true;

	SplineComponent3 = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Long Path 3"));
	SplineComponent3->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	SplineComponent3->ClearSplinePoints();
	SplineComponent3->bDrawDebug = true;

	CameraSplineComponent1 = CreateDefaultSubobject<USplineComponent>(TEXT("Camera Spline Long Path 1"));
	CameraSplineComponent1->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	CameraSplineComponent1->ClearSplinePoints();
	CameraSplineComponent1->bDrawDebug = true;

	CameraSplineComponent2 = CreateDefaultSubobject<USplineComponent>(TEXT("Camera Spline Long Path 2"));
	CameraSplineComponent2->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	CameraSplineComponent2->ClearSplinePoints();
	CameraSplineComponent2->bDrawDebug = true;

	CameraSplineComponent3 = CreateDefaultSubobject<USplineComponent>(TEXT("Camera Spline Long Path 3"));
	CameraSplineComponent3->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	CameraSplineComponent3->ClearSplinePoints();
	CameraSplineComponent3->bDrawDebug = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->SetUsingAbsoluteScale(false);

	CameraBoom->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Route Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	RouteData = CreateDefaultSubobject<UPathData>(TEXT("Route Data"));
	
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

	CameraBoom->TargetArmLength = CameraBoom->TargetArmLength * this->GetActorScale().Length(); // TODO change to use Highest x/y/z instead of the pythag


	
	AudioManager->AmbientSoundComponent->Play();
	


	OrbitTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToOrbiting);
	BeginOrbitTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::BeginToOrbiting);
	// transition to checkpoint is the same as to orbiting for now
	// the different delegate is just to bind different functionality in blueprint
	CheckpointTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToOrbiting);
	MovingTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToMoving);
	SelectTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToSelecting);
	PlayerState = Event;
	//PlayerState = Selecting;
	//SelectTransitionDelegate.Broadcast();
	PathClickedDelegate.AddUniqueDynamic(this, &ARouteExample::GetPathSelected);
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

	cameraTimer += DeltaTime;
	if (cameraTimer >= CameraRate)
	{
		cameraTimer = 0;
		SwitchCamera();
	}

	FString StateName = "";
	switch (PlayerState)
	{
	case PlayerStates::Moving:
		StateName = "Moving";
		MoveAlongPath(RouteData, DeltaTime);
		if (EventsComponent->RollForEvent(RouteData->EventChance, DeltaTime))
			PlayerState = Event;
		break;
	case PlayerStates::Orbiting: OrbitPlanet(RouteData, DeltaTime);
		StateName = "Orbiting";
		break;
	case PlayerStates::Selecting: SelectPath();
		StateName = "Selecting";
		break;
	case PlayerStates::Event:
		StateName = "Event";
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

APlanet* ARouteExample::CreateBasicSphere(FTransform transform)
{

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;



	APlanet* APlanetActor = GetWorld()->SpawnActor<APlanet>(PlanetBP[FMath::RandRange(0, PlanetBP.Num() - 1)], transform, SpawnParams);

	APlanetActor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

	return APlanetActor;
}



void ARouteExample::Generate()
{
	Path1.Empty();
	Path2.Empty();
	Path3.Empty();

	CurrentSpline = NULL;
	CurrentPlanet = NULL;

	RouteData->Splines.Empty();
	RouteData->Stops.Empty();
	RouteData->Index = 0;
	RouteData->Max = 0;

	SplineComponent1->ClearSplinePoints();
	SplineComponent2->ClearSplinePoints();
	SplineComponent3->ClearSplinePoints();

	CameraSplineComponent1->ClearSplinePoints();
	CameraSplineComponent2->ClearSplinePoints();
	CameraSplineComponent3->ClearSplinePoints();

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
	
	if (GetWorld())
		FlushPersistentDebugLines(GetWorld());

	FVector2D Max(Dimensions.X / 2, Dimensions.Y + PointRadius);
	FVector2D Min(Dimensions.X / 2, 0 - PointRadius);



	TArray<FVector2D> vect = PoissonDiscSampling::PoissonDiscGenerator(PointRadius, FVector2D((int)Dimensions.X, (int)Dimensions.Y), RejectionRate);
	vect.Add(Max);
	vect.Add(Min);

	//Probably could do this more efficiently... basically making it so in the engine the actual route isnt really far away from the route

	/*for(auto &point : vect)
	{
		point -= Dimensions / 2;
	}*/

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



	for (int i = 0; i < Path1.Num(); i++)
	{
		FTransform SpawnTransfrom;
		SpawnTransfrom.SetRotation(FQuat4d(0, 0, 0, 1.f));
		SpawnTransfrom.SetScale3D(FVector(1, 1, 1));
		SpawnTransfrom.SetLocation(FVector( Path1[i].X - Dimensions.X / 2, Path1[i].Y - Dimensions.Y / 2, UKismetMathLibrary::Sin(i) * 10));


		if (i >= CubePath1.Num())
		{
			CubePath1.Add(CreateBasicCube(SpawnTransfrom * WorldLocation));
		}

		SplineComponent1->AddSplinePoint((SpawnTransfrom * WorldLocation).GetLocation(), ESplineCoordinateSpace::Type::World, true);
		SpawnTransfrom.AddToTranslation(FVector(0 ,50, 50));
		SpawnTransfrom *= WorldLocation;
		CameraSplineComponent1->AddSplinePoint(SpawnTransfrom.GetLocation(), ESplineCoordinateSpace::Type::World, true);
	}

	for (int i = 0; i < Path2.Num(); i++)
	{
		FTransform SpawnTransfrom;
		SpawnTransfrom.SetRotation(FQuat4d(0, 0, 0, 1.f));
		SpawnTransfrom.SetScale3D(FVector(1, 1, 1));
		SpawnTransfrom.SetLocation(FVector( Path2[i].X - Dimensions.X / 2, Path2[i].Y - Dimensions.Y / 2, UKismetMathLibrary::Sin(i) * 10));


		if (i >= CubePath2.Num())
		{
			CubePath2.Add(CreateBasicCube(SpawnTransfrom * WorldLocation));
		}

		SplineComponent2->AddSplinePoint((SpawnTransfrom * WorldLocation).GetLocation(), ESplineCoordinateSpace::Type::World, true);
		SpawnTransfrom.AddToTranslation(FVector(0, 50, 50));
		SpawnTransfrom *= WorldLocation;
		CameraSplineComponent2->AddSplinePoint(SpawnTransfrom.GetLocation(), ESplineCoordinateSpace::Type::World, true);
	}

	for (int i = 0; i < Path3.Num(); i++)
	{
		FTransform SpawnTransfrom;
		SpawnTransfrom.SetRotation(FQuat4d(0, 0, 0, 1.f));
		SpawnTransfrom.SetScale3D(FVector(1, 1, 1));
		SpawnTransfrom.SetLocation(FVector( Path3[i].X - Dimensions.X / 2, Path3[i].Y - Dimensions.Y / 2,UKismetMathLibrary::Sin(i) * 10));


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
		SpawnTransfrom.SetScale3D(FVector(1, 1, 1));
		SpawnTransfrom.SetLocation(FVector(astar.begin.position.X - Dimensions.X / 2, astar.begin.position.Y - Dimensions.Y / 2, 0));

		Planets.Add(CreateBasicSphere(SpawnTransfrom * WorldLocation));
		SpawnTransfrom.SetLocation(FVector(astar.end.position.X - Dimensions.X / 2, astar.end.position.Y - Dimensions.Y / 2, 0));
		Planets.Add(CreateBasicSphere(SpawnTransfrom * WorldLocation));
		SpawnTransfrom.SetLocation(FVector(checkPoint.X - Dimensions.X / 2, checkPoint.Y - Dimensions.Y / 2 , 0));
		Planets.Add(CreateBasicSphere(SpawnTransfrom * WorldLocation));
	}

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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Spline Count: %i,%i,%i"),SplineComponent1->GetNumberOfSplinePoints(),SplineComponent2->GetNumberOfSplinePoints(),SplineComponent3->GetNumberOfSplinePoints()));
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
	splineTimer += DeltaTime * PlayerMovementSpeed / SplineLength;

	/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple,  FString::Printf( TEXT("Spline Length %f"),SplineLength));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple,  FString::Printf( TEXT("Travel Time %f"),PlayerTravelTime));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple,  FString::Printf( TEXT("Spline Timer %f"),splineTimer));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple,  FString::Printf( TEXT("Spline Count %i"),PathData->Splines[PathData->Index]->GetNumberOfSplinePoints()));*/
	
	float DistanceTraveled = FMath::Lerp(SplineLength, 0, splineTimer);
	FVector PlayerPosition = PathData->Splines[PathData->Index]->GetLocationAtDistanceAlongSpline(DistanceTraveled, ESplineCoordinateSpace::Type::World);
	FRotator PlayerRotation = PathData->Splines[PathData->Index]->GetRotationAtDistanceAlongSpline(DistanceTraveled, ESplineCoordinateSpace::Type::World);

	PlayerRotation = FRotator(PlayerRotation.Pitch - 180, PlayerRotation.Yaw, 180);
	

	
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

	
	if (splineTimer > PathStartEndPercent.Y)
	{
		splineTimer = PathStartEndPercent.X;
		PlayerController->SetViewTargetWithBlend(PathData->Stops[PathData->Index], CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
		// with the checkpoint I would imagine it would be something like this
		/*
		if(Stop is checkPoint)
			CheckpointTransitionDelegate.Broadcast()
		else
			// update the current planet
			// this bool in the planet class is used by the vendor UI
			PathData->Stops[PathData->Index]->CurrentPlanet = true;
			OrbitTransitionDelegate.Broadcast();
		
		*/

		// update the current planet
		// this bool in the planet class is used by the vendor UI
		PathData->Stops[PathData->Index]->CurrentPlanet = true;
		OrbitTransitionDelegate.Broadcast();
	}

	return false; // The Movement is still in progress

}
void ARouteExample::OrbitPlanet(UPathData* PathData, float DeltaTime)
{
	
}
void ARouteExample::SelectPath()
{
	//SwapState(Selecting); TODO maybe not needed/ messes up previous state
	auto player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector LookPosition;
	FVector LookDirection;
	player->DeprojectMousePositionToWorld(LookPosition, LookDirection);

	float mouseX, mouseY;
	player->GetMousePosition(mouseX, mouseY);
	float Path1Distance = 10000000000000000; // TODO maybe do a distance test with the first element but i cba
	float Path2Distance = 10000000000000000;

	DrawDebugLine(GetWorld(), LookPosition, LookPosition + LookDirection * 100, FColor::Emerald, false, 2, 1, 1);
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
		for (auto cube : CubePath1)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(2, 2, 2));
			}
		}
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
		CurrentSpline = SplineComponent1;
		CurrentPlanet = Planets[1];
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Left Side")));

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
		}

		for (auto cube : CubePath1)
		{
			if (cube)
			{
				cube->SetActorScale3D(FVector(1, 1, 1));
			}
		}
		CurrentSpline = SplineComponent2;
		CurrentPlanet = Planets[0];
		WhichPath = true;
	}
	

	ASpaceshipCharacter* Charac = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Charac->Selected;

	if (Charac->Selected) // TODO should be replaced with mouse click instead of a random timer
	{
		
		Charac->Selected = false; // TODO change this back once the player clicks on the ui and move everything inside this if statement so scaling isnt changing all the time
		timer = 0;
		if (WhichPath)
		{
			RouteData->Splines.Add(SplineComponent2);
			RouteData->Splines.Add(SplineComponent3);
			RouteData->Stops.Add(Planets[0]);
			RouteData->Stops.Add(Planets[1]);
			RouteData->Max = RouteData->Splines.Num();
			RouteData->Index = 0;
			RouteData->RouteName = "Long Route";
			RouteData->AtFirstPlanet = false;
		}
		else
		{
			RouteData->Splines.Add(SplineComponent1);
			RouteData->Stops.Add(Planets[1]);
			RouteData->Max = RouteData->Splines.Num();
			RouteData->RouteName = "Short Route";
			RouteData->Index = 0;
			RouteData->AtFirstPlanet = false;
		}

		PathClickedDelegate.Broadcast(RouteData);

	}

}

void ARouteExample::TransitionToMap()
{
	PlayerController->SetViewTargetWithBlend(GetRootComponent()->GetAttachmentRootActor(), CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	SwapState(Selecting);
}

void ARouteExample::SwapToOrbiting()
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

	SwapState(Moving);
	PlayerController->SetViewTargetWithBlend(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	ASpaceshipCharacter* Charac = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	
	//Make the Camera Face the direction we are moving
	float SplineLength = RouteData->Splines[RouteData->Index]->GetSplineLength();
	FVector StartPoint = RouteData->Splines[RouteData->Index]->GetLocationAtDistanceAlongSpline(0, ESplineCoordinateSpace::Type::World);
	FVector EndPoint = RouteData->Splines[RouteData->Index]->GetLocationAtDistanceAlongSpline(SplineLength, ESplineCoordinateSpace::Type::World);
	
	Charac->CameraBoom->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(EndPoint, StartPoint));

	
	
	
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

		ChangeVisibilityOfRoute(false);
	}

}


void ARouteExample::SwapState(PlayerStates State)
{
	PreviousState = PlayerState;
	PlayerState = State;

	if(PreviousState == Moving)
	{
		AudioManager->ThrusterSoundComponent->Stop();
	}
	else if (PlayerState == Moving)
	{
		AudioManager->ThrusterSoundComponent->Play();
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
	
	if(RouteData->Max == 0) // First Planet
	{
	
		SelectTransitionDelegate.Broadcast();
	}
	else if(RouteData->Max == RouteData->Index) // Last Planet
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA: %i"), RouteData->Max));
		Generate();
		SelectTransitionDelegate.Broadcast();
	}
	else // Space Station
	{

		MovingTransitionDelegate.Broadcast();
	}
	
}

void ARouteExample::StartGame()
{
	Generate();
	randomSpinRate = FMath::RandRange(1, 100);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(Planets[2], CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	Planets[2]->CurrentPlanet = true;
	CurrentPlanet = Planets[2];
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetShowMouseCursor(true);
}

void ARouteExample::ChangeVisibilityOfRoute(bool toHide)
{
	/*for (auto it : Planets)
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
	}*/
}