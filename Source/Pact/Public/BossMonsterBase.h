// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossMonsterBase.generated.h"

UCLASS(meta = (Blueprintable))
class PACT_API ABossMonsterBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossMonsterBase();

protected:
	TSubclassOf<class AActor> PlayerClass;

	UPROPERTY(BlueprintReadWrite)
	float maxTurnAngle = 3;

	UPROPERTY(BlueprintReadWrite)
	AActor* player;

	UPROPERTY(BlueprintReadWrite)
	bool inBattle;

	UPROPERTY(BlueprintReadWrite)
	bool attacking;

	UPROPERTY(BlueprintReadWrite)
	bool defeated;

	UPROPERTY(BlueprintReadWrite)
	float health;

	UPROPERTY(BlueprintReadWrite)
	int currentAttack;

	UPROPERTY(BlueprintReadWrite)
	TArray<float> attackStrengths;

	UPROPERTY(BlueprintReadWrite)
	TArray<int> attackHits;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Handler for OnTakeAnyDamage event
	// Subtracts damage from health and calls handleDefeat when health hits 0
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void handleDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	// Event triggered in handleDamage (for implementation in blueprints)
	// override in children for extra damage effects, sounds, animations etc.
	UFUNCTION(BlueprintImplementableEvent)
	void AfterDamage();

	// Called by handleDamage when health reaches 0 (override in children)
	virtual void handleDefeat();

	// Event triggered when health reaches 0 (implement in blueprints)
	// Called after handleDefeat
	UFUNCTION(BlueprintImplementableEvent)
	void OnDefeat();

	// Calculate damage to player on hit (override in children)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual float getPlayerDamage();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void lookAtPlayer();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual FVector getPlayerLocation();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void startBattle();

	// Getters and setters
	UFUNCTION(BlueprintCallable, Category = "Boss")
	virtual bool isDefeated();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	virtual bool isAttacking();

	// pass number of animations, return random index between 0 to number of animations minus 1
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual int simpleBossAI(int AnimationCount);

	// pass array counting number of times each attack has hit character, return index of attack that should be used next
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual int bestAttackBossAI(TArray<int> HitCount);

	// Stores hit information for bestAttackBossAI
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void registerAttackHitPlayer();
};
