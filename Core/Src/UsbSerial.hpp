//
// Created by mcu on 23/06/2026.
//

#ifndef LTLOGGER_USBSERIAL_H
#define LTLOGGER_USBSERIAL_H

#include <string_view>

class UsbSerial {
public:
	// Delete constructors to make this a purely static utility class
	UsbSerial() = delete;

	/**
	 * @brief Sends a string or string literal over USB CDC.
	 * @param message The text to transmit (accepts literals, std::string, etc.)
	 * @return true if transmission started successfully, false if busy
	 */
	static bool transmit(std::string_view message);

	/**
	 * @brief Sends a string and automatically appends a proper "\r\n" line break.
	 */
	static bool transmitLine(std::string_view message);

	/**
	 *
	 * @return Send \n\r literal
	 */
	static bool transmitNR();
};

#endif //LTLOGGER_USBSERIAL_H
