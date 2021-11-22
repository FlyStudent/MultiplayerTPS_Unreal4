#include "Timer.h"

#include "Engine/World.h"

Timer::Timer(UWorld* w, float t)
	: world(w), time(t)
{

}

void Timer::Pause()
{
	world->GetTimerManager().PauseTimer(timer);
}

void Timer::Resume()
{
	world->GetTimerManager().UnPauseTimer(timer);
}


void Timer::Clear()
{
	if (!world)
		return;

	world->GetTimerManager().ClearTimer(timer);
}

float Timer::GetProgess() const
{
	if (!world)
		return 0.f;

	float elapsed = world->GetTimerManager().GetTimerElapsed(timer);
	return elapsed / time;
}