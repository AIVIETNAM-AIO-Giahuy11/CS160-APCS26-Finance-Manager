/*
Todo:
- Add self insert guard to constructor(N, obj)
*/
#pragma once
template<typename type>
class DynamicArray{
private:
    type *_begin, *_end, *block_end;

    void reallocateBlock(size_t newN)
    {
        size_t N = capacity();
        type *nbegin = static_cast<type*>(operator new (sizeof(type)*(newN+1)));
        type *nend, *nblock_end;

        for (size_t i = 0; i < N; i++)
        {
            if (i<newN)
                new (static_cast<void*>(nbegin+i)) type(std::move(*(_begin+i)));
            (_begin+i)->~type();
        }

        nblock_end = nbegin+newN;
        nend = nbegin+std::min(N,newN);

        operator delete(_begin);
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
        operator delete(_begin);
    }

    DynamicArray(size_t N = 0) /* N instance of default obj */
    {
        _begin = static_cast<type*>(operator new(sizeof(type)*(N+1)));
        for (size_t i = 0; i < N; i++)
        {
            new (static_cast<void*>(_begin+i)) type();
        }
        _end = block_end = _begin+N;
    }

    DynamicArray(size_t N, const type obj) /* N instance of obj */
    {
        _begin = static_cast<type*>(operator new(sizeof(type)*(N+1)));
        for (size_t i = 0; i < N; i++)
        {
            new (static_cast<void*>(_begin+i)) type(obj);
        }
        _end = block_end = _begin+N;
    }

    DynamicArray (const DynamicArray& obj) /* Copy constructor */
    {
        size_t N = obj.size();
        _begin = static_cast<type*>(operator new(sizeof(type)*(N+1)));
        for (size_t i = 0; i < N; i++)
        {
            new(static_cast<void*>(_begin+i)) type(obj[i]); 
        }
        _end = block_end = _begin+N;
    }

    DynamicArray (std::initializer_list<type> objlist) /* Initializer list constructor */
    {
        size_t N = objlist.size();
        _begin = static_cast<type*>(operator new(sizeof(type)*(N+1)));
        for (size_t i = 0; i < N; i++)
        {
            new (static_cast<void*>(_begin+i)) type(std::move(objlist.begin()[i]));
        }
        _end = block_end = _begin+N;
    }

    friend void swap(DynamicArray &obj1, DynamicArray &obj2) noexcept /* Overload std::swap */
    {
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
        new (static_cast<void*>(_begin+size())) type(std::move(obj));
        _end++;
    }

    void push_back(const type &obj) /* Named (lvalue) */
    {
        if ((&obj)>=_begin && (&obj)<_end && _end==block_end) /* Self insertion guard */
        {
            type temp_obj = obj;
            push_back(temp_obj);
            return;
        }

        if (_end==block_end) reallocateBlock(std::max(1,(int)size()<<1));
        new (static_cast<void*>(_begin+size())) type(obj);
        _end++;
    }

    void pop_back()
    {
        if (empty()) throw std::out_of_range("Attempting pop_back on empty state");
        _end--;
        _end->~type();
    }

    void reserve(size_t N) {reallocateBlock(N);}

    void resize(size_t N, const type &obj)
    {
        if ((&obj)>=_begin && (&obj)<_end && capacity()<N) /* Self insertion guard */
        {
            type temp_obj = obj;
            resize(N, temp_obj);
            return;
        }

        while (size()>N) pop_back();
        if (size()==N) return;
        if (capacity()<N) reallocateBlock(N);

        for (size_t i = size(); i < N; i++)
        {
            new (static_cast<void*>(_begin+i)) type(obj);
            _end++;
        }
    }
    
    void clear()
    {
        size_t N = size();
        for (size_t i = 0; i < N; i++)
            (_begin+i)->~type();
        _end = _begin;
    }

};