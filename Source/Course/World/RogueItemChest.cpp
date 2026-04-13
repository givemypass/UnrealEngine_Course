// Fill out your copyright notice in the Description page of Project Settings.


#include "RogueItemChest.h"


// Sets default values
ARogueItemChest::ARogueItemChest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	BaseMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMeshComp"));
	RootComponent = BaseMeshComp;
	
	LidMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("LidMeshComp");
	LidMeshComp->SetupAttachment(BaseMeshComp);
}

// Called when the game starts or when spawned
void ARogueItemChest::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(true);
}

// Called every frame
void ARogueItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CurrentAnimationPitch = FMath::FInterpConstantTo(CurrentAnimationPitch, AnimationTargetPitch, DeltaTime, AnimationSpeed);
	LidMeshComp->SetRelativeRotation(FRotator(CurrentAnimationPitch, 0.f, 0.f));
	
	if (FMath::IsNearlyEqual(CurrentAnimationPitch, AnimationTargetPitch))
	{
		SetActorTickEnabled(false);
	}
}

