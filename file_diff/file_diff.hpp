//
// Created by matheus on 24/07/22.
//

#ifndef ROLLING_HASH_FILE_DIFF_FILE_DIFF_HPP
#define ROLLING_HASH_FILE_DIFF_FILE_DIFF_HPP

#include <cassert>
#include <cmath>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

class FileDiff
{
public:
    using Hash = uint64_t;
    struct Signature
    {
        // We will be accessing rolling hashes most of the time, so having them together here
        // is better (cache locality)
        std::vector<Hash> rolling_hashes{};
        std::vector<Hash> strong_hashes{};

        bool operator==(const Signature& rhs) const
        {
            return rolling_hashes == rhs.rolling_hashes && strong_hashes == rhs.strong_hashes;
        }
    };
    using Delta = std::string;

    static auto compute_signature(const std::string& input_string, std::size_t chunk_size) -> Signature;

    static auto compute_delta(const std::string& my_string, const Signature& signature, std::size_t chunk_size)
        -> Delta;

    static auto apply_delta(const std::string& my_string, const Delta& delta, std::size_t chunk_size) -> std::string;

    // Public in order to be tested by Catch2
    static auto split_into_chunks(const std::string& input_string, std::size_t chunk_size) -> std::vector<std::string>;

private:
    static auto compute_single_rolling_hash(const std::string& input) -> Hash;

    static auto compute_rolling_hashes(const std::string& input, std::size_t chunk_size) -> std::vector<Hash>;

    static auto compute_strong_hash(const std::string& input) -> Hash;

private:
    // Ascii size plus one
    static const uint64_t m_rolling_hash_base{ 257 };
    // A big prime number
    static const uint64_t m_rolling_hash_modulo{ static_cast<uint64_t>(1e9 + 7) };
    // This token indicates that the next byte is a literal byte
    static const char human_readable_byte_token{ 'b' };
    // This token indicates that the next number (may be multiple bytes) is the chunk id that matches
    static const char human_readable_reference_token{ '@' };
};

#endif // ROLLING_HASH_FILE_DIFF_FILE_DIFF_HPP
