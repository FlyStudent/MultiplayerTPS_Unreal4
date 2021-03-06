#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SprintComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDashDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRunDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndRunDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRecoverDelegate, bool, recovering);

enum class ESprintState
{
	WALK = 0,
	RUN = 1,
	DASH = 2
};

UCLASS( ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent) )
class DEATHROCKET_PROTO_API USprintComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Dash", meta = (AllowPrivateAccess = "true"))
		bool dashActivate = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool sprintActivate = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed", meta = (AllowPrivateAccess = "true"))
		float walkingSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed", meta = (AllowPrivateAccess = "true"))
		float runningSpeed = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed", meta = (AllowPrivateAccess = "true"))
		float dashingSpeed = 5000.f;

	bool  sprinting = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Dash", meta = (AllowPrivateAccess = "true"))
		float dashTime = 0.05f;
	float curSprintTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Dash", meta = (AllowPrivateAccess = "true"))
		float dashRecoveryTime = 0.1f;
	bool dashRecovering = false;
	class Timer* dashRecoveryTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Dash", meta = (AllowPrivateAccess = "true"))
		float dashInputMaxTime = 0.25f;
	bool processingDash = false;
	class Timer* dashProcessTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Stamina", meta = (AllowPrivateAccess = "true"))
		float maxStamina = 100.f;
	float curStamina;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float staminaRatio = 1.f;
	bool staminaRecovering = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Stamina", meta = (AllowPrivateAccess = "true"))
		float runConsumptionSeconds = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Stamina", meta = (AllowPrivateAccess = "true"))
		float dashConsumptionSeconds = 800.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Stamina", meta = (AllowPrivateAccess = "true"))
		float recoverySeconds = 15.f;
	
	virtual void BeginPlay() override;

	void EndDashProcess();
	void RecoverDash();

	void CheckExhausting();
	void GoToWalk();
	void GoToRun();
	void GoToDash();

public:	
	ESprintState state = ESprintState::WALK;

	USprintComponent();
	~USprintComponent();

	void TickStamina(float DeltaTime, bool isMoving);

	float GetSpeed(const ESprintState& state) const;
	float GetSpeed() const;

	bool CanSprint() const;
	bool CanDash() const;
	bool ProcessingDash() const;
	void Sprint();
	void EndSprint();
	void EndRecover();

	UPROPERTY(BlueprintAssignable, Category = "Components|Run")
		FDashDelegate OnDash;
	
	UPROPERTY(BlueprintAssignable, Category = "Components|Run")
		FRunDelegate OnRun;

	UPROPERTY(BlueprintAssignable, Category = "Components|Run")
		FEndRunDelegate OnEndRun;

	UPROPERTY(BlueprintAssignable, Category = "Components|Run")
		FRecoverDelegate OnStaminaRecovery;

	FORCEINLINE bool IsSprinting() const { return sprinting; }
	FORCEINLINE void SetActivate(bool activate) { sprintActivate = activate; }
	FORCEINLINE float GetRatio() const { return staminaRatio; }
};