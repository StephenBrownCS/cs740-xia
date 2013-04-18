#include <cassert>
#include "Chunk.h"

Chunk::Chunk(const char* const buffer, const int size):
    size(size)
{    
    this->buffer = new char[size];
    for(int i = 0; i < size; i++){
        this->buffer[i] = buffer[i];
    }
}

Chunk::~Chunk(){
    delete buffer;
}

int Chunk::size(){
    return size;
}

char Chunk::operator[](int index){
    assert(index < size);
    return buffer[index];
}



