//
// Created by matheus on 24/07/22.
//

#ifndef IO_HELPERS_HPP
#define IO_HELPERS_HPP

#include <fstream>
#include <sstream>
#include <string>

#include "../file_diff/file_diff.hpp"

namespace io_helpers
{
    auto read_file_to_string(const std::string& file_path) -> std::string;

    auto read_signature_from_file(const std::string& file_path) -> FileDiff::Signature;

    auto save_to_file(const std::string& file_path, const std::string& content) -> void;

    auto save_signature_to_file(const std::string& file_path, const FileDiff::Signature& signature) -> void;
} // namespace io_helpers

#endif // IO_HELPERS_HPP
