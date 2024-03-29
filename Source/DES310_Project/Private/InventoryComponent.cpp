#include "InventoryComponent.h"
// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/

#include "GameInstance_CPP.h"
#include "SpaceshipCharacter.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	ItemDatabase = CreateDefaultSubobject<UItemData>(TEXT("Database"));
	AllPossibleItemsDatabase = CreateDefaultSubobject<UItemData>(TEXT("AllItemsDatabase"));
}


UItem* UInventoryComponent::GetPartByName(FString name, bool &result)
{
	result = false;

	if (ItemDatabase == nullptr)
		return nullptr;
	for (int i = 0; i < ItemDatabase->Data.Num(); i++)
	{
		UObject* object = ItemDatabase->Data[i]->GetDefaultObject();
		UItem* Item = nullptr;
		if(object)
			Item = Cast<UItem>(object);
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Error casting to object"));
		}
		if (Item)
		{		
			if (Item->Name == name)
			{
				result = true;
				return Item;
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Error casting to item"));
		}
	}

	return nullptr;
}

bool UInventoryComponent::GetAllItemsOfType(PartType type, TArray<UItem*>& ItemsOfType)
{
	bool foundItems = false;
	for (auto it : Items)
	{
		if (it->Type == type)
		{
			foundItems = true;
			ItemsOfType.Add(it);
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::FromInt(Items.Num()));
	return foundItems;
}


bool UInventoryComponent::RemoveItem(FString name)
{
	for (int i = 0; i < Items.Num(); i++)
	{
		if (Items[i]->Name == name)
		{
			Items.RemoveAt(i);
			return true;
		}
	}
	return false;
}

void UInventoryComponent::LoadItems(TArray<int> ItemsID)
{
	
	TArray<UItem*> AllItems;
	ConvertDataAsset(AllPossibleItemsDatabase, AllItems);

	for (auto id : ItemsID)
	{
		for (auto item : AllItems)
		{
			if (id == item->ID)
				Items.AddUnique(item);
		}
	}
	AssignSlots();
}

int UInventoryComponent::HowManyItemsOftype(PartType type)
{
	int amount = 0;
	for (auto it : Items)
	{
		if (it->Type == type)
		{
			amount++;
		}
	}
	return amount;
}


bool UInventoryComponent::SwapShipParts(PartType type, UItem* newItem)
{
	bool result = false;
	UItem* PreviousItem = nullptr;
	if (newItem)
	{
		switch (type)
		{
		case SHIELD:
			PreviousItem = ShieldSlot;
			if (PreviousItem->Name != newItem->Name)
			{
				ShieldSlot = newItem;
				result = true;
			}
			break;
		case HULL:
			PreviousItem = HullSlot;
			if (PreviousItem->Name != newItem->Name)
			{
				HullSlot = newItem;
				result = true;
			}
			break;
		case BLASTER:
			PreviousItem = BlasterSlot;
			if (PreviousItem->Name != newItem->Name)
			{
				BlasterSlot = newItem;
				result = true;
			}
			break;
		case ENGINE:
			PreviousItem = EngineSlot;
			if (PreviousItem->Name != newItem->Name)
			{
				EngineSlot = newItem;
				result = true;
			}
			break;
		default:
			break;
		}


		ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(GetOwner());
		if (player)
		{
			player->ApplyItemToStats(newItem);
		}

		if (result)
		{
			RemoveItem(PreviousItem->Name);
			Items.Add(newItem);
		}
	}

	return result;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if(IsSaveLoadSuccessful())
	{
		return;
	}
	
	if (ItemDatabase)
	{
		ConvertDataAsset(ItemDatabase, Items);

		GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red,	TEXT("DEFAULT ASSIGNMENT OF ITEMS"));
		AssignSlots();
	}
	// ...
	
}

bool UInventoryComponent::IsSaveLoadSuccessful()
{
	if (!UGameplayStatics::DoesSaveGameExist(TEXT("Game_Save"), 0))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,	TEXT("SAVE NOT FOUND"));
		return false;
	}

	UGameSave* GameSave = (Cast<UGameInstance_CPP>(UGameplayStatics::GetGameInstance(GetWorld())))->GetGameData();
	
	LoadItems(GameSave->SavedPlayerStats.InventoryItemIDs);

	ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(GetOwner());
	if (player)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Problem casting"));
		player->ApplyInventoryToStats();
	}

	return true;
}



void UInventoryComponent::AssignSlots()
{
	for (auto it : Items)
	{
		if (it)
		{
			switch (it->Type)
			{
			case HULL:
					HullSlot = it;
				break;
			case SHIELD:
					ShieldSlot = it;
				break;
			case BLASTER:
					BlasterSlot = it;
				break;
			case ENGINE:
					EngineSlot = it;
				break;
			default:
				break;
			}
		}
	}
}

void UInventoryComponent::ConvertDataAsset(UItemData* AItemDatabase, TArray<UItem*>&ArrayOfItems)
{
	for (auto it : AItemDatabase->Data)
	{
		UObject* object = it->GetDefaultObject();
		UItem* Item = nullptr;
		if (object)
		{
			Item = Cast<UItem>(object);
			if (Item)
			{
				ArrayOfItems.Add(Item);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Error casting to item"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Error casting to object"));
		}
	}

}



// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

/*
bool UInventoryComponent::AddItem(UItem* Item)
{
	if (Item)
	{
		Items.Add(Item->Name);
		return true;
	}

	return false;

}

bool UInventoryComponent::Discard(UItem* Item)
{
	
	if (Items.Contains(Item->Name))
	{
		Items.Remove(Item->Name);
		return true;
	}

	return false;
}

UItem* UInventoryComponent::GetItemsByType(ItemType type)
{
	return nullptr;
}
*/
