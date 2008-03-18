#include <string>
#include "dot2tex.h"

string texformat(string str) {
    string result;
    string::size_type pos;

    if (strip_command_sequence) {
        for(pos = 0; pos < str.length(); ++pos) {
            switch (str[pos]) {
                case '\\':
                    str.replace(pos, 1, "\\textbackslash ");
                    pos += 1;
                    break;
                case '{':
                    str.replace(pos, 1, "\\{");
                    pos += 1;
                    break;
                case '}':
                    str.replace(pos, 1, "\\{");
                    pos += 1;
                    break;
                case '#':
                    str.replace(pos, 1, "\\#");
                    pos += 1;
                    break;
                default: ;
            }
        }
    }

    result = str;
    return result;
}

