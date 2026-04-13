// Fill out your copyright notice in the Description page of Project Settings.


#include "RogueInteractionComponent.h"

#include "RogueGameTypes.h"
#include "Core/RogueInteractionInterface.h"
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

	ECollisionChannel TraceChannel = COLLISION_INTERACTION;
	GetWorld()->OverlapMultiByChannel(Overlaps, Center, FQuat::Identity, TraceChannel, SphereShape);
	
	DrawDebugSphere(GetWorld(), Center, InteractionRadius, 32, FColor::White);
	
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
			SelectedActor = Overlap.GetActor();
		}
	}
	if (SelectedActor != nullptr)
	{
		DrawDebugBox(GetWorld(), SelectedActor->GetActorLocation(), FVector(60.f), FColor::Green);
	}
}

void URogueInteractionComponent::Interact()
{
	IRogueInteractionInterface* InteractionInterface = Cast<IRogueInteractionInterface>(SelectedActor);
	if (InteractionInterface != nullptr)
	{
		InteractionInterface->Interact();
	}
}

