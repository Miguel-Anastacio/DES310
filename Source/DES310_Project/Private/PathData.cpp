// Fill out your copyright notice in the Description page of Project Settings.


#include "PathData.h"

void UPathData::Reset()
{
	Splines.Empty();
	Stops.Empty();
	Index = 0;
	Max = 0; // Could probably just use Stops.Num
	AtFirstPlanet = true;

	RouteName = "Undefined";

	StoryEventChance = 50;
	RandomEventChance = 50;
	
}

void UPathData::AssignRouteValues()
{
	if (!AssignedValues)
	{
		EventChance = FMath::RandRange(0, 100);
		if (EventChance < 33)
			EventChanceText = "Low";
		else if (EventChance < 66)
			EventChanceText = "Normal";
		else
			EventChanceText = "High";

		CombatEventChance = FMath::RandRange(0, 100);
		if (CombatEventChance < 33)
			CombatChanceText = "Low";
		else if (CombatEventChance < 66)
			CombatChanceText = "Normal";
		else
			CombatChanceText = "High";

		StoryEventChance = 50;
		RandomEventChance = 50;

		AssignedValues = true;
	}
}
