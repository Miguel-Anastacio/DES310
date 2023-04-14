// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceSkyBox.h"

#include <string>

// Sets default values
ASpaceSkyBox::ASpaceSkyBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sky Mesh"));
	SkyMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASpaceSkyBox::BeginPlay()
{
	Super::BeginPlay();

	if(!RandomizeSkyBox)
		return;
	
	Material = SkyMesh->GetMaterial(0);
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material,this);

	float RandomFloat = FMath::RandRange(0.0,1.0);
	DynamicMaterial->SetScalarParameterValue(FName(TEXT("Hue-slide")),RandomFloat);
	if(RandomizeEverything)
	{
		DynamicMaterial->SetScalarParameterValue(FName(TEXT("power")),FMath::RandRange(0.0,1.0));
		DynamicMaterial->SetScalarParameterValue(FName(TEXT("Scene Brightness")),FMath::RandRange(0.0,1.0));
		DynamicMaterial->SetScalarParameterValue(FName(TEXT("starBrightness")),FMath::RandRange(0.0,1.0));
	}
	else
	{
		DynamicMaterial->SetScalarParameterValue(FName(TEXT("power")),0.45);
		DynamicMaterial->SetScalarParameterValue(FName(TEXT("Scene Brightness")),0.6);
		DynamicMaterial->SetScalarParameterValue(FName(TEXT("starBrightness")),0.2);
	}


	SkyMesh->SetMaterial(0,DynamicMaterial);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Material Made: %f"), RandomFloat));
}

// Called every frame
void ASpaceSkyBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
