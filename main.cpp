#include <iostream>

#include "file_diff/file_diff.hpp"
#include "io_helpers/io_helpers.hpp"

auto main(int argc, const char* argv[]) -> int
{
    // We want to update `my_file`  to be equal to `other_file`
    // but our network is slow, so we don't want to send big files through it.

    // The algorithm consists of:
    // 1 - Let's compute a `signature` for `my_file`.
    //          This signature is a much smaller file which "represents" `my_file` (not perfectly, of course).
    // 2 - Send the `signature` for the second machine.
    //          Remember our signature is small, so we do not use many resources to send it through the network.
    // 3 - Second machine receives `signature` and computes a `delta` from `other_file` to `signature`.
    //          This delta has exactly what needs to change from our `my_file` to `other_file`.
    // 4 - Second machine sends us the `delta`.
    //          Again, the delta is a small file.
    // 5 - We use `delta` to update our `my_file` such that in the end it is equal to `other_file`.

    // In the end, we have sent two small files through the network (instead of a potentially big one, such as the full
    // file).
    // The tradeoff is that we have done some computation on both our end and theirs.
    // (We computed `signature` and used `delta` to update the file; they computed `delta`).
    // So we are trading time (computation) for "memory" (in terms of sending the files).

    using namespace std::string_literals;
    const auto usage = "Usage must be one of: \n\
                    ./rolling_hash_file_diff signature old-file signature-file [options]\n\
                    ./rolling_hash_file_diff delta signature-file new-file delta-file [options]\n\
                    ./rolling_hash_file_diff patch basis-file delta-file new-file [options]\n"
                       "You may pass '--chunk-size X' in [options] to explicitly ask for a chunk size to be used.\n"
                       "Note that if you choose to do so, you need to pass the same chunk-size to all other related"
                       "commands.\n"
                       "e.g. \n./rolling_hash_file_diff signature my_file out_file --chunk-size 30\n"
                       "will call the signature command with 30 bytes chunk size.\n"s;

    // TODO: We do not treat user mistakes nor sanitize the input.

    // 1. Check if user specified a chunk size
    auto chunk_size = std::size_t{ 30 }; // Default chunk size is 30 bytes
    for (auto i = 1; i < argc; ++i)
    {
        if (argv[i] == "--chunk-size"s)
        {
            assert(i + 1 < argc);
            chunk_size = static_cast<std::size_t>(std::stoi(argv[i + 1]));
        }
    }

    // 2. Parse the user command
    const auto command = std::string(argv[1]);
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
        std::cout << usage << '\n';
        exit(1);
    }
}
