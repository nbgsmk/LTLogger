//
// Created by peca on 18-Jun-26.
//

#include "DataLogger.h"

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
  * @param  s1: Raw value from sensor 1
  * @param  s2: Raw value from sensor 2
  * @param  s3: Raw value from sensor 3
  * @retval 1 if successful, 0 if the buffer is completely full (Data Rejected)
  */
uint8_t DataLogger_Append(RTC_TimeTypeDef *rtc_time, RTC_DateTypeDef *rtc_date, uint16_t s1, uint16_t s2, uint16_t s3) {
	// 1. BOUNDARY LOCK: If buffer is completely full, abort immediately
	if (LogData.nextpos >= MAX_RECORDS) {
		// popunjeno!
		return 1; // Failure flag: Log rejected to protect historical tracking records
	}

	uint32_t idx = LogData.nextpos;
	LogData.data[idx].time = *rtc_time;
	LogData.data[idx].date = *rtc_date;

	LogData.data[idx].sens1 = s1;
	LogData.data[idx].sens2 = s2;
	LogData.data[idx].sens3 = s3;
	LogData.nextpos ++;

	if (LogData.nextpos == 6) {
		__NOP();
	}
	return 0; // no errors
}

