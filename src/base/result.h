#ifndef RESULT_H
#define RESULT_H

#include <QString>

template <typename T = QString>
struct Result {
    explicit Result(bool success, const T &message) : success(success), value(message) {}
    const bool success;
    const T value;
};

#endif // RESULT_H
