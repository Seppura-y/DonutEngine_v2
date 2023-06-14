#ifndef BUFFERS_H
#define BUFFERS_H


namespace Donut
{
	enum class ShaderDataType
	{
		TypeNone = 0, Float, Float2, Float3, Float4, Int, Int2, Int3, Int4, Mat2, Mat3, Mat4, Bool
	};

	static uint32_t getShaderDataSize(const ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:		return 4;
			case ShaderDataType::Float2:	return 4 * 2;
			case ShaderDataType::Float3:	return 4 * 3;
			case ShaderDataType::Float4:	return 4 * 4;
			case ShaderDataType::Int:		return 4;
			case ShaderDataType::Int2:		return 4 * 2;
			case ShaderDataType::Int3:		return 4 * 3;
			case ShaderDataType::Int4:		return 4 * 4;
			case ShaderDataType::Mat3:		return 4 * 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4 * 4;
			case ShaderDataType::Bool:		return 1;
		}
		DN_CORE_ASSERT(false, "Unknown shader data type!");
		return 0;
	}

	struct BufferElement
	{
		ShaderDataType type_;
		std::string name_;
		uint32_t size_;
		uint32_t offset_;
		bool normalized_;

		BufferElement(const ShaderDataType& type, const std::string& name, bool normalized = false)
			: type_(type), name_(name), size_(getShaderDataSize(type)), offset_(0), normalized_(normalized)
		{

		}

		uint32_t getComponentCount() const
		{
			switch (type_)
			{
				case ShaderDataType::Float:		return 1;
				case ShaderDataType::Float2:	return 2;
				case ShaderDataType::Float3:	return 3;
				case ShaderDataType::Float4:	return 4;
				case ShaderDataType::Int:		return 1;
				case ShaderDataType::Int2:		return 2;
				case ShaderDataType::Int3:		return 3;
				case ShaderDataType::Int4:		return 4;
				case ShaderDataType::Mat3:		return 3 * 3;
				case ShaderDataType::Mat4:		return 4 * 4;
				case ShaderDataType::Bool:		return 1;
			}

			DN_CORE_ASSERT(false, "Unknown shader data type!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}
		BufferLayout(std::initializer_list<BufferElement> elements)
			: elements_(elements)
		{
			caculateOffsetAndStride();
		}

		inline uint32_t getStride() const { return stride_; }
		inline const std::vector<BufferElement>& getBufferElements() const { return elements_; }

		std::vector<BufferElement>::iterator begin() { return elements_.begin(); }
		std::vector<BufferElement>::iterator end() { return elements_.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return elements_.cbegin(); }
		std::vector<BufferElement>::const_iterator end() const { return elements_.cend(); }

	private:
		void caculateOffsetAndStride()
		{
			stride_ = 0;
			uint32_t offset = 0;
			for (auto& ele : elements_)
			{
				ele.offset_ = offset;
				offset += ele.size_;
				stride_ += ele.size_;
			}
		}
	private:
		std::vector<BufferElement> elements_;
		uint32_t stride_ = 0;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {};

		virtual void bind() const = 0;
		virtual void unBind() const = 0;

		virtual void setLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& getLayout() const = 0;

		virtual void setData(const void* data, uint32_t size) = 0;

		static VertexBuffer* create(uint32_t size);
		static VertexBuffer* create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {};

		virtual void bind() const = 0;
		virtual void unBind() const = 0;

		virtual uint32_t getIndicesCount() const = 0;

		static IndexBuffer* create(uint32_t* indices, uint32_t count);
	};


}
#endif