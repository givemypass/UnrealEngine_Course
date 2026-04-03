// Fill out your copyright notice in the Description page of Project Settings.


#include "RogueCharacter.h"

#include "EnhancedInputComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Projectiles/RogueProjectileMagic.h"

// Sets default values
ARogueCharacter::ARogueCharacter()
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
void ARogueCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARogueCharacter::Move(const FInputActionValue& InValue)
{
	const auto InputValue = InValue.Get<FVector2D>();

	// const auto MoveDirection = FVector(InputValue.X, InputValue.Y, 0.f);
	
	auto ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	AddMovementInput(ControlRot.Vector(), InputValue.X);

	const auto RightVector = ControlRot.RotateVector(FVector::RightVector);
	
	AddMovementInput(RightVector, InputValue.Y);
}

void ARogueCharacter::Look(const FInputActionInstance& InValue)
{
	const auto InputValue = InValue.GetValue().Get<FVector2D>();
	AddControllerPitchInput(InputValue.Y);
	AddControllerYawInput(InputValue.X);
}

void ARogueCharacter::AttackTimerEllapsed()
{
	FVector Location = GetMesh()->GetSocketLocation(MuzzleLocation);
	FRotator Rotation = GetControlRotation();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ARogueProjectileMagic* Projectile = GetWorld()->SpawnActor<ARogueProjectileMagic>(ProjectileClass, Location, Rotation, SpawnParameters);
	MoveIgnoreActorAdd(Projectile);
}

void ARogueCharacter::PrimaryAttack()
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(CastingEffect, GetMesh(), MuzzleLocation, FVector::ZeroVector,
	                                             FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	UGameplayStatics::PlaySound2D(this, CastSound);
	
	PlayAnimMontage(AttackAnimMontage);
	FTimerHandle TimerHandle;
	const float AttackDelay = 0.2f;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ARogueCharacter::AttackTimerEllapsed, AttackDelay);
}

// Called every frame
void ARogueCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARogueCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	EnhancedInput->BindAction(Input_Move, ETriggerEvent::Triggered, this, &ARogueCharacter::Move);
	EnhancedInput->BindAction(Input_Look, ETriggerEvent::Triggered, this, &ARogueCharacter::Look);
	EnhancedInput->BindAction(Input_PrimaryAttack, ETriggerEvent::Triggered, this, &ARogueCharacter::PrimaryAttack);
}

