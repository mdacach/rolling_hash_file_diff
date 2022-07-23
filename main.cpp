#include <fstream>
#include <iostream>
#include <sstream>

#include "functions.hpp"

auto read_file_to_string(const std::string& file_path) -> std::string;

auto main(int argc, char* argv[]) -> int
{
    std::cout << "You have entered " << argc << " arguments: " << '\n';
    for (int i = 0; i < argc; ++i)
        std::cout << argv[i] << '\n';
    std::cout << std::flush;
    const auto my_file = read_file_to_string(argv[1]);
    std::cout << my_file << '\n';
    const auto other_file = read_file_to_string(argv[2]);
    std::cout << other_file << '\n';

    // We want to update `my_file` to be equal to `other_file`

    // We first send our signature to the other machine
    const auto chunk_size = std::size_t{ 3 };
    const auto signature = functions::compute_signature(my_file, chunk_size);
    std::cout << "Signature: ";
    for (auto x : signature)
        std::cout << x << " ";
    std::cout << '\n';

    // Other machine will compute its delta from our signature
    const auto delta = functions::compute_delta(other_file, signature, chunk_size);
    std::cout << "Delta: " << delta << '\n';

    // We will receive the delta and update our file
    const auto updated_file = functions::apply_delta(my_file, delta, chunk_size);
    std::cout << "Updated file: " << updated_file << '\n';

    // Write the updated file so that external script can test
    auto output_file = std::ofstream{ "result" };
    if (!output_file)
        throw std::runtime_error("Could not create result file");
    output_file << updated_file;
    return 0;
}

auto read_file_to_string(const std::string& file_path) -> std::string
{
    auto input_file = std::ifstream{ file_path };
    if (!input_file)
        throw std::runtime_error("Could not open file\n");
    auto stream = std::stringstream{};
    stream << input_file.rdbuf();
    return stream.str();
}
