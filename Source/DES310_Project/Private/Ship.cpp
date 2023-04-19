// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 02/02/23
//

#include "Ship.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AShip::AShip()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*playerHealth = 100;

	if (UGameplayStatics::DoesSaveGameExist(TEXT("Save1"), 0))
	{
		SaveData = Cast<UGameSave>(UGameplayStatics::LoadGameFromSlot(TEXT("Save1"), 0));
	}
	else
	{
		SaveData = Cast<UGameSave>(UGameplayStatics::CreateSaveGameObject(UGameSave::StaticClass()));
		SaveData->playerHealth = playerHealth;
		UGameplayStatics::SaveGameToSlot(SaveData, TEXT("Save1"), 0);
	}*/


	bSimGravityDisabled = false;

}

// Called when the game starts or when spawned
void AShip::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;


	/*
	//Utility function to populate array with all Camera Actors in the level 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), CameraActor, FoundActors);

	//Sets Player Controller view to the first CameraActor found 

	PlayerCharacterController->SetViewTargetWithBlend(FoundActors[0], CameraBlendTime, EViewTargetBlendFunction::VTBlend_Linear);
	*/


}

// Called every frame
void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction(TEXT("Save"),IE_Pressed,this,&AShip::Save);
	PlayerInputComponent->BindAction(TEXT("Load"),IE_Pressed,this,&AShip::Load);

}

void AShip::Save()
{
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Game Saved")));
	//SaveData->playerHealth = playerHealth;
	//SaveData->playerHealth += 100;
	//UGameplayStatics::SaveGameToSlot(SaveData, TEXT("Save1"), 0);

	// if(SaveData)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Game Saved")));
	// 	SaveData->playerHealth = playerHealth + 100.f;
	// 	UGameplayStatics::SaveGameToSlot(SaveData,TEXT("Save1"),0);
	// }
	
}

void AShip::Load()
{
	/*if(UGameplayStatics::DoesSaveGameExist(TEXT("Save1"),0))
	{
		SaveData = Cast<UGameSave>(UGameplayStatics::LoadGameFromSlot(TEXT("Save1"),0));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Game Loaded")));

		if (SaveData)
		{
			playerHealth = SaveData->playerHealth;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Health Set to %f"), playerHealth));
		}

	}
	else 
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("No Save Availible")));
	}*/

}