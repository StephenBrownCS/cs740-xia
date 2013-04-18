

class Chunk{
private:
    char* buffer;
    const int size;
    
public:
    Chunk(const char* const buffer, const int size);
    
    int Chunk::size();
    
    char operator[](int index);
    
private:
    // No copy, assignment
    Chunk(Chunk & other);
    Chunk & operator=(Chunk & other);
};