

#ifndef _I2C_CONFIG_
#define _I2C_CONFIG_

#include "imxrt.h"

#include "PinConfigGroup.h"

namespace AmpedUp
{

constexpr uint8_t I2C_COUNT = 3;

enum class I2cHandle
{
    I2C_1,
    // Note: I2C 2 is not exposed by the Teensy 4.1 hardware
    I2C_3,
    I2C_4
};

constexpr uint8_t I2C_PIN_COUNT = 2;
using I2cPinGroup = PinConfigGroup<I2C_PIN_COUNT>;

constexpr PadDriverConfiguration I2C_PAD_CONFIG(true, InputPullupMode::PULL_UP_22K, OutputImpedance::DRIVE_37_OHMS, DriveSpeed::MEDIUM_100MHZ, SlewRate::SLOW, true);

template<I2cHandle HANDLE>
struct I2cConfig;

template<>
struct I2cConfig<I2cHandle::I2C_1>
{
    static inline IMXRT_LPI2C_t* registers_ = &IMXRT_LPI2C1;
    static inline volatile uint32_t* clockConfigRegister_ = &CCM_CCGR2;
    static constexpr uint32_t clockConfigValue_ = CCM_CCGR2_LPI2C1(CCM_CCGR_ON);
    static constexpr IRQ_NUMBER_t interruptId_ = IRQ_LPI2C1;
    static inline I2cPinGroup pins_
    {
        PinConfiguration(IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_01, 3U | 0x10U, IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_01, I2C_PAD_CONFIG, IOMUXC_LPI2C1_SDA_SELECT_INPUT, 1), // 18
        PinConfiguration(IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_00, 3U | 0x10U, IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_00, I2C_PAD_CONFIG, IOMUXC_LPI2C1_SCL_SELECT_INPUT, 1)  // 19
    };

};

template<>
struct I2cConfig<I2cHandle::I2C_3>
{
    static inline IMXRT_LPI2C_t* registers_ = &IMXRT_LPI2C3;
    static inline volatile uint32_t* clockConfigRegister_ = &CCM_CCGR2;
    static constexpr uint32_t clockConfigValue_ = CCM_CCGR2_LPI2C3(CCM_CCGR_ON);
    static constexpr IRQ_NUMBER_t interruptId_ = IRQ_LPI2C3;
    static inline I2cPinGroup pins_
    {
        PinConfiguration(IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_06, 1U | 0x10U, IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_06, I2C_PAD_CONFIG, IOMUXC_LPI2C3_SDA_SELECT_INPUT, 2), // 17
        PinConfiguration(IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_07, 1U | 0x10U, IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_07, I2C_PAD_CONFIG, IOMUXC_LPI2C3_SCL_SELECT_INPUT, 2)  // 16
    };

};

template<>
struct I2cConfig<I2cHandle::I2C_4>
{
    static inline IMXRT_LPI2C_t* registers_ = &IMXRT_LPI2C4;
    static inline volatile uint32_t* clockConfigRegister_ = &CCM_CCGR6;
    static constexpr uint32_t clockConfigValue_ = CCM_CCGR6_LPI2C4_SERIAL(CCM_CCGR_ON);
    static constexpr IRQ_NUMBER_t interruptId_ = IRQ_LPI2C4;
    static inline I2cPinGroup pins_
    {
        PinConfiguration(IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_13, 0U | 0x10U, IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_13, I2C_PAD_CONFIG, IOMUXC_LPI2C4_SDA_SELECT_INPUT, 1), // 25
        PinConfiguration(IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_12, 0U | 0x10U, IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_12, I2C_PAD_CONFIG, IOMUXC_LPI2C4_SCL_SELECT_INPUT, 1)  // 24
    };

};

}

#endif