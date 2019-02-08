// Fill out your copyright notice in the Description page of Project Settings.

#include "BossMonsterBase.h"
#include <iostream>

// Sets default values
ABossMonsterBase::ABossMonsterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABossMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	std::cout << "Hello\n";
}

// Called every frame
void ABossMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABossMonsterBase::isAttacking() { return attacking; }

float ABossMonsterBase::getPlayerDamage() { return 0; }
