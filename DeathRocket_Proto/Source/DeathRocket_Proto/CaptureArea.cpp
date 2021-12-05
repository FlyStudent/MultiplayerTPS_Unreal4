#include "CaptureArea.h"

#include "Components/BoxComponent.h"
#include "CaptureComponent.h"

// Sets default values
ACaptureArea::ACaptureArea()
{
	PrimaryActorTick.bCanEverTick = true;

	areaCollider = CreateDefaultSubobject<UBoxComponent>("AreaCollider");
	areaCollider->SetBoxExtent(FVector(800.f, 800.f, 800.f));
}

// Called when the game starts or when spawned
void ACaptureArea::BeginPlay()
{
	Super::BeginPlay();

	areaCollider->OnComponentBeginOverlap.AddDynamic(this, &ACaptureArea::OnOverlapBegin);
	areaCollider->OnComponentEndOverlap.AddDynamic(this, &ACaptureArea::OnOverlapEnd);
}

void ACaptureArea::SetCaptureTime(float time)
{
	captureTime = 100.f / time;
}

void ACaptureArea::Tick(float DeltaTime)
{
	if (captured)
		return;

	float captureDeltaTime = captureTime * DeltaTime;

	bool goodCapture = capturingTeams.Num() == 1 && capturingTeams[0] == previousCapturingTeam;
	bool fastResetCapture = capturingTeams.Num() >= 1 && capturingTeams.Find(previousCapturingTeam) == INDEX_NONE;
	bool resetCapture = capturingTeams.Num() == 0;

	if (goodCapture)
		curPercent += captureDeltaTime;
	else if (fastResetCapture)
		curPercent -= captureDeltaTime * resetAreaSpeed;
	else if (resetCapture)
		curPercent -= captureDeltaTime / resetAreaSpeed;

	curPercent = FMath::Max(curPercent, 0.f);

	// RENDER COLOR
	{
		bool contest = capturingTeams.Num() > 1;

		if (contest)
			renderCapturingColor = FColor::Silver;
		else
			renderCapturingColor = previousCapturingTeam;
	}

	bool beginCapture = curPercent <= 0.f && capturingTeams.Num() == 1;

	if (beginCapture)
		previousCapturingTeam = capturingTeams[0];

	OnCaptureProcess.Broadcast();

	if (curPercent >= 100.f)
		AreaCaptured();
}

bool ACaptureArea::TryCaptureArea(const FColor& team)
{
	for (auto capturingTeam : capturingTeams)
	{
		if (capturingTeam != team)
			return false;
	}

	return true;
}

void ACaptureArea::AreaCaptured()
{
	captured = true;
	OnCaptureCompleted.Broadcast();
	Destroy();
}

void ACaptureArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (captured)
		return;

	auto captureComp = OtherActor->FindComponentByClass<UCaptureComponent>();

	if (captureComp)
	{
		// Notify the Actor that he entered in the area
		captureComp->BeginOverlap();

		capturingTeams.AddUnique(captureComp->teamColor);
		insiderTeams.Add(captureComp->teamColor);
	}
}

void ACaptureArea::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (captured)
		return;

	auto captureComp = OtherActor->FindComponentByClass<UCaptureComponent>();

	if (captureComp)
	{
		// Notify the Actor that he exits the area
		insiderTeams.RemoveSingle(captureComp->teamColor);
		if (insiderTeams.Find(captureComp->teamColor) == INDEX_NONE)
			capturingTeams.RemoveSingle(captureComp->teamColor);

		captureComp->EndOverlap();
	}
}