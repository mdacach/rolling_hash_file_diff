import filecmp
import os
import subprocess

# For all the test files we have generated through `similar_file_generator.py`
bad_counter = 0
for test_number in range(1, 101):
    # Get the correspondent input files
    file1 = "test_files/test" + str(test_number) + "/file1"
    file2 = "test_files/test" + str(test_number) + "/file2"

    # Calls the executable passing file1 and file2 as arguments
    # Create the signature from file1
    subprocess.run(["../build/rolling_hash_file_diff", "signature", file1, "signature_file"])
    signature_file = open("signature_file", "r")
    # print(signature_file.read())

    # Create the delta from file2 regarding signature
    subprocess.run(["../build/rolling_hash_file_diff", "delta", "signature_file", file2, "delta_file"])
    delta_file = open("delta_file", "r")
    # print(delta_file.read())

    # Patch file1 with delta
    subprocess.run(["../build/rolling_hash_file_diff", "patch", file1, "delta_file", "reconstructed_file"])
    reconstructed_file = open("reconstructed_file", "r")
    # print(reconstructed_file.read())

    # If all went well, `reconstructed_file` should be equal to `file2`
    if filecmp.cmp(file2, "reconstructed_file"):
        print("All went well")
    else:
        print("Hash collision here")
        bad_counter += 1

    print("*********** Statistics ***********")
    print(f"File2 size: {os.stat(file2).st_size}")
    print(f"Signature size: {os.stat('signature_file').st_size}")
    print(f"Delta size: {os.stat('delta_file').st_size}")

    print("Failures: ", bad_counter)
