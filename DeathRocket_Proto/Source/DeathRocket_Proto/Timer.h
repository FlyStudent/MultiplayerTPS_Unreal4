#pragma once

#include "CoreMinimal.h"

class DEATHROCKET_PROTO_API Timer
{
private:
	FTimerHandle	timer;
	float			time = 1.f;
	float			progress = 0.f;

	class UWorld*	world;

public:
	Timer() = default;
	Timer(class UWorld* world, float time);

	void Clear();
	template< class UserClass >
	void Set(UserClass* obj, typename FTimerDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InTimerMethod);

	float GetProgess();
};

template< class UserClass >
void Timer::Set(UserClass* obj, typename FTimerDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InTimerMethod)
{
	if (!world)
		return;

	world->GetTimerManager().SetTimer(timer, obj, InTimerMethod, time, false);
}