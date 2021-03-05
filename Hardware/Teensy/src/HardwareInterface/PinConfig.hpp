
#ifndef _PIN_CONFIG_
#define _PIN_CONFIG_

#include "PadConfig.hpp"

namespace AmpedUp
{

    class PinConfiguration
    {
    public:

    constexpr PinConfiguration(volatile uint32_t& muxReg, uint32_t muxRegValue) : muxRegPtr_(&muxReg), muxRegValue_(muxRegValue)
    {}

    constexpr PinConfiguration(volatile uint32_t& muxReg, uint32_t muxRegValue, volatile uint32_t& configReg, PadDriverConfiguration padConfig) :
            muxRegPtr_(&muxReg), muxRegValue_(muxRegValue), configRegPtr_(&configReg), configRegValue_(padConfig.getRegisterValue())
    {}

    constexpr PinConfiguration(volatile uint32_t& muxReg, uint32_t muxRegValue, volatile uint32_t& configReg, PadDriverConfiguration padConfig, volatile uint32_t& daisyReg, uint32_t daisyRegValue) :
            muxRegPtr_(&muxReg), muxRegValue_(muxRegValue), configRegPtr_(&configReg), configRegValue_(padConfig.getRegisterValue()), daisyRegPtr_(&daisyReg), daisyRegValue_(daisyRegValue)
    {}

    void apply()
    {
        if (muxRegPtr_ != nullptr)
        {
            *muxRegPtr_ = muxRegValue_;
        }

        if (configRegPtr_ != nullptr)
        {
            *configRegPtr_ = configRegValue_;
        }

        if (daisyRegPtr_ != nullptr)
        {
            *daisyRegPtr_ = daisyRegValue_;
        }
    }

    private:
        volatile uint32_t* muxRegPtr_{nullptr};
        uint32_t muxRegValue_{0};

        volatile uint32_t* configRegPtr_{nullptr};
        uint32_t configRegValue_{0};

        volatile uint32_t* daisyRegPtr_{nullptr};
        uint32_t daisyRegValue_{0};
    };

}

#endif