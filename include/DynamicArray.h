/*
Currently working on
- 
*/

template<typename type>
class DynamicArray{
private:
    type *memoryBlock;
    type *begin, *end, *block_end;

    void reallocateBlock(size_t newN)
    {
        size_t N = capacity();
        type *newBlock = static_cast<type*>(operator new (sizeof(type)*(newN+1)));
        type *nbegin, *nend, *nblock_end;

        for (size_t i = 0; i < N; i++)
        {
            if (i<newN)
                new (static_cast<void*>(newBlock+i)) type(std::move(*(begin+i)));
            (begin+i)->~type();
        }
        nbegin = newBlock;

        nblock_end = newBlock+newN;
        nend = newBlock+N;

        operator delete(memoryBlock);
        memoryBlock = newBlock;
        begin = nbegin;
        end = nend;
        block_end = nblock_end;
    }
public:

    ~DynamicArray()
    {
        size_t N = size();
        for (size_t i = 0; i < N; i++)
            (begin+i)->~type();
        operator delete(memoryBlock);
    }

    DynamicArray(size_t N = 0)
    {
        memoryBlock = static_cast<type*>(operator new(sizeof(type)*(N+1)));
        for (size_t i = 0; i < N; i++)
        {
            new (static_cast<void*>(memoryBlock+i)) type();
        }
        begin = memoryBlock;
        end = block_end = memoryBlock+N;
    }

    inline size_t size() {return end-begin;}

    inline size_t capacity() {return block_end-begin;}

    type& operator[](size_t idx)
    {
        if (idx<0 || idx>=size()) throw std::out_of_range("OOB: Index out of bound");
        return *(begin+idx);
    }

    void push_back(type &&obj) /* Temporary value (rvalue) */
    {
        if (end==block_end) reallocateBlock(std::max(1,(int)size()<<1));
        new (static_cast<void*>(memoryBlock+size())) type(std::move(obj));
        end++;
    }

    void push_back(const type &obj) /* Named value (lvalue) */
    {
        if (end==block_end) reallocateBlock(std::max(1,(int)size()<<1));
        new (static_cast<void*>(memoryBlock+size())) type(obj);
        end++;
    }
};