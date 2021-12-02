#include "CaptureComponent.h"

#include "Kismet/GameplayStatics.h"

#include "CaptureArea.h"

UCaptureComponent::UCaptureComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCaptureComponent::BeginPlay()
{
	Super::BeginPlay();
}

FVector UCaptureComponent::GetAreaLocation() const
{
	if (AreaDetected())
		return currentArea->GetActorLocation();

	return FVector::ZeroVector;
}

void UCaptureComponent::AreaConnect()
{
	currentArea->OnCaptureCompleted.AddDynamic(this, &UCaptureComponent::AreaDisconnect);
	isEntered = true;

	OnEnteringArea.Broadcast();
}

void UCaptureComponent::AreaDisconnect()
{
	if (AreaDetected())
	{
		currentArea->OnCaptureCompleted.RemoveDynamic(this, &UCaptureComponent::AreaCapturedBySelf);
		currentArea->OnCaptureCompleted.RemoveDynamic(this, &UCaptureComponent::AreaDisconnect);

		OnExitingArea.Broadcast();
	}

	isEntered = false;
	isCapturing = false;
}

void UCaptureComponent::AreaDestroyed()
{
	currentArea = nullptr;
	OnAreaDestroyed.Broadcast();
}

void UCaptureComponent::AreaCapturedBySelf()
{
	captureCount++;
	OnCaptureCompleted.Broadcast();
}

void UCaptureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (isEntered)
	{
		// Look if the capture area is free
		float deltaCaptureTime = (DeltaTime * 100.f) / captureTime;

		bool allowCapture = currentArea->TryCaptureArea(teamColor);
		// Capture the area if nobody else doing
		if (!isCapturing && allowCapture)
			BeginAreaCapture();
		else if (isCapturing && !allowCapture)
			StopAreaCapture();
		// Capture update
		if (isCapturing && allowCapture)
			currentArea->TickCapturePercent(teamColor, deltaCaptureTime);
	}
	else if (!AreaDetected())
	{
		// TODO : optimize
		SearchArea();
	}
}

void UCaptureComponent::BeginAreaCapture()
{
	if (AreaDetected())
	{
		currentArea->OnCaptureCompleted.AddDynamic(this, &UCaptureComponent::AreaCapturedBySelf);
		isCapturing = true;
	}
}

void UCaptureComponent::StopAreaCapture()
{
	if (AreaDetected())
	{
		currentArea->OnCaptureCompleted.RemoveDynamic(this, &UCaptureComponent::AreaCapturedBySelf);
		isCapturing = false;
	}
}

void UCaptureComponent::SearchArea()
{
	currentArea = Cast<ACaptureArea>(UGameplayStatics::GetActorOfClass(GetWorld(), ACaptureArea::StaticClass()));
	if (AreaDetected())
	{
		OnAreaDetected.Broadcast();
		currentArea->OnCaptureCompleted.AddDynamic(this, &UCaptureComponent::AreaDestroyed);
	}
}

void UCaptureComponent::BeginOverlap()
{
	if (!currentArea)
		SearchArea();

	if (currentArea && !isEntered)
	{
		AreaConnect();
		if (currentArea->TryCaptureArea(teamColor))
			BeginAreaCapture();
	}
}

void UCaptureComponent::EndOverlap()
{
	if (AreaDetected())
		AreaDisconnect();
}