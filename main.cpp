#include <fstream>
#include <iostream>
#include <sstream>

auto read_file_to_string(const std::string& file_path) -> std::string;

auto main(int argc, char* argv[]) -> int
{
    std::cout << "You have entered " << argc << " arguments: " << '\n';
    for (int i = 0; i < argc; ++i)
        std::cout << argv[i] << '\n';
    auto first_file = read_file_to_string(argv[1]);
    auto second_file = read_file_to_string(argv[2]);
    std::cout << "First file: " << first_file << '\n';
    std::cout << "Second file: " << second_file << '\n';
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
