// Copyright 2014 Vinzenz Feenstra
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <pypa/types.hh>
#include <cassert>

namespace pypa {

inline bool isxdigit(char c) {
    return (c >= '0' && c <= '9')
        || (c >= 'a' && c <= 'f')
        || (c >= 'A' && c <= 'F');
}

inline bool islower(char c) {
    return (c >= 'a' && c <= 'z');
}

String make_string(String const & input) {
    String result;
    size_t first_quote  = input.find_first_of('"');
    assert(first_quote != String::npos);
    size_t string_start = input.find_first_not_of('"', first_quote);
    assert(string_start != String::npos);
    size_t string_end   = input.size() - 1;
    while(string_end > 0 && input[string_end] == '"') --string_end;
    assert(string_end != String::npos && string_start < string_end);

    char const * s = input.c_str() + string_start;
    char const * end = s + string_end;
    // bool unicode =

    std::back_insert_iterator<String> p((result));

    while(s < end) {
        char c = *s;
        switch(*s) {
        case '\\':
            ++s;
            c = *s;
            assert(s < end);
            switch(c) {
            case '\n': break;
            case '\\': *p++ = '\\'; break;
            case '\'': *p++ = '\''; break;
            case '\"': *p++ = '\"'; break;
            case 'b': *p++ = '\b'; break;
            case 'f': *p++ = '\014'; break; /* FF */
            case 't': *p++ = '\t'; break;
            case 'n': *p++ = '\n'; break;
            case 'r': *p++ = '\r'; break;
            case 'v': *p++ = '\013'; break; /* VT */
            case 'a': *p++ = '\007'; break; /* BEL, not classic C */
            case '4': case '5': case '6': case '7':
                c = s[-1] - '0';
                if (s < end && '0' <= *s && *s <= '7') {
                    c = (c<<3) + *s++ - '0';
                    if (s < end && '0' <= *s && *s <= '7')
                        c = (c<<3) + *s++ - '0';
                }
                *p++ = c;
                break;
            case 'x':
                if (s+1 < end && isxdigit(s[0]) && isxdigit(s[1]))
                {
                    unsigned int x = 0;
                    c = *s;
                    s++;
                    if (isdigit(c))
                        x = c - '0';
                    else if (islower(c))
                        x = 10 + c - 'a';
                    else
                        x = 10 + c - 'A';
                    x = x << 4;
                    c = *s;
                    s++;
                    if (isdigit(c))
                        x += c - '0';
                    else if (islower(c))
                        x += 10 + c - 'a';
                    else
                        x += 10 + c - 'A';
                    *p++ = x;
                    break;
                }
                /* skip \x */
                if (s < end && isxdigit(s[0]))
                    s++; /* and a hexdigit */
                break;
        default:
            *p++ = '\\';
            s--;
        }
        default:
            *p++ = *s++;
            break;
        }
    }
    return result;
}


}
