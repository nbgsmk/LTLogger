#include "ZRT.hpp"
#include "main.h"

#include <cstdio>

// ZRT implementation converted from the original C functions. This file provides
// a lightweight class that can be instantiated in C++ code (e.g., main.cpp).

uint32_t ZRT::GetBKPreg(uint32_t rtc_backup_reg) {
	RTC_HandleTypeDef hrtc = { .Instance = RTC };
	return HAL_RTCEx_BKUPRead(&hrtc, rtc_backup_reg);
}

void ZRT::SetBKPreg(uint32_t rtc_backup_reg, uint32_t val) {
	RTC_HandleTypeDef hrtc = { .Instance = RTC };
	HAL_PWR_EnableBkUpAccess();
	HAL_RTCEx_BKUPWrite(&hrtc, rtc_backup_reg, val);
	HAL_PWR_DisableBkUpAccess();
}

void ZRT::GetTimeDateString(char *rezult_bfr, size_t rezult_bfr_size) {
	RTC_HandleTypeDef hrtc = { .Instance = RTC };

	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	// 1. ALWAYS read TIME, DATE in that order! Use RTC_FORMAT_BIN to get standard decimal numbers
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	snprintf(rezult_bfr, rezult_bfr_size, "20%02d-%02d-%02dT%02d:%02d:%02d",
			sDate.Year, sDate.Month, sDate.Date,
			sTime.Hours, sTime.Minutes, sTime.Seconds);
	__NOP();
}

void ZRT::GetTimeDate(RTC_TimeTypeDef *rezTime, RTC_DateTypeDef *rezDate) {
	RTC_HandleTypeDef hrtc = { .Instance = RTC };

	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	// 1. ALWAYS read TIME, DATE in that order! Use RTC_FORMAT_BIN to get standard decimal numbers
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	*rezTime = sTime;
	*rezDate = sDate;
}
