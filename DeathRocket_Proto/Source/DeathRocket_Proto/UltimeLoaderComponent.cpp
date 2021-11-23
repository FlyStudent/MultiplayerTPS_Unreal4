#include "UltimeLoaderComponent.h"

#include "Ultime.h"

// Sets default values for this component's properties
UUltimeLoaderComponent::UUltimeLoaderComponent()
{
	//PrimaryComponentTick.bCanEverTick = true;
}

void UUltimeLoaderComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UUltimeLoaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UUltimeLoaderComponent::Increase(float value)
{
	curPercent = FMath::Min<float>(curPercent + value, 100.f);

	if (curPercent == 100.f)
		OnUltimeLoaded.Broadcast();
}

void UUltimeLoaderComponent::IncreaseByKill()
{
	Increase(killIncreaePercent);
}

void UUltimeLoaderComponent::IncreaseByCapture()
{
	Increase(captureIncreasePercent);
}

void UUltimeLoaderComponent::SetUltime(Ultime* ult)
{
	ulti = ult;
}

void UUltimeLoaderComponent::Use()
{
	if (!ulti)
		return;

	ulti->Use();
	OnUltimeUsed.Broadcast();
}


