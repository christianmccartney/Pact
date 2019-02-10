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
	TSubclassOf<class UUserWidget> HealthBarClass;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* myHealthBar;

	UPROPERTY(BlueprintReadWrite)
	bool attacking;

	UPROPERTY(BlueprintReadWrite)
	bool defeated;

	UPROPERTY(BlueprintReadWrite)
	float health;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Handler for OnTakeAnyDamage event
	UFUNCTION()
	virtual void handleDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	// Called by handleDamage when health reaches 0 (override in children)
	UFUNCTION(BlueprintCallable, Category = "Boss")
	virtual void handleDefeat();

	// Calculate damage to player on hit (override in children)
	UFUNCTION(BlueprintCallable, Category = "Boss")
	virtual float getPlayerDamage();

	// Getters and setters
	UFUNCTION(BlueprintCallable, Category = "Boss")
	virtual bool isDefeated();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	virtual bool isAttacking();
};
