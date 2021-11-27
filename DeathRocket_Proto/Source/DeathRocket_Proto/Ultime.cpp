#include "Ultime.h"

#include "DeathRocket_ProtoCharacter.h"

Ultime::Ultime()
{
	ultimeRocket = ERocketType::BIG;
}

Ultime::~Ultime()
{
}

void Ultime::Use(ADeathRocket_ProtoCharacter* user)
{
	// Spawn GIGA ROCKET or CONTROLLED ROCKET
	if (user)
	{
		user->ForceReload();
		user->ForceAim();
		user->AddAmmunitions(ultimeRocket);
	}
}