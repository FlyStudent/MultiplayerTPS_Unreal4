// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeathRocket_ProtoCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "HealthComponent.h"
#include "SprintComponent.h"
#include "UltimeLoaderComponent.h"
#include "Rocket.h"
#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
// ADeathRocket_ProtoCharacter

ADeathRocket_ProtoCharacter::ADeathRocket_ProtoCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	// Configure character movement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.5f;
	GetCharacterMovement()->MaxAcceleration = 100000.f;
	GetCharacterMovement()->JumpZVelocity = 1300.f;
	GetCharacterMovement()->AirControl = 0.3f;
	GetCharacterMovement()->FallingLateralFriction = 0.5f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	healthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	sprintComp = CreateDefaultSubobject<USprintComponent>(TEXT("SprintComponent"));
	ultimeComp = CreateDefaultSubobject<UUltimeLoaderComponent>(TEXT("UltimeComponent"));
	// Create Rocket Luncher
	RocketLauncher = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RocketLuncher"));
	RocketLauncher->SetupAttachment(GetMesh(), "RightArm");

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// Setting values
	curAmmo = ammoMax;
}

ADeathRocket_ProtoCharacter::~ADeathRocket_ProtoCharacter()
{
	delete fireTimer;
	delete reloadTimer;
	delete gamepadUltimeTimer;
}

void ADeathRocket_ProtoCharacter::BeginPlay()
{
	Super::BeginPlay();

	fireTimer = new Timer(GetWorld(), fireRate);
	reloadTimer = new Timer(GetWorld(), reloadTime);
	gamepadUltimeTimer = new Timer(GetWorld(), gamepadUltiInputTime);

	// Setting values
	fov = FollowCamera->FieldOfView;
	curFov = fov;

	if (healthComp)
	{
		healthComp->OnKill.AddDynamic(this, &ADeathRocket_ProtoCharacter::OnDeath);
	}

	if (sprintComp)
	{
		sprintComp->OnDash.AddDynamic(this, &ADeathRocket_ProtoCharacter::Dash);
		sprintComp->OnRun.AddDynamic(this, &ADeathRocket_ProtoCharacter::Sprint);
		sprintComp->OnEndRun.AddDynamic(this, &ADeathRocket_ProtoCharacter::EndSprint);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADeathRocket_ProtoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("ChangeCamSide", IE_Pressed, this, &ADeathRocket_ProtoCharacter::changeCamSide);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ADeathRocket_ProtoCharacter::Fire);
	PlayerInputComponent->BindAction("Ultime", IE_Pressed, ultimeComp, &UUltimeLoaderComponent::Use);
	PlayerInputComponent->BindAction("Gamepad Ultime", IE_Pressed, this, &ADeathRocket_ProtoCharacter::GamepadUltimeInput);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ADeathRocket_ProtoCharacter::Aim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ADeathRocket_ProtoCharacter::StopAiming);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ADeathRocket_ProtoCharacter::Reload);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, sprintComp, &USprintComponent::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, sprintComp, &USprintComponent::EndSprint);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADeathRocket_ProtoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADeathRocket_ProtoCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADeathRocket_ProtoCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADeathRocket_ProtoCharacter::LookUpAtRate);
}

void ADeathRocket_ProtoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool isMoving = !GetCharacterMovement()->Velocity.Equals(FVector::ZeroVector);
	if (isMoving || curFov == ads)
	{
		FRotator rotation = GetControlRotation();
		SetActorRotation(FRotator(0.f, rotation.Yaw, 0.f));
	}

	// Sprint
	if (GetCharacterMovement()->IsFalling())
		GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity.GetClampedToMaxSize(inAirMaxSpeed);

	sprintComp->TickStamina(DeltaTime, isMoving);

	// Camera
	{
		FVector actualCamLoc = FollowCamera->GetRelativeLocation();
		FVector newSide = FMath::VInterpTo(actualCamLoc, { actualCamLoc.X, cameraYOffset * shoulder, actualCamLoc.Z }, DeltaTime, 10.f);
		FollowCamera->SetRelativeLocation(newSide);

		FollowCamera->FieldOfView = FMath::Lerp<float>(FollowCamera->FieldOfView, curFov, DeltaTime * 10.f);
	}

	// Reload
	if (reloading && isMoving)
		reloadTimer->Pause();
	else if (reloading)
		reloadTimer->Resume();

	UpdateTimersProgress();
	BroadcastUIUpdate();
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString("Move"));
}

void ADeathRocket_ProtoCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADeathRocket_ProtoCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ADeathRocket_ProtoCharacter::MoveForward(float Value)
{
	if (stopMovement && Value == 0.0f)
		stopMovement = false;

	if ((Controller != nullptr) && (Value != 0.0f) && !stopMovement)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ADeathRocket_ProtoCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !stopMovement)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ADeathRocket_ProtoCharacter::Fire()
{
	if (firing || curAmmo <= 0)
		return;

	if (reloading)
	{
		reloading = false;
		reloadTimer->Clear();
	}

	FRotator rotation = GetControlRotation();
	SetActorRotation(FRotator(0.f, rotation.Yaw, 0.f));

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FVector camLoc = FollowCamera->GetRelativeLocation();
	FVector location = RocketLauncher->GetSocketLocation(FName("RocketCanon"));
	GetWorld()->SpawnActor<ARocket>(rocketClass, location, GetControlRotation(), spawnParams);

	firing = true;
	--curAmmo;
	OnAmmoUpdate.Broadcast();
	if (curAmmo == 0)
	{
		EndFire();
		Reload();
		return;
	}
	fireTimer->Reset(this, &ADeathRocket_ProtoCharacter::EndFire);
}

void ADeathRocket_ProtoCharacter::EndFire()
{
	firing = false;
}

void ADeathRocket_ProtoCharacter::Reload()
{
	// Check if player is jumping
	if (curAmmo == ammoMax || reloading)
		return;

	reloading = stopMovement = true;
	StopAiming();

	reloadTimer->Reset(this, &ADeathRocket_ProtoCharacter::EndReload);
}

void ADeathRocket_ProtoCharacter::EndReload()
{
	curAmmo = ammoMax;
	OnAmmoUpdate.Broadcast();

	reloading = false;
}

void ADeathRocket_ProtoCharacter::GamepadUltimeInput()
{
	if (gamepadUltimeUse)
	{
		ultimeComp->Use();
	}
	else
	{
		gamepadUltimeUse = true;
		gamepadUltimeTimer->Reset(this, &ADeathRocket_ProtoCharacter::CancelGamepadUltimeInput);
	}
}

void ADeathRocket_ProtoCharacter::CancelGamepadUltimeInput()
{
	gamepadUltimeUse = false;
}

void ADeathRocket_ProtoCharacter::UpdateTimersProgress()
{
	float ratio = fireTimer->GetProgess();
	fireProgress = ratio < 0.f ? 0.f : ratio;

	ratio = reloadTimer->GetProgess();
	reloadProgress = ratio < 0.f ? 0.f : ratio;
}

void ADeathRocket_ProtoCharacter::BroadcastUIUpdate()
{
	if (fireProgress > 0.f)
	{
		OnFireCDUpdate.Broadcast(true);
		lastFireUpdate = true;
	}
	else if (lastFireUpdate)
	{
		OnFireCDUpdate.Broadcast(false);
		lastFireUpdate = false;
	}

	if (reloadProgress > 0.f)
	{
		OnReloadCDUpdate.Broadcast(true);
		lastReloadUpdate = true;
	}
	else if (lastReloadUpdate)
	{
		OnReloadCDUpdate.Broadcast(false);
		lastReloadUpdate = false;
	}

	if (sprintComp->GetRatio() < 1.f)
	{
		OnStaminaUpdate.Broadcast(true);
		lastStaminaUpdate = true;
	}
	else if (lastStaminaUpdate)
	{
		OnStaminaUpdate.Broadcast(false);
		lastStaminaUpdate = false;
	}

	if (ultimeComp->GetRatio() != lastUltimeRatio)
	{
		OnUltimeUpdate.Broadcast(true);
		lastUltimeRatio = ultimeComp->GetRatio();
	}
}

void ADeathRocket_ProtoCharacter::changeCamSide()
{
	shoulder *= -1;
}

void ADeathRocket_ProtoCharacter::Aim()
{
	if (reloading && curAmmo <= 0)
		return;
	sprintComp->EndSprint();

	curFov = ads;
}

void ADeathRocket_ProtoCharacter::StopAiming()
{
	curFov = fov;
}

void ADeathRocket_ProtoCharacter::TakeDamage()
{
	healthComp->Hurt(1);
}

void ADeathRocket_ProtoCharacter::OnDeath()
{

}

void ADeathRocket_ProtoCharacter::Sprint()
{
	StopAiming();
	curFov = runFov;

	GetCharacterMovement()->MaxWalkSpeed = sprintComp->GetSpeed();
}

void ADeathRocket_ProtoCharacter::Dash()
{
	Sprint();

	GetCharacterMovement()->MaxWalkSpeed = sprintComp->GetSpeed();
}

void ADeathRocket_ProtoCharacter::EndSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = sprintComp->GetSpeed();
	curFov = fov;
}
