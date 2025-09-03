#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MPGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPROJECT_API AMPGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

	// 로그인 메시지
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLogin(const FString& InName = FString(TEXT("None")));
};
