#ifndef DLOGGER_HPP
#define DLOGGER_HPP

#include <cstdint>
#include <cstddef>
#include <string>

namespace Logging {

#define NUM_SENSORS 3

	typedef struct {
		uint8_t rtcYear;
		uint8_t rtcMonth;
		uint8_t rtcDate;
		uint8_t rtcWeekday;
		uint8_t rtcHour;
		uint8_t rtcMinute;
		uint8_t rtcSecond;

		uint16_t sensors[NUM_SENSORS];
	} SingleLogRecord_t;

	class DLogger {
	private:
		SingleLogRecord_t* ramBuffer; // Reinterpreted pointer to start of memory
		size_t maxCapacity;           // Calculated max structures that fit
		size_t nextPos;

	public:
		// Constructor accepts plain byte buffer and its size in KB
		DLogger(uint8_t* publicRawBfr, size_t bufferSizeKB);

		void append(const SingleLogRecord_t& entry);

		// Retrieves a record from the raw RAM buffer by index.
		std::string  getRecordStr(uint32_t idx) const;


		uint32_t getMaxCapacity() const;
		uint32_t getNextPos() const;
		bool isFull() const;
		void flush();
		void reset();


	};

} // namespace Logging

#endif // DLOGGER_HPP
