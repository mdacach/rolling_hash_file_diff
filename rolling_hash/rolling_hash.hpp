//
// Created by matheus on 24/07/22.
//

#ifndef ROLLING_HASH_HPP
#define ROLLING_HASH_HPP

#include <cstdint>
#include <deque>
#include <string_view>
#include <vector>

class RollingHash
{
public:
    using Hash = uint64_t;

public:
    RollingHash() = default;

    RollingHash(uint64_t alphabet_base, uint64_t modulo, uint64_t window_size, std::string_view initial_input);

    auto get_current_hash() const -> Hash;

    auto slide_window(char c) -> void;

private:
    auto append(char c) -> void;

    auto remove_first() -> void;

    auto get_ascii_value_from_char(char c) -> uint64_t;

private:
    Hash m_current_hash{};
    std::deque<char> m_current_string{};
    const uint64_t m_alphabet_base{ 257 };
    const uint64_t m_modulo{ static_cast<uint64_t>(1e9 + 7) };
    const std::size_t m_window_size{ 3 };
    std::vector<uint64_t> m_precomputed_base_powers{};
};

#endif // ROLLING_HASH_HPP
