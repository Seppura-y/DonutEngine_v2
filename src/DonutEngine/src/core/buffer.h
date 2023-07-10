#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <cstring>

namespace Donut
{
	struct Buffer
	{
		uint8_t* data_ = nullptr;
		uint64_t size_ = 0;

		Buffer() = default;

		Buffer(uint64_t size)
		{
			allocate(size);
		}

		Buffer(const Buffer&) = default;

		static Buffer copy(Buffer other)
		{
			Buffer result(other.size_);
			memcpy(result.data_, other.data_, other.size_);
			return result;
		}

		void allocate(uint64_t size)
		{
			release();

			data_ = new uint8_t[size];
			size_ = size;
		}

		void release()
		{
			delete[] data_;

			data_ = nullptr;
			size_ = 0;
		}

		template<typename T>
		T* as()
		{
			return (T*)data_;
		}

		operator bool() const
		{
			return (bool)data_;
		}
	};


	struct ScopedBuffer
	{
		ScopedBuffer(Buffer buffer)
			: buffer_(buffer)
		{

		}

		ScopedBuffer(uint64_t size)
			: buffer_(size)
		{

		}

		~ScopedBuffer()
		{
			buffer_.release();
		}

		uint8_t* getData() { return buffer_.data_; }
		uint64_t getSize() { return buffer_.size_; }

		template<typename T>
		T* as()
		{
			return buffer_.as<T>();
		}

		operator bool() const 
		{
			return buffer_;
		}

	private:
		Buffer buffer_;
	};
}
#endif