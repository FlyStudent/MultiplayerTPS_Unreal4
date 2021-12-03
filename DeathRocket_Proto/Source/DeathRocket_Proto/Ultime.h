#pragma once

#include "CoreMinimal.h"

enum class ERocketType : uint8;

class DEATHROCKET_PROTO_API Ultime final
{
protected:

	UPROPERTY(EditDefaultsOnly)
	ERocketType ultimeRocket;

public:

	virtual void Use(class ADeathRocket_ProtoCharacter* user);

	Ultime();
	~Ultime() = default;
};
