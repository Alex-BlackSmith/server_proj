//
// Created by root on 22.03.19.
//

#ifndef SOKOBAN_PROJ_TWODIMARRAY_H
#define SOKOBAN_PROJ_TWODIMARRAY_H

#include <iostream>
//#include <libtcod.h>
#include <ostream>


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

#endif //SOKOBAN_PROJ_TWODIMARRAY_H
