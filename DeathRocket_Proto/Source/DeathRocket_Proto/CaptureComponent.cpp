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

void UCaptureComponent::AreaCapturedBySelf()
{
	OnCaptureCompleted.Broadcast();
}

void UCaptureComponent::AreaDisconnect()
{
	capturingArea->ExitCaptureArea(this);
	isCapturing = false;
}


void UCaptureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (capturingArea)
	{

		// Look if the capture area is free
		float deltaCaptureTime = (DeltaTime * 100.f) / captureTime;

		// Capture the area if nobody else doing
		if (!isCapturing && capturingArea->TryCaptureArea(this))
			BeginAreaCapture();
		// Capture update
		if (isCapturing)
			capturingArea->TickCapturePercent(this, deltaCaptureTime);
	}
}

void UCaptureComponent::BeginAreaCapture()
{
	if (capturingArea)
	{
		capturingArea->OnCaptureCompleted.AddDynamic(this, &UCaptureComponent::AreaCapturedBySelf);
		isCapturing = true;
	}
}

void UCaptureComponent::BeginOverlap(ACaptureArea* area)
{
	if (area && !capturingArea)
	{
		area->OnCaptureCompleted.AddDynamic(this, &UCaptureComponent::AreaDisconnect);
		if (area->TryCaptureArea(this))
			BeginAreaCapture();
	}
}

void UCaptureComponent::EndOverlap(ACaptureArea* area)
{
	if (area)
	{
		area->OnCaptureCompleted.RemoveDynamic(this, &UCaptureComponent::AreaCapturedBySelf);
		area->OnCaptureCompleted.RemoveDynamic(this, &UCaptureComponent::AreaDisconnect);
		AreaDisconnect();
	}
}