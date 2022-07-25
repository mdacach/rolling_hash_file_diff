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
    /**
     * Initializes the structure with `initial_input` string (and hash).
     * @param alphabet_base Base to use for hashing computations. Should be bigger than the alphabet.
     * @param modulo Modulo to use for hashing. Should be prime.
     * @param window_size Size of the window for the structure to slide.
     * @param initial_input Initial window.
     */
    RollingHash(uint64_t alphabet_base, uint64_t modulo, uint64_t window_size, std::string_view initial_input);

    /**
     * Get current hash from string in structure.
     * @return Current hash value.
     */
    auto get_current_hash() const -> Hash;

    /**
     * Updates the structure to have the hash of the next "sliding window".
     * `c` is appended and the first one is removed. This maintains the invariant that
     * the underlying string is always of the same size (thus a fixed "window" which just "slides" through the input).
     * @param c Character to add to the structure.
     */
    auto slide_window(char c) -> void;

private:
    // As this is only used for the rolling hash in file diff,
    // we do not expose `append` nor `remove_first` operations to maintain the fixed window size.
    // In a more general application, we could make those functions public and add some more.

    /**
     * Appends a char to the structure.
     * After this operation, `m_current_hash` and `m_current_string` are updated.
     * @param c Character to add to structure.
     */
    auto append(char c) -> void;

    /**
     * Removes the first character (leftmost) from the structure.
     * After this operation, `m_current_hash` and `m_current_string` are updated.
     */
    auto remove_first() -> void;

    /**
     * Get ascii value from a character.
     * @param c Character to get value from.
     * @return Ascii value as unsigned.
     */
    static auto get_ascii_value_from_char(char c) -> uint64_t;

private:
    // Hash for the current underlying string in the structure.
    Hash m_current_hash{};
    // Maintain the underlying string. Efficient operations from `push_back` and `push_front`.
    std::deque<char> m_current_string{};
    // Base for hashing. 257 is good for ASCII values, as we are using in file diff.
    const uint64_t m_alphabet_base{ 257 };
    // Mod for hashing. 1e9 + 7 is a prime which is not too big (we have room for operations as we use 64 bits), but
    // big enough so we do not expect many collisions.
    const uint64_t m_modulo{ static_cast<uint64_t>(1e9 + 7) };
    // Fixed window size throughout the structure.
    const std::size_t m_window_size{ 3 };
    // For efficiency purposes, we precompute all the base powers we may need to use.
    // Otherwise, we would need to pay some performance price everytime to calculate those.
    // It's possible to precompute because we know the fixed window size.
    std::vector<uint64_t> m_precomputed_base_powers{};
};

#endif // ROLLING_HASH_HPP
