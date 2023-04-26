// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemData.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class DES310_PROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Database)
	UItemData* ItemDatabase;

	// store the data asset as an array of items
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<UItem*> Items;

	UItem* GetPartByName(FString name, bool& result);

	UFUNCTION(BlueprintCallable)
	bool GetAllItemsOfType(PartType type, TArray<UItem*>& ItemsOfType);

	// maybe create a player inventory class that inherits from this 
	// there are the slots and their methods
	UFUNCTION(BlueprintCallable)
	UItem* GetEquippedShield() { return ShieldSlot; };
	UFUNCTION(BlueprintCallable)
	UItem* GetEquippedHull() { return HullSlot; };
	UFUNCTION(BlueprintCallable)
	UItem* GetEquippedEngine() { return EngineSlot; }; 
	UFUNCTION(BlueprintCallable)
	UItem* GetEquippedBlaster() { return BlasterSlot; };


	// add functions to remove item and add item
	UFUNCTION(BlueprintCallable)
	bool RemoveItem(FString name);


	// swap item from slot, then delete the old one
	// maybe move to player inventory class 
	UFUNCTION(BlueprintCallable)
	bool SwapShipParts(PartType type, UItem* newItem);


	// converts database into an array of items
	void ConvertDataAsset(UItemData* AItemDatabase, TArray<UItem*>&ArrayOfItems);
protected:

	// player inventory has 4 slots
	// SHIELD, HULL, Blaster, ENgine
	UItem* ShieldSlot = nullptr;
	UItem* HullSlot = nullptr;
	UItem* BlasterSlot = nullptr;
	UItem* EngineSlot = nullptr;

	// Called when the game starts
	virtual void BeginPlay() override;
	// goes through the database and attaches each part to a slot
	// maybe move to player inventory class or change it to be more versatile
	void AssignSlots();




public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
