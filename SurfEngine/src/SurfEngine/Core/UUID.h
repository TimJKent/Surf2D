#pragma once
#include <xhash>

namespace SurfEngine {
	class UUID {
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;
		
		operator uint64_t() const { return m_UUID; }

		std::string ToString();

	private:
		uint64_t m_UUID;
	};
}

namespace std{
	template<>
	struct hash<SurfEngine::UUID>
	{
		std::size_t operator()(const SurfEngine::UUID& uuid) const {
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};

}