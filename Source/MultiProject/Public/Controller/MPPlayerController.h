#pragma once

#include "CoreMinimal.h"
#include "Macros.h"
#include "GameFramework/PlayerController.h"
#include "MPPlayerController.generated.h"

class UMPNotificationText;
class UMPChattingWidget;
class UUserWidget;
/**
 * 
 */
UCLASS()
class MULTIPROJECT_API AMPPlayerController : public APlayerController
{
	GENERATED_BODY()

//----------------------------------------------------------------------------------------------------------------------
	
#pragma region FUNCTION_Override
public:

	AMPPlayerController();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	
#pragma endregion
	
//----------------------------------------------------------------------------------------------------------------------

#pragma region FUNCTION
public:
	
	/* _상단 알림창 설정_ */
	// 설명 : UI 생성 -> 화면에 ADD
	void SetNotificationText();
	
	/* _채팅창 설정_ */
	// 설명 : 인풋 모드 설정 -> UI 생성 -> 화면에 ADD
	void SetChattingMode();
	
	/* _채팅 메시지 저장 및 출력_ */
	// 설명 : 채팅 메시지 저장 -> PrintChatMessage 호출
	void SetChatMessage(const FString& InChatMessage);

	/* _채팅 메시지 출력_ */
	// 설명 : 화면에 메시지 출력
	FORCEINLINE void PrintChatMessage(const FString& InChatMessage) const
	{
		MacrosFunctionLibrary::PrintMessageWithNetMode(this, InChatMessage, 10.f);
	}
	
	/* _채팅 출력 요청 RPC_ */
	// 설명 : 화면에 메시지 출력 할 수 있게 서버로 요청
	UFUNCTION(Server, Reliable)
	void ToServerRPC_PrintChatMessage(const FString& InChatMessage);

	/* _채팅 출력 실행 RPC_ */
	// 설명 : 클라이언트들에게 화면에 메시지 출력하라고 호출
	UFUNCTION(Client, Reliable)
	void ToClientRPC_PrintChatMessage(const FString& InChatMessage);

#pragma endregion

//----------------------------------------------------------------------------------------------------------------------
	
#pragma region VARIABLE
protected:

	// 생성할 상단 알리참 설정 
	UPROPERTY(EditDefaultsOnly, Category = "Chatting | TSubclassOf")
	TSubclassOf<UMPNotificationText> NotificationTextWidgetClass;

	// 생성 후 상단 알림창 저장 포인터b
	UPROPERTY()
	TObjectPtr<UMPNotificationText> NotificationTextWidgetInstance;
	
	// 생성할 채팅창 위젯 설정 
	UPROPERTY(EditDefaultsOnly, Category = "Chatting | TSubclassOf")
	TSubclassOf<UMPChattingWidget> ChattingWidgetClass;

	// 생성 후 채팅창 저장 포인터
	UPROPERTY()
	TObjectPtr<UMPChattingWidget> ChattingWidgetInstance;

public:
	// 상단 알림창 메시지
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;
	
	// 채팅 메시지
	FString ChatMessage;
	
#pragma endregion 
};
