#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MPPawn.generated.h"

UCLASS()
class MULTIPROJECT_API AMPPawn : public APawn
{
	GENERATED_BODY()

public:
	AMPPawn();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
};
