#include "Timer.h"

#include "Engine/World.h"

Timer::Timer(UWorld* w, float t)
	: world(w), time(t)
{

}

void Timer::Clear()
{
	if (!world)
		return;

	world->GetTimerManager().ClearTimer(timer);
	progress = 0.f;
}

float Timer::GetProgess()
{
	if (!world)
		return 0.f;

	float elapsed = world->GetTimerManager().GetTimerElapsed(timer);
	progress = elapsed / time;

	return progress;
}