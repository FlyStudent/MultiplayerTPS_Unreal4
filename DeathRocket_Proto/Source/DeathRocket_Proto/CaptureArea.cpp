#include "CaptureArea.h"

#include "Components/BoxComponent.h"
#include "CaptureComponent.h"

// Sets default values
ACaptureArea::ACaptureArea()
{
	//PrimaryActorTick.bCanEverTick = true;

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

// Called every frame
void ACaptureArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACaptureArea::TickCapturePercent(const FColor& team, float deltaPercent)
{
	if (captured)
		return;

	// If capture reset, begin true capture
	if (tickFactor == -1.f * resetAreaSpeed && (curPercent <= 0.f || team == previousCapturingTeam))
		tickFactor = 1.f;
	// If previous capturer gone, need to reset capture
	else if (tickFactor == 1.f && team != previousCapturingTeam)
		tickFactor = -1.f * resetAreaSpeed;

	curPercent += deltaPercent * tickFactor;

	if (tickFactor == 1.f)
		previousCapturingTeam = team;

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

		capturingTeams.Add(captureComp->teamColor);
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
		capturingTeams.RemoveSingle(captureComp->teamColor);
		captureComp->EndOverlap();
	}
}