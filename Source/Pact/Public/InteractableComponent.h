// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UserWidgetWithParent.h"
#include "Delegates/Delegate.h"
#include "InteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractSignature);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACT_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractableComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString actionName = "interact";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool showInteractPrompt = true;

protected:
	TSubclassOf<class UUserWidgetWithParent> InteractPromptClass;

	UPROPERTY(BlueprintReadWrite)
	UUserWidgetWithParent* interactPrompt = nullptr;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FInteractSignature OnInteractedWith;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FInteractSignature OnShowInteractable;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FInteractSignature OnHideInteractable;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void interactWith();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void showInteractable();
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void hideInteractable();
};
