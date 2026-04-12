// Fill out your copyright notice in the Description page of Project Settings.


#include "RogueInteractionComponent.h"

#include "Engine/OverlapResult.h"


// Sets default values for this component's properties
URogueInteractionComponent::URogueInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called every frame
void URogueInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	const APlayerController* PC = CastChecked<APlayerController>(GetOwner());
	const FVector Center = PC->GetPawn()->GetActorLocation();
	
	
	TArray<FOverlapResult> Overlaps;
	FCollisionShape SphereShape;
	SphereShape.SetSphere(InteractionRadius);
	
	GetWorld()->OverlapMultiByChannel(Overlaps, Center, FQuat::Identity, ECC_Visibility, SphereShape);
	
	DrawDebugSphere(GetWorld(), Center, InteractionRadius, 32, FColor::White);
	
	AActor* BestActor = nullptr;
	float BestDotProduct = -0.1;
	
	for (auto Overlap : Overlaps)
	{
		FVector OverlapCenter = Overlap.GetActor()->GetActorLocation();
		DrawDebugBox(GetWorld(), OverlapCenter, FVector(50.f), FColor::Red);
		const auto DotProduct = FVector::DotProduct((OverlapCenter - Center).GetSafeNormal(), PC->GetControlRotation().Vector());	
		FString DebugText = FString::Printf(TEXT("Dot = %f"), DotProduct);
		DrawDebugString(GetWorld(), OverlapCenter, DebugText, nullptr, FColor::White, 0.f, true);
		if (DotProduct > BestDotProduct)
		{
			BestDotProduct = DotProduct;
			BestActor = Overlap.GetActor();
		}
	}
	if (BestActor != nullptr)
	{
		DrawDebugBox(GetWorld(), BestActor->GetActorLocation(), FVector(60.f), FColor::Green);
	}
}

