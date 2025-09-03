#include "Player/MPPlayerState.h"

// 추가됨
#include "Net/UnrealNetwork.h"

AMPPlayerState::AMPPlayerState()
: PlayerName(TEXT("None"))
, CurrentGuessCount(1)
, MaxGuessCount(3)
, bHasGuessedThisTurn(false)
, TurnRemainingTime(0.f)
{
	bReplicates = true;
}

void AMPPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerName);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
}
