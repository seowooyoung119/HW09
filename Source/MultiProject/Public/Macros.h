#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(MultiMacros, Log, All);

// 포인터 체크 매크로
#define CHECKF_NULL(Ptr) checkf(Ptr != nullptr, TEXT(#Ptr " is null! Assign it in Blueprint or Defaults."))
#define ENSURE_NULL(Ptr) ensureMsgf(Ptr != nullptr, TEXT(#Ptr " is null! Assign it in Blueprint or Defaults."))

// if Valid -> return 체크 매크로
#define IF_NOT_VALID_JUST_RETURN(Ptr)  if (!IsValid(Ptr)) { UE_LOG(MultiMacros, Error, TEXT(#Ptr " is null!")); return;} // (반환값 없음)
#define IF_NOT_VALID_RETURN_NULL(Ptr) if (!IsValid(Ptr)) { UE_LOG(MultiMacros, Error, TEXT(#Ptr " is null!")); return nullptr; } // (반환값 있음 : null)
#define IF_NOT_VALID_RETURN_INT32(Ptr, InInt32) if (!IsValid(Ptr)) { UE_LOG(MultiMacros, Error, TEXT(#Ptr " is null!")); return InInt32; } // (반환값 있음 : Int32)
#define IF_NOT_VALID_RETURN_FLOAT(Ptr, InFloat) if (!IsValid(Ptr)) { UE_LOG(MultiMacros, Error, TEXT(#Ptr " is null!")); return InFloat; } // (반환값 있음 : Float)
#define IF_NOT_VALID_RETURN_STRING(Ptr, InString) if (!IsValid(Ptr)) { UE_LOG(MultiMacros, Error, TEXT(#Ptr " is null!")); return InString; } // (반환값 있음 : string)

// cast valid 매크로
#define CAST_VALID(Type, VarName, Object) if (Type* VarName = Cast<Type>(Object); IsValid(VarName)) // Object를 Type 유효성 검사 후 VarName로 가져옴

// assign valid 매크로
#define ASSIGN_VALID(Type, VarName, Expr) if (Type* VarName = (Expr); IsValid(VarName)) // Expr 로 가져와서 Type 타입의 VarName로 가져오고 유효성 검사

// 디버깅 매크로
class MacrosFunctionLibrary
{
public:
	
	static void PrintMessageWithNetMode(const AActor* InWorldContextActor, const FString& InString, float InTimeToDisplay = 1.f, FColor InColor = FColor::Cyan)
	{
		IF_NOT_VALID_JUST_RETURN(GEngine);
		IF_NOT_VALID_JUST_RETURN(InWorldContextActor);

		switch (ENetMode NetMode = InWorldContextActor->GetNetMode())
		{
		case NM_Standalone:
			{
				FString StandaloneInString = FString::Printf(TEXT("NM_Standalone : %s"), *InString);
				GEngine->AddOnScreenDebugMessage(-1, InTimeToDisplay, InColor, StandaloneInString);
				UE_LOG(MultiMacros, Log, TEXT("%s"), *StandaloneInString);
				break;
			}
		case NM_Client:
			{
				FString ClientInString = FString::Printf(TEXT("NM_Client : %s"), *InString);
				GEngine->AddOnScreenDebugMessage(-1, InTimeToDisplay, InColor, ClientInString);
				UE_LOG(MultiMacros, Log, TEXT("%s"), *ClientInString);
				break;
			}
		case NM_ListenServer:
			{
				FString ServerInString = FString::Printf(TEXT("NM_ListenServer : %s"), *InString);
				GEngine->AddOnScreenDebugMessage(-1, InTimeToDisplay, InColor, ServerInString);
				UE_LOG(MultiMacros, Log, TEXT("%s"), *ServerInString);
				break;
			}
		
		case NM_DedicatedServer:
			{
				FString DedicatedInString = FString::Printf(TEXT("NM_DedicatedServer : %s"), *InString);
				UE_LOG(MultiMacros, Log, TEXT("%s"), *DedicatedInString);
				break;
			}
		default:
			{
				UE_LOG(MultiMacros, Log, TEXT("Can't Find NodeMode Anything"));
				break;
			}
		}
	}

	static FString ToStringNetMode(const AActor* InWorldContextActor)
	{
		FString NetModeString = TEXT("None");

		IF_NOT_VALID_RETURN_INT32(InWorldContextActor, NetModeString);

		switch (ENetMode NetMode = InWorldContextActor->GetNetMode())
		{
		case NM_Client:
			NetModeString = TEXT("[Client]");
			break;
        
		case NM_Standalone:
			NetModeString = TEXT("[StandAlone]");
			break;
        
		case NM_ListenServer:
			NetModeString = TEXT("[ListenServer]");
			break;
        
		case NM_DedicatedServer:
			NetModeString = TEXT("[DedicatedServer]");
			break;
        
		default:
			NetModeString = TEXT("[Unknown]");
			break;
		}

		return NetModeString;
	}

	static FString ToStringNetRole(const AActor* InActor)
	{
		FString NetRoleString = TEXT("None");

		IF_NOT_VALID_RETURN_STRING(InActor, NetRoleString);

		FString LocalRole = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), InActor->GetLocalRole());
		FString RemoteRole = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), InActor->GetRemoteRole());

		NetRoleString = FString::Printf(TEXT("[L] %s : [R] : %s"), *LocalRole, *RemoteRole);

		return NetRoleString;
	}
};