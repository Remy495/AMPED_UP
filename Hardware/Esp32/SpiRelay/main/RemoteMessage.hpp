

#ifndef _REMOTE_MESSAGE_
#define _REMOTE_MESSAGE_

#include <cstring>
#include <algorithm>

#include "ConstantsCommon.hpp"
#include "BinaryUtil.hxx"
#include "CheckedMessage.h"
#include "TypedBuffer.h"
#include "RemoteMessageHeader.hpp"

namespace AmpedUp
{

class RemoteMessage
{
public:

    RemoteMessage() = default;

    ~RemoteMessage() = default;

    RemoteMessageHeader getHeader() const
    {
        return checkedHeader_.getInstance().getPayload();
    }

    void setHeader(const RemoteMessageHeader& header)
    {
        checkedHeader_ = header;
    }

    bool headerIsValid() const
    {
        // In order for header to be valid, the checksum must be OK and the fragment payload size must be less than the
        // maximum transmission unit.
        return checkedHeader_.getInstance().isValid() && getHeader().getFragmentPayloadSize() <= Constants::REMOTE_MESSAGE_MTU;
    }

    uint8_t* getHeaderData()
    {
        return checkedHeader_.getData();
    }

    const uint8_t* getHeaderData() const
    {
        return checkedHeader_.getData();
    }

    uint8_t* getPayloadData()
    {
        return payloadData_;
    }

    const uint8_t* getPayloadData() const
    {
        return payloadData_;
    }

    void setPayloadData(const uint8_t* payloadData, uint32_t size)
    {
        size = std::min(size, Constants::REMOTE_MESSAGE_MTU);
        memcpy(payloadData_, payloadData, size);
    }

private:
    alignas(BinaryUtil::word_t) TypedBuffer<CheckedMessage<RemoteMessageHeader>> checkedHeader_{RemoteMessageHeader(RemoteMessageType::NOTHING_TO_REPORT)};
    alignas(BinaryUtil::word_t) uint8_t payloadData_[Constants::REMOTE_MESSAGE_MTU]{};

};

}

#endif