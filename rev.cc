// Copyright 2012 Philip Puryear
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>

#include "memrev/memrev.h"

using namespace std;

namespace {

/// A reversely-linked, singly-linked list of pointers to memory blocks.
template<typename T>
struct mem_block_rlist {
    T* block_start;
    struct mem_block_rlist* prev;
};

void fatal(const char* msg, ...) {
    va_list ap;
    fprintf(stderr, "rev: fatal: ");
    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

void print_reversed_lines(FILE* file, const char* filename) {
    const size_t kBlockSize = 4096 - 2 * MEMREV_VECTOR_SIZE + 1;
    while (!feof(file)) {
        struct mem_block_rlist<char>* block_list = NULL;
        size_t block_length;
        bool print_newline = true;

        // Read a line out of the file and store it in an rlist of memory
        // blocks of size kBlockSize.
        for (;;) {
            struct mem_block_rlist<char>* prev_block_list = block_list;
            block_list = new struct mem_block_rlist<char>;
            block_list->prev = prev_block_list;

            char alignas(MEMREV_VECTOR_SIZE) *block = new char[kBlockSize];
            if (!fgets(block, kBlockSize, file)) {
                if (feof(file)) {
                    // If we hit EOF without reading anything, just force this
                    // block to be a zero-length string and let the machinery
                    // below take care of the rest.
                    block[0] = '\0';
                } else {
                    fatal("error reading from %s", filename);
                }
            }
            block_list->block_start = block;
            block_length = strlen(block);

            if (block_length > 0 && block[block_length - 1] == '\n') {
                // We use print_newline to remember whether to print a newline,
                // so "forget" the trailing newline from this block.
                block_length--;
                break;
            }
            if (block_length < kBlockSize - 1) {
                // We hit EOF without reading a newline, so don't print one
                // later.
                print_newline = false;
                break;
            }
        }

        // Print the line in reverse.
        while (block_list) {
            char* block = block_list->block_start;
            memrev_reverse(block, 1, block_length);
            fwrite(block, 1, block_length, stdout);

            // Every block other than the last one (in input order) must be
            // full; i.e. its size (including NUL) must be kBlockSize.
            block_length = kBlockSize - 1;

            delete [] block_list->block_start;
            struct mem_block_rlist<char>* prev_block_list = block_list->prev;
            delete block_list;
            block_list = prev_block_list;
        }

        if (print_newline)
            putchar('\n');
    }
}

void usage() {
    fprintf(stderr,
"usage: rev [options] [file ...]\n"
"\n"
"Options:\n"
"  -h, --help   show this message and exit\n");
}

} // namespace

int main(int argc, char** argv) {
    static const struct option long_options[] = {
        { "help", no_argument, NULL, 'h' },
        { NULL, 0, NULL, 0 }
    };
    for (;;) {
        int c = getopt_long(argc, argv, "h", long_options, NULL);
        if (c == -1)
            break;

        switch (c) {
        case 'h':
        default:
            usage();
            return EXIT_FAILURE;
        }
    }
    argv += optind;
    argc -= optind;

    if (argc == 0) {
        print_reversed_lines(stdin, "stdin");
    } else {
        for (int i = 0; i < argc; i++) {
            FILE* file = fopen(argv[i], "r");
            if (!file)
                fatal("could not open file: %s", argv[i]);
            print_reversed_lines(file, argv[i]);
            fclose(file);
        }
    }
    return 0;
}
