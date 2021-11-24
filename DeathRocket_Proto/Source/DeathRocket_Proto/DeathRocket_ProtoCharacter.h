// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DeathRocket_ProtoCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAmmoEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWidgetEvent, bool, visible);

UCLASS(config=Game)
class ADeathRocket_ProtoCharacter : public ACharacter
{
	GENERATED_BODY()

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARocket> rocketClass;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	// FIRE AND RELOAD
	bool firing = false;
	bool reloading = false;
	bool stopMovement = false;

	UPROPERTY(EditAnywhere, Category = Weapon)
	float fireRate = 1.f;
	UPROPERTY(EditAnywhere, Category = Weapon)
	float reloadTime = 5.f;
	UPROPERTY(EditAnywhere, Category = Weapon)
	float gamepadUltiInputTime = 0.1f;

	class Timer* fireTimer;
	class Timer* reloadTimer;
	class Timer* gamepadUltimeTimer;
	// for the UI
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float fireProgress = 0.f;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float reloadProgress = 0.f;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FWidgetEvent OnFireCDUpdate;
	bool lastFireUpdate = false;
	UPROPERTY(BlueprintAssignable, Category = Event)
	FWidgetEvent OnReloadCDUpdate;
	bool lastReloadUpdate = false;
	UPROPERTY(BlueprintAssignable, Category = Event)
	FWidgetEvent OnUltimeUpdate;
	float lastUltimeRatio = 0.f;

	bool gamepadUltimeUse = false;

	void Fire();
	void EndFire();

	void Reload();
	void EndReload();

	void GamepadUltimeInput();
	void CancelGamepadUltimeInput();

	void UpdateTimersProgress();
	void BroadcastUIUpdate();

	// CAMERA
	int   shoulder = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float cameraYOffset = 30.f;
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

	UPROPERTY(BlueprintAssignable, Category = Event)
		FWidgetEvent OnStaminaUpdate;
	bool lastStaminaUpdate = false;

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

	void TakeDamage();

	UFUNCTION()
	void OnDeath();

	UFUNCTION()
	void Sprint();
	UFUNCTION()
	void Dash();
	UFUNCTION()
	void EndSprint();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* healthComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USprintComponent* sprintComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UUltimeLoaderComponent* ultimeComp;

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
