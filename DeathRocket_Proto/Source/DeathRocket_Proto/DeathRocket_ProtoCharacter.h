// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DamageableInterface.h"
#include "DeathRocket_ProtoCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAmmoEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FScoreEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUIEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWidgetEvent, bool, visible);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FKillFeedEvent, FString, killerName,
															  FColor, killerColor,
															  FString, victimName,
															  FColor, victimColor);

UENUM(BlueprintType)
enum class ERocketType : uint8 {
	BASIC  UMETA(DisplayName = "BASIC"),
	BIG UMETA(DisplayName = "BIG"),
	TRIPLE UMETA(DisplayName = "TRIPLE")
};

UCLASS(config=Game)
class ADeathRocket_ProtoCharacter : public ACharacter, public IDamageableInterface
{
	GENERATED_BODY()

	float defaultMaxAcceleration;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* RocketLauncher;

public:
	ADeathRocket_ProtoCharacter();
	~ADeathRocket_ProtoCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float horizontalSensitivity = 45.f;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float verticalSensitivity = 45.f;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TMap<ERocketType, TSubclassOf<class ARocket>> rocketClasses;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TArray<ERocketType> specialAmmos;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	uint8 maxSpecialAmmos = 1;

	TArray<AActor*> ActorsToIgnore;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	// FIRE AND RELOAD
	bool firing = false;
	bool reloading = false;

	UPROPERTY(EditAnywhere, Category = Weapon)
	float fireRate = 1.f;
	UPROPERTY(EditAnywhere, Category = Weapon)
	float reloadTime = 3.f;
	UPROPERTY(EditAnywhere, Category = Weapon)
	float gamepadUltiInputTime = 0.1f;

	class Timer* fireTimer;
	class Timer* reloadTimer;
	// for the UI
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float fireProgress = 0.f;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float reloadProgress = 0.f;

	// UI UPDATE
	UPROPERTY(BlueprintAssignable, Category = Event)
	FWidgetEvent OnFireCDUpdate;
	bool  lastFireUpdate = false;
	UPROPERTY(BlueprintAssignable, Category = Event)
	FWidgetEvent OnReloadCDUpdate;
	bool  lastReloadUpdate = false;
	UPROPERTY(BlueprintAssignable, Category = Event)
	FWidgetEvent OnStaminaUpdate;
	bool lastStaminaUpdate = false;
	UPROPERTY(BlueprintAssignable, Category = Event)
	FWidgetEvent OnUltimeUpdate;
	float lastUltimeRatio = 0.f;

	UFUNCTION(BlueprintCallable)
	float GetAreaDirectionAngle() const;

	class Timer* gamepadUltimeTimer;
	bool gamepadUltimeUse = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString nickName = "Player";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Team)
	FColor team;
	UFUNCTION(BlueprintCallable)
	void SetTeamColor(const FColor& teamColor);

	ADeathRocket_ProtoCharacter* lastDamager = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Team)
	int allyDmgReduction = 2;
	int kills = 0, deaths = 0;

	void Fire();
	void EndFire();

	UFUNCTION()
	void CreateDefaultUltime();

	void Reload();
	void InputReload();
	void EndReload();

	void GamepadUltimeInput();
	void CancelGamepadUltimeInput();

	void UpdateTimersProgress();
	void BroadcastUIUpdate();

	// CAMERA
	int   shoulder = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float cameraYOffset = 30.f;

	bool aimForced = false;
	// Aim Down Sight (when aiming)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ads = 50.f;
	// Run fov (when running)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float runFov = 110.f;
	// BASIC Field of view (when not aiming)
	float fov;
	float curFov;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float healthRatio = 1.f;

	// SPECIAL
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float inAirMaxSpeed = 1600.f;

	// AMMUNITION
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	int   ammoMax = 3;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int   curAmmo;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FAmmoEvent OnAmmoUpdate;

	// METHODS
	void changeCamSide();
	void Aim();
	void StopAiming();

	UPROPERTY(BlueprintAssignable, Category = Event)
	FUIEvent OnHitmarkerDisplay;
	UFUNCTION()
	void OnDeath();

	void UpdateDeathDisplay();

	UFUNCTION(BlueprintCallable)
	void Respawn();

	bool isOnRagdoll = false;
	FTransform meshTransform;

	void SetRagdollOn();
	void SetRagdollOff();

	UFUNCTION()
	void Sprint();
	UFUNCTION()
	void Dash();
	UFUNCTION()
	void EndSprint();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	class Timer* secondTripleBulletTimer;
	class Timer* thirdTripleBulletTimer;
	float TripleBulletTime = 0.2f;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* healthComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USprintComponent* sprintComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UUltimeLoaderComponent* ultimeComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Capture, meta = (AllowPrivateAccess = "true"))
	class UCaptureComponent* captureComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AScoreManager* scoreManager;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FScoreEvent OnScoreDisplay;
	UPROPERTY(BlueprintAssignable, Category = Event)
	FScoreEvent OnScoreHide;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION (BlueprintCallable)
	void ForceAim();
	UFUNCTION(BlueprintCallable)
	void ForceReload();

	void Score();
	void EndScore();

	class AKillFeedManager* killfeedManager;
	UPROPERTY(BlueprintAssignable, Category = Event)
	FKillFeedEvent OnDisplayFeed;

	UFUNCTION(BlueprintCallable)
	bool AddAmmunitions(ERocketType type, int count = 1, bool ultime = false);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FString GetNickName() const { return nickName; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FColor GetTeam() const { return team; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int  GetKillsCount() const { return kills; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int  GetDeathsCount() const { return deaths; };

	UFUNCTION(BlueprintCallable)
	int  GetCaptureCount() const;

	void OnDamage(AActor* from, int damage) override;

	class ASpawnManager* spawnManager;
};
