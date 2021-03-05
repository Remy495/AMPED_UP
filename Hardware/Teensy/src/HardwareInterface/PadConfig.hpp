


#ifndef _PAD_CONFIG_
#define _PAD_CONFIG_

#include <cstdint>
#include "imxrt.h"

enum class InputPullupMode : uint32_t
{
    NONE,
    KEEPER,
    PULL_UP_22K,
    PULL_UP_47K,
    PULL_UP_100K,
    PULL_DOWN_100K
};

enum class DriveSpeed : uint32_t
{
    LOW_50MHZ = 0,
    MEDIUM_100MHZ,
    HIGH_150MHZ,
    MAX_200MHZ
};

enum class OutputImpedance : uint32_t
{
    DRIVE_DISABLED = 0,
    DRIVE_150_OHMS,
    DRIVE_75_OHMS,
    DRIVE_50_OHMS,
    DRIVE_37_OHMS,
    DRIVE_30_OHMS,
    DRIVE_25_OHMS,
    DRIVE_21_OHMS
};

enum class SlewRate : uint32_t
{
    SLOW,
    FAST
};


//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Represents  
///
struct PadDriverConfiguration
{
public:

    //////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Create a configuration for a pad driver
    ///
    /// @param[in] hysteresisEnabled Whether the pad's input path should act as a normal CMOS trigger (ie. threshold at
    ///                              1.8 volts) or a hysteresis trigger (which remembers its current state and is
    ///                              "reluctang" to switch logic values)
    /// @param[in] pullupMode The configuration for the pullup resistor on the pad's input path
    /// @param[in] outputImpedance The output impedance of the pad's output path
    /// @param[in] speed Optomize the pad to switch logic values at a particular rate (in terms of noise and power
    ///                  management)
    /// @param[in] slewRate Whether the pad should be optomized for frequencies on the higher end or the lower end of
    ///                     the speed parameter
    /// @param[in] openDrainEnabled Whether the output path of the pad should act as a normal output or an open drain
    ///                             output (i.e. a current sink but not a current source)
    ///
    constexpr PadDriverConfiguration(bool hysteresisEnabled, InputPullupMode pullupMode, OutputImpedance outputImpedance = OutputImpedance::DRIVE_37_OHMS, DriveSpeed speed = DriveSpeed::HIGH_150MHZ, SlewRate slewRate = SlewRate::SLOW, bool openDrainEnabled = false)
    {

        // Set hysteresis bit
        if (hysteresisEnabled)
        {
            padConfig_ |= IOMUXC_PAD_HYS;
        }

        // Set pullup / keeper bits
        switch (pullupMode)
        {
        case InputPullupMode::KEEPER:
            padConfig_ |= IOMUXC_PAD_PKE;
            break;
        case InputPullupMode::PULL_UP_22K:
            padConfig_ |= IOMUXC_PAD_PKE | IOMUXC_PAD_PUE | IOMUXC_PAD_PUS(3);
            break;
        case InputPullupMode::PULL_UP_47K:
            padConfig_ |= IOMUXC_PAD_PKE | IOMUXC_PAD_PUE | IOMUXC_PAD_PUS(1);
            break;
        case InputPullupMode::PULL_UP_100K:
            padConfig_ |= IOMUXC_PAD_PKE | IOMUXC_PAD_PUE | IOMUXC_PAD_PUS(2);
            break;
        case InputPullupMode::PULL_DOWN_100K:
            padConfig_ |= IOMUXC_PAD_PKE | IOMUXC_PAD_PUE | IOMUXC_PAD_PUS(0);
            break;
        default:
            // No pullup or keeper
            break;
        }

        // Set output impedance bits
        padConfig_ |= IOMUXC_PAD_DSE(static_cast<uint32_t>(outputImpedance));

        // Set the speed bits
        padConfig_ |= IOMUXC_PAD_SPEED(static_cast<uint32_t>(speed));

        // Set slew rate bit
        if (slewRate == SlewRate::FAST)
        {
            padConfig_ |= IOMUXC_PAD_SRE;
        }

        // Set open drain bit
        if (openDrainEnabled)
        {
            padConfig_ |= IOMUXC_PAD_ODE;
        }
    }

    constexpr uint32_t getRegisterValue()
    {
        return padConfig_;
    }

private:
    uint32_t padConfig_{0};
};

#endif