
#ifndef CHUNK_H_
#define CHUNK_H_

class Chunk{
private:
    char* buffer;
    const int length;
    
public:
    Chunk(const char* const buffer, const int length);
    
    ~Chunk();

    int size() const;
    
    char operator[](int index);
    
private:
    // No copy, assignment
    Chunk(Chunk & other);
    Chunk & operator=(Chunk & other);
};

#endif
