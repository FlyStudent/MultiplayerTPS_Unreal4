#include "CaptureArea.h"

#include "Components/BoxComponent.h"

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
	
}

// Called every frame
void ACaptureArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

