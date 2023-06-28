#ifndef UUID_H
#define UUID_H

//#include <xhash>

namespace Donut
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return uuid_; }
	private:
		uint64_t uuid_;


	};
}

namespace std 
{
	template <typename T> struct hash;
	template<>
	struct hash<Donut::UUID>
	{
		std::size_t operator()(const Donut::UUID& uuid) const
		{
			//return hash<uint64_t>()((uint64_t)uuid);
			return (uint64_t)uuid;
		}
	};
}
#endif