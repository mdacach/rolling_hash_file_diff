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
    using Signature = std::vector<Hash>;
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
};

#endif // ROLLING_HASH_FILE_DIFF_FILE_DIFF_HPP
