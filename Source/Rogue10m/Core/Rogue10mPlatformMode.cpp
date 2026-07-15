// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mPlatformMode.h"

ERogue10mPlatformMode URogue10mPlatformModeLibrary::GetCurrentPlatformMode()
{
#if PLATFORM_WINDOWS
	return ERogue10mPlatformMode::WindowsDesktop;
#elif PLATFORM_ANDROID
	return ERogue10mPlatformMode::AndroidMobile;
#else
	return ERogue10mPlatformMode::Unknown;
#endif
}

bool URogue10mPlatformModeLibrary::IsDesktopMode()
{
	return GetCurrentPlatformMode() == ERogue10mPlatformMode::WindowsDesktop;
}

bool URogue10mPlatformModeLibrary::IsMobileMode()
{
	return GetCurrentPlatformMode() == ERogue10mPlatformMode::AndroidMobile;
}

FText URogue10mPlatformModeLibrary::GetCurrentPlatformModeText()
{
	switch (GetCurrentPlatformMode())
	{
	case ERogue10mPlatformMode::WindowsDesktop:
		return FText::FromString(TEXT("Windows 데스크탑"));
	case ERogue10mPlatformMode::AndroidMobile:
		return FText::FromString(TEXT("Android 모바일"));
	default:
		return FText::FromString(TEXT("알 수 없음"));
	}
}
