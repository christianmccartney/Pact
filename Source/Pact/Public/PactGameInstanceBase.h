// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BossMonsterBase.h"
#include "PactGameInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class PACT_API UPactGameInstanceBase : public UGameInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	bool getBossDefeated(ABossMonsterBase* boss);

	UFUNCTION(BlueprintImplementableEvent)
	void setBossDefeated(ABossMonsterBase* boss, bool defeated);
	
};
