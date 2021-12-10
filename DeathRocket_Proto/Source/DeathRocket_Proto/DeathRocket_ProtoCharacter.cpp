// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeathRocket_ProtoCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "UltimeLoaderComponent.h"
#include "SpawnManager.h"
#include "HealthComponent.h"
#include "SprintComponent.h"
#include "CaptureComponent.h"
#include "PauseComponent.h"
#include "AimBotComponent.h"

#include "Rocket.h"
#include "Ultime.h"
#include "Timer.h"
#include "ScoreManager.h"
#include "KillFeedManager.h"

#define MAX_ACCELERATION 500000.f

//////////////////////////////////////////////////////////////////////////
// ADeathRocket_ProtoCharacter

ADeathRocket_ProtoCharacter::ADeathRocket_ProtoCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	// Configure character movement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.5f;
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
	GetMesh()->SetupAttachment(RootComponent);

	healthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	sprintComp = CreateDefaultSubobject<USprintComponent>(TEXT("SprintComponent"));
	ultimeComp = CreateDefaultSubobject<UUltimeLoaderComponent>(TEXT("UltimeComponent"));
	captureComp = CreateDefaultSubobject<UCaptureComponent>(TEXT("AreaCaptureComponent"));
	pauseComp = CreateDefaultSubobject<UPauseComponent>(TEXT("PauseComponent"));
	aimBotComp = CreateDefaultSubobject<UAimBotComponent>(TEXT("AimBotComponent"));
	// Create Rocket Luncher
	RocketLauncher = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RocketLuncher"));
	RocketLauncher->SetupAttachment(GetMesh(), "RightArm");

	ActorsToIgnore.Add(this);

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
	delete secondTripleBulletTimer;
	delete thirdTripleBulletTimer;
	delete lastDamagerTimer;
	delete fastStreakTimer;
}

void ADeathRocket_ProtoCharacter::BeginPlay()
{
	Super::BeginPlay();

	fireTimer = new Timer(GetWorld(), fireRate);
	reloadTimer = new Timer(GetWorld(), reloadTime);
	gamepadUltimeTimer = new Timer(GetWorld(), gamepadUltiInputTime);
	secondTripleBulletTimer = new Timer(GetWorld(), TripleBulletTime);
	thirdTripleBulletTimer = new Timer(GetWorld(), TripleBulletTime * 2);
	lastDamagerTimer = new Timer(GetWorld(), keepLastDamagerTime);
	fastStreakTimer = new Timer(GetWorld(), fastStreakTime);

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	defaultMaxAcceleration = GetCharacterMovement()->MaxAcceleration;

	// Setting values
	fov = FollowCamera->FieldOfView;
	curFov = fov;
	lastDamager = this;

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

	if (captureComp && ultimeComp)
	{
		captureComp->teamColor = team;
		if (ultimeComp)
			captureComp->OnCaptureCompleted.AddDynamic(ultimeComp, &UUltimeLoaderComponent::IncreaseByCapture);
	}

	if (ultimeComp)
	{
		ultimeComp->OnUltimeUsed.AddDynamic(this, &ADeathRocket_ProtoCharacter::CreateDefaultUltime);
		CreateDefaultUltime();
	}

	if (captureComp)
	{
		captureComp->OnCaptureCompleted.AddDynamic(this, &ADeathRocket_ProtoCharacter::ZoneCapturedFeed);
	}
	
	spawnManager = Cast<ASpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnManager::StaticClass()));
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
	FInputActionBinding& aimToggle = PlayerInputComponent->BindAction("Aim", IE_Released, this, &ADeathRocket_ProtoCharacter::StopAiming);
	aimToggle.bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ADeathRocket_ProtoCharacter::Reload);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, sprintComp, &USprintComponent::Sprint);
	FInputActionBinding& sprintToggle = PlayerInputComponent->BindAction("Sprint", IE_Released, sprintComp, &USprintComponent::EndSprint);
	sprintToggle.bExecuteWhenPaused = true;

	PlayerInputComponent->BindAction("Scoreboard", IE_Pressed, this, &ADeathRocket_ProtoCharacter::Score);
	FInputActionBinding& scoreToggle = PlayerInputComponent->BindAction("Scoreboard", IE_Released, this, &ADeathRocket_ProtoCharacter::EndScore);
	scoreToggle.bExecuteWhenPaused = true;

	PlayerInputComponent->BindAxis("MoveForward", this, &ADeathRocket_ProtoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADeathRocket_ProtoCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADeathRocket_ProtoCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADeathRocket_ProtoCharacter::LookUpAtRate);

	FInputActionBinding& pauseToggle = PlayerInputComponent->BindAction("Pause", IE_Pressed, pauseComp, &UPauseComponent::Pause);
	pauseToggle.bExecuteWhenPaused = true;
}

void ADeathRocket_ProtoCharacter::SetTeamColor(const FColor& teamColor)
{
	team = teamColor;

	if (captureComp)
		captureComp->teamColor = teamColor;
}

float ADeathRocket_ProtoCharacter::GetAreaDirectionAngle() const
{
	if (captureComp && captureComp->AreaDetected())
	{
		FVector loc = GetActorLocation();
		FVector areaLoc = captureComp->GetAreaLocation();

		FRotator lookAt = UKismetMathLibrary::FindLookAtRotation(loc, areaLoc);
		FRotator rot = GetControlRotation();

		return rot.Yaw - lookAt.Yaw;
	}

	return 0.f;
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

	if (sprintComp)
		sprintComp->TickStamina(DeltaTime, isMoving);

	// Camera
	{
		FVector actualCamLoc = FollowCamera->GetRelativeLocation();
		FVector newSide = FMath::VInterpTo(actualCamLoc, { actualCamLoc.X, cameraYOffset * shoulder, actualCamLoc.Z }, DeltaTime, 10.f);
		FollowCamera->SetRelativeLocation(newSide);

		FollowCamera->FieldOfView = FMath::Lerp<float>(FollowCamera->FieldOfView, curFov, DeltaTime * 10.f);

		if (aimBotComp->target)
		{
			FVector toTarget = aimBotComp->target->GetActorLocation() - FollowCamera->GetComponentLocation();
			GetController()->SetControlRotation(UKismetMathLibrary::RInterpTo(GetControlRotation(),
												toTarget.Rotation(),
												GetWorld()->GetDeltaSeconds(), aimBotComp->aimBotStrengh));

			//camera direction
			FVector fw = GetControlRotation().RotateVector({ 1.f, 0.f, 0.f });
			aimBotComp->CheckTarget(fw, GetActorLocation());
		}
	}

	// Reload
	if (curAmmo <= 0 && !reloading && !isMoving)
		Reload();

	UpdateTimersProgress();
	BroadcastUIUpdate();
}

void ADeathRocket_ProtoCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * horizontalSensitivity * GetWorld()->GetDeltaSeconds());
}

void ADeathRocket_ProtoCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * verticalSensitivity * GetWorld()->GetDeltaSeconds());
}

void ADeathRocket_ProtoCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ADeathRocket_ProtoCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
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
	if (!healthComp->GetIsAlive() || firing || curAmmo <= 0)
		return;

	if (reloading)
	{
		reloading = false;
		reloadTimer->Clear();
	}

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	spawnParams.Owner = this;

	FVector camForward = FollowCamera->GetForwardVector();
	FVector camLocWorld = FollowCamera->GetComponentLocation();

	FRotator rotation = GetControlRotation();
	SetActorRotation(FRotator(0.f, rotation.Yaw, 0.f));


	FHitResult HitObject;
	//FHitResult HitObject2;
	bool Hit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), camLocWorld,  camLocWorld + camForward * 10000, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitObject, true, FColor::White, FColor::Red, 0.3f);
	
	// Spawn rocket according to first ammunition's type, else default type
	TSubclassOf<class ARocket> rocketClass;
	if (specialAmmos.Num() != 0)
	{
		rocketClass = rocketClasses[specialAmmos[0]];
		specialAmmos.RemoveAt(0);
	}
	else
		rocketClass = rocketClasses[ERocketType::BASIC];

	//secondTripleBulletTimer->Reset();
	FVector spawnLocation = RocketLauncher->GetSocketLocation(FName("RocketCanon"));
	if (rocketClass != rocketClasses[ERocketType::TRIPLE])
	{
		if (ARocket* rocket = GetWorld()->SpawnActor<ARocket>(rocketClass, spawnLocation, GetControlRotation(), spawnParams))
		{
			FVector RocketDir = Hit ? HitObject.Location - spawnLocation : camForward;

			RocketDir.Normalize();

			rocket->Initialize(RocketDir);
		}
	}
	else
	{
		FVector spawnDistance = FollowCamera->GetForwardVector().RotateAngleAxis(-90.f, FVector::UpVector) * 50.f;
		spawnLocation -= spawnDistance;
		// triple spawn
		for (int i = 0; i < 3; i++)
		{
			if (ARocket* rocket = GetWorld()->SpawnActor<ARocket>(rocketClass, spawnLocation, GetControlRotation(), spawnParams))
			{
				FVector RocketDir = camForward;

				RocketDir.Normalize();

				rocket->Initialize(RocketDir);

				spawnLocation += spawnDistance;
			}
		}
	}
	// Cancel forced aim (used for ultime)
	if (aimForced)
	{
		aimForced = false;
		StopAiming();
	}
	// Variable things...
	firing = true;
	--curAmmo;
	OnAmmoUpdate.Broadcast();
	// Reload
	if (curAmmo == 0)
	{
		EndFire();
		return;
	}
	else
		fireTimer->Reset(this, &ADeathRocket_ProtoCharacter::EndFire);
}

void ADeathRocket_ProtoCharacter::EndFire()
{
	firing = false;
}

void ADeathRocket_ProtoCharacter::CreateDefaultUltime()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, team, FString("Create"));
	Ultime* ultime = new Ultime();
	ultimeComp->SetUltime(ultime);
}

void ADeathRocket_ProtoCharacter::ZoneCapturedFeed()
{
	if (killfeedManager)
	{
		killfeedManager->CaptureHappened(nickName, team);
	}
}

bool ADeathRocket_ProtoCharacter::AddAmmunitions(ERocketType type, int count, bool ultime)
{
	// Can't pick ammo when reloading
	if (reloading || (!ultime && curFov == ads))
		return false;

	// Ultime ammo ignore max ammo
	if (ultime)
	{
		for (int i = 0; i < count; i++)
			specialAmmos.Insert(type, 0);

		curAmmo += count;
		OnAmmoUpdate.Broadcast();
		return true;
	}
	// Other ammo don't ignore max ammo, however we can exceed the limit in a raw only
	else if (specialAmmos.Num() < maxSpecialAmmos)
	{
		for (int i = 0; i < count; i++)
			specialAmmos.Add(type);

		curAmmo += count;
		OnAmmoUpdate.Broadcast();
		return true;
	}

	return false;
}

void ADeathRocket_ProtoCharacter::Reload()
{
	if (!healthComp->GetIsAlive() || curAmmo >= ammoMax || reloading)
		return;

	StopAiming();
	sprintComp->SetActivate(false);

	reloading = true;
	GetCharacterMovement()->MaxWalkSpeed = sprintComp->GetSpeed() / 2.f;

	reloadTimer->SetTime(reloadTime / (curAmmo + 1));
	reloadTimer->Reset(this, &ADeathRocket_ProtoCharacter::EndReload);
}

void ADeathRocket_ProtoCharacter::EndReload()
{
	curAmmo = ammoMax + specialAmmos.Num();

	OnAmmoUpdate.Broadcast();

	sprintComp->SetActivate(true);
	GetCharacterMovement()->MaxWalkSpeed = sprintComp->GetSpeed();

	reloading = false;
}

void ADeathRocket_ProtoCharacter::ForceReload()
{
	EndReload();
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
	if (!healthComp->GetIsAlive() || (reloading && curAmmo <= 0) || curFov == ads)
		return;

	sprintComp->EndSprint();
	GetCharacterMovement()->MaxWalkSpeed = sprintComp->GetSpeed() / 2.f;

	curFov = ads;
	horizontalSensitivity /= 2.25f;
	verticalSensitivity /= 2.25f;

	//camera direction
	FVector fw = GetControlRotation().RotateVector({ 1.f, 0.f, 0.f });
	aimBotComp->SelectTarget(fw, GetActorLocation());
}

void ADeathRocket_ProtoCharacter::StopAiming()
{
	if (aimForced || curFov != ads)
		return;

	//is player running? if yes -> fov is runFov
	curFov =  fov;
	GetCharacterMovement()->MaxWalkSpeed = sprintComp->GetSpeed();

	horizontalSensitivity *= 2.25f;
	verticalSensitivity *= 2.25f;

	aimForced = false;

	aimBotComp->LoseTarget();
}

void ADeathRocket_ProtoCharacter::ForceAim()
{
	aimForced = true;
	Aim();
}

void ADeathRocket_ProtoCharacter::EarnKill()
{
	++kills;
	++streak;
	++fastStreak;
	fastStreakTimer->Reset(this, &ADeathRocket_ProtoCharacter::ResetFastStreak);

	if (streak == 5)
		OnStreakReached.Broadcast("KILLING SPREE");

	if (fastStreak == 0)
		return;

	switch (fastStreak)
	{
	case 2:
		OnStreakReached.Broadcast("DOUBLE KILL");
		break;
	case 3:
		OnStreakReached.Broadcast("TRIPLE KILL");
		break;
	case 4:
		OnStreakReached.Broadcast("QUADRIPLE KILL");
		break;
	case 5:
		OnStreakReached.Broadcast("QUINTUPLE KILL");
		break;
	default:
		break;
	}

	if (fastStreak > 5)
	{
		FString ks = FString::FromInt(fastStreak) + FString(" KILLS STREAK");
		OnStreakReached.Broadcast(ks);
	}
}

void ADeathRocket_ProtoCharacter::OnDeath()
{
	specialAmmos.Empty();

	SetRagdollOn();
	UpdateDeathDisplay();
}

void ADeathRocket_ProtoCharacter::UpdateDeathDisplay()
{
	deaths++;

	if (!lastDamager)
		return;

	if (lastDamager->killfeedManager)
		lastDamager->killfeedManager->KillHappened(lastDamager->GetNickName(), lastDamager->team,
												   nickName, team);

	if (lastDamager->team == team)
	{
		lastDamager->kills--;
		return;
	}

	lastDamager->OnHitmarkerDisplay.Broadcast();

	lastDamager->EarnKill();
	lastDamager->ultimeComp->IncreaseByKill();
}

void ADeathRocket_ProtoCharacter::SetRagdollOn()
{
	if (isOnRagdoll)
		return;

	isOnRagdoll = true;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->Deactivate();

	meshTransform = GetMesh()->GetRelativeTransform();
	GetMesh()->SetSimulatePhysics(true);

	CameraBoom->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

}

void ADeathRocket_ProtoCharacter::SetRagdollOff()
{
	if (!isOnRagdoll)
		return;

	isOnRagdoll = false;

	CameraBoom->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	GetMesh()->SetSimulatePhysics(false);

	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	GetMesh()->SetRelativeTransform(meshTransform);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	GetCharacterMovement()->Activate();
}

void ADeathRocket_ProtoCharacter::Respawn()
{
	healthComp->Reset();
	sprintComp->EndRecover();
	EndReload();
	ResetLastDamager();

	//streaks
	streak = 0;
	ResetFastStreak();

	GetCharacterMovement()->StopMovementImmediately();

	if (spawnManager)
	{
		if (AController* controller = GetController())
		{
			if (APlayerController* playerController = Cast<APlayerController>(controller))
				spawnManager->SpawnControllerAtPlayerStart(playerController);
		}
	}

	SetRagdollOff();
}

void ADeathRocket_ProtoCharacter::Sprint()
{
	StopAiming();
	curFov = runFov;

	GetCharacterMovement()->MaxAcceleration = defaultMaxAcceleration;
	GetCharacterMovement()->MaxWalkSpeed = sprintComp->GetSpeed();
}

void ADeathRocket_ProtoCharacter::Dash()
{
	GetCharacterMovement()->MaxAcceleration = MAX_ACCELERATION;
	GetCharacterMovement()->MaxWalkSpeed = sprintComp->GetSpeed();
}

void ADeathRocket_ProtoCharacter::EndSprint()
{
	//is player aiming?
	if (curFov == ads)
		return;

	curFov = fov;
	GetCharacterMovement()->MaxAcceleration = defaultMaxAcceleration;
	GetCharacterMovement()->MaxWalkSpeed = sprintComp->GetSpeed();
}

void ADeathRocket_ProtoCharacter::Score()
{
	if (!scoreManager)
		return;

	OnScoreDisplay.Broadcast();
}

void ADeathRocket_ProtoCharacter::EndScore()
{
	if (!scoreManager)
		return;

	OnScoreHide.Broadcast();
}

void ADeathRocket_ProtoCharacter::ResetLastDamager()
{
	lastDamager = this;
}

void ADeathRocket_ProtoCharacter::ResetFastStreak()
{
	fastStreak = 0;
}

void ADeathRocket_ProtoCharacter::OnDamage_Implementation(AActor* from, int damage)
{
	if (ADeathRocket_ProtoCharacter* player = Cast<ADeathRocket_ProtoCharacter>(from))
	{
		int dmg = damage;
		if (player->team == team)
			dmg = damage / allyDmgReduction;

		lastDamager = player;

		lastDamagerTimer->Reset(this, &ADeathRocket_ProtoCharacter::ResetLastDamager);
		healthComp->Hurt(dmg);
	}
}

int  ADeathRocket_ProtoCharacter::GetCaptureCount() const
{
	if (captureComp)
		return captureComp->GetCaptureCount();
	return 0;
}
