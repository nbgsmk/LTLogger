//
// Created by peca on 17-Jun-26.
//

#ifndef LTLOGGER_ZRTC_H
#define LTLOGGER_ZRTC_H

#include "main.h"

uint32_t zRTC_Get_BKPreg(uint32_t rtc_backup_reg);
void zRTC_Set_BKPreg(uint32_t rtc_backup_reg, uint32_t val);
void zRTC_GetTimeDateString(char *rezult_bfr, size_t rezult_bfr_size);
void zRTC_GetTimeDate(RTC_TimeTypeDef *rezTime, RTC_DateTypeDef *rezDate);

#endif //LTLOGGER_ZRTC_H
