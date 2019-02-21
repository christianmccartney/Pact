// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "UserWidgetWithParent.h"

// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Find the InteractableComponentClass
	ConstructorHelpers::FClassFinder<UUserWidget> InteractPromptClassFinder(TEXT("/Game/GameObjects/Widgets/InteractPrompt"));
	InteractPromptClass = InteractPromptClassFinder.Class;
}


// Called when the game starts
void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInteractableComponent::interactWith() {
	if (showInteractPrompt && interactPrompt != nullptr) {
		interactPrompt->RemoveFromParent();
		interactPrompt = nullptr;
	}
	OnInteractedWith.Broadcast();
}

void UInteractableComponent::showInteractable() {
	// Create interact prompt widget if the flag is set to true
	if (showInteractPrompt) {
		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		interactPrompt = CreateWidget<UUserWidgetWithParent>(controller, InteractPromptClass);
		if (interactPrompt) {
			interactPrompt->parent = this;
			interactPrompt->AddToViewport();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Unable to create interact prompt"));
		}
	}
	OnShowInteractable.Broadcast();
}
void UInteractableComponent::hideInteractable() {
	// destroy prompt widget if we have one
	if (showInteractPrompt && interactPrompt != nullptr) {
		interactPrompt->RemoveFromParent();
		interactPrompt = nullptr;
	}
	OnHideInteractable.Broadcast();
}
