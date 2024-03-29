// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance_CPP.h"
#include "Engine.h"
#include "GameSave.h"
#include "RouteExample.h"

void UGameInstance_CPP::Init()
{
	// check if there is game data to load it
	// or create a new one if the game is just starting

	if (UGameplayStatics::DoesSaveGameExist(TEXT("Game_Save"), 0))
	{
		// load game data
		GameSave = Cast<UGameSave>(UGameplayStatics::LoadGameFromSlot(SaveSlot, 0));
	}
	else {
		// create a new game data
		GameSave = Cast<UGameSave>(UGameplayStatics::CreateSaveGameObject(UGameSave::StaticClass()));
	}

}

void UGameInstance_CPP::SaveGameData(ARouteExample* CurrentRoute)
{
	if (GameSave)
	{
		// empty vector containers
		GameSave->SavedPlayerStats.StatsByteData.Empty();
		GameSave->SavedPlayerStats.InventoryItemIDs.Empty();
		GameSave->SavedRouteData.Planets.Empty();
		GameSave->SavedRouteData.Details.Empty();

		GameSave->SavedRouteData.Spline1Points.Empty();
		GameSave->SavedRouteData.Spline2Points.Empty();
		GameSave->SavedRouteData.Spline3Points.Empty();


		
		ASpaceshipCharacter* Player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

		GameSave->SavedPlayerStats.playerName = Player->GetFName();
		GameSave->SavedPlayerStats.PlayerPos = Player->GetActorLocation();
		GameSave->SavedPlayerStats.PlayerIndex = Player->Index;

		// setup player byte array writer
		FMemoryWriter MemoryWriter(GameSave->SavedPlayerStats.StatsByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, true);

		// Find only variables with UPROPERTY(SaveGame)
		Ar.ArIsSaveGame = true;
		
		// Converts Actor's SaveGame UPROPERTIES into binary array
		Player->StatsPlayerComponent->Serialize(Ar);

		// get inventory and save item IDs
		auto InventoryArray = Player->PlayerInventoryComponent->GetInventoryItems();
		for(auto i : InventoryArray)
		{
			
			GameSave->SavedPlayerStats.InventoryItemIDs.Add(i->ID);
			
		}

		// ------------ ROUTE DATA ---------- //
		FRouteObjectPair RouteObject;

		// save planet info
		for(auto planet : CurrentRoute->Planets)
		{
			RouteObject = {planet->GetName(), planet->Index, planet->GetActorLocation()};
			GameSave->SavedRouteData.Planets.Add(RouteObject);
		}

		// save route detail info
		for(auto detail : CurrentRoute->Details)
		{
			RouteObject = {detail->GetName(), detail->Index, detail->GetActorLocation()};
			GameSave->SavedRouteData.Details.Add(RouteObject);
		}
	
		TArray<FInterpCurvePoint<UE::Math::TVector<double>>> Spline1Points = CurrentRoute->Spline1->Spline->SplineCurves.Position.Points;
		TArray<FInterpCurvePoint<UE::Math::TVector<double>>> Spline2Points = CurrentRoute->Spline2->Spline->SplineCurves.Position.Points;
		TArray<FInterpCurvePoint<UE::Math::TVector<double>>> Spline3Points = CurrentRoute->Spline3->Spline->SplineCurves.Position.Points;

		// Iterate over each spline point and convert it to world space
		for (auto SplinePoint : Spline1Points)
		{
			FVector PointOnSpline = CurrentRoute->Spline1->Spline->GetComponentTransform().TransformPosition(SplinePoint.OutVal);
			GameSave->SavedRouteData.Spline1Points.Add(PointOnSpline);
		}
		
		for (auto SplinePoint : Spline2Points)
		{
			FVector PointOnSpline = CurrentRoute->Spline2->Spline->GetComponentTransform().TransformPosition(SplinePoint.OutVal);
			GameSave->SavedRouteData.Spline2Points.Add(PointOnSpline);
		}
		
		for (auto SplinePoint : Spline3Points)
		{
			FVector PointOnSpline = CurrentRoute->Spline3->Spline->GetComponentTransform().TransformPosition(SplinePoint.OutVal);
			GameSave->SavedRouteData.Spline3Points.Add(PointOnSpline);
		}

		GameSave->SavedRouteData.SkyboxHue = CurrentRoute->GetCurrentSkyboxHue();	
		GameSave->SavedRouteData.CurrentQuestID = CurrentRoute->CurrentQuest->ID;
		

		UGameplayStatics::SaveGameToSlot(GameSave, SaveSlot, 0);

	}
}

UGameSave* UGameInstance_CPP::GetGameData()
{
	return GameSave;
}
