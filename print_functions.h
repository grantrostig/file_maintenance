#ifndef PRINT_FUNCTIONS_H
#define PRINT_FUNCTIONS_H

#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <iterator>

template<typename T>            // utility function to print
std::ostream &operator<<(std::ostream &out, const std::set<T> &v) {
    if (!v.empty()) {
        out << '[';
        std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
        out << "\b\b]";
    } else out<< "[<empty set>]";
    return out;
}

template<typename T>            // utility function to print
std::ostream &operator<<(std::ostream &out, const std::unordered_set<T> &v) {
    if (!v.empty()) {
        out << '[';
        std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
        out << "\b\b]";
    } else out<< "[<empty unordered_set>]";
    return out;
}

template<typename T>            // utility function to print vectors
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v) {
    if (!v.empty()) {
        out << '[';
        std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
        out << "\b\b]";
    } else out<< "[<empty vector>]";
    return out;
}

#endif // PRINT_FUNCTIONS_H
