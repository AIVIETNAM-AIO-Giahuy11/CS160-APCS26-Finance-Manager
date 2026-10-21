/*
Currently working on
- expandBlock
- push_back
- 
*/

template<typename type>
class DynamicArray{
private:
    type *memoryBlock;
    type *begin, *end, *block_end;

    void expandBlock()
    {
        size_t N = capacity();
        size_t newN = (N==0? 1 : N<<1);
        type *newBlock = static_cast<type*>(operator new (sizeof(type)*newN));
        type *nbegin, *nend, *nblock_end;

        nbegin = newBlock;
        for (size_t i = 0; i < N; i++)
        {
            new (static_cast<void*>(newBlock)) type(std::move(*(begin+i)));
            (begin+i)->~type();
        }
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

    DynamicArray()
    {
        memoryBlock = static_cast<type*>(operator new(sizeof(type)));
        begin = end = block_end = memoryBlock;
    }

    DynamicArray(size_t N)
    {
        memoryBlock = static_cast<type*>(operator new(sizeof(type)*(N+1)));
        for (size_t i = 0; i < N; i++)
        {
            new (static_cast<void*>(memoryBlock+i)) type();
        }
        begin = memoryBlock;
        end = block_end = memoryBlock+N;
    }

    inline size_t size()
    {
        return end-begin;
    }

    inline size_t capacity()
    {
        return block_end-begin;
    }

    type& operator[](size_t idx)
    {
        if (idx<0 || idx>=size()) throw std::out_of_range("OOB: Index out of bound");
        return *(begin+idx);
    }

    void push_back(const type &obj)
    {
        // std::cout << "Attempted push_back " << obj << '\n';
        if (end==block_end) expandBlock();
        end++;
        new (static_cast<void*>(memoryBlock+size()-1)) type(std::move(obj));
    }
};