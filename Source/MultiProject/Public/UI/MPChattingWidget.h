#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MPChattingWidget.generated.h"

class AMPPlayerController;
class UCanvasPanel;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class MULTIPROJECT_API UMPChattingWidget : public UUserWidget
{
	GENERATED_BODY()

//----------------------------------------------------------------------------------------------------------------------

#pragma region FUNCTION_Override 
public:
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

#pragma endregion

//----------------------------------------------------------------------------------------------------------------------
	
#pragma region FUNCTION
protected:

	/* _엔터 입력 시, 채팅 메시지 저장 및 출력_ */
	// 설명 : 엔터 입력 시, 컨트롤러의 SetChatMessage 호출
	UFUNCTION()
	void OnChatCommitted(const FText& ChatText, ETextCommit::Type CommitMethod);

	/* _위젯의 오너 플레이어 컨트롤러 반환_ */
	// 설명 : 위젯의 오너 플레이어 컨트롤러 반환하여 오너 플레이어 컨트롤러의 멤버 함수를 호출할 수 있게 해줌 
	AMPPlayerController* GetMPPlayerController();
	
#pragma endregion
	
//----------------------------------------------------------------------------------------------------------------------

#pragma region VARIABLE
public:
	
	// 캔버스 패널
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> ChattingCanvasPanel;

	// 채팅창 
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> ChattingEditableTextBox;
	
#pragma endregion 
};
