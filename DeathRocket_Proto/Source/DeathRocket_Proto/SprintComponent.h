#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SprintComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDashDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRunDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndRunDelegate);

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed", meta = (AllowPrivateAccess = "true"))
		float walkingSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed", meta = (AllowPrivateAccess = "true"))
		float runningSpeed = 800.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed", meta = (AllowPrivateAccess = "true"))
		float dashingSpeed = 8000.f;

	bool  sprinting = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Dash", meta = (AllowPrivateAccess = "true"))
		float dashMaxTime = 0.05f;
	float curSprintTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Dash", meta = (AllowPrivateAccess = "true"))
		float dashRecoveryTime = 1.f;
	bool dashRecovering = false;
	class Timer* dashRecoveryTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Stamina", meta = (AllowPrivateAccess = "true"))
		float maxStamina = 100.f;
	float curStamina;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float staminaRatio = 1.f;
	bool staminaRecup = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Stamina", meta = (AllowPrivateAccess = "true"))
		float runConsumptionSeconds = 40.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Stamina", meta = (AllowPrivateAccess = "true"))
		float dashConsumptionSeconds = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Stamina", meta = (AllowPrivateAccess = "true"))
		float recoverySeconds = 30.f;
	
	virtual void BeginPlay() override;

	void RecoverDash();

	void GoToWalk();
	void GoToRun();
	void GoToDash();

public:	
	ESprintState state = ESprintState::WALK;

	USprintComponent();
	~USprintComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void TickStamina(float DeltaTime, bool isMoving);

	float GetSpeed(const ESprintState& state) const;
	float GetSpeed() const;

	bool CanSprint() const;
	bool CanDash() const;
	void Sprint();
	void EndSprint();

	UPROPERTY(BlueprintAssignable, Category = "Components|Run")
	FDashDelegate OnDash;
	
	UPROPERTY(BlueprintAssignable, Category = "Components|Run")
	FRunDelegate OnRun;

	UPROPERTY(BlueprintAssignable, Category = "Components|Run")
	FEndRunDelegate OnEndRun;

	FORCEINLINE bool IsSprinting() const { return sprinting; }
};
