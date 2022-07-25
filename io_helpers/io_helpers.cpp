//
// Created by matheus on 24/07/22.
//

#include "io_helpers.hpp"

namespace io_helpers
{
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
} // namespace io_helpers
