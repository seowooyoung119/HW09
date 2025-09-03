#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "MPNotificationText.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPROJECT_API UMPNotificationText : public UUserWidget
{
	GENERATED_BODY()

public:
	
	// 캔버스 패널
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> NotificationCanvasPanel;

	// 채팅창 
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> NotificationTextBlock;
};
