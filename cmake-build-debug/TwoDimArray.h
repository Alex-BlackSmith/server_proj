//
// Created by root on 22.03.19.
//

#ifndef SOKOBAN_PROJ_TWODIMARRAY_H
#define SOKOBAN_PROJ_TWODIMARRAY_H

#include <iostream>
//#include <libtcod.h>
#include <ostream>
#include <string>

using std::string;
using std::ostream;
using std::ifstream;
using std::istream;
template <typename T> class TwoDimArray {
public:
    TwoDimArray() = default;

    TwoDimArray(const unsigned argLen,const unsigned argWth) : length(argLen), width(argWth)
    {
        value = new T *[length];
        for (auto i = 0; i < length; i++){
            value[i] = new T[width] {};
        }
    }

    ~TwoDimArray() {}


    void setObjPos(const unsigned posX, const unsigned posY, T obj) {
        value[posX][posY] = obj;
    }
    char getObjPos (const unsigned posX, const unsigned posY) const{
        return value[posX][posY];
    }

    unsigned  getDimX() const {
        return width;
    }

    unsigned  getDimY() const {
        return length;
    }
    TwoDimArray getArray(){

        return value;
    }
    friend ostream& operator<< (ostream& stream, TwoDimArray<char>& TwoDArray);
    friend istream& operator>> (istream &file, TwoDimArray<char>& TwoDArray);

private:
    unsigned length;
    unsigned width;
    T **value;
};

istream& operator>> (istream &file, TwoDimArray<char> &TwoDArray) {
    unsigned len, wth;
    file >> len >> wth;
    TwoDimArray<char> mapLocal(len, wth);
    TwoDArray = mapLocal;
    string buf;
    getline(file,buf);
    for (auto i = 0; i < len ; i++){
        for (auto j = 0; j <= wth; j++){
            TwoDArray.setObjPos(i,j,file.get());
        }
    }
}

ostream& operator<<(ostream& stream, TwoDimArray<char>& TwoDArray) {
    for (auto i = 0; i < TwoDArray.length; i++){
        for (auto j = 0; j < TwoDArray.width; j++){
            stream << TwoDArray.getObjPos(i,j);
        }
        std::cout << std::endl;
    }
    return stream;
}

#endif //SOKOBAN_PROJ_TWODIMARRAY_H
