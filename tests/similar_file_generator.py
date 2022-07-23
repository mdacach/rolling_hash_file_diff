# Create similar files to be used when testing the file diff program
import random
import string


def create_random_chunks(number_chunks, size_chunks):
    chunks = []
    for i in range(number_chunks):
        random_chunk = ''.join(random.choice(string.printable) for _ in range(size_chunks))
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


tests_to_generate = 100
for i in range(tests_to_generate):
    directory_path = "test_files/test" + str(i + 1)
    # os.makedirs(directory_path)

    number_random_chunks = random.randint(1, 50)
    size_chunks = random.randint(1, 200)
    created_chunks = create_random_chunks(10, 5)

    file1_iterations = random.randint(1, 1000)
    file2_iterations = random.randint(1, 1000)
    file1_chunk_probability = random.random()
    file2_chunk_probability = random.random()
    file1 = generate_file(file1_iterations, file1_chunk_probability, created_chunks)
    file2 = generate_file(file2_iterations, file2_chunk_probability, created_chunks)
    open(directory_path + "/file1", "w").write(file1)
    open(directory_path + "/file2", "w").write(file2)
