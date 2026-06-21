//
// Created by peca on 18-Jun-26.
//

#include <stdio.h>

#include "DataLogger.h"

#include <string.h>


// --- Global Memory Allocation ---
// Defining this variable globally allocates exactly ~60,008 bytes in RAM (.bss section)
LogData_t LogData = {0};

/**
  * @brief  Initializes data logging tracking counters.
  * @retval None
  */
void DataLogger_Init(void) {
	LogData.nextpos = 0;
}


/**
  * @brief  Writes a new snapshot record linearly to the non-circular log buffer.
  * @param  sensorsArray: raw values from all sensors
  * @param  sensorsArray_size: number of sensors connected
  * @retval 1 if successful, 0 if the buffer is completely full (Data Rejected)
  */
uint8_t DataLogger_Append(RTC_TimeTypeDef *rtc_time, RTC_DateTypeDef *rtc_date, const uint16_t sensorsArray[], size_t sensorsArray_size) {
	if (LogData.nextpos > MAX_RAM_RECORDS) {
		// popunjeno!
		return 1; // Failure flag
	}

	uint32_t idx = LogData.nextpos;
	LogData.data[idx].rtcHour = rtc_time->Hours;
	LogData.data[idx].rtcMinute = rtc_time->Minutes;
	LogData.data[idx].rtcSecond = rtc_time->Seconds;

	LogData.data[idx].rtcYear = rtc_date->Year;
	LogData.data[idx].rtcMonth = rtc_date->Month;
	LogData.data[idx].rtcDate = rtc_date->Date;
	LogData.data[idx].rtcWeekday = rtc_date->WeekDay;

	for (int i = 0; i < sensorsArray_size; ++i) {
		LogData.data[idx].sensors[i] = sensorsArray[i];
	}
	LogData.nextpos ++;

	if (LogData.nextpos == 6) {
		__NOP();
	}
	return 0; // no errors
}



uint8_t DataLogger_GetRecordString(char *out_str, size_t out_str_len, uint32_t idx) {
	if (idx >= LogData.nextpos) {
		return 1;
	}
	uint8_t hr = LogData.data[idx].rtcHour;
	uint8_t min = LogData.data[idx].rtcMinute;
	uint8_t sec = LogData.data[idx].rtcSecond;

	uint8_t yy = LogData.data[idx].rtcYear;
	uint8_t mon = LogData.data[idx].rtcMonth;
	uint8_t dd = LogData.data[idx].rtcDate ;
	uint8_t weekday = LogData.data[idx].rtcWeekday;


	uint16_t s0 = LogData.data[idx].sensors[0];
	uint16_t s1 = LogData.data[idx].sensors[1];
	uint16_t s2 = LogData.data[idx].sensors[2];

	// -55 to +125.0625 => max 8 characters
	snprintf(out_str, out_str_len, "20%02d-%02d-%02dT%02d:%02d:%02d; (day %01d); sensors; %8u; %8u; %8u;\r\n", yy, mon, dd, hr, min, sec, weekday, s0, s1, s2);

	return 0;
}