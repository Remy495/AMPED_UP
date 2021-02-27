


#ifndef CHECKEDMESSAGE_H_
#define CHECKEDMESSAGE_H_


template<typename T>
class CheckedMessage
{
public:

	CheckedMessage(const T& payload) : payload_(payload)
	{
		checksum_ = computeChecksum();
	}
	
	bool isValid() const
	{
		return checksum_ == computeChecksum();
	}
	
	const T& getPayload() const
	{
		return payload_;
	}

private:
	using checksum_t = uint8_t;
	static constexpr checksum_t INITIAL_CHECKSUM = 0xFF;
	static constexpr uint32_t SIZE_IN_CHECKSUMS = sizeof(T) / sizeof(checksum_t);

	checksum_t checksum_;
	T payload_;
	
	checksum_t computeChecksum() const
	{
		checksum_t checksum = INITIAL_CHECKSUM;
		
		checksum_t* messageData = reinterpret_cast<checksum_t*>(&payload_);
		for (uint32_t i = 0; i < SIZE_IN_CHECKSUMS; i++)
		{
			checksum ^= messageData[i];
		}
		
		return checksum;
	}
};


#endif /* CHECKEDMESSAGE_H_ */