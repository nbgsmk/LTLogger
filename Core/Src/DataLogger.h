//
// Created by peca on 18-Jun-26.
//

#ifndef LTLOGGER_DATALOGGER_H
#define LTLOGGER_DATALOGGER_H


#include "main.h" // Ensures access to HAL structures (RTC_HandleTypeDef, etc.)
#include <assert.h>

// --- Configuration ---
#define MAX_RAM_RECORDS 4500
#define MAX_RAM_BUFFER_KB 64
#define NUM_SENSORS 3		// za sada tacno tri!!
static_assert(NUM_SENSORS == 3, "Nije implementirano nista drugo, samo tri");

// --- Data Structures ---
typedef struct {				// __attribute__((packed))
	uint8_t rtcYear;
	uint8_t rtcMonth;
	uint8_t rtcDate;
	uint8_t rtcWeekday;
	uint8_t rtcHour;
	uint8_t rtcMinute;
	uint8_t rtcSecond;			// nema potrebe za ovolikom preciznoscu

	uint16_t sensors[NUM_SENSORS];		// data from sensor 1, 2...
} SingleLogRecord_t;
static_assert(MAX_RAM_RECORDS * sizeof(SingleLogRecord_t) <= (MAX_RAM_BUFFER_KB * 1024), "CRITICAL: SingleLogRecord_t[MAX_RECORDS] will exceed the assigned Kilobyte RAM limit!");

typedef struct {
	uint32_t nextpos;
	SingleLogRecord_t data[MAX_RAM_RECORDS];
} LogData_t;		// Mora biti GLOBAL varijabla da bi se video ram usage at compile time


// Extern definition makes the instance visible across your entire project
extern LogData_t LogData;


// --- Public Function Prototypes ---
void DataLogger_Init(void);
uint8_t DataLogger_Append(RTC_TimeTypeDef *rtc_time, RTC_DateTypeDef *rtc_date, const uint16_t sensorsArray[], size_t sensorsArray_size);
uint8_t DataLogger_GetRecordString(char *out_str, size_t out_str_len, uint32_t idx);

#endif //LTLOGGER_DATALOGGER_H
