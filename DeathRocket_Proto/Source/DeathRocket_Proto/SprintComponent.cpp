#include "SprintComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Timer.h"

// Sets default values for this component's properties
USprintComponent::USprintComponent()
{
	//PrimaryComponentTick.bCanEverTick = true;
	curStamina = maxStamina;
}

USprintComponent::~USprintComponent()
{
	delete dashRecoveryTimer;
	delete dashProcessTimer;
}


// Called when the game starts
void USprintComponent::BeginPlay()
{
	Super::BeginPlay();

	dashRecoveryTimer = new Timer(GetWorld(), dashRecoveryTime);
	dashProcessTimer = new Timer(GetWorld(), dashInputMaxTime);
}


// Called every frame
void USprintComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USprintComponent::TickStamina(float DeltaTime, bool isMoving)
{
	// Stop Run/Dash if not moving
	if (!isMoving && state != ESprintState::WALK)
		GoToWalk();
	// Come back to Run if moving 
	else if (isMoving && sprinting && state == ESprintState::WALK)
		GoToRun();

	switch (state)
	{
	case ESprintState::RUN:

		curSprintTime += DeltaTime;
		curStamina -= runConsumptionSeconds * DeltaTime;

		CheckExhausting();

		break;

	case ESprintState::DASH:

		curSprintTime += DeltaTime;
		curStamina -= dashConsumptionSeconds * DeltaTime;

		if (curSprintTime >= dashTime)
			GoToWalk();

		break;

	default: // WALK
		curStamina = FMath::Min(curStamina + recoverySeconds * DeltaTime, maxStamina);
		if (curStamina >= maxStamina && staminaRecovering)
		{
			staminaRecovering = false;
			OnStaminaRecovery.Broadcast(staminaRecovering);
		}

		break;
	}

	staminaRatio = curStamina / maxStamina;
}

float USprintComponent::GetSpeed(const ESprintState& s) const
{
	switch (s) {
	case ESprintState::RUN:
		return runningSpeed;
	case ESprintState::DASH:
		return dashingSpeed;
	default:
		return walkingSpeed;
	}
}

float USprintComponent::GetSpeed() const
{
	return GetSpeed(state);
}

bool USprintComponent::CanSprint() const
{
	return !staminaRecovering;
}

bool USprintComponent::CanDash() const
{
	return dashActivate && !dashRecovering;
}

void USprintComponent::Sprint()
{
	if (!CanSprint())
		return;

	if (CanDash())
	{
		GoToRun();
		processingDash = true;
		dashProcessTimer->Reset(this, &USprintComponent::EndDashProcess);
	}
	else
	{
		GoToRun();
	}

	sprinting = true;
}

void USprintComponent::EndSprint()
{
	if (!sprinting)
		return;

	if (processingDash)
		GoToDash();
	else
		GoToWalk();
}

void USprintComponent::EndDashProcess()
{
	processingDash = false;
}

void USprintComponent::RecoverDash()
{
	dashRecovering = false;
}

void USprintComponent::CheckExhausting()
{
	if (curStamina <= 0.f)
	{
		curStamina = 0.1f;
		staminaRecovering = true;
		OnStaminaRecovery.Broadcast(staminaRecovering);
		EndSprint();
	}
}

void USprintComponent::GoToWalk()
{
	state = ESprintState::WALK;
	sprinting = false;
	CheckExhausting();
	OnEndRun.Broadcast();
}

void USprintComponent::GoToRun()
{
	state = ESprintState::RUN;

	curSprintTime = 0.f;
	OnRun.Broadcast();
}

void USprintComponent::GoToDash()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString("Dash"));
	state = ESprintState::DASH;
	OnDash.Broadcast();


	curSprintTime = 0.f;
	dashRecovering = true;
	dashRecoveryTimer->Reset(this, &USprintComponent::RecoverDash);
}

