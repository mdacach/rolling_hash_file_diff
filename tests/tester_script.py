import filecmp
import os
import subprocess

# We have generated test cases with `similar_file_generator.py`
bad_counter = 0  # Cases where we could not reconstruct the file -> algorithm failed
tests_directory = "/home/matheus/Development/Eiger/rolling_hash_file_diff/tests/test_files/similar_files/"
# Run for each test inside the chosen directory
number_tests = sum(os.path.isdir(tests_directory + p) for p in os.listdir(tests_directory))

# Total size of bytes sent through the network if we did not use the algorithm
send_file_total_size = 0
# Total size of bytes sent through the network using the algorithm
# This is size(signature) + size(delta)
algorithm_total_size = 0
processed_counter = 0

total_signature_file_size = 0
total_delta_file_size = 0

algorithm_better_counter = 0
for test_number in range(1, number_tests + 1):
    # Each test case has two files, the original (file1) and the updated (file2)
    # Ultimately, we want to be able to reconstruct updated from original
    file1 = tests_directory + "test" + str(test_number) + "/file1"
    file2 = tests_directory + "test" + str(test_number) + "/file2"

    # Note that the calls below *creates* new files with our given name
    # Calls the executable passing file1 and file2 as arguments
    # Create the signature from file1
    subprocess.run(["../build/rolling_hash_file_diff", "signature", file1, "signature_file"])
    # `signature_file` is the newly-created file with the corresponding signature

    # Create the delta from file2 regarding signature
    subprocess.run(["../build/rolling_hash_file_diff", "delta", "signature_file", file2, "delta_file"])
    # `delta_file` is the newly-created file with the corresponding delta

    # Patch file1 with delta
    subprocess.run(["../build/rolling_hash_file_diff", "patch", file1, "delta_file", "reconstructed_file"])
    # `reconstructed_file` is the newly-created file with the result of adding the delta to file1

    # If all went well, `reconstructed_file` should be equal to `file2`
    if not filecmp.cmp(file2, "reconstructed_file"):
        print("Something wrong here, probably a hash collision")
        bad_counter += 1

    # Statistics for each test case
    # Depending on how similar the files are, the algorithm has very different efficiency
    # TODO: Better statistics generated from tests
    send_file_total_size += os.stat(file2).st_size
    algorithm_total_size += os.stat('signature_file').st_size + os.stat('delta_file').st_size

    # Analogous to having just sent the new file over the network
    updated_file_size = os.stat(file2).st_size

    # Files from the algorithm
    signature_file_size = os.stat('signature_file').st_size
    delta_file_size = os.stat('delta_file').st_size

    total_signature_file_size += signature_file_size
    total_delta_file_size += delta_file_size

    processed_counter += 1
    if updated_file_size >= signature_file_size + delta_file_size:
        algorithm_better_counter += 1
    print(f"#{processed_counter}/{number_tests}")

print(f"Finished testing the algorithm for {number_tests} test cases in {tests_directory}");
print("Algorithm failures: ", bad_counter)
print(f"Total size for simply sending: {send_file_total_size}")
print(f"Total size for algorithm: {algorithm_total_size}")
print(f"Total size from signatures: {total_signature_file_size}")
print(f"Total size from deltas: {total_delta_file_size}")
print(f"Algorithm better in {algorithm_better_counter}/{number_tests} cases")
