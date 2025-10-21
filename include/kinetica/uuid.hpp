#ifndef KINETICA_UUID_HPP
#define KINETICA_UUID_HPP

#include <string>
#include <cstdint>
#include <array>
#include <random>

namespace Kinetica {

    class CUUID {
    public:
        CUUID();
        explicit CUUID(const std::string& str);
        explicit CUUID(const std::array<std::uint8_t, 16>& bytes);

        bool isValid() const { return m_bValid; }
        std::string toString() const;
        const std::array<std::uint8_t, 16>& getBytes() const { return m_bytes; }

        bool operator==(const CUUID& other) const;
        bool operator!=(const CUUID& other) const { return !(*this == other); }
        bool operator<(const CUUID& other) const;

        static CUUID generate();
        static CUUID nil();

    private:
        std::array<std::uint8_t, 16> m_bytes{};
        bool m_bValid = false;
    };

} // namespace Kinetica

// Hash support for std::unordered_map
namespace std {
    template<>
    struct hash<Kinetica::CUUID> {
        std::size_t operator()(const Kinetica::CUUID& uuid) const noexcept {
            // Simple hash of first 8 bytes (good enough for most cases)
            return *reinterpret_cast<const std::size_t*>(uuid.getBytes().data());
        }
    };
} // namespace std

#endif
