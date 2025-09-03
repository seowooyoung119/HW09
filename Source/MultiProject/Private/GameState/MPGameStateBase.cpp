#include "GameState/MPGameStateBase.h"

// 추가됨
#include "Kismet/GameplayStatics.h"
#include "Controller/MPPlayerController.h"
#include "Macros.h"

void AMPGameStateBase::MulticastRPCBroadcastLogin_Implementation(const FString& InName)
{
	if (HasAuthority()) return;

	ASSIGN_VALID(APlayerController, LocalPlayer, UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		CAST_VALID(AMPPlayerController, LocalClient, LocalPlayer)
		{
			FString Notification = InName + TEXT(" has joined the game.");
			LocalClient->PrintChatMessage(Notification);
		}
	}
}
