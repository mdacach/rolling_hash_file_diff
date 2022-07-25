//
// Created by matheus on 24/07/22.
//

#include "file_diff.hpp"
#include "../rolling_hash/rolling_hash.hpp"

#include <map>

auto FileDiff::compute_signature(const std::string& input_string, const std::size_t chunk_size) -> Signature
{
    const auto chunks = split_into_chunks(input_string, chunk_size);

    auto result = Signature{};
    result.rolling_hashes.reserve(std::size(chunks));
    result.strong_hashes.reserve(std::size(chunks));
    for (const auto& chunk : chunks)
    {
        result.rolling_hashes.push_back(compute_single_rolling_hash(chunk));
        result.strong_hashes.push_back(compute_strong_hash(chunk));
    }
    return result;
}

auto FileDiff::compute_delta(const std::string& my_string, const Signature& signature, const std::size_t chunk_size)
    -> Delta
{
    // These are rolling hashes for every possible chunk (regarding shifting)
    const auto all_hashes = compute_rolling_hashes(my_string, chunk_size);

    auto get_hash = [&all_hashes](auto start_index)
    {
        assert(start_index < std::size(all_hashes));
        return all_hashes.at(start_index);
    };

    // For each "our" rolling hash, we need to know
    // 1 - Whether we have the same hash in signature
    // 2 - The associated *strong* hash in signature, if yes
    // With a map, we can answer (1) in O(log n) and keeping track
    // of the id we can answer (2) in O(1).
    const auto rolling_hash_to_id = [&signature]
    {
        const auto& rolling_hashes = signature.rolling_hashes;
        using ID = std::size_t;
        auto result = std::map<Hash, ID>{};
        for (std::size_t i = 0; i < std::size(rolling_hashes); ++i)
            result[rolling_hashes.at(i)] = i;
        return result;
    }();

    auto result = Delta{};
    for (std::size_t start = 0; start < std::size(my_string);)
    {
        auto add_byte = [&result, &start, &my_string]
        {
            result += human_readable_byte_token;
            result += my_string.at(start);
            start += 1;
        };

        auto add_chunk_reference = [&result, &start, &chunk_size](const auto chunk_id)
        {
            result += human_readable_reference_token;
            result += std::to_string(chunk_id);
            // We have just processed all this chunk
            start += chunk_size;
        };

        // If we do not have enough characters to complete a chunk, it will not match
        if (start + chunk_size - 1 >= std::size(my_string))
        {
            add_byte();
            continue;
        }

        const auto this_hash = get_hash(start);
        const auto where = rolling_hash_to_id.find(this_hash);
        const auto match_rolling_hash = where != std::end(rolling_hash_to_id);
        if (match_rolling_hash)
        {
            // This chunk is a potential match.
            // To be careful with a hash collision here, we will check if the
            // strong hash also matches (chances of collision are *very* low then)

            const auto& strong_hashes = signature.strong_hashes;
            const auto this_string = my_string.substr(start, chunk_size);
            const auto this_strong_hash = compute_strong_hash(this_string);

            // As signature is a pair of {rolling_hash, strong_hash}, we can find the
            // candidate chunk's strong hash by the index
            const auto [_, candidate_id] = *where;
            const auto candidate_strong_hash = strong_hashes.at(candidate_id);

            const auto is_indeed_match = this_strong_hash == candidate_strong_hash;
            if (is_indeed_match)
                add_chunk_reference(candidate_id);
            else
                add_byte();
        }
        else
        {
            // If not even the rolling hashes matched, it's surely not the same string
            add_byte();
        }
    }
    return result;
}

auto FileDiff::apply_delta(const std::string& basis_string, const Delta& delta, const std::size_t chunk_size)
    -> std::string
{
    const auto chunks = split_into_chunks(basis_string, chunk_size);
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
    const auto hasher = RollingHash(m_rolling_hash_base, m_rolling_hash_modulo, chunk_size, input);
    return hasher.get_current_hash();
}

auto FileDiff::compute_rolling_hashes(const std::string& input, const std::size_t chunk_size) -> std::vector<Hash>
{
    // Compute the initial window
    if (std::size(input) < chunk_size)
        return { compute_single_rolling_hash(input) };

    const auto first_chunk = input.substr(0, chunk_size);
    auto hasher = RollingHash(m_rolling_hash_base, m_rolling_hash_modulo, chunk_size, first_chunk);

    auto current_hash = hasher.get_current_hash();
    auto result = std::vector<Hash>{};
    result.push_back(current_hash);
    for (std::size_t i = chunk_size; i < std::size(input); ++i)
    {
        hasher.slide_window(input.at(i));
        result.push_back(hasher.get_current_hash());
    }

    return result;
}

auto FileDiff::compute_strong_hash(const std::string& input) -> Hash
{
    return std::hash<std::string>{}(input);
}
