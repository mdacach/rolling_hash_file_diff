#include <iostream>

#include "file_diff/file_diff.hpp"
#include "io_helpers/io_helpers.hpp"

auto main(int argc, const char* argv[]) -> int
{
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
        const auto old_file = io_helpers::read_file_to_string(argv[2]);
        const auto signature_file = argv[3];
        const auto signature = FileDiff::compute_signature(old_file, chunk_size);
        io_helpers::save_signature_to_file(signature_file, signature);
    }
    else if (command == "delta")
    {
        const auto signature = io_helpers::read_signature_from_file(argv[2]);
        const auto new_file = io_helpers::read_file_to_string(argv[3]);
        const auto delta_file = argv[4];
        const auto delta = FileDiff::compute_delta(new_file, signature, chunk_size);
        io_helpers::save_to_file(delta_file, delta);
    }
    else if (command == "patch")
    {
        const auto basis_file = io_helpers::read_file_to_string(argv[2]);
        const auto delta_file = io_helpers::read_file_to_string(argv[3]);
        const auto reconstructed_file = argv[4];
        const auto reconstructed = FileDiff::apply_delta(basis_file, delta_file, chunk_size);
        io_helpers::save_to_file(reconstructed_file, reconstructed);
    }
    else
    {
        std::cout << "Usage must be one of: \n\
            ./rolling_hash_file_diff signature old-file signature-file\n\
            ./rolling_hash_file_diff delta signature-file new-file delta-file\n\
            ./rolling_hash_file_diff patch basis-file delta-file new-file\n";
    }
}
