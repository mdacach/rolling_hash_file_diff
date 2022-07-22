#include "../functions.hpp"
#include "catch.hpp"

TEST_CASE("Strings are split into chunks")
{
    GIVEN("A string and a chunk size")
    {
        const auto original_string = std::string{ "Whatever our souls are made of, his and mine are the same." };
        WHEN("The chunk size is bigger than the string length")
        {
            const auto chunk_size = std::size_t{ 200 };
            THEN("The result is the whole string as a chunk")
            {
                const auto split_string = functions::split_into_chunks(original_string, chunk_size);
                REQUIRE(std::size(split_string) == 1);
                REQUIRE(split_string.front() == original_string);
            }
        }

        WHEN("The chunk size is one")
        {
            const auto chunk_size = std::size_t{ 1 };
            THEN("The result is each letter as a chunk")
            {
                const auto split_string = functions::split_into_chunks(original_string, chunk_size);
                REQUIRE(std::size(split_string) == std::size(original_string));
                auto is_same_letter = [](auto letter, const auto& letter_as_string)
                {
                    return letter == letter_as_string.front();
                };
                REQUIRE(std::ranges::equal(original_string, split_string, is_same_letter));
            }
        }

        WHEN("The chunk size is less than the string length")
        {
            AND_WHEN("The string length is a multiple of chunk size")
            {
                const auto chunk_size = std::size_t{ 2 };
                THEN("All chunks have the same size")
                {
                    const auto split_string = functions::split_into_chunks(original_string, chunk_size);
                    for (const auto& chunk : split_string)
                        REQUIRE(std::size(chunk) == chunk_size);
                }
            }
            AND_WHEN("The string length is not a multiple of chunk size")
            {
                const auto chunk_size = std::size_t{ 10 };
                THEN("All chunks but the last have the same size")
                {
                    auto split_string = functions::split_into_chunks(original_string, chunk_size);
                    const auto& last_chunk = split_string.back();
                    split_string.pop_back();
                    for (const auto& chunk : split_string)
                        REQUIRE(std::size(chunk) == chunk_size);
                    REQUIRE(std::size(last_chunk) != chunk_size);
                    REQUIRE(std::size(last_chunk) == 8);
                }
            }
        }
    }

    GIVEN("A 37-length string")
    {
        const auto original_string = std::string{ "Do not go gentle into that good night" };

        WHEN("Chunk size is 5")
        {
            const auto chunk_size = std::size_t{ 5 };
            const auto split_string = functions::split_into_chunks(original_string, chunk_size);
            THEN("Chunks are substrings of length 5")
            {
                REQUIRE(split_string.at(0) == "Do no");
                REQUIRE(split_string.at(1) == "t go ");
                REQUIRE(split_string.at(2) == "gentl");
                REQUIRE(split_string.at(3) == "e int");
                REQUIRE(split_string.at(4) == "o tha");
                REQUIRE(split_string.at(5) == "t goo");
                REQUIRE(split_string.at(6) == "d nig");
            }
            AND_THEN("Last chunk is of length 2")
            {
                REQUIRE(split_string.at(7) == "ht");
            }
        }
    }
}