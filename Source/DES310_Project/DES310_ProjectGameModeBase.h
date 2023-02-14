// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DES310_ProjectGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DES310_PROJECT_API ADES310_ProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
	TSubclassOf< class APawn> PlayerPawnClass;
	ADES310_ProjectGameModeBase();

};
