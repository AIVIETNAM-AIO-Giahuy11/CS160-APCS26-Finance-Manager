/*
# Supported functions:
## constructors:
    DynamicArray() : construct empty array
    DynamicArray(N) : N instance of default value
    DynamicArray(N, obj) : N copy of obj
    DynamicArray(other) : copy from other
    DynamicArray({list}) : construct from curly bracket list
## accessing
    operator[] : self-explanatory
    front() : get first element
    back() : get last element
    begin() : get pointer to the first element
    end() : get pointer to the right of last element
    size() : array size
    capacity() : allocated size
    empty() : self-explanatory
## modifying
    clear() : self-explanatory
    pop_back() : remove last element and shrink array
    push_back(obj) : append obj to array. Cost 1 copy for lvalue, or 1 move for rvalue.
    reserve(N) : modify allocated memory size, shrink array if necessary
    resize(N, obj) : modify array size and construct with obj if needed, can shrink array
    swap(arr1, arr2) : self-explanatory
    operator=(other) : copy other to an empty array and swap with the current array (copy and swap)
    erase(idx) : remove element given by index
    insert(idx,obj) : insert obj to the left of the previous a[idx] (obj is the new arr[idx])
*/
#pragma once
template<typename type>
class DynamicArray{
private:
    type *_begin, *_end, *block_end;

    void reallocate(size_t newN)
    {
        size_t N = size();
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
        /* Note: self insertion is safe as you can't call a constructor on already constructed object */
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
        if (idx>=size()) throw std::out_of_range("OOB: Index out of bound");
        return *(_begin+idx);
    }

    void push_back(type &&obj) /* Temporary (rvalue) */
    {
        if ((&obj)>=_begin && (&obj)<_end && _end==block_end) /* Self insertion guard */
        {
            type temp_obj = obj;
            push_back(temp_obj);
            return;
        }

        if (_end==block_end) reallocate(std::max(1,size()<<1));
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

        if (_end==block_end) reallocate(std::max(1,size()<<1));
        new (static_cast<void*>(_begin+size())) type(obj);
        _end++;
    }

    void pop_back()
    {
        if (empty()) throw std::out_of_range("Attempting pop_back on empty state");
        _end--;
        _end->~type();
    }

    void reserve(size_t N) {reallocate(N);}

    void resize(size_t N, const type &obj = type())
    {
        if ((&obj)>=_begin && (&obj)<_end && capacity()<N) /* Self insertion guard */
        {
            type temp_obj = obj;
            resize(N, temp_obj);
            return;
        }

        while (size()>N) pop_back();
        if (size()==N) return;
        if (capacity()<N) reallocate(std::max(capacity()<<1, N)); // Exponential reallocation if possible

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

    void erase(size_t idx)
    {
        size_t N = size();
        if (idx>=N) throw std::out_of_range("Erase OOB: Index out of bound");
        for (size_t i = idx; i<N-1; i++)
        {
            std::swap(*(_begin+i), *(_begin+i+1));
        }
        pop_back();
    }

    void insert(size_t idx, const type &obj)
    {
        if (idx>size()) throw std::out_of_range("Insert OOB: Index out of bound");
        push_back(obj);
        for (size_t i = size()-1; i>idx; i--)
            std::swap(*(_begin+i), *(_begin+i-1));
    }
};