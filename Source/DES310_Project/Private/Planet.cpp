// Fill out your copyright notice in the Description page of Project Settings.

/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/
#include "Planet.h"
#include "SpaceshipCharacter.h"
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = DummyRoot;
	
	SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	SphereCollisionComponent->SetupAttachment(RootComponent);
	SphereCollisionComponent->InitSphereRadius(20.0f);
	SphereCollisionComponent->SetGenerateOverlapEvents(true);

	PlanetMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Planet Mesh"));
	if (PlanetMeshComponent)
		PlanetMeshComponent->SetupAttachment(RootComponent);

	FocusPlanetCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Planet Camera"));
	FocusPlanetCamera->SetupAttachment(RootComponent);
	FocusPlanetCamera->bUsePawnControlRotation = false;

	Tags.Add(TEXT("Planet"));

	RotationPerFrame = FRotator(0.0, 0.0, 2.0f);
	

}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();
	SphereCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APlanet::OnOverlapBegin);
	SphereCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &APlanet::OnOverlapEnd);

	FTransform SpawnTransform;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnTransform.SetRotation(FQuat4d(0, 0, 0, 1.0f));
	SpawnTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
	SpawnTransform.SetLocation(GetActorLocation());

	VendorActor = GetWorld()->SpawnActor<AVendor>(Vendor, SpawnTransform, SpawnParams);

	UObject* object = nullptr;
	if(QuestTemplate)
		object = QuestTemplate->GetDefaultObject();
	else
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Error casting from template"));
	if(object)
		Quest = Cast<UQuest>(object);
	else
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Error casting to object"));
}

// Called every frame
void APlanet::Tick(float DeltaTime)
{
	// on every frame change rotationg for a smooth rotating actor
	FQuat QuatRotation = RotationPerFrame.Quaternion();

	PlanetMeshComponent->AddWorldRotation(QuatRotation, false, 0, ETeleportType::None);
	Super::Tick(DeltaTime);

}

void APlanet::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CurrentPlanet = true;
	ASpaceshipCharacter* Player = Cast<ASpaceshipCharacter>(OtherActor);
	if (Player)
	{
		Player->MoveCameraTo(this);
	}
}

void APlanet::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyInde)
{

}
