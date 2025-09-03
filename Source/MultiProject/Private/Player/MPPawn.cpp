#include "Player/MPPawn.h"

// 추가됨
#include "Macros.h"

AMPPawn::AMPPawn()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMPPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRole = MacrosFunctionLibrary::ToStringNetRole(this);
	FString CombinedString = FString::Printf(TEXT("MPPawn::BeginPlay() %s [%s]"), *MacrosFunctionLibrary::ToStringNetMode(this), *NetRole);
	MacrosFunctionLibrary::PrintMessageWithNetMode(this, CombinedString, 10.f);
}

void AMPPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	FString NetRole = MacrosFunctionLibrary::ToStringNetRole(this);
	FString CombinedString = FString::Printf(TEXT("MPPawn::PossessedBy() %s [%s]"), *MacrosFunctionLibrary::ToStringNetMode(this), *NetRole);
	MacrosFunctionLibrary::PrintMessageWithNetMode(this, CombinedString, 10.f);
	
}
