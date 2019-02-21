// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidgetWithParent.generated.h"

/**
 * 
 */
UCLASS()
class PACT_API UUserWidgetWithParent : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	UObject* parent;
	
};
