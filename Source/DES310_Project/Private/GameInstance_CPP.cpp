// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance_CPP.h"
#include "Engine.h"
#include "GameSave.h"

void UGameInstance_CPP::Init()
{
	// check if there is game data to load it
	// or create a new one if the game is just starting

	if (!UGameplayStatics::DoesSaveGameExist(TEXT("Game_Save"), 0))
	{
		// create a new game data
		GameSave = Cast<UGameSave>(UGameplayStatics::CreateSaveGameObject(UGameSave::StaticClass()));
	}
	else {
		GameSave = Cast<UGameSave>(UGameplayStatics::LoadGameFromSlot(SaveSlot, 0));
	}

}

void UGameInstance_CPP::SaveGameData()
{
	if (GameSave)
	{
		GameSave->SavedPlayerStats.ByteData.Empty();
		ASpaceshipCharacter* Player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

		GameSave->SavedPlayerStats.playerName = Player->GetFName();
		GameSave->SavedPlayerStats.playerPos = Player->GetActorLocation();

		FMemoryWriter MemoryWriter(GameSave->SavedPlayerStats.ByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, true);
		// Find only variables with UPROPERTY(SaveGame)
		Ar.ArIsSaveGame = true;
		// Converts Actor's SaveGame UPROPERTIES into binary array
		Player->StatsPlayerComponent->Serialize(Ar);

		UGameplayStatics::SaveGameToSlot(GameSave, SaveSlot, 0);

	}
}

UGameSave* UGameInstance_CPP::GetGameData()
{
	
	return GameSave;

}
