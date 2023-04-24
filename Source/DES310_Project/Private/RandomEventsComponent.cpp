// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/

#include "RandomEventsComponent.h"
#include "SpaceshipCharacter.h"
#include "StatsComponent.h"
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

bool URandomEventsComponent::RollForEvent(int32 ChanceOfEventInThisRoute, float deltaTime, int StoryChance, int RandomChance)
{
	// roll to see if event happens
	// if yes then choose a random event
	EventTimer += deltaTime;
	float roll =  FMath::RandRange(0, 100);
	
	if (EventTimer > GameplayEventTick)
	{
		if (roll < ChanceOfEventInThisRoute)
		{
			// determine the type of event
			int typeOfEventRoll = FMath::RandRange(0, 100);	

			if (typeOfEventRoll <= StoryChance)
				RollEventFromArray(StoryEventsList);
			else 
				RollEventFromArray(RandomEventsList);

			EventTimer = 0;
			if(CurrentEvent != nullptr)
				return true;
		}
	}

	return false;
}

UGameEvents* URandomEventsComponent::RollEventFromArray(TArray<UGameEvents*>& eventsList)
{
	// roll for an event
	int32 indexOfEvent = FMath::RandRange(0, eventsList.Num() - 1);
	// make sure there is a possible event
	if (eventsList.Num() > 0)
	{
		if (AnyEventsPossible(eventsList))
		{
			// if event has already fired then roll again
			while (eventsList[indexOfEvent]->HasFired)
			{
				indexOfEvent = FMath::RandRange(0, eventsList.Num() - 1);
			}

			eventsList[indexOfEvent]->HasFired = true;

			CurrentEvent = eventsList[indexOfEvent];
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Rolled Event"));
			GameplayEventFiredDelegate.Broadcast();
			// increase the tick of the events
			GameplayEventTick *= GameplayEventTickMultiplier;

			return CurrentEvent;
		}
	}

	return nullptr;


}


// Called when the game starts
void URandomEventsComponent::BeginPlay()
{
	Super::BeginPlay();
	ConvertDataAssets();

	GameplayEventFiredDelegate.AddUniqueDynamic(this, &URandomEventsComponent::EventFired);

	Player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if(Player)
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Player Casted"));

	ResetEvents();
	// ...
	
}

void URandomEventsComponent::ConvertDataAssets()
{
	// this might be a problem if we have a lot of events
	// it might lead to too many casting operations happening in the begin play - bad in performance
	// works for now
	// probably it would be better to only do the casting when we need to use the event object
	for (auto it : EventsDatabase->Data)
	{
		UObject* object = it->GetDefaultObject();
		UGameEvents* Event = nullptr;
		if (object)
		{
			Event = Cast<UGameEvents>(object);
			if (Event)
			{
				switch (Event->Type)
				{
				case STORY:
					StoryEventsList.Add(Event);
					break;
				case RANDOM:
					RandomEventsList.Add(Event);
					break;
				}
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
	for (auto it : RandomEventsList)
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
	//route->SwapState(Fighting);
	//route->CheckpointTransitionDelegate.Broadcast();
	// depnding on the effects of the option
	// change the players stats 

	if (OptionPicked == nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Option NULL"));
	}
	else {
		ASpaceshipCharacter* PlayerOBJ = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		PlayerOBJ->StatsPlayerComponent->IncreaseCurrency(OptionPicked->CreditsGained);
		PlayerOBJ->StatsPlayerComponent->XPSystem(OptionPicked->XPGained);
	}

	if(route)
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Handling Event"));
	
	if(OptionPicked)
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, OptionPicked->Description.ToString());
	
	// if the option leads to combat then change the state
	// and pass the enemy data associated with the event ??? 

	route->MovingTransitionDelegate.Broadcast();
}


// Called every frame
void URandomEventsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

bool URandomEventsComponent::AnyEventsPossible(TArray<UGameEvents*>& eventsList)
{
	for (auto it : eventsList)
	{
		if (!it->HasFired)
		{
			return true;
		}
	}
	return false;
}

void URandomEventsComponent::ResetEvents()
{
	for (auto it : RandomEventsList)
	{
		it->HasFired = false;
	}

	for (auto it : StoryEventsList)
	{
		it->HasFired = false;
	}

	for (auto it : CombatEventsList)
	{
		it->HasFired = false;
	}
}

void URandomEventsComponent::EventFired()
{

}

