#include "Public/Controller/MPPlayerController.h"

// 추가
#include "GameFramework/GameModeBase.h"
#include "GameMode/MPGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "MultiProject/Public/Macros.h"
#include "Net/UnrealNetwork.h"
#include "Player/MPPlayerState.h"
#include "UI/MPChattingWidget.h"
#include "UI/MPNotificationText.h"

AMPPlayerController::AMPPlayerController()
{
	bReplicates = true;
}

void AMPPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, NotificationText);
}

void AMPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 채팅창 설정
	SetChattingMode();

	// 상단 알림창 설정
	SetNotificationText();
}


void AMPPlayerController::SetNotificationText()
{
	CHECKF_NULL(NotificationTextWidgetClass);
	
	// 로컬 플레이어 확인
	if (!IsLocalController()) return;
	
	// 생성 후 화면에 붙이기
	NotificationTextWidgetInstance = CreateWidget<UMPNotificationText>(this, NotificationTextWidgetClass);
	CHECKF_NULL(NotificationTextWidgetInstance);
	NotificationTextWidgetInstance->AddToViewport();
}

void AMPPlayerController::SetChattingMode()
{
	CHECKF_NULL(ChattingWidgetClass);

	// 로컬 플레이어 확인
	if (!IsLocalController()) return;
	
	// 인풋 모드 설정 (UI 포커스 + 마우스 보이기)
	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);
	bShowMouseCursor = true;
	
	// 생성 후 화면에 붙이기
	ChattingWidgetInstance = CreateWidget<UMPChattingWidget>(GetWorld(), ChattingWidgetClass);
	CHECKF_NULL(ChattingWidgetInstance);
	ChattingWidgetInstance->AddToViewport();
}

void AMPPlayerController::SetChatMessage(const FString& InChatMessage)
{
	// 채팅 메시지 저장
	ChatMessage = InChatMessage;

	// 로컬 플레이어인지 확인
	if (!IsLocalController()) return;
	
	// 서버로 채팅 출력 요청
	ASSIGN_VALID(AMPPlayerState, MPPlayerState, GetPlayerState<AMPPlayerState>())
	{
		FString CombinedMessage = MPPlayerState->GetPlayerInfo() + TEXT(": ") + InChatMessage;
		ToServerRPC_PrintChatMessage(CombinedMessage);
	}
}


void AMPPlayerController::ToClientRPC_PrintChatMessage_Implementation(const FString& InChatMessage)
{
	PrintChatMessage(InChatMessage);
}

void AMPPlayerController::ToServerRPC_PrintChatMessage_Implementation(const FString& InChatMessage)
{
	ASSIGN_VALID(AGameModeBase, GameModeBase, UGameplayStatics::GetGameMode(this))
	{
		CAST_VALID(AMPGameModeBase, MPGameModeBase, GameModeBase)
		{
			MPGameModeBase->PrintChatMessage(this, InChatMessage);
		}
	}
}


