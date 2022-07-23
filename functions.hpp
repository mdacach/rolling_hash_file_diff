//
// Created by matheus on 22/07/22.
//

#ifndef ROLLING_HASH_FILE_DIFF_FUNCTIONS_HPP
#define ROLLING_HASH_FILE_DIFF_FUNCTIONS_HPP

#include <cmath>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

// TODO: restructure project
namespace functions
{
    auto split_into_chunks(const std::string& input_string, const std::size_t chunk_size) -> std::vector<std::string>
    {
        if (std::size(input_string) <= chunk_size)
            return { input_string };

        auto result = std::vector<std::string>{};
        auto number_of_chunks = std::size(input_string) / chunk_size;
        if (std::size(input_string) % chunk_size != 0)
            ++number_of_chunks;
        result.reserve(number_of_chunks);
        for (std::size_t current_index = 0; current_index < std::size(input_string);)
        {
            auto current_string = std::string{};
            current_string.reserve(chunk_size);
            for (std::size_t offset = 0; offset < chunk_size; ++offset)
            {
                if (current_index + offset >= std::size(input_string))
                    break;
                current_string.push_back(input_string.at(current_index + offset));
            }
            current_index += chunk_size;
            result.push_back(current_string);
        }
        return result;
    }

    using Hash = uint64_t;
    using Signature = std::vector<Hash>;
    auto compute_signature(const std::string& input_string, const std::size_t chunk_size) -> Signature
    {
        const auto chunks = split_into_chunks(input_string, chunk_size);
        // TODO: this hash function will probably change
        auto get_hash = [](const auto& string)
        {
            return std::hash<std::string>{}(string);
        };
        auto result = std::vector<Hash>{};
        result.reserve(std::size(chunks));
        for (const auto& chunk : chunks)
            result.push_back(get_hash(chunk));
        return result;
    }

    using Delta = std::string;
    auto compute_delta(const std::string& my_string, const Signature& signature, const std::size_t chunk_size) -> Delta
    {
        // TODO: change this to rolling hash
        auto get_hash = [](const auto& string)
        {
            return std::hash<std::string>{}(string);
        };
        auto get_chunk_starting_at = [&my_string, chunk_size](const auto start)
        {
            // We either get `chunk_size` values or the leftover ones.
            if (start + chunk_size < std::size(my_string))
                return my_string.substr(start, chunk_size);
            return my_string.substr(start);
        };
        auto result = Delta{};
        for (std::size_t start = 0; start < std::size(my_string);)
        {
            const auto chunk = get_chunk_starting_at(start);
            const auto this_hash = get_hash(chunk);
            const auto where = std::ranges::find(signature, this_hash);
            if (where != std::ranges::end(signature))
            {
                const auto equal_chunk_id = std::distance(std::ranges::begin(signature), where);
                result += '@';
                result += std::to_string(equal_chunk_id);
                // We have already processed all this chunk
                start += chunk_size;
            }
            else
            {
                result += my_string.at(start);
                start += 1;
            }
        }
        return result;
    }
} // namespace functions

#endif // ROLLING_HASH_FILE_DIFF_FUNCTIONS_HPP
