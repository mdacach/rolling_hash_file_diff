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

        // A rolling hash and a strong hash for each "chunk" in our file.
        std::vector<Hash> rolling_hashes{};
        std::vector<Hash> strong_hashes{};

        bool operator==(const Signature& rhs) const
        {
            return rolling_hashes == rhs.rolling_hashes && strong_hashes == rhs.strong_hashes;
        }
    };
    using Delta = std::string;

    /**
     * Computes the "signature" for `input_string` and `chunk_size`.
     * \n
     * Signature consists of two hashes for each chunk and is used when matching chunks between files.
     * Chunk size will directly
     * @param input_string String to compute "signature" from.
     * @param chunk_size Chunk size to use when splitting the file as part of signature process.
     * @return Signature of `input_string`.
     */
    static auto compute_signature(const std::string& input_string, std::size_t chunk_size) -> Signature;

    /**
     * Computes the delta from `my_string` regarding `signature`.
     * \n
     * Signature represents the contents of another file, through hashes.
     * We compare `my_string` with signatures to create the "delta" (differences).
     * This "delta" can then be used to update the original file (the one from whom the`signature` was
     * computed) to become `my_string`.
     * Note we need to have the same `chunk_size` here as `compute_signature`, for matching the appropriate
     * portions of `my_string`.
     * @param my_string String to compute differences from `signature`.
     * @param signature Signature of the basis file, previously computed by `compute_signature`.
     * @param chunk_size Chunk size used when previously computing `signature`.
     * @return
     */
    static auto compute_delta(const std::string& my_string, const Signature& signature, std::size_t chunk_size)
        -> Delta;

    /**
     * Updates `basis_string` using `delta`.
     * \n
     * After this operation, `basis_string` will become equal to the initial file we used in
     * `compute_signature`.
     * Note we need to have the same `chunk_size` here as `compute_signature` and `compute_delta` for
     * the reconstruction to work.
     * @param basis_string File to be updated.
     * @param delta Delta to use when updating.
     * @param chunk_size Chunk size used when previous computing `signature` and `compute_delta`.
     * @return
     */
    static auto apply_delta(const std::string& basis_string, const Delta& delta, std::size_t chunk_size) -> std::string;

    // Public in order to be tested by Catch2
    /**
     * Splits a given string into "chunks" of `chunk_size` size. The last chunk may have a smaller size.
     * @param input_string String to be split into "chunks".
     * @param chunk_size Size for each chunk, if possible.
     * @return Chunks, in order.
     */
    static auto split_into_chunks(const std::string& input_string, std::size_t chunk_size) -> std::vector<std::string>;

private:
    /**
     * Computes the rolling hash for a single input.
     * \n
     * Note that the input does not need to have the window length here.
     * @param input String to calculate rolling hash from.
     * @return Rolling hash value.
     */
    static auto compute_single_rolling_hash(const std::string& input) -> Hash;

    /**
     * Computes rolling hashes for all "sliding windows" of `chunk_size` in `input`.
     * @param input String to calculate rolling hashes from.
     * @param chunk_size Size of each "window".
     * @return A rolling hash value for each window, in order.
     */
    static auto compute_rolling_hashes(const std::string& input, std::size_t chunk_size) -> std::vector<Hash>;

    /**
     * Computes a "strong" hash for a single input.
     * \n
     * Specifically, it computes C++'s implementation-defined hash for std::string.
     * We would change this to a particularly chosen hash function in a production application.
     * @param input String to calculate hash from.
     * @return Hash value.
     */
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
