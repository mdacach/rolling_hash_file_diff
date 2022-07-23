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


created_chunks = create_random_chunks(10, 5)
file1 = generate_file(50, 0.3, created_chunks)
file2 = generate_file(50, 0.3, created_chunks)
open("test_files/file1", "w").write(file1)
open("test_files/file2", "w").write(file2)
