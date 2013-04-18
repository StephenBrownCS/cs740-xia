#include <cassert>
#include "Chunk.h"

Chunk::Chunk(const char* const buffer, const int length):
    length(length)
{    
    this->buffer = new char[length];
    for(int i = 0; i < length; i++){
        this->buffer[i] = buffer[i];
    }
}

Chunk::~Chunk(){
    delete buffer;
}

int Chunk::size() const{
    return length;
}

char Chunk::operator[](int index){
    assert(index < length);
    return buffer[index];
}



