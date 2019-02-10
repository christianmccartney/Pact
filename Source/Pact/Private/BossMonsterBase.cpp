// Fill out your copyright notice in the Description page of Project Settings.

#include "BossMonsterBase.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values
ABossMonsterBase::ABossMonsterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OnTakeAnyDamage.AddDynamic(this, &ABossMonsterBase::handleDamage);
	// Find the BossHealthBar class
	ConstructorHelpers::FClassFinder<UUserWidget> HealthBarClassFinder(TEXT("/Game/GameObjects/Widgets/BossHealthBar"));
	if (!HealthBarClassFinder.Succeeded()) {
		UE_LOG(LogTemp, Error, TEXT("Could not find BossHealthBar widget class"));
		return;
	}
	HealthBarClass = HealthBarClassFinder.Class;
}

// Called when the game starts or when spawned
void ABossMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	defeated = false;
	attacking = false;
	health = 100;

	// Create health bar
	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	myHealthBar = CreateWidget<UUserWidget>(controller, HealthBarClass);
	if (myHealthBar) {
		// set visible
		myHealthBar->AddToViewport();
		// Set health bar's Boss property to this object
		FName bossPropertyName(TEXT("Boss"));
		UObjectProperty* bossProperty = FindField<UObjectProperty>(HealthBarClass, bossPropertyName);
		if (bossProperty) {
			bossProperty->SetPropertyValue_InContainer(myHealthBar, this);
		}
	}
}

// Called every frame
void ABossMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ABossMonsterBase::isAttacking() { return attacking; }

bool ABossMonsterBase::isDefeated() { return defeated; }

float ABossMonsterBase::getPlayerDamage() { return 0; }

void ABossMonsterBase::handleDefeat() {
	myHealthBar->RemoveFromParent();
};

void ABossMonsterBase::handleDamage(AActor* damagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) {
	if (!defeated) {
		health -= Damage;
		if (health <= 0) {
			health = 0;
			defeated = true;
			handleDefeat();
		}
	}
}
