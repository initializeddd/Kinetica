#include <kinetica/uuid.hpp>
#include <random>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cctype>

namespace Kinetica {
    static thread_local std::random_device rd_instance;
    static thread_local std::mt19937_64 gen_instance(rd_instance());

    CUUID::CUUID() : m_bValid(false) {}

    // Constructor "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
    CUUID::CUUID(const std::string& str) : m_bValid(false) {
        if (str.size() != 36) return;

        const char* s = str.c_str();
        std::array<uint8_t, 16> temp_bytes{}; // Initialize to 0

        auto parse_hex_byte = [](const char* start) -> uint8_t {
            char hex_str[3] = {start[0], start[1], '\0'};
            int val;
            if (std::sscanf(hex_str, "%x", &val) == 1 && val >= 0 && val <= 255) {
                return static_cast<uint8_t>(val);
            }
            return 0; // Return 0 if parsing fails
        };

        if (s[8] != '-' || s[13] != '-' || s[18] != '-' || s[23] != '-') {
            return; // Invalid format
        }

        temp_bytes[0] = parse_hex_byte(s + 0);
        temp_bytes[1] = parse_hex_byte(s + 2);
        temp_bytes[2] = parse_hex_byte(s + 4);
        temp_bytes[3] = parse_hex_byte(s + 6);

        temp_bytes[4] = parse_hex_byte(s + 9);
        temp_bytes[5] = parse_hex_byte(s + 11);

        temp_bytes[6] = parse_hex_byte(s + 14);
        temp_bytes[7] = parse_hex_byte(s + 16);

        temp_bytes[8] = parse_hex_byte(s + 19);
        temp_bytes[9] = parse_hex_byte(s + 21);

        temp_bytes[10] = parse_hex_byte(s + 24);
        temp_bytes[11] = parse_hex_byte(s + 26);
        temp_bytes[12] = parse_hex_byte(s + 28);
        temp_bytes[13] = parse_hex_byte(s + 30);
        temp_bytes[14] = parse_hex_byte(s + 32);
        temp_bytes[15] = parse_hex_byte(s + 34);

        m_bytes = temp_bytes;
        m_bValid = true; // Assume valid if parsing passed basic checks
    }

    CUUID::CUUID(const std::array<std::uint8_t, 16>& bytes) : m_bytes(bytes), m_bValid(true) {}

    std::string CUUID::toString() const {
        if (!m_bValid) return "nil"; // Or return an empty string, depending on preference
        std::ostringstream oss;
        auto& b = m_bytes;
        oss << std::hex << std::setfill('0');
        oss << std::setw(2) << static_cast<int>(b[0])
        << std::setw(2) << static_cast<int>(b[1])
        << std::setw(2) << static_cast<int>(b[2])
        << std::setw(2) << static_cast<int>(b[3]) << "-"
        << std::setw(2) << static_cast<int>(b[4])
        << std::setw(2) << static_cast<int>(b[5]) << "-"
        << std::setw(2) << static_cast<int>(b[6])
        << std::setw(2) << static_cast<int>(b[7]) << "-"
        << std::setw(2) << static_cast<int>(b[8])
        << std::setw(2) << static_cast<int>(b[9]) << "-"
        << std::setw(2) << static_cast<int>(b[10])
        << std::setw(2) << static_cast<int>(b[11])
        << std::setw(2) << static_cast<int>(b[12])
        << std::setw(2) << static_cast<int>(b[13])
        << std::setw(2) << static_cast<int>(b[14])
        << std::setw(2) << static_cast<int>(b[15]);
        return oss.str();
    }

    bool CUUID::operator==(const CUUID& other) const {
        // Check validity first if needed, or just compare bytes
        // Assuming both valid or both invalid can be equal
        return m_bytes == other.m_bytes; // This compares the std::array directly
    }

    bool CUUID::operator<(const CUUID& other) const {
        return m_bytes < other.m_bytes; // Lexicographical comparison of std::array
    }

    CUUID CUUID::generate() {
        std::array<std::uint8_t, 16> bytes;
        for (auto& b : bytes) {
            b = static_cast<std::uint8_t>(gen_instance());
        }
        // Set version (4) and variant (1) bits according to RFC 4122
        // Version 4: 4 bits in the 7th byte (0-based index 6) -> set bits 4-7 to 0100 (4)
        bytes[6] = (bytes[6] & 0x0F) | 0x40; // Clear upper 4 bits, set to 0100
        // Variant: 2 bits in the 9th byte (0-based index 8) -> set bits 6-7 to 10
        bytes[8] = (bytes[8] & 0x3F) | 0x80; // Clear upper 2 bits, set to 10
        return CUUID(bytes);
    }

    CUUID CUUID::nil() {
        static const std::array<uint8_t, 16> zero_bytes{};
        static const CUUID nil_uuid(zero_bytes);
        return nil_uuid;
    }

} // namespace Kinetica
