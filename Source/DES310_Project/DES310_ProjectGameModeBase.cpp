// Copyright Epic Games, Inc. All Rights Reserved.

#include "DES310_ProjectGameModeBase.h"
#include "Kismet/GameplayStatics.h"
ADES310_ProjectGameModeBase::ADES310_ProjectGameModeBase()
{
	DefaultPawnClass = PlayerPawnClass;
}
void ADES310_ProjectGameModeBase::SetPlayer(TSubclassOf< class APawn> PlayerBP)
{
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
	APawn* currentPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	Player = GetWorld()->SpawnActor<APawn>(PlayerBP, SpawnInfo);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(Player);
	Player->SetActorHiddenInGame(true);
	currentPlayer->Destroy();
}
