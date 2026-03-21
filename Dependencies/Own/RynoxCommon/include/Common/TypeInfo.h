#pragma once

#include <atomic>
#include <string_view>
#include <cstdint>
#include <concepts>

namespace Rynox::Common
{
    template<typename Domain>
    concept SequenceDomain = requires
    {
        { Domain::NextSequence() } -> std::convertible_to<uint32_t>;
    };

    struct DefaultSequenceDomain
    {
        static uint32_t NextSequence() noexcept
        {
            static std::atomic<uint32_t> counter = 0;
            return counter.fetch_add(1, std::memory_order_relaxed);
        }
    };

    namespace
    {
        constexpr uint32_t FNV_OFFSET = 0x811C9DC5;
        constexpr uint32_t FNV_PRIME = 0x01000193;
    }

    template<typename T, SequenceDomain Domain>
    struct TypeSeq
    {
        static uint32_t value() noexcept
        {
            static const uint32_t index = Domain::NextSequence();
            return index;
        }
    };

    template<typename T>
    consteval std::string_view TypeName() noexcept
    {
#if defined(__clang__) || defined(__GNUC__)
        std::string_view pretty = __PRETTY_FUNCTION__;
        auto start = pretty.find("T = ") + 4;
        auto end = pretty.rfind(']');
#elif defined(_MSC_VER)
        std::string_view pretty = __FUNCSIG__;
        auto start = pretty.find("TypeName<") + 9;
        auto end = pretty.rfind('>');
#else
#   error "Unsupported compiler"
#endif
        return pretty.substr(start, end - start);
    }

    template<typename T>
    consteval uint32_t TypeHash() noexcept
    {
        constexpr std::string_view name = TypeName<T>();

        uint32_t hash = FNV_OFFSET;
        uint32_t prime = FNV_PRIME;

        for (char c : name)
            hash = (hash ^ static_cast<uint32_t>(c)) * prime;

        return hash;
    }

    class TypeInfo
    {
    public:
        constexpr TypeInfo(uint32_t seq, uint32_t hash, std::string_view name) noexcept
            : m_Seq(seq), m_Hash(hash), m_Name(name) {
        }

        uint32_t seq()  const noexcept { return m_Seq; }
        uint32_t hash() const noexcept { return m_Hash; }
        std::string_view name() const noexcept { return m_Name; }

        bool operator==(const TypeInfo& other) const noexcept { return m_Hash == other.m_Hash; }
        bool operator!=(const TypeInfo& other) const noexcept { return m_Hash != other.m_Hash; }

    private:
        uint32_t m_Seq;
        uint32_t m_Hash;
        std::string_view m_Name;
    };

    template<typename T, SequenceDomain Domain = DefaultSequenceDomain>
    TypeInfo TypeID() noexcept
    {
        return TypeInfo
        {
            TypeSeq<T, Domain>::value(),
            TypeHash<T>(),
            TypeName<T>()
        };
    }
}