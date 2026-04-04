// Fill out your copyright notice in the Description page of Project Settings.


#include "BarrelActor.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
ABarrelActor::ABarrelActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComponent);
	
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(StaticMeshComp);
}

// Called when the game starts or when spawned
void ABarrelActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABarrelActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABarrelActor::Explode()
{
	for (const auto Element : LeakNiagaraComponents)
	{
		if (!IsValid(Element))
		{
			continue;
		}
		Element->Deactivate();
		Element->DestroyComponent();
	}
	LeakNiagaraComponents.Empty();
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionEffect, GetActorLocation(), GetActorRotation());
	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation(), GetActorRotation());
	RadialForceComp->FireImpulse();
}

void ABarrelActor::StartLeaking(struct FDamageEvent const& DamageEvent)
{
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		auto WorldHitLocation = PointDamageEvent->HitInfo.ImpactPoint;
		FVector LocalHitLocation = StaticMeshComp->GetComponentTransform().InverseTransformPosition(WorldHitLocation);
		auto NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(LeakEffect, StaticMeshComp, NAME_None, LocalHitLocation,
		                                             FRotator::ZeroRotator, EAttachLocation::Type::KeepRelativeOffset,
		                                             false);
		LeakNiagaraComponents.Add(NiagaraComp);
		auto AudioComp = UGameplayStatics::SpawnSoundAttached(LeakSound, NiagaraComp, NAME_None, FVector::ZeroVector,
		                                     FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true, 1,1,0,nullptr,nullptr, true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Not implemented"));
	}
}

float ABarrelActor::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                               class AController* EventInstigator, AActor* DamageCauser)
{
	// if (!IsExploded)
	{
		StartLeaking(DamageEvent);
		if (!GetWorldTimerManager().IsTimerActive(ExplosionTimerHandle))
		{
			GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &ABarrelActor::Explode, 3);
		}
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

