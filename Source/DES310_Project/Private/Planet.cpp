// Fill out your copyright notice in the Description page of Project Settings.

/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/
#include "Planet.h"
#include "Vendor.h"
#include "SpaceshipCharacter.h"
#include "Animation/AnimInstanceProxy.h"


APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = DummyRoot;
	
	/*SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	SphereCollisionComponent->SetupAttachment(RootComponent);
	SphereCollisionComponent->InitSphereRadius(20.0f);
	SphereCollisionComponent->SetGenerateOverlapEvents(true);*/

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point"));
	PointLight->SetupAttachment(RootComponent);
	
	PlanetMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Planet Mesh"));
	PlanetMeshComponent->SetWorldLocation(FVector(0, 0, 0));
	if (PlanetMeshComponent)
		PlanetMeshComponent->SetupAttachment(RootComponent);

	Tags.Add(TEXT("Planet"));

	RotationPerFrame = FRotator(0.0, 0.0, 0.0f);



	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->SetUsingAbsoluteScale(false);
	CameraBoom->TargetArmLength = 3000.f;
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	CameraBoom->SetWorldRotation(FRotator(FMath::RandRange(300, 360), FMath::RandRange(0, 360), 0));



	FocusPlanetCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Planet Camera"));
	FocusPlanetCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FocusPlanetCamera->bUsePawnControlRotation = false;

	Line1 = FText::FromString("Planet A");
	Line2 = FText::FromString("You Are Maybe Here");
	Line3 = FText::FromString("Distance: 100 AU");

}

void APlanet::SetRandomQuest()
{
	UObject* object = nullptr;
	int index = FMath::RandRange(0, QuestTemplates.Num() - 1);
	if (QuestTemplates.Num() > 0 && QuestTemplates[index])
		object = QuestTemplates[index]->GetDefaultObject();
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("QUEST - Error casting from template"));
	if (object)
		Quest = Cast<UQuest>(object);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("QUEST - Error casting to object"));
}


void APlanet::OnPlanetDestroyed(AActor* Act)
{
	if (VendorActor)
		VendorActor->Destroy();
}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();
	/*SphereCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APlanet::OnOverlapBegin);
	SphereCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &APlanet::OnOverlapEnd);*/

	this->OnDestroyed.AddDynamic(this, &APlanet::OnPlanetDestroyed);
	
	FTransform SpawnTransform;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnTransform.SetRotation(FQuat4d(0, 0, 0, 1.0f));
	SpawnTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
	SpawnTransform.SetLocation(GetActorLocation());

	CameraBoom->TargetArmLength = CameraBoom->TargetArmLength * this->GetActorScale().Length();
	PointLight->SourceRadius = this->GetActorScale().GetMax(); // TODO set to use average or the max



	/*
	Line1 = FText::FromString(Name);
	if(IsFirstPlanet)
	{
		Line2 = FText::FromString("You Are Here!");
		Line3 = FText::FromString(" ");

	}
	else
	{
		if(IsCheckpoint)
		{
			Line2 = FText::FromString("Shop Available");
		}
		else
		{
			Line2 = FText::FromString("Destination");
		}

		Line3 = FText::FromString("Distance: 100 AU");
	}*/

	

	SetRandomQuest();
}

// Called every frame
void APlanet::Tick(float DeltaTime)
{
	// on every frame change rotationg for a smooth rotating actor
	FQuat QuatRotation = RotationPerFrame.Quaternion();

	PlanetMeshComponent->AddWorldRotation(QuatRotation, false, 0, ETeleportType::None);
	Super::Tick(DeltaTime);

}

/*
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
*/

void APlanet::SetPlanetIconUI()
{
	if (IsFirstPlanet)
	{
		Icon = AllIcons[0];

	}
	else
	{
		if (IsCheckpoint)
		{
			Icon = AllIcons[1];
		}
		else
		{
			Icon = AllIcons[2];
		}
	}
}