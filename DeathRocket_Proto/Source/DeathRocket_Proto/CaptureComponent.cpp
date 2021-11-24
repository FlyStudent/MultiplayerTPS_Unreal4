#include "CaptureComponent.h"

#include "CaptureArea.h"

UCaptureComponent::UCaptureComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCaptureComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UCaptureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (capturingArea)
	{
		if (!isCapturing && capturingArea->curPercent == 0.f)
			isCapturing = true;

		if (isCapturing)
		{
			float deltaCaptureTime = (DeltaTime * 100.f) / captureTime;
			capturingArea->curPercent += deltaCaptureTime;
		}
	}
}

void UCaptureComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto area = Cast<ACaptureArea>(OtherActor);

	if (area)
		capturingArea = area;
}