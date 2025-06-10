#pragma once
// #include <iostream>
#include "../service/egByteArray.h"
#include "../nodes/egDataNodesType.h"
#include <QByteArray>
#include <QList>
#include <QVariant>

/*
class QByteArray {
public:
    QByteArray(const char *data, int size = -1) {}
    void clear() {}
    void append(const char *str, int len) {}
    int size()    { return 0; }
    char * data() { return nullptr; }
};
*/

inline void ByteArrayToQtByteArray(EgByteArrayType& byteArray, QByteArray& qtBA);

inline void QtByteArrayToByteArray(QByteArray& qtBA, EgByteArrayType& byteArray);

QByteArray& operator >> (QByteArray& qtBA, EgByteArrayType& byteArray);

EgByteArrayType& operator >> (EgByteArrayType& byteArray, QByteArray& qtBA);

void egDataNodeFromList(EgDataNodeType& newNode, QList<QVariant>& addValues);