#include "InventoryComponent.h"
// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/

#include "SpaceshipCharacter.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	ItemDatabase = CreateDefaultSubobject<UItemData>(TEXT("Database"));
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
	if (ItemDatabase)
	{
		ConvertDataAsset(ItemDatabase, Items);
		AssignSlots();
	}
	// ...
	
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
				if (HullSlot == nullptr)
					HullSlot = it;
				break;
			case SHIELD:
				if (ShieldSlot == nullptr)
					ShieldSlot = it;
				break;

			case BLASTER:
				if (BlasterSlot == nullptr)
					BlasterSlot = it;
				break;
			case ENGINE:
				if (EngineSlot == nullptr)
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
