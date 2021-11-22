#pragma once

#include "CoreMinimal.h"

class DEATHROCKET_PROTO_API Timer
{
private:
	FTimerHandle	timer;
	float			time = 1.f;

	class UWorld*	world;

public:
	Timer() = default;
	Timer(class UWorld* world, float time);

	void Pause();
	void Resume();
	void Clear();
	template< class UserClass >
	void Set(UserClass* obj, typename FTimerDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InTimerMethod);
	template< class UserClass >
	void Reset(UserClass* obj, typename FTimerDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InTimerMethod);

	float GetProgess() const;
};

template< class UserClass >
void Timer::Set(UserClass* obj, typename FTimerDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InTimerMethod)
{
	if (!world)
		return;

	world->GetTimerManager().SetTimer(timer, obj, InTimerMethod, time, false);
}

template< class UserClass >
void Timer::Reset(UserClass* obj, typename FTimerDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InTimerMethod)
{
	Clear();
	Set(obj, InTimerMethod);
}