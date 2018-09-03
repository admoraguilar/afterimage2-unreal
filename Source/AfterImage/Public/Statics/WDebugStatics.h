// Copyright 2014-2017 Frolic Games, All Rights Reserved.

#pragma once

#include "Public/Core/Object/WObject.h"
#include "WDebugStatics.generated.h"




#define QUICK_ON_SCREEN_DEBUG_MESSAGE(FMTTEXT) \
{ \
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT(FMTTEXT)); \
}
#define QUICK_ON_EDITOR_DEBUG_MESSAGE(FMTTEXT) \
{ \
	UE_LOG(LogTemp, Warning, TEXT(FMTTEXT)); \
}

#define QUICK_ON_SCREEN_DEBUG_MESSAGE_WITH_PARAMS(FMTTEXT, ...) \
{ \
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(FMTTEXT), ##__VA_ARGS__)); \
}
#define QUICK_ON_EDITOR_DEBUG_MESSAGE_WITH_PARAMS(FMTTEXT, ...) \
{ \
	UE_LOG(LogTemp, Warning, TEXT(FMTTEXT), ##__VA_ARGS__); \
}

#define ON_SCREEN_DEBUG_MESSAGE_WITH_PARAMS(KEY, DISPLAYTIME, DISPLAYCOLOR, FMTTEXT, ...) \
{ \
	GEngine->AddOnScreenDebugMessage(KEY, DISPLAYTIME, DISPLAYCOLOR, FString::Printf(TEXT(FMTTEXT), ##__VA_ARGS__)); \
}

#define ON_EDITOR_DEBUG_MESSAGE_WITH_PARAMS(LOGNAME, LOGTYPE, FMTTEXT, ...) \
{ \
	UE_LOG(LOGNAME, LOGTYPE, TEXT(FMTTEXT), ##__VA_ARGS__); \
}




/**
*
*/
UCLASS()
class AFTERIMAGE_API UWDebugStatics : public UWObject {
	GENERATED_BODY()

public:
	template<class TEnum> static FString GetEnumByteAsString(FString EnumName, TEnumAsByte<TEnum> Enum) {
		FString OutputText = "NULL";
		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);

		if (!EnumPtr) OutputText = NSLOCTEXT("Invalid", "Invalid", "Invalid").ToString();
		else OutputText = EnumPtr->GetDisplayNameText(Enum).ToString();

		return OutputText;
	}

	template<class TEnum> static FString GetEnumAsString(FString EnumName, TEnum Enum) {
		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
		if (!EnumPtr) return FString("Invalid");

		return EnumPtr->GetEnumName((int32)Enum);
	}
};

