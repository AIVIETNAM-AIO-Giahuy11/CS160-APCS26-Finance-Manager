/*
Todo:
- Add more constructors
*/
#pragma once
template<typename type>
class DynamicArray{
private:
    type *memoryBlock;
    type *_begin, *_end, *block_end;

    void reallocateBlock(size_t newN)
    {
        size_t N = capacity();
        type *newBlock = static_cast<type*>(operator new (sizeof(type)*(newN+1)));
        type *nbegin, *nend, *nblock_end;

        for (size_t i = 0; i < N; i++)
        {
            if (i<newN)
                new (static_cast<void*>(newBlock+i)) type(std::move(*(_begin+i)));
            (_begin+i)->~type();
        }
        nbegin = newBlock;

        nblock_end = newBlock+newN;
        nend = newBlock+std::min(N,newN);

        operator delete(memoryBlock);
        memoryBlock = newBlock;
        _begin = nbegin;
        _end = nend;
        block_end = nblock_end;
    }
public:

    ~DynamicArray()
    {
        size_t N = size();
        for (size_t i = 0; i < N; i++)
            (_begin+i)->~type();
        operator delete(memoryBlock);
    }

    DynamicArray(size_t N = 0) /* N instance of default obj */
    {
        memoryBlock = static_cast<type*>(operator new(sizeof(type)*(N+1)));
        for (size_t i = 0; i < N; i++)
        {
            new (static_cast<void*>(memoryBlock+i)) type();
        }
        _begin = memoryBlock;
        _end = block_end = memoryBlock+N;
    }

    DynamicArray(size_t N, const type &obj) /* N instance of obj */
    {
        memoryBlock = static_cast<type*>(operator new(sizeof(type)*(N+1)));
        for (size_t i = 0; i < N; i++)
        {
            new (static_cast<void*>(memoryBlock+i)) type(obj);
        }
        _begin = memoryBlock;
        _end = block_end = memoryBlock+N;
    }

    DynamicArray (const DynamicArray& obj) /* Copy constructor */
    {
        size_t N = obj.size();
        memoryBlock = static_cast<type*>(operator new(sizeof(type)*(N+1)));
        for (size_t i = 0; i < N; i++)
        {
            new(static_cast<void*>(memoryBlock+i)) type(obj[i]); 
        }
        _begin = memoryBlock;
        _end = block_end = memoryBlock+N;
    }

    DynamicArray (std::initializer_list<type> objlist) /* Initializer list constructor */
    {
        size_t N = objlist.size();
        memoryBlock = static_cast<type*>(operator new(sizeof(type)*(N+1)));
        for (size_t i = 0; i < N; i++)
        {
            new (static_cast<void*>(memoryBlock+i)) type(std::move(objlist.begin()[i]));
        }
        _begin = memoryBlock;
        _end = block_end = memoryBlock+N;
    }

    friend void swap(DynamicArray &obj1, DynamicArray &obj2) noexcept /* Overload std::swap */
    {
        std::swap(obj1.memoryBlock, obj2.memoryBlock);
        std::swap(obj1._begin, obj2._begin);
        std::swap(obj1._end, obj2._end);
        std::swap(obj1.block_end, obj2.block_end);
    }

    DynamicArray& operator=(DynamicArray other) /* Copy and swap */
    {
        swap(*this, other);
        return *this;
    }

    inline size_t size() const {return _end-_begin;}
    inline size_t capacity() const {return block_end-_begin;}
    inline type* begin() const {return _begin;}
    inline type* end() const {return _end;}
    inline bool empty() const {return _begin==_end;}
    
    type& back() const
    {
        if (empty()) throw std::out_of_range("Attempting back() on empty state");
        return *(_end-1);
    }

    type& front() const
    {
        if (empty()) throw std::out_of_range("Attempting front() on empty state");
        return *_begin;
    }

    type& operator[](size_t idx) const
    {
        if (idx<0 || idx>=size()) throw std::out_of_range("OOB: Index out of bound");
        return *(_begin+idx);
    }

    void push_back(type &&obj) /* Temporary (rvalue) */
    {
        if (_end==block_end) reallocateBlock(std::max(1,(int)size()<<1));
        new (static_cast<void*>(memoryBlock+size())) type(std::move(obj));
        _end++;
    }

    void push_back(const type &obj) /* Named (lvalue) */
    {
        if ((&obj)>=_begin && (&obj)<_end) /* Handling self insertion */
        {
            type temp_obj = obj;
            push_back(temp_obj);
            return;
        }
        if (_end==block_end) reallocateBlock(std::max(1,(int)size()<<1));
        new (static_cast<void*>(memoryBlock+size())) type(obj);
        _end++;
    }

    void pop_back()
    {
        if (empty()) throw std::out_of_range("Attempting pop_back on empty state");
        _end--;
        _end->~type();
    }

    void reserve(size_t N) {reallocateBlock(N);}
    
    void clear()
    {
        size_t N = size();
        for (size_t i = 0; i < N; i++)
            (_begin+i)->~type();
        _end = _begin;
    }

};