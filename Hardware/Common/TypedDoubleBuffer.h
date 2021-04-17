

#ifndef TYPEDDOUBLEBUFFER_H_
#define TYPEDDOUBLEBUFFER_H_

#include "TypedBuffer.h"
#include "StaticBuffer.hxx"

namespace AmpedUp
{

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Represents a double-buffer composed of two versions of a particular buffer type which can be used to lighten the synchronization burden between a reader and a writer.
/// @details Maintains two buffers, one for the reader and one for the writer, which can be swapped once both sides have finished their work (thus letting
///          the reader see what the writer just wrote and giving the writer a fresh buffer to write to). This is useful because it reduces conflict between
///          the reader and the writer to a single moment: when the buffers are swapped.
///
template<typename T>
class DoubleBuffer_t
{
public:

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Create a zero-initialized double buffer
	///
	constexpr DoubleBuffer_t() = default;
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Destructor
	///
	~DoubleBuffer_t() = default;
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Access the buffer currently assigned to the reader
	/// 
	/// @note: Do not hold onto the reference returned by this function outside of a synchronized context, as the current reader buffer will become the next writer buffer the next time the buffers are swapped.
	///
	constexpr T& getReaderBuffer()
	{
		return isSwapped_ ? buf1_ : buf2_;
	}

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Access the buffer currently assigned to the reader
	///
	/// @note: Do not hold onto the reference returned by this function outside of a synchronized context, as the current reader buffer will become the next writer buffer the next time the buffers are swapped.
	///
	constexpr const T& getReaderBuffer() const
	{
		return isSwapped_ ? buf1_ : buf2_;
	}

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Access the buffer currently assigned to the writer
	///
	/// @note: Do not hold onto the reference returned by this function outside of a synchronized context, as the current writer buffer will become the next reader buffer the next time the buffers are swapped.
	///
	constexpr T& getWriterBuffer()
	{
		return isSwapped_ ? buf2_ : buf1_;
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Access the buffer currently assigned to the writer
	///
	/// @note: Do not hold onto the reference returned by this function outside of a synchronized context, as the current writer buffer will become the next reader buffer the next time the buffers are swapped.
	///
	constexpr const T& getWriterBuffer() const
	{
		return isSwapped_ ? buf2_ : buf1_;
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Swap the reader and writer buffers. This should be done when both the reader and the writer are known not to be accessing the buffers.
	///
	constexpr void swapBuffers()
	{
		isSwapped_ = !isSwapped_;
	}

private:
	T buf1_{};
	T buf2_{};
	bool isSwapped_{false};
};

template<typename T>
using TypedDoubleBuffer = DoubleBuffer_t<TypedBuffer<T>>;

template<uint32_t size>
using StaticDoubleBuffer = DoubleBuffer_t<StaticBuffer<size>>;

template<uint32_t size>
using WordAlignedStaticDoubleBuffer = DoubleBuffer_t<WordAlignedStaticBuffer<size>>;

}




#endif /* TYPEDDOUBLEBUFFER_H_ */