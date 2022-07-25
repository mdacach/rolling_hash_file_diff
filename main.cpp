#include <fstream>
#include <iostream>
#include <sstream>

#include "file_diff/file_diff.hpp"

auto read_file_to_string(const std::string& file_path) -> std::string;
auto save_to_file(const std::string& file_path, const std::string& content) -> void;
auto save_signature_to_file(const std::string& file_path, const FileDiff::Signature& signature) -> void;
auto read_signature_from_file(const std::string& file_path) -> FileDiff::Signature;

auto main(int argc, const char* argv[]) -> int
{
    //    std::cout << "You have entered " << argc << " arguments: " << '\n';
    //    for (int i = 0; i < argc; ++i)
    //        std::cout << argv[i] << '\n';
    //    std::cout << std::flush;

    // TODO: change this to be variable
    const auto chunk_size = std::size_t{ 30 };

    // We want to update `my_file` to be equal to `other_file`
    // We first send our signature to the other machine

    // Other machine will compute its delta from our signature
    // We will receive the delta and update our file
    const auto command = std::string(argv[1]);
    // TODO: We do not treat user mistakes very well here
    if (command == "signature")
    {
        const auto old_file = read_file_to_string(argv[2]);
        const auto signature_file = argv[3];
        const auto signature = FileDiff::compute_signature(old_file, chunk_size);
        save_signature_to_file(signature_file, signature);
    }
    else if (command == "delta")
    {
        const auto signature = read_signature_from_file(argv[2]);
        const auto new_file = read_file_to_string(argv[3]);
        const auto delta_file = argv[4];
        const auto delta = FileDiff::compute_delta(new_file, signature, chunk_size);
        save_to_file(delta_file, delta);
    }
    else if (command == "patch")
    {
        const auto basis_file = read_file_to_string(argv[2]);
        const auto delta_file = read_file_to_string(argv[3]);
        const auto reconstructed_file = argv[4];
        const auto reconstructed = FileDiff::apply_delta(basis_file, delta_file, chunk_size);
        save_to_file(reconstructed_file, reconstructed);
    }
    else
    {
        std::cout << "Usage must be one of: \n\
            ./rolling_hash_file_diff signature old-file signature-file\n\
            ./rolling_hash_file_diff delta signature-file new-file delta-file\n\
            ./rolling_hash_file_diff patch basis-file delta-file new-file\n";
    }
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

auto save_to_file(const std::string& file_path, const std::string& content) -> void
{
    // Write the updated file so that external script can test
    auto output_file = std::ofstream{ file_path };
    if (!output_file)
        throw std::runtime_error("Could not save results to file");
    output_file << content;
}

auto save_signature_to_file(const std::string& file_path, const FileDiff::Signature& signature) -> void
{
    auto as_string = std::string{};
    assert(std::size(signature.rolling_hashes) == std::size(signature.strong_hashes));
    const auto size = std::size(signature.rolling_hashes);
    for (std::size_t i = 0; i < size; ++i)
    {
        const auto rolling_hash = signature.rolling_hashes.at(i);
        const auto strong_hash = signature.strong_hashes.at(i);
        // TODO: Document this file structure.
        as_string += std::to_string(rolling_hash) + '\n';
        as_string += std::to_string(strong_hash) + '\n';
    }
    save_to_file(file_path, as_string);
}

auto read_signature_from_file(const std::string& file_path) -> FileDiff::Signature
{
    auto input_file = std::ifstream{ file_path };
    if (!input_file)
        throw std::runtime_error("Could not open file\n");
    auto result = FileDiff::Signature{};
    auto current_hash = FileDiff::Hash{};
    // TODO: Document this input operations.
    bool is_rolling = true;
    while (input_file >> current_hash)
    {
        if (is_rolling)
            result.rolling_hashes.push_back(current_hash);
        else
            result.strong_hashes.push_back(current_hash);
        is_rolling = !is_rolling;
    }
    return result;
}
