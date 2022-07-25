# Create files to be used when testing the file diff program
import os
import random
import string


def create_random_chunks(number_chunks, size_chunks, human_readable=False):
    chunks = []
    for i in range(number_chunks):
        random_chunk = ''.join(
            random.choice(string.ascii_letters if human_readable else string.printable) for _ in range(size_chunks))
        chunks.append(random_chunk)

    print(f'chunks: {chunks}')
    return chunks


def generate_file(iterations, chunk_probability, chunks):
    file = ""
    for _ in range(iterations):
        roll = random.random()
        if roll < chunk_probability:
            file += random.choice(chunks)
        else:
            file += random.choice(string.printable)
    return file


def generate_big_files_for_automated_test(directory_path):
    tests_to_generate = 100
    if not os.path.exists(directory_path):
        os.makedirs(directory_path)

    for i in range(tests_to_generate):
        path_to_save = directory_path + "/test" + str(i + 1)
        if not os.path.exists(path_to_save):
            os.makedirs(path_to_save)

        number_random_chunks = random.randint(1, 50)
        size_chunks = random.randint(1, 500)
        created_chunks = create_random_chunks(number_random_chunks, size_chunks)

        file1_iterations = random.randint(1, 1000)
        file2_iterations = random.randint(1, 1000)
        file1_chunk_probability = random.random()
        file2_chunk_probability = random.random()
        file1 = generate_file(file1_iterations, file1_chunk_probability, created_chunks)
        file2 = generate_file(file2_iterations, file2_chunk_probability, created_chunks)
        open(path_to_save + "/file1", "w").write(file1)
        open(path_to_save + "/file2", "w").write(file2)


def generate_small_files_for_manual_test(directory_path):
    if not os.path.exists(directory_path):
        os.makedirs(directory_path)
    tests_to_generate = 1000
    for i in range(tests_to_generate):
        path_to_save = directory_path + "/test" + str(i + 1)
        if not os.path.exists(path_to_save):
            os.makedirs(path_to_save)

        number_random_chunks = random.randint(1, 3)
        size_chunks = random.randint(1, 10)
        created_chunks = create_random_chunks(number_random_chunks, size_chunks, human_readable=True)

        file1_iterations = random.randint(1, 100)
        file2_iterations = random.randint(1, 100)
        file1_chunk_probability = random.random()
        file2_chunk_probability = random.random()
        file1 = generate_file(file1_iterations, file1_chunk_probability, created_chunks)
        file2 = generate_file(file2_iterations, file2_chunk_probability, created_chunks)
        open(path_to_save + "/file1", "w").write(file1)
        open(path_to_save + "/file2", "w").write(file2)


def generate_very_similar_files(directory_path):
    if not os.path.exists(directory_path):
        os.makedirs(directory_path)
    tests_to_generate = 100
    for i in range(tests_to_generate):
        path_to_save = directory_path + "/test" + str(i + 1)
        if not os.path.exists(path_to_save):
            os.makedirs(path_to_save)

        number_random_chunks = random.randint(1, 10)
        size_chunks = random.randint(300, 300)
        created_chunks = create_random_chunks(number_random_chunks, size_chunks, human_readable=True)

        file1_iterations = random.randint(1, 100)
        file2_iterations = random.randint(1, 100)
        file1_chunk_probability = 0.95
        file2_chunk_probability = 0.95
        file1 = generate_file(file1_iterations, file1_chunk_probability, created_chunks)
        file2 = generate_file(file2_iterations, file2_chunk_probability, created_chunks)
        open(path_to_save + "/file1", "w").write(file1)
        open(path_to_save + "/file2", "w").write(file2)

# generate_big_files_for_automated_test("test_files/hard_files")
# generate_very_similar_files("test_files/very_similar_files")
