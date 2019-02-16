// Fill out your copyright notice in the Description page of Project Settings.

#include "BossMonsterBase.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PactGameInstanceBase.h"
#include <random>
#include <map>

// Sets default values
ABossMonsterBase::ABossMonsterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OnTakeAnyDamage.AddDynamic(this, &ABossMonsterBase::handleDamage);
	// Find the BossHealthBar class
	ConstructorHelpers::FClassFinder<UUserWidget> HealthBarClassFinder(TEXT("/Game/GameObjects/Widgets/BossHealthBar"));
	HealthBarClass = HealthBarClassFinder.Class;
	// Find the ThirdPersonPlayer class
	ConstructorHelpers::FClassFinder<AActor> PlayerClassFinder(TEXT("/Game/GameObjects/Actors/ThirdPersonCharacter"));
	PlayerClass = PlayerClassFinder.Class;
}

// Called when the game starts or when spawned
void ABossMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	defeated = false;
	attacking = false;
	inBattle = false;
	health = 100;

	// Create health bar
	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	myHealthBar = CreateWidget<UUserWidget>(controller, HealthBarClass);
	if (myHealthBar) {
		// Set health bar's Boss property to this object
		FName bossPropertyName(TEXT("Boss"));
		UObjectProperty* bossProperty = FindField<UObjectProperty>(HealthBarClass, bossPropertyName);
		if (bossProperty) {
			bossProperty->SetPropertyValue_InContainer(myHealthBar, this);
		}
	}

	// Find a reference to the player object
	TArray<AActor*> playerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerClass, playerActors);
	if (playerActors.Num() > 0) {
		player = playerActors.Last();
	} else {
		UE_LOG(LogTemp, Error, TEXT("Could not find player object"))
	}

	// Get defeated state from GameInstance
	UPactGameInstanceBase* gameInstance = Cast<UPactGameInstanceBase>(GetWorld()->GetGameInstance());
	if (gameInstance) {
		defeated = gameInstance->getBossDefeated(this);
	} else {
		UE_LOG(LogTemp, Error, TEXT("Failed to cast GameInstance to PactGameInstanceBase"));
	}
}

// Called every frame
void ABossMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABossMonsterBase::startBattle() {
	if (!defeated) {
		inBattle = true;
		// set health bar visible
		myHealthBar->AddToViewport();
	}
}

bool ABossMonsterBase::isAttacking() { return attacking; }

bool ABossMonsterBase::isDefeated() { return defeated; }

float ABossMonsterBase::getPlayerDamage() { return attackStrengths[currentAttack]; }

void ABossMonsterBase::handleDefeat() {
	myHealthBar->RemoveFromParent();
	// set defeated state in game instance
	UPactGameInstanceBase* gameInstance = Cast<UPactGameInstanceBase>(GetWorld()->GetGameInstance());
	if (gameInstance) {
		gameInstance->setBossDefeated(this, true);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to cast GameInstance to PactGameInstanceBase"));
	}
}

void ABossMonsterBase::handleDamage(AActor* damagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) {
	if (!defeated) {
		health -= Damage;
		AfterDamage();
		if (health <= 0) {
			health = 0;
			defeated = true;
			inBattle = false;
			handleDefeat();
			OnDefeat();
		}
	}
}

void ABossMonsterBase::lookAtPlayer() {
	FVector myLocation = GetActorLocation();
	FVector playerLocation = getPlayerLocation();
	FRotator currentRotation = GetActorRotation().GetNormalized();
	FRotator lookRotation = UKismetMathLibrary::FindLookAtRotation(myLocation, playerLocation).GetNormalized();
	lookRotation.Pitch = 0;
	lookRotation.Roll = 0;
	float turnAngle = UKismetMathLibrary::Abs(currentRotation.Yaw - lookRotation.Yaw);
	if ((currentRotation.Yaw > 0) != (lookRotation.Yaw > 0)) {
		turnAngle = UKismetMathLibrary::Abs(turnAngle - 360);
	}
	if (turnAngle > maxTurnAngle) {
		int sign = (currentRotation.Yaw - lookRotation.Yaw) > 0 ? -1 : 1;
		lookRotation.Yaw = currentRotation.Yaw + (sign * maxTurnAngle);
	}
	SetActorRotation(lookRotation);
}

FVector ABossMonsterBase::getPlayerLocation() {
	return player->GetActorLocation();
}

int ABossMonsterBase::simpleBossAI(int AnimationCount) {
	return rand() % AnimationCount;
}

int ABossMonsterBase::bestAttackBossAI(TArray<int> HitCount) {
	int length = HitCount.Num();
	//double* scalars = new double[length];
	//int largest = -1;
	int i = 0;

	std::random_device rd;
	std::mt19937 gen(rd());

	std::vector<int> weights;

	for (; i < length; i++) {
		weights.push_back(HitCount[i]);
	}

	std::discrete_distribution<> d(weights.begin(), weights.end());

	return d(gen);

	/*
	for (int i = 0; i < length; i++) {
		if (HitCount[i] > largest) {
			largest = HitCount[i];
			index = i;
		}
	}

	if (largest == 0) {
		index = -1;
	}

	for (int i = 0; i < length; i++) {
		if (i == index) {
			scalars[i] = (1.2 + ((rand() % 100) / 100)) * HitCount[i];
		} else {
			scalars[i] = (1.0 + ((rand() % 100) / 100)) * HitCount[i];
		}
	}

	largest = -1;
	for (int i = 0; i < length; i++) {
		if (scalars[i] > largest) {
			largest = scalars[i];
			index = i;
		}
	}

	if (largest == 1) {
		index = rand() % length;
	}

	*/

	//delete[] scalars;
	//return largest;
}

void ABossMonsterBase::registerAttackHitPlayer() {
	// Later use damage amounts instead of counts
	attackHits[currentAttack]++;
}