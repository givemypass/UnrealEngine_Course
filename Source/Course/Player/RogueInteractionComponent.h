// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RogueInteractionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COURSE_API URogueInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	float InteractionRadius = 800;

public:
	// Sets default values for this component's properties
	URogueInteractionComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
