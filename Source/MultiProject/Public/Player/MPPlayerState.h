#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MPPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPROJECT_API AMPPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMPPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	FORCEINLINE FString GetPlayerInfo()
	{
		FString PlayerInfo = PlayerName + TEXT("(") + FString::FromInt(CurrentGuessCount) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");
		return PlayerInfo;
	}
	
public:

	// 플레이어 이름
	UPROPERTY(Replicated)
	FString PlayerName;

	// 현재 시도 회수
	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	// 최대 시도 가능 회숫
	UPROPERTY(Replicated)
	int32 MaxGuessCount;

	// 턴 내 행동 여부
	bool bHasGuessedThisTurn; 

	// 남은 턴 시간
	float TurnRemainingTime;
};
