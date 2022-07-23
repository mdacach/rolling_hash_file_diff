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
    subprocess.run(["../build/rolling_hash_file_diff", file1, file2])

    # After this call, we have created a `result` file (what file1 becomes after the whole process)
    # If all went well, `result` should be equal to `file2`
    # Note that result is created outside of the specific test directory
    # and is reused between tests
    if filecmp.cmp(file2, "result"):
        print("All went well")
    else:
        print("Hash collision here")
        bad_counter += 1

    print(os.stat(file1))
    print(os.stat(file2))
    print(os.stat("result"))

print("Failures: ", bad_counter)
