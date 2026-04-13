// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RogueItemChest.generated.h"

UCLASS()
class COURSE_API ARogueItemChest : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARogueItemChest();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> BaseMeshComp;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> LidMeshComp;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	float AnimationSpeed = 50.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	float AnimationTargetPitch = 120.f;
	
	float CurrentAnimationPitch = 0.0f;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
