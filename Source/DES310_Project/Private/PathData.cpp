// Fill out your copyright notice in the Description page of Project Settings.


#include "PathData.h"

void UPathData::Reset()
{
	Splines.Empty();
	Stops.Empty();
	Index = 0;
	Max = 0; // Could probably just use Stops.Num
	AtFirstPlanet = true;

	//In Percent?
	EventChance = 10;
	StoryQuestChance = 10;
	RandomQuestChance = 10;

	RouteName = "Undefined";


	
}
