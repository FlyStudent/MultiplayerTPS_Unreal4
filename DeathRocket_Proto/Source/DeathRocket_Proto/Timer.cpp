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
}

float Timer::GetProgess()
{
	if (!world)
		return 0.f;

	float elapsed = world->GetTimerManager().GetTimerElapsed(timer);
	return elapsed / time;
}