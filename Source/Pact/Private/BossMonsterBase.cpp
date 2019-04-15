// Fill out your copyright notice in the Description page of Project Settings.

#include "BossMonsterBase.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "PactGameInstanceBase.h"
#include <random>
#include <map>

// Sets default values
ABossMonsterBase::ABossMonsterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OnTakeAnyDamage.AddDynamic(this, &ABossMonsterBase::handleDamage);
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
		soulConsumed = gameInstance->getBossSoulConsumed(this);
		if (defeated) {
			OnSpawnedDead();
		}
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
	}
}

bool ABossMonsterBase::isAttacking() { return attacking; }

bool ABossMonsterBase::isDefeated() { return defeated; }

float ABossMonsterBase::getPlayerDamage() {
	return defeated || !bodyDamagesPlayer ? 0 : currentAttackDamage;
}

void ABossMonsterBase::handleDefeat() {
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
	if (!defeated && inBattle) {
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

void ABossMonsterBase::lookAtPlayer(float rotationOffset) {
	lookAtLocation(getPlayerLocation(), rotationOffset);
}

void ABossMonsterBase::lookAtLocation(FVector location, float rotationOffset) {
	FVector myLocation = GetActorLocation();
	FRotator currentRotation = GetActorRotation().GetNormalized();
	FRotator lookRotation = UKismetMathLibrary::FindLookAtRotation(myLocation, location).GetNormalized();
	lookRotation.Pitch = currentRotation.Pitch;
	lookRotation.Roll = currentRotation.Roll;
	lookRotation.Yaw += rotationOffset;
	float turnAngle = currentRotation.Yaw - lookRotation.Yaw;
	if (turnAngle > 180) {
		turnAngle -= 360;
	}
	else if (turnAngle < -180) {
		turnAngle += 360;
	}
	if (UKismetMathLibrary::Abs(turnAngle) > maxTurnAngle) {
		int sign = turnAngle > 0 ? -1 : 1;
		lookRotation.Yaw = currentRotation.Yaw + (sign * maxTurnAngle);
	}
	SetActorRotation(lookRotation);
}

bool ABossMonsterBase::playerIsBehind(float rotationOffset) {
	FVector myLocation = GetActorLocation();
	FVector playerLocation = getPlayerLocation();
	FRotator currentRotation = GetActorRotation().GetNormalized();
	FRotator lookRotation = UKismetMathLibrary::FindLookAtRotation(myLocation, playerLocation).GetNormalized();
	float zdiff = lookRotation.Yaw - (currentRotation.Yaw + rotationOffset);
	while (zdiff < -180) {
		zdiff += 360;
	}
	while (zdiff > 180) {
		zdiff -= 360;
	}
	return UKismetMathLibrary::Abs(zdiff) >= 90;
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
	int smoothing = 50;

	std::random_device rd;
	std::mt19937 gen(rd());

	std::vector<int> weights;

	for (; i < length; i++) {
		weights.push_back(HitCount[i] + smoothing);
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

void ABossMonsterBase::registerAttackHitPlayer(float damage) {
	if (inBattle) {
		damage = damage < 0 ? currentAttackDamage : damage;
		if (currentAttack >= 0 && currentAttack < attackHits.Num()) {
			attackHits[currentAttack] += (int32)damage;
		}
		AfterHitPlayer(damage);
	}
}

int ABossMonsterBase::bestAttackFromRangesBossAi(TArray<int> attack_ranges, int prev_attack_one, int prev_attack_two) {

	int length = attack_ranges.Num();
	double distance = FVector::Dist(getPlayerLocation(), GetActorLocation());
	std::mt19937 generator;
	std::vector<double> weights;

	std::random_device rd;
	std::mt19937 gen(rd());

	for (int i = 0; i < length; i++) {
		double diff = (attack_ranges[i] - distance);
		if (diff > 0) { // check that the attack is in range
			if ((i == prev_attack_one) && (i == prev_attack_two)) { // dont want to use an attack more than twice in a row
				weights.push_back(0);
			} else {
				// weight the choice such that the closer the player is to an attack boundary, the more likely that attack will be chosen
				weights.push_back(pow((1/diff)*100, 2));
			}
		} else {
			weights.push_back(0);
		}
	}

	std::discrete_distribution<> d(weights.begin(), weights.end());
	int ret = d(gen);

	// check if ret = 0
	if (ret==0) {
		for (int i = 0; i < length; i++) {
			// if any of the weights are not 0 then return ret, a move was chosen
			if (weights[i]!=0) {
				return ret;
			}
		}
		// if all of the weights are 0 then return length, the boss should not make an attack right now
		return length;
	}

	return ret;

	/*
	if(should_attack) {
		return -1;
	}
	else {
		int best_attack = 0;
		double min_distance = MAX_dbl;
		for (int i = 0; i < length; i++) {
			double distance_diff = (attack_ranges[i] - distance);
			if ((distance_diff > 0) && (distance_diff < min_distance)) {

				weights.push_back();


			}
		}
		std::discrete_distribution<> d(weights.begin(), weights.end());
		int should_move = one_in_two(generator);
		if(!best_attack && should_move) {
			return -1;
		} else {
			return best_attack;
		}
	}
	*/
}

FVector ABossMonsterBase::chooseDestination(FVector previous_move, FVector velocity) {
	FVector new_move = previous_move;
	std::mt19937 generator;

	return new_move;
}

