


#ifndef TYPEDBUFFER_H_
#define TYPEDBUFFER_H_

#include <new>
#include <algorithm>

#include "BinaryUtil.hxx"

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Represents a binary buffer containing an instance of a particular type
///
template<typename T, bool wordAlign>
class TypedBuffer_t
{
	public:
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Create a zero-initialized typed binary buffer
	///
	constexpr TypedBuffer_t() = default;
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Copy constructor
	///
	constexpr TypedBuffer_t(const TypedBuffer_t<T, wordAlign>& other) : TypedBuffer_t(other.getInstance())
	{}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Create a typed binary buffer containing a copy of a particular type instance
	/// 
	/// @param[in] instance The instance to copy into the binary buffer
	///
	constexpr TypedBuffer_t(const T& instance)
	{
		new(data_) T(instance);
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Destructor
	///
	~TypedBuffer_t() = default;
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Copy assignment operator
	///
	constexpr TypedBuffer_t& operator=(const TypedBuffer_t<T, wordAlign>& other)
	{
		operator=(other.getInstance());
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Copy a type instance into this typed binary buffer
	///
	/// @param[in] instance The instance to copy into the binary buffer
	///
	constexpr TypedBuffer_t& operator=(const T& instance)
	{
		new(data_) T(instance);
		return *this;
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Access the raw binary data of this binary buffer
	///
	constexpr uint8_t* getData()
	{
		return data_;
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Access the raw binary data of this binary buffer
	///
	constexpr const uint8_t* getData() const
	{
		return data_;
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Access the type instance contained inside this binary buffer
	///
	constexpr T& getInstance()
	{
		T* instancePtr = reinterpret_cast<T*>(data_);
		return *instancePtr;
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Access the type instance contained inside this binary buffer
	///
	constexpr const T& getInstance() const
	{
		const T* instancePtr = reinterpret_cast<const T*>(data_);
		return *instancePtr;
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the size of this typed binary buffer
	///
	constexpr uint32_t getSize() const
	{
		return bufferSize;
	}

	template<typename... ArgT>
	constexpr void emplace(const ArgT&... args)
	{
		new(data_) T(args...);
	}

	void deleteInstance()
	{
		getInstance().~T();
	}
	
	private:

	static constexpr uint32_t bufferAlignment = wordAlign ? std::max(alignof(T), alignof(AmpedUp::BinaryUtil::word_t)) : alignof(T);
	static constexpr uint32_t bufferSize = wordAlign ? AmpedUp::BinaryUtil::bytesFillWords(sizeof(T)) : sizeof(T);


	alignas(bufferAlignment) uint8_t data_[bufferSize]{0};
};

template<typename T>
using TypedBuffer = TypedBuffer_t<T, false>;

template<typename T>
using WordAlignedTypedBuffer = TypedBuffer_t<T, true>;


#endif /* TYPEDBUFFER_H_ */