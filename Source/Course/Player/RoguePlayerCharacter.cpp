// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguePlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Projectiles/RogueProjectileMagic.h"

// Sets default values
ARoguePlayerCharacter::ARoguePlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);	
	SpringArmComponent->bUsePawnControlRotation = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	
	MuzzleLocation = "Muzzle_01";
}

// Called when the game starts or when spawned
void ARoguePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARoguePlayerCharacter::Move(const FInputActionValue& InValue)
{
	const auto InputValue = InValue.Get<FVector2D>();

	// const auto MoveDirection = FVector(InputValue.X, InputValue.Y, 0.f);
	
	auto ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	AddMovementInput(ControlRot.Vector(), InputValue.X);

	const auto RightVector = ControlRot.RotateVector(FVector::RightVector);
	
	AddMovementInput(RightVector, InputValue.Y);
}

void ARoguePlayerCharacter::Look(const FInputActionInstance& InValue)
{
	const auto InputValue = InValue.GetValue().Get<FVector2D>();
	AddControllerPitchInput(InputValue.Y);
	AddControllerYawInput(InputValue.X);
}

void ARoguePlayerCharacter::AttackTimerEllapsed()
{
	FVector Location = GetMesh()->GetSocketLocation(MuzzleLocation);
	FRotator Rotation = GetControlRotation();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ARogueProjectileMagic* Projectile = GetWorld()->SpawnActor<ARogueProjectileMagic>(ProjectileClass, Location, Rotation, SpawnParameters);
	MoveIgnoreActorAdd(Projectile);
}

void ARoguePlayerCharacter::PrimaryAttack()
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(CastingEffect, GetMesh(), MuzzleLocation, FVector::ZeroVector,
	                                             FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	UGameplayStatics::PlaySound2D(this, CastSound);
	
	PlayAnimMontage(AttackAnimMontage);
	FTimerHandle TimerHandle;
	const float AttackDelay = 0.2f;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ARoguePlayerCharacter::AttackTimerEllapsed, AttackDelay);
}

// Called every frame
void ARoguePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARoguePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	EnhancedInput->BindAction(Input_Move, ETriggerEvent::Triggered, this, &ARoguePlayerCharacter::Move);
	EnhancedInput->BindAction(Input_Look, ETriggerEvent::Triggered, this, &ARoguePlayerCharacter::Look);
	EnhancedInput->BindAction(Input_PrimaryAttack, ETriggerEvent::Triggered, this, &ARoguePlayerCharacter::PrimaryAttack);
	EnhancedInput->BindAction(Input_Jump, ETriggerEvent::Triggered, this, &ARoguePlayerCharacter::Jump);
}

