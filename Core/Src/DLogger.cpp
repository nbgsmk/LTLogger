#include "DLogger.hpp"

#include <format>

namespace Logging {

	DLogger::DLogger(uint8_t* publicRawBfr, size_t bufferSizeKB) {
		// Cast the plain byte address to our structured log record type
		ramBuffer = reinterpret_cast<SingleLogRecord_t*>(publicRawBfr);

		size_t totalBytes = bufferSizeKB * 1024;
		maxCapacity = totalBytes / sizeof(SingleLogRecord_t);

		nextPos = 0;
	}

	void DLogger::append(const SingleLogRecord_t& entry) {
		if (isFull()) {
			flush(); // Clear space by executing your hardware dump routine
		}
		ramBuffer[nextPos] = entry;
		nextPos = (nextPos + 1);
	}


	std::string DLogger::getRecordStr(uint32_t idx) const {
		// 1. Safety and boundary checks
		if (idx >= maxCapacity) {
			return "";
		}

		// 2. Retrieve into a local variable
		SingleLogRecord_t slr = ramBuffer[idx];

		// 3. Create a buffer for snprintf
		char buffer[256];

		uint8_t yy = slr.rtcYear;
		uint8_t mon = slr.rtcMonth;
		uint8_t dd = slr.rtcDate ;
		uint8_t weekday = slr.rtcWeekday;

		uint8_t hr = slr.rtcHour;
		uint8_t min = slr.rtcMinute;
		uint8_t sec = slr.rtcSecond;

		uint16_t s0 = slr.sensors[0];
		uint16_t s1 = slr.sensors[1];
		uint16_t s2 = slr.sensors[2];

		// -55 to +125.0625 => max 8 characters
		// char bfr[200] = {0};
		// snprintf(bfr, sizeof(bfr), "20%02d-%02d-%02dT%02d:%02d:%02d; (day %01d); sensors; %8u; %8u; %8u", yy, mon, dd, hr, min, sec, weekday, s0, s1, s2);

		std::string logLine = std::format(
			"20{:02d}-{:02d}-{:02d}T{:02d}:{:02d}:{:02d}; (day {:01d}); sensors; {:8d}; {:8d}; {:8d};\r\n",
			yy, mon, dd, hr, min, sec, weekday, s0, s1, s2
		);
		// 5. Return the constructed string
		return logLine;
	}

	uint32_t DLogger::getMaxCapacity() const {
		return maxCapacity;
	}

	uint32_t DLogger::getNextPos() const {
		return nextPos;
	}

	bool DLogger::isFull() const {
		return nextPos >= maxCapacity;
	}


	void DLogger::flush() {
		// while (!isEmpty()) {	TODO
		// }
		reset();
	}

	void DLogger::reset() {
		nextPos = 0;
	}



} // namespace Logging
