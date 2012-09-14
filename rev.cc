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

template<typename T>
void WriteReversedLines(basic_istream<T>& input, const char* input_name,
                        basic_ostream<T>& output) {
    while (true) {
        basic_string<T> buffer;
        getline(input, buffer);
        if (input.fail() && !input.eof())
            FATAL("error reading from " << input_name);

        memrev_reverse(&buffer[0], sizeof(T), buffer.length());
        output << buffer;
        if (input.eof())
            break;
        output << endl;
    }
}

template<typename T>
void WriteReversedLines(const char* filename, basic_ostream<T>& output) {
    basic_ifstream<T> file(filename);
    if (file.fail())
        FATAL("could not open file: " << filename);
    WriteReversedLines(file, filename, output);
}

void SetGlobalLocale() {
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

void PrintUsage() {
    cerr <<
"usage: rev [options] [file ...]\n"
"\n"
"Options:\n"
"  -h, --help       show this message and exit\n"
"  -W, --no-wchar   do not treat inputs as wide-character sources (faster)\n";
}

} // namespace

int main(int argc, char** argv) {
    SetGlobalLocale();

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
            PrintUsage();
            return EXIT_FAILURE;
        }
    }
    argv += optind;
    argc -= optind;

    if (argc == 0) {
        if (use_wchar)
            WriteReversedLines(wcin, "stdin", wcout);
        else
            WriteReversedLines(cin, "stdin", cout);
    } else {
        for (int i = 0; i < argc; i++) {
            if (use_wchar)
                WriteReversedLines(argv[i], wcout);
            else
                WriteReversedLines(argv[i], cout);
        }
    }
    return 0;
}
