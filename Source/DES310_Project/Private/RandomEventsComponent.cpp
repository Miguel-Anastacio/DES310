// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/

#include "RandomEventsComponent.h"
#include "RouteExample.h"

// Sets default values for this component's properties
URandomEventsComponent::URandomEventsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	EventsDatabase = CreateDefaultSubobject<URandomEventsData>("Events Database");
	// ...
}

UGameEvents* URandomEventsComponent::RollForEvent(int32 ChanceOfEventInThisRoute, float deltaTime)
{
	// roll to see if event happens
	// if yes then choose a random event
	EventTimer += deltaTime;
	float roll =  FMath::RandRange(0, 100);
	if (EventsList.Num() > 0)
	{
		if (EventTimer > GameplayEventTick)
		{
			if (roll < ChanceOfEventInThisRoute && !EventHasFiredOnThisRoute)
			{
				int32 indexOfEvent = FMath::RandRange(0, EventsList.Num() - 1);
				EventHasFiredOnThisRoute = true;
				CurrentEvent = EventsList[indexOfEvent];
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Rolled Event"));
				GameplayEventFiredDelegate.Broadcast();
				return EventsList[indexOfEvent];
			}
		}
	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Empty"));
	return nullptr;
}


// Called when the game starts
void URandomEventsComponent::BeginPlay()
{
	Super::BeginPlay();
	ConvertDataAssets();

	GameplayEventFiredDelegate.AddUniqueDynamic(this, &URandomEventsComponent::EventFired);
	// ...
	
}

void URandomEventsComponent::ConvertDataAssets()
{
	for (auto it : EventsDatabase->Data)
	{
		UObject* object = it->GetDefaultObject();
		UGameEvents* Event = nullptr;
		if (object)
		{
			Event = Cast<UGameEvents>(object);
			if (Event)
			{
				EventsList.Add(Event);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Error casting to event"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Error casting to object"));
		}

	}

	// iterate through each event and cast the options to values that we can use
	for (auto it : EventsList)
	{
		it->EventOptions.Empty();
		for(int i= 0; i < it->Options.Num(); i++)
		{
			UObject* object = nullptr;
			if(it->Options[i])
				object = it->Options[i]->GetDefaultObject();
			UEventOption* EventOption = nullptr;
			if (object)
			{
				EventOption = Cast<UEventOption>(object);
				if (EventOption)
				{
					it->EventOptions.Add(EventOption);
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Error casting to option"));
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Error casting to object"));
			}
		}

		
	}
}

void URandomEventsComponent::HandleEvent(UEventOption* OptionPicked)
{
	ARouteExample* route = Cast<ARouteExample>(GetOwner());
	EventIsDisplayed = false;
	CurrentEvent = nullptr;
	route->PlayerState = Moving;
	// depnding on the effects of the option
	// change the players stats 
	if(route)
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Handling Event"));
	
	if(OptionPicked)
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, OptionPicked->Description.ToString());
	// if the option leads to combat then change the state
	// and pass the enemy data associated with the event ??? 

}


// Called every frame
void URandomEventsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void URandomEventsComponent::EventFired()
{

}

