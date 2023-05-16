// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceSkyBox.h"


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

	if(!RandomizeSkyBox) // Keep default skybox
		return;

	//Create Dynamic Material from default
	Material = SkyMesh->GetMaterial(0);
	DynamicMaterial = UMaterialInstanceDynamic::Create(Material,this);

	//Get random color
	const float RandomFloat = FMath::RandRange(0.0,1.0);
	CurrentHue = RandomFloat;
	
	DynamicMaterial->SetScalarParameterValue(FName(TEXT("Hue-slide")),CurrentHue);
	if(RandomizeEverything) // Randomize every aspect
	{
		DynamicMaterial->SetScalarParameterValue(FName(TEXT("power")),FMath::RandRange(0.0,1.0));
		DynamicMaterial->SetScalarParameterValue(FName(TEXT("Scene Brightness")),FMath::RandRange(0.0,1.0));
		DynamicMaterial->SetScalarParameterValue(FName(TEXT("starBrightness")),FMath::RandRange(0.0,1.0));
	}

	SkyMesh->SetMaterial(0,DynamicMaterial); // Apply custom material
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Material Made: %f"), RandomFloat));
}

// Called every frame
void ASpaceSkyBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Blend) // Make the color shift in hue
	{
		CurrentHue += Offset * DeltaTime;
		DynamicMaterial->SetScalarParameterValue(FName(TEXT("Hue-slide")),CurrentHue);
	}

	//CurrentHue = FMath::Lerp(CurrentHue,TargetHue, DeltaTime);
	
}

void ASpaceSkyBox::OffsetColor()
{

	/*int RandomMultiplier = FMath::RandRange(1,5);
	float RandomHue = (1.0/6.0) * RandomMultiplier; 
	TargetHue = CurrentHue + RandomHue;*/

	//Picks a Color that isn't the current color
	TargetHue = FMath::RandRange(0.0,1.0);
	CurrentHue = TargetHue;
	DynamicMaterial->SetScalarParameterValue(FName(TEXT("Hue-slide")),CurrentHue);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, FString::Printf(TEXT("Skybox Color Changed")));

}

void ASpaceSkyBox::SetColor(float Hue)
{
	CurrentHue = Hue;
	DynamicMaterial->SetScalarParameterValue(FName(TEXT("Hue-slide")),CurrentHue);
}

