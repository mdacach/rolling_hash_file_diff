//
// Created by matheus on 24/07/22.
//

#include "file_diff.hpp"

auto FileDiff::compute_signature(const std::string& input_string, const std::size_t chunk_size) -> Signature
{
    const auto chunks = split_into_chunks(input_string, chunk_size);
    auto get_hash = [](const auto& string)
    {
        return compute_single_rolling_hash(string);
    };
    auto result = std::vector<Hash>{};
    result.reserve(std::size(chunks));
    for (const auto& chunk : chunks)
        result.push_back(get_hash(chunk));
    return result;
}

auto FileDiff::compute_delta(const std::string& my_string, const Signature& signature, const std::size_t chunk_size)
    -> Delta
{
    // TODO: change this to rolling hash
    const auto all_hashes = compute_rolling_hashes(my_string, chunk_size);
    auto get_hash = [&all_hashes](auto start_index)
    {
        assert(start_index < std::size(all_hashes));
        return all_hashes.at(start_index);
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
        //            const auto chunk = get_chunk_starting_at(start);
        // If we do not have enough characters to complete a chunk, we will
        // not match it against the other file
        if (start + chunk_size - 1 >= std::size(my_string))
        {
            result += 'b';
            result += my_string.at(start);
            start += 1;
            continue;
        }

        const auto this_hash = get_hash(start);
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
            result += 'b';
            result += my_string.at(start);
            start += 1;
        }
    }
    return result;
}

auto FileDiff::apply_delta(const std::string& my_string, const Delta& delta, const std::size_t chunk_size)
    -> std::string
{
    const auto chunks = split_into_chunks(my_string, chunk_size);
    auto result = std::string{};
    auto parse_number = [&delta](auto start)
    {
        auto number_as_string = std::string{};
        while (start < std::size(delta) && std::isdigit(delta.at(start)))
        {
            number_as_string.push_back(delta.at(start));
            start += 1;
        }
        return number_as_string;
    };
    for (std::size_t current_index = 0; current_index < std::size(delta);)
    {
        // Current symbol is either a
        // 1 - Reference to chunk token "@", and is followed by the chunk id
        // 2 - 'b' representing a literal byte, followed by the actual byte
        const auto current_symbol = delta.at(current_index);
        // TODO: get rid of "magic" @ and designate a const variable for token symbol
        if (current_symbol == '@')
        {
            // The next symbols represent the id of the matching chunk
            const auto id_as_string = parse_number(current_index + 1);
            const auto id = static_cast<std::size_t>(std::stoi(id_as_string));
            current_index += 1 + id_as_string.length(); // Accounts for the @ and the id
            result += chunks.at(id);
        }
        else
        {
            // This represents that the following one is a byte itself
            assert(current_symbol == 'b');
            result += delta.at(current_index + 1); // current_index + 1 is the actual byte
            current_index += 2;
        }
    }
    return result;
}

// Public in order to be tested by Catch2
auto FileDiff::split_into_chunks(const std::string& input_string, const std::size_t chunk_size)
    -> std::vector<std::string>
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

auto FileDiff::compute_single_rolling_hash(const std::string& input) -> Hash
{
    const auto chunk_size = std::size(input);
    auto mod = static_cast<uint64_t>(1e9 + 7);
    auto base = 257; // A prime number
    auto base_powers = std::vector<uint64_t>(chunk_size + 1);
    base_powers.at(0) = 1;
    for (std::size_t i = 1; i <= chunk_size; ++i)
        base_powers.at(i) = (base_powers.at(i - 1) * base) % mod;

    auto current_hash = Hash{ 0 };
    auto current_power = base;
    for (std::size_t i = 0; i < chunk_size; ++i)
    {
        const auto power = chunk_size - i - 1;

        const auto value_for_this_char = input.at(i) * base_powers.at(power);
        current_hash += value_for_this_char;
        current_hash %= mod;
    }
    return current_hash;
}

auto FileDiff::compute_rolling_hashes(const std::string& input, const std::size_t chunk_size) -> std::vector<Hash>
{
    auto mod = static_cast<uint64_t>(1e9 + 7);
    auto base = 257; // A prime number
    auto base_powers = std::vector<uint64_t>(chunk_size + 1);
    base_powers.at(0) = 1;
    for (std::size_t i = 1; i <= chunk_size; ++i)
        base_powers.at(i) = (base_powers.at(i - 1) * base) % mod;

    // Compute the initial window
    if (std::size(input) < chunk_size)
        return { compute_single_rolling_hash(input) };

    auto current_hash = compute_single_rolling_hash(input.substr(0, chunk_size));

    auto result = std::vector<Hash>{};
    result.push_back(current_hash);
    for (std::size_t i = chunk_size; i < std::size(input); ++i)
    {
        const auto to_add = input.at(i);
        const auto to_remove = input.at(i - chunk_size);
        // We add char at i to the hash
        // And remove the first character of the hash

        current_hash *= base;
        current_hash %= mod;

        current_hash += to_add;
        current_hash %= mod;

        current_hash -= to_remove * base_powers.at(chunk_size);
        current_hash %= mod;

        result.push_back(current_hash);
    }

    return result;
}
