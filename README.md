# rolling_hash_file_diff
A simplified rsync algorithm from [Andrew Tridgell's Ph.D. thesis](https://www.samba.org/~tridge/phd_thesis.pdf).

## Build instructions
This repository uses CMake. You will need at least version 3.22 of CMake and general build tools for your platform.
Build instructions here are specific for Linux, but the commands should be similar for your platform.
1. Clone the repository
2. Create a directory for the CMake build 
* 
    `mkdir build`
3. Enter the directory
* `cd build`
4. Build with cmake
* `cmake ..`
5. Run make
* `make`

Make will create `rolling_hash_file_diff` executable under `build/` and `tests` under `build/tests`. 
You should be able to run them both. All tests should pass.

The project also comes with a Python script for testing the full algorithm. You can find the script under `tests/tester_script.py,` as long as you have built the executable (with Make, above), you should be able to just run the tester script directly. More test_files can be found under `tests/test_files/`.

## [Overview video with the build process and testing for Linux](https://www.dropbox.com/s/xuaz6lazp6e1ly0/rolling_hash_file_diff_overview.mp4?dl=0)

## Testing
There are unit tests with Catch2 under the `tests/` directory.
There is also a Python script with some end-to-end test_cases under the same directory.
After building the executables, you should be able to run both the unit_tests under `build/tests` and the `tester_script.py` under `tests/.`

## Notes
1. Note that the `signature` and `delta` files generated are *human-readable*, adding significant overhead to the algorithm's performance (file size).
This means that the algorithm will not be very good unless the files are heavily similar. We could achieve much better performance by ditching the human-readable output
and working with the underlying bits instead.
2. We do not sanitize user input nor treat any user mistakes.
3. You can also pass a --chunk-size parameter for each operation, but make sure to pass the **same** size for **all** operations if you do so.

## References:

* [Andrew Tridgell's Ph.D. thesis](https://www.samba.org/~tridge/phd_thesis.pdf)
* [rsync tech report](https://rsync.samba.org/tech_report/tech_report.html)
* [rdiff manual page](https://man.archlinux.org/man/rdiff.1.en)
* [MIT OCW lecture](https://www.youtube.com/watch?v=BRO7mVIFt08)
* [MIT OCW recitation](https://www.youtube.com/watch?v=w6nuXg0BISo)
* [Algorithms Live](https://www.youtube.com/watch?v=rA1ZevamGDc)
* [cp-algorithms](https://cp-algorithms.com/string/string-hashing.html#calculation-of-the-hash-of-a-string)
