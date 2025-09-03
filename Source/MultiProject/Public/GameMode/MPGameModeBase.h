#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MPGameModeBase.generated.h"

class AMPPlayerController;
/**
 * 
 */
UCLASS()
class MULTIPROJECT_API AMPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

//----------------------------------------------------------------------------------------------------------------------
	
#pragma region FUNCTION_Override
public:
	AMPGameModeBase();
	virtual void BeginPlay() override;
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
		
#pragma endregion
	
//----------------------------------------------------------------------------------------------------------------------

#pragma region FUNCTION
public:
	
	// 랜덤 숫자 3개 생성 
	static FString GenerateSecretNumber();

	// 턴 시작
	void StartPlayerTurn(AMPPlayerController* InCurrentPlayerController);

	// 턴 타이머 업데이트
	void UpdateTurnTimer(AMPPlayerController* InCurrentPlayerController);
	void UpdateTurnUI(AMPPlayerController* InCurrentPlayerController);

	// 턴 종료
	void EndPlayerTurn(AMPPlayerController* InCurrentPlayerController);

	// 다음 플레이어 차례
	AMPPlayerController* GetNextPlayerController(AMPPlayerController* InCurrentPlayerController);
	
	// 플레이어가 입력한 값이 게임 규칙에 맞는 3자리 추측 숫자인지 검사
	static bool IsGuessNumber(const FString& InNumber);

	// 숫자 야구 게임의 판정 함수
	static FString JudgeResult(const FString& InSecretNumber, const FString& InGuessNumber);

	// 결과 출력
	void PrintChatMessage(AMPPlayerController* InChattingPlayerController, const FString& InChatMessage);
	
	// 시도 가능 횟수 증가
	static void IncreaseGuessCount(AMPPlayerController* InChattingPlayerController);
	
	// 결과 브로드캐스트 (RPC)
	void BroadcastChatMessageByRPC(const FString& Message);

	// 승부 판정
	void JudgeGame(AMPPlayerController* InChattingPlayerController, int InStrikeCount);

	// 무승부 체크
	bool IsDraw();

	// 승리 시 
	void HandleWin(AMPPlayerController* InWinnerController);

	// 상단 UI 갱신용 (프로퍼티 레플리케이트)
	void BroadcastNotification(const FString& InNotification) const;
	
	// 게임 리셋
	void ResetGame();

#pragma endregion

//----------------------------------------------------------------------------------------------------------------------
	
#pragma region VARIABLE
protected:

	// 정답 숫자
	FString SecretNumber;
	
	// 플레이어 저장 컨테이너
	TArray<TObjectPtr<AMPPlayerController>> AllPlayerControllers;

private:

	// 현재 플레이어 
	UPROPERTY()
	AMPPlayerController* CurrentTurnPlayer;

	// 현재 플레이어 인덱스
	UPROPERTY()
	int32 CurrentPlayerIndex;
	
	// 리셋 게임 타이머 핸들
	FTimerHandle TimerHandle_ResetGame;
	
	// 플레이어 턴제 타이머 핸들 관리 컨테이너
	UPROPERTY()
	TMap<AMPPlayerController*, FTimerHandle> PlayerTurnTimers;

	// 턴 제한 시간 (초)
	float MaxTurnTime;

#pragma endregion 
};
