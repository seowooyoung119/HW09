#include "UI/MPChattingWidget.h"

// 추가
#include "Components/EditableTextBox.h"
#include "Controller/MPPlayerController.h"
#include "Macros.h"

void UMPChattingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 바인딩
	if (!ChattingEditableTextBox->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatCommitted))
	{
		ChattingEditableTextBox->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatCommitted);		
	}	
}

void UMPChattingWidget::NativeDestruct()
{
	Super::NativeDestruct();

	// 언바인딩
	if (ChattingEditableTextBox->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatCommitted))
	{
		ChattingEditableTextBox->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatCommitted);		
	}	
}

void UMPChattingWidget::OnChatCommitted(const FText& ChatText, ETextCommit::Type CommitMethod)
{
	// 입력이 들어왔을 때 
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// 플레이어 컨트롤러 가져오기
		ASSIGN_VALID(AMPPlayerController, MPlayerController, GetMPPlayerController())
		{
			// 채팅 메시지 저장 + [출력]
			MPlayerController->SetChatMessage(ChatText.ToString());

			// 입력 초기화
			ChattingEditableTextBox->SetText(FText::GetEmpty());
		}
	}
}

AMPPlayerController* UMPChattingWidget::GetMPPlayerController()
{
	ASSIGN_VALID(APlayerController, OwningPlayerController, GetOwningPlayer())
	{
		CAST_VALID(AMPPlayerController, OwningMPlayerController, OwningPlayerController)
		{
			return OwningMPlayerController;
		}
	}
	return nullptr;
}