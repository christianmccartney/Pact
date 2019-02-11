// Fill out your copyright notice in the Description page of Project Settings.

#include "BossMonsterBase.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


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

float ABossMonsterBase::getPlayerDamage() { return 0; }

void ABossMonsterBase::handleDefeat() {
	myHealthBar->RemoveFromParent();
}

void ABossMonsterBase::handleDamage(AActor* damagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) {
	if (!defeated) {
		health -= Damage;
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
