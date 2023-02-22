// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RandomEventsData.h"
#include "RandomEventsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class DES310_PROJECT_API URandomEventsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Database)
	URandomEventsData* EventsDatabase;

	UPROPERTY(EditAnywhere)
		float GameplayEventTick = 1.0f;
	float EventTimer = 0.0f;


	UPROPERTY(VisibleAnywhere)
	TArray<UGameEvents*> EventsList;

	UFUNCTION(BlueprintCallable)
	UGameEvents* GetCurrentEvent() {
		return CurrentEvent;
	};

	UFUNCTION(BlueprintCallable)
		bool GetIsEventDisplayed() { 
		return EventIsDisplayed; 
	};

	UFUNCTION(BlueprintCallable)
		void SetIsEventDisplayed(bool status) {
		EventIsDisplayed = status;
		
	};

	UFUNCTION(BlueprintCallable)
	void HandleEvent(UEventOption* option);

	// Sets default values for this component's properties
	URandomEventsComponent();

	// returns the event that is supposed to happen
	// returns null if no event is to happen
	UGameEvents* RollForEvent(int32 ChanceOfEventInThisRoute, float deltaTime);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetEventHasFiredOnThisRoute(bool RouteStatus) { EventHasFiredOnThisRoute = RouteStatus; };

protected:
	
	bool EventHasFiredOnThisRoute = false;
	bool EventIsDisplayed = false;
	// pointer that stores the current event
	// used to display the UI
	UGameEvents* CurrentEvent = nullptr;

	// Called when the game starts
	virtual void BeginPlay() override;
	
	void ConvertDataAssets();
		
};
