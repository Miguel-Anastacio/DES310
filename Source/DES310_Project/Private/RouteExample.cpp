// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 10/02/23
//


#include "RouteExample.h"
#include "Kismet/KismetMathLibrary.h"
#include "SpaceshipCharacter.h"


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


	SplineComponent1 = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Short Path"));
	SplineComponent1->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
	SplineComponent1->ClearSplinePoints();
	SplineComponent1->bDrawDebug = true;

	SplineComponent2 = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Long Path"));
	SplineComponent2->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
	SplineComponent2->ClearSplinePoints();
	SplineComponent2->bDrawDebug = true;
	
	SplineComponent3 = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Long Path 2"));
	SplineComponent3->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
	SplineComponent3->ClearSplinePoints();
	SplineComponent3->bDrawDebug = true;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->SetUsingAbsoluteScale(false);

	CameraBoom->bDoCollisionTest = false; 

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Route Camera"));
	Camera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	CameraBoom->SetWorldRotation(FRotator(-90, 180, 0));

	PlayerState = PlayerStates::Selecting;

	// create event component
	EventsComponent = CreateDefaultSubobject<URandomEventsComponent>(TEXT("Events Component"));


	
}

// Called when the game starts or when spawned
void ARouteExample::BeginPlay()
{
	Super::BeginPlay();
	Generate();
	randomSpinRate = FMath::RandRange(1,100);
	//auto temp = CreateBasicCube(FVector(0,0,10),FRotator());
	UGameplayStatics::GetPlayerController(GetWorld(),0)->SetViewTargetWithBlend(this,CameraTransitionSpeed,EViewTargetBlendFunction::VTBlend_Linear);
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(),0);
	PlayerController->SetShowMouseCursor(true);

	CameraBoom->TargetArmLength = CameraDistance * this->GetActorScale().Length(); // TODO change to use Highest x/y/z instead of the pythag

	OrbitTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToOrbiting);
	MovingTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToMoving);
	SelectTransitionDelegate.AddUniqueDynamic(this, &ARouteExample::SwapToSelecting);

	PlayerState = Selecting;
	SelectTransitionDelegate.Broadcast();
	PathClickedDelegate.AddUniqueDynamic(this, &ARouteExample::GetPathSelected);
}

// Called every frame
void ARouteExample::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	timer +=  DeltaTime;
	if(timer >= RouteTickRate)
	{
		timer = 0;
		//Generate();
	}

	cameraTimer +=  DeltaTime;
	if(cameraTimer >= CameraRate)
	{
		cameraTimer = 0;
		SwitchCamera();
	}

	switch  (PlayerState)
	{
	case PlayerStates::Moving:
		MoveAlongPath(RouteData,DeltaTime);
		if (EventsComponent->RollForEvent(RouteData.EventChance, DeltaTime))
			PlayerState = Event;
		break;
	case PlayerStates::Orbiting: OrbitPlanet(RouteData,DeltaTime);
		break;
	case PlayerStates::Selecting: SelectPath();
		break;
	case PlayerStates::Event: 
		break;
	}
	
}

//An example of how to use the route system
//instead of including this example just copy the code and remove the debug lines from them
//from there the points given out by A* can be used however you want

APath* ARouteExample::CreateBasicCube(FTransform transform)
{
	
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;
	
	APath* MyNewActor = GetWorld()->SpawnActor<APath>(PathBP,transform,SpawnParam);
	MyNewActor->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
	
	return MyNewActor;
	
}

APlanet*  ARouteExample::CreateBasicSphere(FTransform transform)
{
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	/*
	APlanet* MyNewActor = GetWorld()->SpawnActor<APlanet>(PlanetBP,transform,SpawnParam);*/

	int randomNumber = 0;
	APlanet* APlanetActor = GetWorld()->SpawnActor<APlanet>(PlanetBP[FMath::RandRange(0, PlanetBP.Num() - 1)], transform, SpawnParams);

	APlanetActor->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
	
	return APlanetActor;
}

TArray<FVector2D> ARouteExample::SmoothLine(TArray<FVector2D>& vect)
{
	int NumberOfPoint = vect.Num();
	TArray<FVector2D> SmoothedPoints;
	FVector2D Average(0,0);
	float Total = 0;

	for(auto point : vect)
	{
		Average = FVector2D(0,0);
		Total = 0;
		for(auto otherPoint : vect)
		{
			float Distance = FVector2D::Distance(otherPoint, point);
			if(Distance != 0)
			{
				Average += otherPoint * 1 / (Distance * SmoothFactor);
				Total += 1 / (Distance * SmoothFactor);
			}
			else
			{
				Average += otherPoint;
				Total += 1;
			}
			
		}
		SmoothedPoints.Add(Average / Total);
	}
	return SmoothedPoints;
}

void ARouteExample::Generate()
{
	Path1.Empty();
	Path2.Empty();
	Path3.Empty();

	for (auto& Planet : Planets)
	{
		Planet->Destroy();
	}

	for (auto& Cube : CubePath1)
	{
		Cube->Destroy();
	}

	for (auto& Cube : CubePath2)
	{
		Cube->Destroy();
	}
	for (auto& Cube : CubePath3)
	{
		Cube->Destroy();
	}

	
	if (GetWorld())
		FlushPersistentDebugLines(GetWorld());

	FVector2D Max(Dimensions.X/2,Dimensions.Y + PointRadius);
	FVector2D Min(Dimensions.X/2,0 - PointRadius);



	TArray<FVector2D> vect = PoissonDiscSampling::PoissonDiscGenerator(PointRadius,FVector2D((int)Dimensions.X,(int)Dimensions.Y),RejectionRate);
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
	
	for (int i = 0; i < vect.Num(); i++)
	{
		astar.AddPoint(vect[i]);
	}

	for(int i = 0; i < triangleList.Num(); i++)
	{
		FVector2D p1 = triangleList[i].Vertex[0];
		FVector2D p2 = triangleList[i].Vertex[1];
		FVector2D p3 = triangleList[i].Vertex[2];
		int id1 = astar.AddPoint(p1);
		int id2 = astar.AddPoint(p2);
		int id3 = astar.AddPoint(p3);
		

		astar.ConnectPoints(id1,id2);
		astar.ConnectPoints(id2,id3);
		astar.ConnectPoints(id3,id1);
		
		if(min.Y > p1.Y)
		{
			minID = id1;
		}
		else if(max.Y < p1.Y)
		{
			maxID = id1;
		}

		if (abs(p1.X + 5) > maxX && p1.Y < 250 && p1.Y > 50)
		{
			maxX = abs(p1.X);
			maxXID = id1;
		}

		if (abs(p2.X + 5) > maxX && p2.Y < 250 && p2.Y > 50)
		{
			maxX = abs(p2.X);
			maxXID = id2;
		}

		if (abs(p3.X + 5) > maxX && p3.Y < 250 && p3.Y > 50)
		{
			maxX = abs(p3.X);
			maxXID = id3;
		}

		if(min.Y > p2.Y)
		{
			minID = id2;
		}
		else if(max.Y < p2.Y)
		{
			maxID = id2;
		}

		if(min.Y > p3.Y)
		{
			minID = id3;
		}
		else if(max.Y < p3.Y)
		{
			maxID = id3;
		}
	}

	for(auto point : astar.points)
	{
		if(FMath::RandBool())
		{
			point.blocked = true;
		}
	}


	FColor leftRouteColor = FColor::Orange;
	FColor rightRouteColor = FColor::Orange;


	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%i AStar Positions"), astar.points.Num()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Begin Point: %i"), minID));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("End Point: %i"), maxID));

	FTransform WorldLocation;
	float scale = 1;
	if(GetRootComponent())
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

	/*Path2 = SmoothLine(Path2);
	Path1 = SmoothLine(Path1);*/
	
	for (int i = 0; i < Path1.Num(); i++)
	{
		FTransform SpawnTransfrom;
		SpawnTransfrom.SetRotation(FQuat4d(0,0,0,1.f));
		SpawnTransfrom.SetScale3D(FVector(0.1,0.1,0.1));
		SpawnTransfrom.SetLocation(FVector(1,Path1[i].X - Dimensions.X/2,Path1[i].Y - Dimensions.Y/2));
		SpawnTransfrom *= WorldLocation;
		
		if (i >= CubePath1.Num())
		{
			CubePath1.Add(CreateBasicCube(SpawnTransfrom));
		}
		CubePath1[i]->SetActorTransform(SpawnTransfrom);
		SplineComponent1->AddSplinePoint(SpawnTransfrom.GetLocation(), ESplineCoordinateSpace::Type::World,true);
	}

	for (int i = 0; i < Path2.Num(); i++)
	{
		FTransform SpawnTransfrom;
		SpawnTransfrom.SetRotation(FQuat4d(0,0,0,1.f));
		SpawnTransfrom.SetScale3D(FVector(0.1,0.1,0.1));
		SpawnTransfrom.SetLocation(FVector(1,Path2[i].X- Dimensions.X/2,Path2[i].Y- Dimensions.Y/2));
		SpawnTransfrom *= WorldLocation;
		
		if (i >= CubePath2.Num())
		{
			CubePath2.Add(CreateBasicCube(SpawnTransfrom));
		}
		CubePath2[i]->SetActorTransform(SpawnTransfrom);
		SplineComponent2->AddSplinePoint(SpawnTransfrom.GetLocation(), ESplineCoordinateSpace::Type::World,true);
	}

	for (int i = 0; i < Path3.Num(); i++)
	{
		FTransform SpawnTransfrom;
		SpawnTransfrom.SetRotation(FQuat4d(0,0,0,1.f));
		SpawnTransfrom.SetScale3D(FVector(0.1,0.1,0.1));
		SpawnTransfrom.SetLocation(FVector(1,Path3[i].X- Dimensions.X/2,Path3[i].Y- Dimensions.Y/2));
		SpawnTransfrom *= WorldLocation;
		
		if (i >= CubePath3.Num())
		{
			CubePath3.Add(CreateBasicCube(SpawnTransfrom));
		}
		CubePath3[i]->SetActorTransform(SpawnTransfrom);
		SplineComponent3->AddSplinePoint(SpawnTransfrom.GetLocation(), ESplineCoordinateSpace::Type::World,true);
	}


	FTransform SpawnTransfrom;
	if(Planets.Num() < 3)
	{
		
		SpawnTransfrom.SetRotation(FQuat4d(0,0,0,1.f));
		SpawnTransfrom.SetScale3D(FVector(0.5,0.5,0.5));
		SpawnTransfrom.SetLocation(FVector(1,astar.begin.position.X- Dimensions.X/2,astar.begin.position.Y- Dimensions.Y/2));
		
		Planets.Add(CreateBasicSphere(SpawnTransfrom * WorldLocation));
		SpawnTransfrom.SetLocation(FVector(1,astar.end.position.X- Dimensions.X/2,astar.end.position.Y- Dimensions.Y/2));
		Planets.Add(CreateBasicSphere(SpawnTransfrom * WorldLocation));
		SpawnTransfrom.SetLocation(FVector(1,checkPoint.X- Dimensions.X/2,checkPoint.Y- Dimensions.Y/2));
		Planets.Add(CreateBasicSphere(SpawnTransfrom * WorldLocation));
	}

	SpawnTransfrom *= WorldLocation;
	
	auto playerController = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
	if(playerController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Player Moved")));
		
		playerController->SetActorLocation(FVector(SpawnTransfrom.GetLocation().X,SpawnTransfrom.GetLocation().Y,1000));
	}

	AveragePathPosition1 = FVector(0,0,0);
	int i =0;
	for(auto pathPoint : CubePath1)
	{
		AveragePathPosition1 += pathPoint->GetActorLocation();
		i++;
	}
	AveragePathPosition1 /= i;
	
	AveragePathPosition2 = FVector(0,0,0);
	i =0;
	for(auto pathPoint : CubePath2)
	{
		AveragePathPosition2 += pathPoint->GetActorLocation();
		i++;

	}
	for(auto pathPoint : CubePath3)
	{
		AveragePathPosition2 += pathPoint->GetActorLocation();
		i++;

	}

	AveragePathPosition2 /= i;

	
	auto player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	player->ProjectWorldLocationToScreen(AveragePathPosition1,AveragePathPosition12D);
	player->ProjectWorldLocationToScreen(AveragePathPosition2,AveragePathPosition22D);
	

	
	//bool longPath = false;
	//
	//for(int i =0; i < PathsWanted; i++)
	//{

	//	if (longPath)
	//	{
	//		astar.search(minID, maxXID);
	//		for (int j = 0; j < astar.path.Num() - 1; j++)
	//		{
	//			DrawDebugSphere(GetWorld(), WorldLocation.TransformPosition(FVector(-5, astar.path[j].X, astar.path[j].Y)), scale * DisplayRadius / 6, 10, FColor::Yellow, true, 5);
	//			DrawDebugLine(GetWorld(), WorldLocation.TransformPosition(FVector(-5, astar.path[j].X, astar.path[j].Y)), WorldLocation.TransformPosition(FVector(-5, astar.path[j + 1].X, astar.path[j + 1].Y)), FColor::Orange, true, 5, 0, scale * 1);
	//		}
	//		astar.search(maxXID, maxID);
	//		for (int j = 0; j < astar.path.Num() - 1; j++)
	//		{
	//			DrawDebugSphere(GetWorld(), WorldLocation.TransformPosition(FVector(-5, astar.path[j].X, astar.path[j].Y)), scale * DisplayRadius / 6, 10, FColor::Yellow, true, 5);
	//			DrawDebugLine(GetWorld(), WorldLocation.TransformPosition(FVector(-5, astar.path[j].X, astar.path[j].Y)), WorldLocation.TransformPosition(FVector(-5, astar.path[j + 1].X, astar.path[j + 1].Y)), FColor::Orange, true, 5, 0, scale * 1);
	//		}
	//	}
	//	else
	//	{
	//		astar.search(minID, maxID);
	//		for (int j = 0; j < astar.path.Num() - 1; j++)
	//		{
	//			DrawDebugSphere(GetWorld(), WorldLocation.TransformPosition(FVector(-5, astar.path[j].X, astar.path[j].Y)), scale * DisplayRadius / 6, 10, FColor::Yellow, true, 5);
	//			DrawDebugLine(GetWorld(), WorldLocation.TransformPosition(FVector(-5, astar.path[j].X, astar.path[j].Y)), WorldLocation.TransformPosition(FVector(-5, astar.path[j + 1].X, astar.path[j + 1].Y)), FColor::Orange, true, 5, 0, scale * 1);
	//			int id = astar.findPoint(astar.path[j]);
	//			if (id != -1 && j != 0)
	//			{
	//				astar.points[id].blocked = true;
	//			}
	//		}


	//		if (astar.path.Num() - 1 > 0)
	//		{
	//			DrawDebugLine(GetWorld(), WorldLocation.TransformPosition(FVector(-5, astar.path[astar.path.Num() - 1].X, astar.path[astar.path.Num() - 1].Y)), WorldLocation.TransformPosition(FVector(-5, astar.begin.position.X, astar.begin.position.Y)), FColor::Orange, true, 5, 0, scale * 1);
	//			DrawDebugSphere(GetWorld(), WorldLocation.TransformPosition(FVector(-5, astar.path[astar.path.Num() - 1].X, astar.path[astar.path.Num() - 1].Y)), scale * DisplayRadius / 6, 10, FColor::Yellow, true, 5);
	//		}

	//	}

	//	longPath = true;
	//}


	//DrawDebugSphere(GetWorld(), WorldLocation.TransformPosition(FVector(-5, astar.begin.position.X, astar.begin.position.Y)), scale *DisplayRadius / 2, 10, FColor::Green, true, 5);
	//DrawDebugSphere(GetWorld(), WorldLocation.TransformPosition(FVector(-5, astar.end.position.X, astar.end.position.Y)), scale *DisplayRadius / 2, 10, FColor::White, true, 5);


	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Path Points: %i"), astar.path.Num()));

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

bool ARouteExample::MoveAlongPath(PathData& PathData, float DeltaTime)
{
	
	//Get The current Spline Track Position and Apply to the player
	float SplineLength = PathData.Splines[PathData.Index]->GetSplineLength();
	splineTimer += DeltaTime * PlayerTravelTime / SplineLength;
	float DistanceTraveled = FMath::Lerp(SplineLength,0,splineTimer);
	FVector PlayerPosition = PathData.Splines[PathData.Index]->GetLocationAtDistanceAlongSpline(DistanceTraveled, ESplineCoordinateSpace::Type::World);
	FRotator PlayerRotation = PathData.Splines[PathData.Index]->GetRotationAtDistanceAlongSpline(DistanceTraveled, ESplineCoordinateSpace::Type::World);

	PlayerRotation = FRotator(PlayerRotation.Pitch - 180, PlayerRotation.Yaw, 180);
	
	UGameplayStatics::GetPlayerCharacter(GetWorld(),0)->SetActorLocation(PlayerPosition);
	UGameplayStatics::GetPlayerCharacter(GetWorld(),0)->SetActorRotation(PlayerRotation);

	if(splineTimer > 1)
	{
		splineTimer = 0;
		PlayerController->SetViewTargetWithBlend(PathData.Stops[PathData.Index],CameraTransitionSpeed,EViewTargetBlendFunction::VTBlend_Linear);
		//PlayerState = Orbiting;
		OrbitTransitionDelegate.Broadcast();
	}

	return false; // The Movement is still in progress
	
}
void ARouteExample::OrbitPlanet(PathData& PathData, float DeltaTime)
{
	//TODO switch this to the statemachine and when it leaves up the index but for now, timer
	orbitTimer += DeltaTime;
	if(orbitTimer > 1000)
	{
		orbitTimer = 0;
		PathData.Index += 1;
		if(PathData.Index >= PathData.Max)
		{
			//TODO probably make a runctions for reseting everyting
			
			PlayerState = PlayerStates::Selecting;
			Generate();
			PathData.Splines.Empty();
			PathData.Stops.Empty();
			PathData.Index = 0;
			PathData.Max = 0;

			SplineComponent1->ClearSplinePoints();
			SplineComponent2->ClearSplinePoints();
			SplineComponent3->ClearSplinePoints();

			PlayerController->SetViewTargetWithBlend(GetRootComponent()->GetAttachmentRootActor(),CameraTransitionSpeed,EViewTargetBlendFunction::VTBlend_Linear);
		}
		else
		{
			PlayerState = PlayerStates::Moving;
			PlayerController->SetViewTargetWithBlend(UGameplayStatics::GetPlayerCharacter(GetWorld(),0),CameraTransitionSpeed,EViewTargetBlendFunction::VTBlend_Linear);
		}

	}
}
void ARouteExample::SelectPath()
{
	PlayerState = Selecting;
	auto player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector LookPosition;
	FVector LookDirection;
	player->DeprojectMousePositionToWorld(LookPosition,LookDirection);
	
	float mouseX, mouseY;
	player->GetMousePosition(mouseX, mouseY);
	float Path1Distance = 10000000000000000; // TODO maybe do a distance test with the first element but i cba
	float Path2Distance = 10000000000000000;

	DrawDebugLine(GetWorld(),LookPosition,LookPosition + LookDirection * 100, FColor::Emerald,false, 2, 1,1);
	bool WhichPath = false; // TODO if we stick to 2 paths this is fine but could be improved
	
	for(auto path : CubePath1)
	{
		float distance =  FVector::Distance(UKismetMathLibrary::FindClosestPointOnLine(path->GetActorLocation(),LookPosition,LookDirection),path->GetActorLocation());
		if(distance < Path1Distance)
		{
			Path1Distance = distance;
		}
	}

	for(auto path : CubePath2)
	{
		float distance =  FVector::Distance(UKismetMathLibrary::FindClosestPointOnLine(path->GetActorLocation(),LookPosition,LookDirection),path->GetActorLocation());
		if(distance < Path2Distance)
		{
			Path2Distance = distance;
		}
	}

	for(auto path : CubePath3)
	{
		float distance =  FVector::Distance(UKismetMathLibrary::FindClosestPointOnLine(path->GetActorLocation(),LookPosition,LookDirection),path->GetActorLocation());
		if(distance < Path2Distance)
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
				cube->SetActorScale3D(FVector(4, 4, 4));
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
				cube->SetActorScale3D(FVector(1, 1, 1));
			}
		}
		CurrentSpline = SplineComponent2;
		CurrentPlanet = Planets[0];
		WhichPath = true;
	}

	ASpaceshipCharacter* Charac = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	Charac->Selected;
	
	if(	Charac->Selected) // TODO should be replaced with mouse click instead of a random timer
	{
		PathClickedDelegate.Broadcast();
		Charac->Selected = false;
		timer = 0;
		if(WhichPath)
		{
			RouteData.Splines.Add(SplineComponent2);
			RouteData.Splines.Add(SplineComponent3);
			RouteData.Stops.Add(Planets[0]);
			RouteData.Stops.Add(Planets[1]);
			RouteData.Max = RouteData.Splines.Num();
			RouteData.Index = 0;
		}
		else
		{
			RouteData.Splines.Add(SplineComponent1);
			RouteData.Stops.Add(Planets[1]);
			RouteData.Max = RouteData.Splines.Num();
			RouteData.Index = 0;
		}
		//MovingTransitionDelegate.Broadcast();

	}
	
}

void ARouteExample::TransitionToMap()
{
	PlayerController->SetViewTargetWithBlend(GetRootComponent()->GetAttachmentRootActor(), CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	PlayerState = Selecting;
}

void ARouteExample::SwapToOrbiting()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Swapping to Orbiting")));
	PlayerState = Orbiting;
}

void ARouteExample::SwapToMoving()
{
	PlayerController->SetViewTargetWithBlend(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	PlayerState = PlayerStates::Moving;
}

void ARouteExample::SwapToSelecting()
{
	PlayerController->SetViewTargetWithBlend(GetRootComponent()->GetAttachmentRootActor(), CameraTransitionSpeed, EViewTargetBlendFunction::VTBlend_Linear);
	PlayerState = Selecting;
}

void ARouteExample::GetPathSelected()
{
	// path = currentPath;
}
