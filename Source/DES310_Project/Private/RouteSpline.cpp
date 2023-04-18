// Fill out your copyright notice in the Description page of Project Settings.


#include "RouteSpline.h"

int ARouteSpline::counter = 0;

class USplineMeshComponent;
// Sets default values
ARouteSpline::ARouteSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* root = this->CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	root->Mobility = EComponentMobility::Type::Movable;
	this->SetRootComponent(root);
	
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Short Path 1"));
	Spline->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	Spline->ClearSplinePoints();
	Spline->bDrawDebug = true;


}

// Called when the game starts or when spawned
void ARouteSpline::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ARouteSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARouteSpline::SetMaterial(int MaterialIndex)
{
	/*if(MaterialIndex >= Materials.Num())
		MaterialIndex = 0;*/

	for(int i =0; i < Meshes.Num();i++)
	{
		Meshes[i]->SetMaterial(0,Materials[MaterialIndex]);
	}
}

void ARouteSpline::CreateSpline()
{
	for(int i = 0; i < Spline->GetNumberOfSplinePoints(); i ++)
	{
		FTransform Transform = GetTransform();
		USplineMeshComponent* Mesh = Cast<USplineMeshComponent>(AddComponentByClass(USplineMeshComponent::StaticClass(),false,Transform,false));
		if(Mesh)
		{
			Mesh->Mobility = EComponentMobility::Type::Movable;
			Mesh->SetStaticMesh(SplineMesh);
			Mesh->SetMaterial(0,Materials[counter]);
			Mesh->SetForwardAxis(ESplineMeshAxis::Z);
			Mesh->SetStartAndEnd(Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local),Spline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local),Spline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local),Spline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local));
			Mesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			Meshes.Add(Mesh);

		}
	}

	counter++;
	if(counter >= 3)
	{
		counter = 0;
	}
	
}



