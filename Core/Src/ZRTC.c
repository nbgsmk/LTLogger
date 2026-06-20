//
// Created by peca on 17-Jun-26.
//

#include "ZRTC.h"

#include <stdio.h>

uint32_t zRTC_Get_BKPreg(uint32_t rtc_backup_reg) {
	RTC_HandleTypeDef hrtc = {
		.Instance = RTC
	};
	return HAL_RTCEx_BKUPRead(&hrtc, rtc_backup_reg);
}

void zRTC_Set_BKPreg(uint32_t rtc_backup_reg, uint32_t val) {
	RTC_HandleTypeDef hrtc = {
		.Instance = RTC
	};
	HAL_PWR_EnableBkUpAccess();
	HAL_RTCEx_BKUPWrite(&hrtc, rtc_backup_reg, val);
	HAL_PWR_DisableBkUpAccess();
}

void zRTC_GetTimeDateString(char *rezult_bfr, size_t rezult_bfr_size) {
	RTC_HandleTypeDef hrtc = {
		.Instance = RTC
	};

	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	// 1. ALWAYS read TIME, DATE in that order! Use RTC_FORMAT_BIN to get standard decimal numbers
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	snprintf(rezult_bfr, rezult_bfr_size,  "%02d:%02d:%02d | 20%02d-%02d-%02d \n",
				sTime.Hours, sTime.Minutes, sTime.Seconds,
				sDate.Year, sDate.Month, sDate.Date);
	__NOP();
}

void zRTC_GetTimeDate(RTC_TimeTypeDef *rezTime, RTC_DateTypeDef *rezDate) {
	RTC_HandleTypeDef hrtc = {
		.Instance = RTC
	};

	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	// 1. ALWAYS read TIME, DATE in that order! Use RTC_FORMAT_BIN to get standard decimal numbers
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	*rezTime = sTime;
	*rezDate = sDate;
}
