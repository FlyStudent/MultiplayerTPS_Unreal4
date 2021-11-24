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

void UCaptureComponent::BroadcastDelegate()
{
	OnCaptureCompleted.Broadcast(); 
}


void UCaptureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (capturingArea)
	{
		// Look if the capture area is free
		float deltaCaptureTime = (DeltaTime * 100.f) / captureTime;

		if (!isCapturing && capturingArea->TryCaptureArea(this))
			BeginAreaCapture();

		if (isCapturing)
			capturingArea->TickCapturePercent(this, deltaCaptureTime);
	}
}

void UCaptureComponent::BeginAreaCapture()
{
	if (capturingArea)
	{
		capturingArea->OnCaptureCompleted.AddDynamic(this, &UCaptureComponent::BroadcastDelegate);
		isCapturing = true;
	}
}

void UCaptureComponent::BeginOverlap(ACaptureArea* area)
{
	if (area && !capturingArea)
	{
		if (area->TryCaptureArea(this))
			BeginAreaCapture();
	}
}

void UCaptureComponent::EndOverlap(ACaptureArea* area)
{
	if (area)
	{
		area->OnCaptureCompleted.RemoveDynamic(this, &UCaptureComponent::BroadcastDelegate);
		area->ExitCaptureArea(this);
		isCapturing = false;
	}
}