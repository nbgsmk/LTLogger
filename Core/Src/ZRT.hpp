#ifndef ZRT_HPP
#define ZRT_HPP

#include "main.h"
#include <cstdint>
#include <cstddef>

class ZRT {
public:
    ZRT() = default;

    static uint32_t GetBKPreg(uint32_t rtc_backup_reg);
    static void SetBKPreg(uint32_t rtc_backup_reg, uint32_t val);

    static void GetTimeDateString(char *rezult_bfr, size_t rezult_bfr_size);
    static void GetTimeDate(RTC_TimeTypeDef *rezTime, RTC_DateTypeDef *rezDate);
};

#endif // ZRT_HPP
