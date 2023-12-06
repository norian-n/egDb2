#pragma once
// #include <iostream>
#include "../service/egByteArray.h"

class QByteArray {
public:
    QByteArray(const char *data, int size = -1) {}
    void clear() {}
    void append(const char *str, int len) {}
    int size()    { return 0; }
    char * data() { return nullptr; }
};

inline void ByteArrayToQtByteArray(EgByteArrayType& byteArray, QByteArray& qtBA) {
    qtBA.clear();
    qtBA.append((const char *) byteArray.arrayData, (int) byteArray.dataSize);
}

inline void QtByteArrayToByteArray(QByteArray& qtBA, EgByteArrayType& byteArray) {
    byteArray.dataSize  = (uint64_t) qtBA.size();
    byteArray.reassignDataArray();
    memcpy((void*)byteArray.arrayData, (void*) qtBA.data(), byteArray.dataSize);
}

QByteArray& operator >> (QByteArray& qtBA, EgByteArrayType& byteArray) {
    QtByteArrayToByteArray(qtBA, byteArray);
    return qtBA;
}

EgByteArrayType& operator >> (EgByteArrayType& byteArray, QByteArray& qtBA) {
    ByteArrayToQtByteArray(byteArray, qtBA);
    return byteArray;
}