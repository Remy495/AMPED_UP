

#ifndef PINCONFIGGROUP_H_
#define PINCONFIGGROUP_H_

#include <cstdint>

#include "PinConfig.h"

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Represents a group of related pin configurations and provides an interface for applying them in bulk
///
template<uint32_t size>
struct PinConfigGroup
{
public:

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Create an empty pin group
	///
	constexpr PinConfigGroup() = default;
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Destructor
	///
	~PinConfigGroup() = default;
	
	// Note: This class is intended to be constructed using aggregate initialization
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Apply one or more of the pin configurations in this group
	/// 
	/// @param[in] pinNumber... The indices of ths pins to configure
	///
	template<typename... Vargs>
	inline void enablePins(uint32_t pinNumber, Vargs... others) const
	{
		pins[pinNumber].apply();
		enablePins(others...);
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Apply all of the pin configurations in this group
	///
	inline void enableAllPins() const
	{
		for(int i = 0; i < size; i++)
		{
			pins[i].apply();
		}
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the number of pin configurations in this group
	///
	constexpr uint32_t getSize() const
	{
		return size;
	}
	
	PinConfiguration pins[size];
	
private:

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Base cast of enablePins()
	///
	inline void enablePins() const
	{
		// This function doesn't need to do anything, it is just the base case of the enablePins recursive template
	}
};


#endif /* PINCONFIGGROUP_H_ */