#include "egQtInterface.h"

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

inline void ByteArrayToQtByteArray(EgByteArrayType& byteArray, QByteArray& qtBA) {
    qtBA.clear();
    qtBA.append((const char *) byteArray.arrayData, (size_t) (byteArray.dataSize-1));
}

inline void QtByteArrayToByteArray(QByteArray& qtBA, EgByteArrayType& byteArray) {
    byteArray.dataSize  = (uint64_t) qtBA.size()+1;
    byteArray.reassignDataArray();
    memcpy((void*)byteArray.arrayData, (void*) qtBA.data(), byteArray.dataSize);
    byteArray.arrayData[byteArray.dataSize-1] = 0;
}

QByteArray& operator >> (QByteArray& qtBA, EgByteArrayType& byteArray) {
    QtByteArrayToByteArray(qtBA, byteArray);
    return qtBA;
}

EgByteArrayType& operator >> (EgByteArrayType& byteArray, QByteArray& qtBA) {
    ByteArrayToQtByteArray(byteArray, qtBA);
    return byteArray;
}

void egDataNodeFromList(EgDataNodeType& newNode, QList<QVariant>& addValues) {
    for (int i = 0; i < addValues.size(); ++i) {
        if (addValues.at(i).type() == 2) { // int type FIXME add other QVariant types
            int value = addValues.at(i).toInt();
            newNode << value;
        } else {
            QByteArray qArray = addValues.at(i).toByteArray();
            EgByteArrayType egArray(qArray.size()+1);
            qArray >> egArray;
            // PrintByteArray(egArray);
            newNode << egArray;
        }
    }
}
