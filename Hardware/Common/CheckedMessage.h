


#ifndef CHECKEDMESSAGE_H_
#define CHECKEDMESSAGE_H_

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Represents a structure to be sent/recieved over a serial communication protocol and calculates, stores, and
///        verifies a simple checksum to detect bit errors that occur between the sender and receiver
///
/// @note This structure is packed, as it is intented to have its raw binary representation sent over the wire
///
template<typename T>
class __attribute__((__packed__)) CheckedMessage
{
public:

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Construct a checked message containing a particular payload and generate the matching checksum
	///
	/// @param[in] payload the payload of the message
	///
	CheckedMessage(const T& payload) : payload_(payload)
	{
		checksum_ = computeChecksum();
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Check whether or not the checksum for this message matches the payload (for a received message)
	///
	bool isValid() const
	{
		return checksum_ == computeChecksum();
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the payload of this message
	///
	T getPayload() const
	{
		return payload_;
	}

private:
	using checksum_t = uint8_t;
	static constexpr checksum_t INITIAL_CHECKSUM = 0xFF;
	static constexpr uint32_t SIZE_IN_CHECKSUMS = sizeof(T) / sizeof(checksum_t);

	T payload_;
	checksum_t checksum_;
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Calculate the correct checksum for this message's payload
	///
	/// @return The checksum for the message's payload
	///
	checksum_t computeChecksum() const
	{
		// Start with an initial non-zero value (so that a message of all zeros is not valid)
		checksum_t checksum = INITIAL_CHECKSUM;
		
		// XOR the checksum with each byte of the payload
		const checksum_t* messageData = reinterpret_cast<const checksum_t*>(&payload_);
		for (uint32_t i = 0; i < SIZE_IN_CHECKSUMS; i++)
		{
			checksum ^= messageData[i];
		}
		
		return checksum;
	}
};


#endif /* CHECKEDMESSAGE_H_ */