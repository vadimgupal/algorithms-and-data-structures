#include <initializer_list>
#include <memory>
#include <iterator>
#include<stdexcept>
#include<iostream>
#include<limits>
#include<algorithm>


template<typename T>
class MyList
{
private:
    struct mynode {
        T data;
        mynode* next;
        mynode* prev;
        mynode(T d, mynode* p, mynode* n) : data(d), prev(p), next(n) {};
        mynode(T&& d) : data(std::move(d)), prev(nullptr), next(nullptr) {};
    };
    mynode* top;
    size_t _size;

public:
    using value_type      = T;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;


    template<typename Pointer, typename Reference>
    class BaseIterator : public std::iterator<std::bidirectional_iterator_tag, value_type, difference_type, Pointer, Reference>
    {
    public:
        mynode* p;
        BaseIterator(mynode* pp) : p(pp) {};
        friend bool operator==(BaseIterator<Pointer, Reference> v1, BaseIterator<Pointer, Reference> v2) {
            return v1.p == v2.p;
        }
        friend bool operator!=(BaseIterator v1, BaseIterator v2) {
            return !(v1 == v2);
        }
        Pointer operator->()
        {
            return p->data;
        }
        BaseIterator& operator--()
        {
            p = p->prev;
            return *this;
        }
        BaseIterator& operator--(int)
        {
            BaseIterator res(*this);
            --p;
            return res;
        }
        BaseIterator& operator++()
        {
            p = p->next;
            return *this;
        }
        BaseIterator& operator++(int)
        {
            BaseIterator res(*this);
            ++p;
            return res;
        }
        Reference operator*()
        {
            return p->data;
        }
    };

    using iterator               = BaseIterator<pointer, reference>;
    using const_iterator         = BaseIterator<const_pointer, const_reference>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    MyList()
    {
        top = static_cast<mynode*>(operator new(sizeof(mynode)));
        top->next = top;
        top->prev = top;
        _size = 0;
    }
    MyList(const MyList& other):MyList()
    {
        for (T x : other)
            push_back(x);
    }
    MyList(MyList&& other):MyList()
    {
        std::swap(this->top, other.top);
        std::swap(this->_size, other._size);
    }
    MyList(std::initializer_list<T> init):MyList()
    {
        for (auto x : init)
            push_back(x);
    }

    MyList& operator=(const MyList& other)
    {
        if (this != &other)
        {
            clear();

            for (const auto& value : other)
                push_back(value);
        }
        return *this;
    }
    MyList& operator=(MyList&& other) noexcept
    {
        if (this != &other) {
            //clear();

            std::swap(top, other.top);
            std::swap(_size, other._size);
        }
        return *this;
    }
    MyList& operator=(std::initializer_list<T> ilist)
    {
        clear();
        for (const auto& x : ilist)
            push_back(x);
        return *this;
    }

    reference front()
    {
        return top->next->data;
    }
    const_reference front() const
    {
        return top->next->data;
    }
    reference back()
    {
        return top->prev->data;
    }
    const_reference back() const
    {
        return top->prev->data;
    }

    iterator begin() noexcept
    {
        return iterator(top->next);
    }
    const_iterator begin() const noexcept
    {
        return const_iterator(top->next);
    }
    const_iterator cbegin() const noexcept
    {
        return const_iterator(top->next);
    }
    iterator end() noexcept
    {
        return iterator(top);
    }
    const_iterator end() const noexcept
    {
        return const_iterator(top);
    }
    const_iterator cend() const noexcept
    {
        return const_iterator(top);
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(top);
    }
    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(top);
    }
    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(top);
    }
    reverse_iterator rend() noexcept
    {
        return reverse_iterator(top->next);
    }
    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(top->next);
    }
    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(top->next);
    }

    bool empty() const noexcept { return _size == 0; };
    size_type size() const noexcept { return _size; };

    void clear() noexcept
    {
        while (!empty())
            pop_back();
    }

    iterator insert(const_iterator pos, const T& value)
    {
        mynode* x = pos.p->prev;
        mynode* t = new mynode(value, x, pos.p);
        pos.p->prev = t;
        x->next = t;
        _size++;
        return iterator(t);
    }
    iterator insert(const_iterator pos, T&& value)
    {
        mynode* x = pos.p->prev;
        mynode* t = new mynode(std::move(value));
        t->prev = x;
        t->next = pos.p;
        pos.p->prev = t;
        x->next = t;
        _size++;
        return iterator(t);
    }

    iterator erase(iterator pos)
    {
        if (pos.p == top)
            throw std::invalid_argument("");
        mynode* pr = pos.p->prev;
        mynode* nx = pos.p->next;
        delete pos.p;
        pr->next = nx;
        nx->prev = pr;
        --_size;
        return iterator(nx);
    }

    iterator erase(const_iterator pos)
    {
        if (pos.p == top)
            throw std::invalid_argument("");
        mynode* pr = pos.p->prev;
        mynode* nx = pos.p->next;
        delete pos.p;
        pr->next = nx;
        nx->prev = pr;
        --_size;
        return iterator(nx);
    }
    iterator erase(iterator first, iterator last)
    {
        while (first != last)
            erase(first++);
        return last;
    }
    iterator erase(const_iterator first, const_iterator last)
    {
        while (first != last)
            erase(first++);
        return last;
    }

    void push_back(const T& value)
    {
        insert(top, value);
    }
    void push_back(T&& value)
    {
        insert(top, std::move(value));
    }
    void pop_back()
    {
        erase(iterator(top->prev));
    }

    void push_front(const T& value)
    {
        insert(top->next, value);
    }
    void push_front(T&& value)
    {
        insert(top->next, std::move(value));
    }

    void pop_front()
    {
        erase(iterator(top->next));
    }

    void swap(MyList& other) noexcept
    {
        std::swap(top, other.top);
        std::swap(_size, other._size);
    }

    void reverse() noexcept
    {
        mynode* f = top->next;
        mynode* l = top->prev;
        for (int i = 0;i < _size / 2;i++)
        {
            std::swap(f->data, l->data);
            f = f->next;
            l = l->prev;
        }
    }

    ~MyList() { clear();operator delete(top,sizeof(mynode)); top = nullptr; };


    //методы не из заголовка
    /*size_t count_positive() const
    {
        size_t count = 0;
        const_iterator beg = cbegin();
        while (beg != cend())
        {
            if (*beg > 0)count++;
            ++beg;
        }
        return count;
    }*/
    size_t count_positive() const
    {
        return count_if(begin(), end(), [](int x) {return x > 0;});
    }

    std::pair<T, size_t> count_max_element()
    {
        size_t count = 0;
        T max = std::numeric_limits<T>::min();
        iterator beg = begin();
        while (beg != end().p)
        {
            if (*beg > max)
            {
                count = 1;
                max = *beg;
            }
            else if (*beg == max) count++;
            ++beg;
        }
        return { max,count };
    }

    std::pair<const MyList, const MyList> divide_in_pos_and_negs()
    {
        MyList l1, l2;
        mynode* cur = top->next;
        while (cur != top)
        {
            mynode* nextcur = cur->next;
            cur->next = nullptr;
            if (nextcur != top)
                nextcur->prev = nullptr;
            if (cur->data >= T())
            {
                if (l1.top == l1.top->next)
                {
                    l1.top->next = cur;
                    cur->prev = l1.top;
                    l1.top->prev = cur;
                    cur->next = l1.top;
                }
                else
                {
                    l1.top->prev->next = cur;
                    cur->next = l1.top;
                    cur->prev = l1.top->prev;
                    l1.top->prev = cur;
                }
            }
            else
            {
                if (l2.top == l2.top->next)
                {
                    l2.top->next = cur;
                    cur->prev = l2.top;
                    l2.top->prev = cur;
                    cur->next = l2.top;
                }
                else
                {
                    l2.top->prev->next = cur;
                    cur->next = l2.top;
                    cur->prev = l2.top->prev;
                    l2.top->prev = cur;
                }
            }
            cur = nextcur;
        }
        top->next = top;
        top->prev = top;
        _size = 0;
        return std::make_pair(l1, l2);
    }

    // Вывод списка слева направо
    void print_left_to_right() const {
        mynode* current = top->next;
        while (current != top) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }

    // Вывод списка справа налево
    void print_right_to_left() const {
        mynode* current = top->prev;
        while (current != top) {
            std::cout << current->data << " ";
            current = current->prev;
        }
        std::cout << std::endl;
    }
    void println()const
    {
        std::cout << "вывод слева направо:";
        print_left_to_right();
        std::cout << "вывод справа налево:";
        print_right_to_left();
    }
};
