#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define NOW 2020

#define FMT                                                               \
    "Dear %c. %s,\n"                                                      \
    "  Thank you for your message! We appreciate "                        \
    "the wonderful %d years for your growing up. We will reply soon.\n\n" \
    "Best Wishes.\n"

vector<string> split(string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

void reply(string s) {
    vector<string> v = split(s, " ");

    printf(FMT, v.at(0).c_str()[0], v.at(1).c_str(), (NOW - atoi(v.at(2).c_str())));
}

int main() {
    string s;
    cout << "input: ";
    getline(cin, s);

    reply(s);

    return 0;
}
