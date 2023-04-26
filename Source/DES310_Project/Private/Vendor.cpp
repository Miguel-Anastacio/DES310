// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/

#include "Vendor.h"


// Sets default values
AVendor::AVendor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

// Called when the game starts or when spawned
void AVendor::BeginPlay()
{
	Super::BeginPlay();

	
}

void AVendor::AddRandomItemFromArray(TArray<UItem*> items)
{
	int index = FMath::RandRange(0, items.Num() - 1);
	InventoryComponent->Items.AddUnique(items[index]);
}

// Called every frame
void AVendor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVendor::CreateRandomInventoryFromAllItems()
{
	// first randomize number of items (min 4 max allItems * 0.75)
	int maxItems = 0;
	if (AllItemsDatabase)
	{
		maxItems = AllItemsDatabase->Data.Num() * 0.75;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Database is null"));
	}
	int numberOfItems = FMath::RandRange(4, maxItems);

	// make sure it has an item of each type
	TArray<UItem*> ShieldItems;
	TArray<UItem*> HullItems;
	TArray<UItem*> EngineItems;
	TArray<UItem*> BlasterItems;
	TArray<UItem*> AllItems;
	
	if (AllItemsDatabase)
	{
		InventoryComponent->ConvertDataAsset(AllItemsDatabase, AllItems);

		for (auto it : AllItems)
		{
			switch (it->Type)
			{
			case SHIELD: ShieldItems.AddUnique(it);
				break;
			case HULL: HullItems.AddUnique(it);
				break;
			case ENGINE: EngineItems.AddUnique(it);
				break;
			case BLASTER: BlasterItems.AddUnique(it);
				break;
			default:
				break;
			}
		}

		AddRandomItemFromArray(ShieldItems);
		AddRandomItemFromArray(HullItems);
		AddRandomItemFromArray(EngineItems);
		AddRandomItemFromArray(BlasterItems);

		// fill the rest of the inventory with random items
		while (InventoryComponent->Items.Num() < numberOfItems)
		{
			int index = FMath::RandRange(0, AllItems.Num() - 1);
			InventoryComponent->Items.AddUnique(AllItems[index]);
		}

	}

}


