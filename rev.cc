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

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <locale>
#include <stdexcept>
#include <string>
#include <getopt.h>

#include "memrev.h"

using namespace std;

#define FATAL(msg) \
    { cerr << "rev: error: " << msg << endl; exit(EXIT_FAILURE); }

#define WARNING(msg) \
    { cerr << "rev: warning: " << msg << endl; }

namespace {

template<typename char_type>
void write_reversed_lines(basic_istream<char_type>& input,
                          const char* input_filename,
                          basic_ostream<char_type>& output) {
    while (true) {
        basic_string<char_type> buffer;
        getline(input, buffer);
        if (input.fail() && !input.eof())
            FATAL("error reading from " << input_filename);

        memrev_reverse(&buffer[0], sizeof(char_type), buffer.length());
        output << buffer;

        if (input.eof())
            break;
        output << endl;
    }
}

template<typename char_type>
void write_reversed_lines(const char* filename,
                          basic_ostream<char_type>& output) {
    basic_ifstream<char_type> file(filename);
    if (file.fail())
        FATAL("could not open file: " << filename);
    write_reversed_lines(file, filename, output);
    file.close();
}

void set_global_locale() {
    locale user_locale;
    try {
        user_locale = locale("");
    } catch (runtime_error& e) {
        WARNING("user-preferred locale is not usable");
        WARNING("falling back to \"C\" locale");
        return;
    }
    locale::global(user_locale);

    // Imbue all of the standard streams with the new locale.
    cin.imbue(user_locale);
    cout.imbue(user_locale);
    cerr.imbue(user_locale);
    clog.imbue(user_locale);
    wcin.imbue(user_locale);
    wcout.imbue(user_locale);
    wcerr.imbue(user_locale);
    wclog.imbue(user_locale);
}

void usage() {
    cerr <<
"usage: rev [options] [file ...]\n"
"\n"
"Options:\n"
"  -h, --help       show this message and exit\n"
"  -W, --no-wchar   do not treat inputs as wide-character sources (faster)\n";
}

} // namespace

int main(int argc, char** argv) {
    set_global_locale();

    bool use_wchar = true;
    static const struct option long_options[] = {
        { "help", no_argument, NULL, 'h' },
        { "no-wchar", no_argument, NULL, 'W' },
        { NULL, 0, NULL, 0 }
    };
    while (true) {
        int c = getopt_long(argc, argv, "hW", long_options, NULL);
        if (c == -1)
            break;

        switch (c) {
        case 'W':
            use_wchar = false;
            break;
        case 'h':
        default:
            usage();
            return EXIT_FAILURE;
        }
    }
    argv += optind;
    argc -= optind;

    if (argc == 0) {
        if (use_wchar)
            write_reversed_lines(wcin, "stdin", wcout);
        else
            write_reversed_lines(cin, "stdin", cout);
    } else {
        for (int i = 0; i < argc; i++) {
            if (use_wchar)
                write_reversed_lines(argv[i], wcout);
            else
                write_reversed_lines(argv[i], cout);
        }
    }
    return 0;
}
