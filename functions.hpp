//
// Created by matheus on 22/07/22.
//

#ifndef ROLLING_HASH_FILE_DIFF_FUNCTIONS_HPP
#define ROLLING_HASH_FILE_DIFF_FUNCTIONS_HPP

#include <cmath>
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
} // namespace functions

#endif // ROLLING_HASH_FILE_DIFF_FUNCTIONS_HPP
