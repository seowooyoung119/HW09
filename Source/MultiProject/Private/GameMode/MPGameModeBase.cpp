#include "Public/GameMode/MPGameModeBase.h"

// 추가됨
#include "EngineUtils.h"
#include "Controller/MPPlayerController.h"
#include "GameState/MPGameStateBase.h"
#include "Player/MPPlayerState.h"
#include "Macros.h"

AMPGameModeBase::AMPGameModeBase()
: CurrentTurnPlayer(nullptr)
, CurrentPlayerIndex(0)
, MaxTurnTime(10.f)
{
}

void AMPGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 정답 숫자 생성 및 저장
	SecretNumber = GenerateSecretNumber();
}

void AMPGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	// 플레이어 컨트롤러 캐스팅 + 유효성 체크
	CAST_VALID(AMPPlayerController, NewPlayerController, NewPlayer)
	{
		// 플레이어 상태 가져오기 + 유효성 체크
		ASSIGN_VALID(AMPPlayerState, NewPlayerstate, NewPlayerController->GetPlayerState<AMPPlayerState>())
		{
			// 게임 상태 가져오기 + 유효성 체크
			ASSIGN_VALID(AMPGameStateBase, MPGameStateBase, GetGameState<AMPGameStateBase>())
			{
				// 로그인 알림 메시지 초기화 및 설정
				NewPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		
				// 서버 내부 플레이어 컨테이너에 저장
				AllPlayerControllers.Add(NewPlayerController);
		
				// 플레이어 이름 부여 (Player1, Player2 ...)
				NewPlayerstate->PlayerName = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());

				// 새 플레이어 브로드캐스트
				MPGameStateBase->MulticastRPCBroadcastLogin(NewPlayerstate->PlayerName);

				// 첫 번째 플레이어면 턴 시작
				if (AllPlayerControllers.Num() == 1)
				{
					StartPlayerTurn(NewPlayerController);
					CurrentTurnPlayer = NewPlayerController;
					CurrentPlayerIndex = 0;
				}
			}
		}
	}
}

void AMPGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 모든 플레이어 턴 타이머 정리
	for (auto& Pair : PlayerTurnTimers)
	{
		if (GetWorldTimerManager().IsTimerActive(Pair.Value))
		{
			GetWorldTimerManager().ClearTimer(Pair.Value);
		}
	}
	
	// ResetGame 타이머 정리
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_ResetGame))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_ResetGame);
	}
	
	Super::EndPlay(EndPlayReason);
}

FString AMPGameModeBase::GenerateSecretNumber()
{
	// 1~9까지 숫자 후 배열에 저장
	TArray<int32> Numbers = {1,2,3,4,5,6,7,8,9};

	// 랜덤 숫자 3개 뽑기를 저장용 변수
	FString Result;
	
	// 랜덤 숫자 3개 뽑기 (중복 허용 X)
	for (int32 i = 0; i < 3; ++i)
	{
		int32 MaxNum = Numbers.Num() - 1;
		int32 Index = FMath::RandRange(0, MaxNum);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	// 정답 숫자 반환
	UE_LOG(MultiMacros, Error, TEXT("Secret Number : %s"), *Result);
	return Result;
}

void AMPGameModeBase::StartPlayerTurn(AMPPlayerController* InCurrentPlayerController)
{
	ASSIGN_VALID(AMPPlayerState, PlayerState, InCurrentPlayerController->GetPlayerState<AMPPlayerState>())
	{
		PlayerState->TurnRemainingTime = MaxTurnTime;
		PlayerState->bHasGuessedThisTurn = false;

		// 모든 플레이어에게 턴 상태 알림
		for (const auto& Player : AllPlayerControllers)
		{
			ASSIGN_VALID(AMPPlayerState, EachPlayerState, Player->GetPlayerState<AMPPlayerState>())
			{
				if (Player == InCurrentPlayerController)
				{
					// 현재 턴 플레이어
					Player->NotificationText = FText::FromString
					(
						FString::Printf(TEXT("Your Turn! Time: %d sec (Attempts: %d/%d)"), 
						FMath::CeilToInt(PlayerState->TurnRemainingTime),
						EachPlayerState->CurrentGuessCount,
						EachPlayerState->MaxGuessCount)
					);
				}
				else
				{
					// 대기 중인 플레이어
					Player->NotificationText = FText::FromString
					(
						FString::Printf(TEXT("%s's Turn - Please wait (Your Attempts: %d/%d)"), 
						*PlayerState->PlayerName,
						EachPlayerState->CurrentGuessCount,
						EachPlayerState->MaxGuessCount)
					);
				}
			}
		}
		
		// 기존 타이머가 있으면 먼저 정리
		if (FTimerHandle* ExistingTimer = PlayerTurnTimers.Find(InCurrentPlayerController))
		{
			if (GetWorldTimerManager().IsTimerActive(*ExistingTimer))
			{
				GetWorldTimerManager().ClearTimer(*ExistingTimer);
			}
		}
		
		// 새 타이머 핸들 생성 및 추가
		FTimerHandle& TimerHandle = PlayerTurnTimers.FindOrAdd(InCurrentPlayerController);
		
		// 타이머 시작
		GetWorldTimerManager().SetTimer(TimerHandle, [this, InCurrentPlayerController]()
		{
			UpdateTurnTimer(InCurrentPlayerController);
		}, 1.f, true);
	}
}

void AMPGameModeBase::UpdateTurnTimer(AMPPlayerController* InCurrentPlayerController)
{
	ASSIGN_VALID(AMPPlayerState, PlayerState, InCurrentPlayerController->GetPlayerState<AMPPlayerState>())
	{
		// 1초씩 깎기
		PlayerState->TurnRemainingTime -= 1.f;

		// UI 갱신
		UpdateTurnUI(InCurrentPlayerController);
		
		if (PlayerState->TurnRemainingTime <= 0.f)
		{
			// 시간 초과 → 기회 소진
			if (!PlayerState->bHasGuessedThisTurn)
			{
				IncreaseGuessCount(InCurrentPlayerController);
				BroadcastChatMessageByRPC(FString::Printf(TEXT("%s's turn timed out!"), *PlayerState->PlayerName));
			}
			EndPlayerTurn(InCurrentPlayerController);
		}
	}
}

void AMPGameModeBase::UpdateTurnUI(AMPPlayerController* InCurrentPlayerController)
{
	ASSIGN_VALID(AMPPlayerState, PlayerState, InCurrentPlayerController->GetPlayerState<AMPPlayerState>())
	{
		// 현재 턴 플레이어만 시간 업데이트
		InCurrentPlayerController->NotificationText = FText::FromString
		(
			FString::Printf(TEXT("Your Turn! Time: %d sec (Attempts: %d/%d)"), 
			FMath::CeilToInt(PlayerState->TurnRemainingTime),
			PlayerState->CurrentGuessCount,
			PlayerState->MaxGuessCount)
		);
	}
}

void AMPGameModeBase::EndPlayerTurn(AMPPlayerController* InCurrentPlayerController)
{
	// 타이머 정리
	if (FTimerHandle* TimerHandle = PlayerTurnTimers.Find(InCurrentPlayerController))
	{
		if (GetWorldTimerManager().IsTimerActive(*TimerHandle))
		{
			GetWorldTimerManager().ClearTimer(*TimerHandle);
		}
		PlayerTurnTimers.Remove(InCurrentPlayerController);
	}
	
	// 다음 플레이어 턴 시작 로직
	ASSIGN_VALID(AMPPlayerController, NextPlayerController, GetNextPlayerController(InCurrentPlayerController))
	{
		CurrentTurnPlayer = NextPlayerController;
		StartPlayerTurn(NextPlayerController);
	}
}

AMPPlayerController* AMPGameModeBase::GetNextPlayerController(AMPPlayerController* InCurrentPlayerController)
{
	if (!InCurrentPlayerController) return nullptr;
	if (AllPlayerControllers.Num() == 0) return nullptr;

	int32 CurrentIndex = AllPlayerControllers.IndexOfByKey(InCurrentPlayerController);
	if (CurrentIndex == INDEX_NONE) return nullptr;

	// 다음 인덱스 계산 (마지막이면 0으로 돌아감)
	int32 NextIndex = (CurrentIndex + 1) % AllPlayerControllers.Num();

	return AllPlayerControllers[NextIndex];
}

bool AMPGameModeBase::IsGuessNumber(const FString& InNumber)
{
	// 문자열 길이 체크
	if (InNumber.Len() != 3) return false;
	
	TSet<TCHAR> UniqueDigits;
	
	// 각 문자가 1~9 사이의 숫자인지 체크 + 중복 체크
	for (TCHAR Num : InNumber)
	{
		// 숫자가 아니거나 0이면 false
		if (!(FChar::IsDigit(Num)) || Num == '0') return false;
		
		// 이미 있는 숫자면 중복 → false
		if (UniqueDigits.Contains(Num)) return false;
		
		// 고유 숫자 집합에 추가
		UniqueDigits.Add(Num);
	}

	return true;
}

FString AMPGameModeBase::JudgeResult(const FString& InSecretNumber, const FString& InGuessNumber)
{
	int32 StrikeCount = 0;
	int32 BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		// 같은 위치에 같은 숫자 = 스트라이크
		if (InSecretNumber[i] == InGuessNumber[i]) ++StrikeCount;

		// 다른 위치지만 포함되어 있으면 = 볼
		else if (InSecretNumber.Contains(FString(1, &InGuessNumber[i]))) ++BallCount;
	}

	// OUT 체크 (스트라이크/볼 모두 0)
	if (StrikeCount == 0 && BallCount == 0) return TEXT("OUT");

	// 결과 문자열 반환
	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
	
}

void AMPGameModeBase::PrintChatMessage(AMPPlayerController* InChattingPlayerController, const FString& InChatMessage)
{
	// 현재 턴인 플레이어가 아니면 무시
	if (CurrentTurnPlayer != InChattingPlayerController)
	{
		ASSIGN_VALID(AMPPlayerState, PlayerState, InChattingPlayerController->GetPlayerState<AMPPlayerState>())
		{
			InChattingPlayerController->ToClientRPC_PrintChatMessage(FString::Printf(TEXT("[%s] It's not your turn! Wait for your turn."), *PlayerState->PlayerName));
		}
		return;
	}
	
	constexpr int32 GuessLength = 3;
	
	// 입력 메시지에서 숫자 3자리 추출
	const int32 StartIndex = InChatMessage.Len() - GuessLength;
	const FString GuessNumber = InChatMessage.RightChop(StartIndex);
	FString FinalMessage = InChatMessage;

	// 유효한 숫자일 경우
	if (IsGuessNumber(GuessNumber))
	{
		// 결과 계산
		const FString Result = JudgeResult(SecretNumber, GuessNumber);

		// 최종 메시지 합치기
		FinalMessage = FString::Format(TEXT("{0} -> {1}"), { InChatMessage, Result });

		// 시도 횟수 증가
		IncreaseGuessCount(InChattingPlayerController);

		// 스트라이크 수로 게임 상태 판정
		const int32 StrikeCount = FCString::Atoi(*Result.Left(1));
	
		// 승리/무승부 처리
		JudgeGame(InChattingPlayerController, StrikeCount);
	}
	else
	{
		// 잘못된 입력이면 턴 유지
		InChattingPlayerController->ToClientRPC_PrintChatMessage(TEXT("Invalid input! Enter 3 digits (1-9, no duplicates)"));
		return;
	}

	// 턴 내 행동 완료 표시
	ASSIGN_VALID(AMPPlayerState, PlayerState, InChattingPlayerController->GetPlayerState<AMPPlayerState>())
	{
		PlayerState->bHasGuessedThisTurn = true;
	}

	// 최종 메시지 브로드캐스
	BroadcastChatMessageByRPC(FinalMessage);
}

void AMPGameModeBase::IncreaseGuessCount(AMPPlayerController* InChattingPlayerController)
{
	// 플레이어 시도 횟수 증가
	ASSIGN_VALID(AMPPlayerState, InChattingPlayerState, InChattingPlayerController->GetPlayerState<AMPPlayerState>())
	{
		++(InChattingPlayerState->CurrentGuessCount);
	}
}

void AMPGameModeBase::BroadcastChatMessageByRPC(const FString& Message)
{
	// 모든 플레이어에게 채팅 메시지 전송
	for (TActorIterator<AMPPlayerController> It(GetWorld()); It; ++It)
	{
		CAST_VALID(AMPPlayerController, MPPlayerController, *It)
		{
			MPPlayerController->ToClientRPC_PrintChatMessage(Message);
		}
	}
}

void AMPGameModeBase::JudgeGame(AMPPlayerController* InChattingPlayerController, int InStrikeCount)
{
	// 스트라이크 3개면 승리 처리
	if (InStrikeCount == 3)
	{
		HandleWin(InChattingPlayerController);
		return;
	}

	// 무승부 체크
	if (IsDraw())
	{
		BroadcastNotification(TEXT("Draw..."));
		
		// 3초 후에 게임 초기화 호출
		GetWorldTimerManager().SetTimer
		(
			TimerHandle_ResetGame,          
			this,                       
			&AMPGameModeBase::ResetGame, 
			3.0f,                      
			false                       
		);
	}
	else
	{
		// 승리 / 무승부 아니면 다음 플레이어 턴 진행
		EndPlayerTurn(InChattingPlayerController);
	}
}

bool AMPGameModeBase::IsDraw()
{
	for (const auto& PlayerController : AllPlayerControllers)
	{
		ASSIGN_VALID(AMPPlayerState, PlayerState, PlayerController->GetPlayerState<AMPPlayerState>())
		{
			if (PlayerState->CurrentGuessCount <= PlayerState->MaxGuessCount)
			{
				// 아직 시도 가능한 플레이어 있음
				return false;
			}
		}
	}

	// 모두 시도 다 함 → 무승부
	return true;
}

void AMPGameModeBase::HandleWin(AMPPlayerController* InWinnerController)
{
	ASSIGN_VALID(AMPPlayerState, WinnerState, InWinnerController->GetPlayerState<AMPPlayerState>())
	{
		// 승리 메시지 생성
		const FString Notification = FString::Printf(TEXT("%s has won the game. Ready for Next Round"), *WinnerState->PlayerName);

		// 모든 플레이어에게 알림
		BroadcastNotification(Notification);
		
		// 3초 후에 게임 초기화 호출
		GetWorldTimerManager().SetTimer
		(
			TimerHandle_ResetGame,          
			this,                       
			&AMPGameModeBase::ResetGame, 
			3.0f,                      
			false                       
		);
	}
}

void AMPGameModeBase::BroadcastNotification(const FString& InNotification) const
{
	const FText Notification = FText::FromString(InNotification);

	// 모든 플레이어에게 알림 메시지 전송
	for (const auto& PlayerController : AllPlayerControllers)
	{
		// NotificationText가 블루프린트 위젯과 바인딩되어 있으므로 UI가 자동 갱신
		PlayerController->NotificationText = Notification;
	}
}

void AMPGameModeBase::ResetGame()
{
	// 새로운 정답 숫자 생성
	SecretNumber = GenerateSecretNumber();

	// 모든 플레이어 시도 횟수 초기화 (1부터 시작)
	for (const auto& PlayerController : AllPlayerControllers)
	{
		PlayerController->NotificationText = FText::FromString(TEXT("Restart Game"));;
		
		ASSIGN_VALID(AMPPlayerState, EachPlayerState, PlayerController->GetPlayerState<AMPPlayerState>())
		{
			EachPlayerState->CurrentGuessCount = 1;
		}
	}

	// 첫 번째 플레이어부터 다시 턴 시작
	if (AllPlayerControllers.Num() > 0)
	{
		CurrentPlayerIndex = 0;
		CurrentTurnPlayer = AllPlayerControllers[0];
		StartPlayerTurn(CurrentTurnPlayer);
	}
}
