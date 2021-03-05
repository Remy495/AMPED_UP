


#ifndef TYPEDBUFFER_H_
#define TYPEDBUFFER_H_

#include <new>

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Represents a binary buffer containing an instance of a particular type
///
template<typename T>
class TypedBuffer
{
	public:
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Create a zero-initialized typed binary buffer
	///
	constexpr TypedBuffer() = default;
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Copy constructor
	///
	constexpr TypedBuffer(const TypedBuffer<T>& other) : TypedBuffer(other.getInstance())
	{}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Create a typed binary buffer containing a copy of a particular type instance
	/// 
	/// @param[in] instance The instance to copy into the binary buffer
	///
	constexpr TypedBuffer(const T& instance)
	{
		new(data_) T(instance);
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Destructor
	///
	~TypedBuffer() = default;
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Copy assignment operator
	///
	constexpr TypedBuffer& operator=(const TypedBuffer<T>& other)
	{
		operator=(other.getInstance());
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Copy a type instance into this typed binary buffer
	///
	/// @param[in] instance The instance to copy into the binary buffer
	///
	constexpr TypedBuffer& operator=(const T& instance)
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
		return sizeof(T);
	}
	
	private:
	alignas(T) uint8_t data_[sizeof(T)]{0};
};


#endif /* TYPEDBUFFER_H_ */