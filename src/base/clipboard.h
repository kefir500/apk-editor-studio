#ifndef CLIPBOARD_H
#define CLIPBOARD_H

template <typename T>
struct ClipboardEntry
{
    ClipboardEntry() : move(false) {}
    ClipboardEntry(T data, bool move = false) : data(data), move(move) {}
    T data;
    bool move;
};

#endif // CLIPBOARD_H
