
#ifndef _PIN_CONFIG_
#define _PIN_CONFIG_

#include <cstdint>

namespace AmpedUp
{

    class PinConfig
    {
    public:

    PinConfig(uint32_t& muxReg, uint32_t muxRegValue) : muxRegPtr_(&muxReg), muxRegValue_(muxRegValue)
    {}

    PinConfig(uint32_t& muxReg, uint32_t muxRegValue, uint32_t& configReg, uint32_t configRegValue) :
            muxRegPtr_(&muxReg), muxRegValue_(muxRegValue), configRegPtr_(&configReg), configRegValue_(configRegValue)
    {}

    PinConfig(uint32_t& muxReg, uint32_t muxRegValue, uint32_t& configReg, uint32_t configRegValue, uint32_t& daisyReg, uint32_t daisyRegValue) :
            muxRegPtr_(&muxReg), muxRegValue_(muxRegValue), configRegPtr_(&configReg), configRegValue_(configRegValue), daisyRegPtr_(&daisyReg), daisyRegValue_(daisyRegValue)
    {}

    void initialize()
    {
        if (muxRegPtr_)
        {
            *muxRegPtr_ = muxRegValue_;
        }

        if (configRegPtr_)
        {
            *configRegPtr_ = configRegValue_;
        }

        if (daisyRegPtr_)
        {
            *daisyRegPtr_ = daisyRegValue_;
        }
    }

    private:
        uint32_t* muxRegPtr_{nullptr};
        uint32_t muxRegValue_{0};

        uint32_t* configRegPtr_{nullptr};
        uint32_t configRegValue_{0};

        uint32_t* daisyRegPtr_{nullptr};
        uint32_t daisyRegValue_{0};
    };

}

#endif