//
// Created by peca on 18-Jun-26.
//

#ifndef LTLOGGER_DATALOGGER_H
#define LTLOGGER_DATALOGGER_H


#include "main.h" // Ensures access to HAL structures (RTC_HandleTypeDef, etc.)
#include <assert.h>

// --- Configuration ---
#define MAX_RECORDS 3000
#define MAX_BUFFER_KB 100

// --- Data Structures ---
typedef struct {
	RTC_DateTypeDef date;		// rtc date and time
	RTC_TimeTypeDef time;
	uint16_t sens1;				// data from sensor 1, 2...
	uint16_t sens2;
	uint16_t sens3;
} SingleLogRecord_t;
typedef struct {
	uint32_t nextpos;
	SingleLogRecord_t data[MAX_RECORDS];
} LogData_t;
// LogData_t LogData = {0};	// GLOBAL variable shows ram usage at compile time

// --- COMPILE TIME SAFETY CHECK ---

// Ensures your struct configuration never silently overruns your physical memory block limits
static_assert(sizeof(LogData_t) <= (MAX_BUFFER_KB * 1024), "CRITICAL ERROR: LogData_t struct exceeds the assigned Kilobyte RAM limit!");

// --- Public API Global Variables ---
// Extern definition makes the instance visible across your entire project
extern LogData_t LogData;


// --- Public Function Prototypes ---
void DataLogger_Init(void);
uint8_t DataLogger_Append(RTC_TimeTypeDef *rtc_time, RTC_DateTypeDef *rtc_date, uint16_t s1, uint16_t s2, uint16_t s3);


#endif //LTLOGGER_DATALOGGER_H
