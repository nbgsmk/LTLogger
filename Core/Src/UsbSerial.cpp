//
// Created by mcu on 23/06/2026.
//

#include "UsbSerial.hpp"


#include "usbd_cdc_if.h" // Contains CDC_Transmit_FS
#include "usbd_cdc.h"    // Contains USB status definitions

bool UsbSerial::transmit(std::string_view message) {
	if (message.empty()) {
		return true;
	}
	uint32_t abortDelay = 2000;
	uint8_t usbStat = USBD_BUSY;

	// Modern C++ explicit pointer casting
	auto* dataPtr = reinterpret_cast<uint8_t*>(const_cast<char*>(message.data()));
	auto length = static_cast<uint16_t>(message.size());

	// uint8_t result = CDC_Transmit_FS(dataPtr, length);
	while (abortDelay > 0) {
		usbStat = CDC_Transmit_FS((uint8_t *) dataPtr, length);
		if (USBD_BUSY == usbStat) {
			abortDelay--;
			HAL_Delay(2); // sacekaj da USBD bude spreman
		} else {
			HAL_Delay(5); // throttle a little bit
			break;
		}
	}

	// Return true if the transmission was successfully scheduled
	return (usbStat == USBD_OK);
}

bool UsbSerial::transmitLine(std::string_view message) {
	// Transmit the main body of the message
	bool stat = transmit(message);
	if (stat == false) {
		return false;
	}
	return transmit("\n\r");
}

bool UsbSerial::transmitNR() {
	return transmit("\n\r");
}