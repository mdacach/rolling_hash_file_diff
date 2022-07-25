//
// Created by matheus on 24/07/22.
// Implementation References:
// Algorithms Live: https://www.youtube.com/watch?v=rA1ZevamGDc
// MIT OCW: https://www.youtube.com/watch?v=w6nuXg0BISo
//          https://www.youtube.com/watch?v=BRO7mVIFt08
// Codeforces: https://codeforces.com/blog/entry/60445
// CP-Algorithms: https://cp-algorithms.com/string/string-hashing.html
//

#include "rolling_hash.hpp"

#include <cassert>

auto RollingHash::get_current_hash() const -> Hash
{
    return m_current_hash;
}

auto RollingHash::append(char c) -> void
{

    const auto char_value = get_ascii_value_from_char(c);
    // "Shift the hash to the left"
    m_current_hash *= m_alphabet_base;
    m_current_hash %= m_modulo;

    m_current_hash += char_value;
    m_current_hash %= m_modulo;
    // We always take the modulo to ensure that the value is always left in a valid range.

    m_current_string.push_back(c);
}

auto RollingHash::remove_first() -> void
{
    const auto current_length = std::size(m_current_string);
    assert(!m_current_string.empty());
    const auto first_character = m_current_string.front();

    const auto char_value = get_ascii_value_from_char(first_character);

    // We must remove the contribution of this value in our hash
    const auto factor = m_precomputed_base_powers.at(current_length - 1);
    const auto char_contribution = (char_value * factor) % m_modulo;
    // We need to be careful with underflow here, as we are using unsigned integers
    if (char_contribution > m_current_hash)
    {
        // As both values involved are between 0 and (m_modulo - 1)
        // It suffices to add a m_modulo to the current_hash
        // Then the result will not be negative
        m_current_hash += m_modulo;
        // Note that here m_current_hash may be bigger than m_modulo (breaks our invariant)
        // But our subtraction will fix it
    }
    m_current_hash -= char_contribution;
    m_current_hash %= m_modulo;

    // We always take the modulo to ensure that the value is always left in a valid range.
    // We have just removed this char
    m_current_string.pop_front();
}

auto RollingHash::slide_window(char c) -> void
{
    append(c);
    remove_first();
}

auto RollingHash::get_ascii_value_from_char(char c) -> uint64_t
{
    return static_cast<uint64_t>(c);
}

RollingHash::RollingHash(uint64_t alphabet_base, uint64_t modulo, uint64_t window_size, std::string_view initial_input)
    : m_alphabet_base{ alphabet_base }, m_modulo{ modulo }, m_window_size{ window_size }
{
    // Precompute the powers for performance reasons.
    m_precomputed_base_powers.reserve(m_window_size);
    m_precomputed_base_powers.push_back(1);

    // We need to know up to base^window_size for operations.
    for (std::size_t i = 1; i <= window_size; ++i)
    {
        const auto next_power = (m_precomputed_base_powers.back() * m_alphabet_base) % m_modulo;
        m_precomputed_base_powers.push_back(next_power);
    }

    // Start the structure with the hash of the initial input.
    for (auto c : initial_input)
        append(c);
}
