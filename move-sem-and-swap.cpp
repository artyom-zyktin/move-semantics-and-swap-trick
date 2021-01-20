#define SWAP_TRICK

class Foo
{
protected:

    int* array;
    size_t size;

public:

    Foo()
        : array(nullptr), size(0)
    {
        cout << "Default constructor at " << this << endl;
    }

    Foo(int* a, size_t s)
        : array(nullptr), size(0)
    {
        this->array = new int[s];

        if (this->array)
        {
            /*If a critical error occurs during copying, we will get a memory leak. We need to handle this.*/
            try
            {
                size = s;
                std::copy(a, a + size, array);
            }
            catch (exception&)
            {
                cout << "Can't copy from arg to this\n";
                delete[] array;
                array = nullptr;
                throw;
            }
        }

        cout << "Foo constructed at " << this << endl;
    }

    Foo(const Foo& other)
        : array(nullptr), size(0)
    {
        cout << "Copy constructor at " << this << endl;

        if (this != &other)
        {
            array = new int[other.size];

            if (array)
            {
                /*If a critical error occurs during copying, we will get a memory leak.We need to handle this.*/
                try
                {
                    size = other.size;
                    copy(other.array, other.array + this->size, this->array);
                }
                catch (exception&)
                {
                    cout << "Can't copy from other to this\n";
                    delete[] array;
                    array = nullptr;
                    throw;
                }
            }
        }
    }

    Foo& operator=(const Foo& other)
    {
        cout << "Copy operator\n";

        if (this != &other)
        {
#ifndef SWAP_TRICK
            if (array) delete [] array;

            array = new int[other.size];

            if (array)
            {
                /*If a critical error occurs during copying, we will get a memory leak. We need to handle this.*/
                try
                {
                    size = other.size;
                    copy(other.array, other.array + size, array);
                }
                catch (exception&)
                {
                    cout << "Can't copy from other to this\n";
                    delete[] array;
                    array = nullptr;
                    throw;
                }
            }
#else
            /*Fun exercise to understand why this works similarly to the code without SWAP_TRIK definition.*/
            Foo copyable(other);

            size = other.size;
            swap(copyable.array, this->array);
#endif
        }

        return *this;
    }

    /*R-value object will be destroyed anyway. We give him our resources, which should be destroyed.*/
    Foo(Foo&& other) noexcept
        : array(nullptr), size(0)
    {
        if (this != &other)
        {
            size = other.size;
            array = other.array;

            /*Destructor of "other" shouldn't free memory currently owned by "this"*/
            other.array = nullptr;
        }

        cout << "Move constructor at " << this << endl;
    }

    /*R-value object will be destroyed anyway. We give him our resources, which should be destroyed.*/
    Foo& operator=(Foo&& other) noexcept
    {
        cout << "Move operator\n";

        if (this != &other)
        {
#ifndef SWAP_TRICK
            if (array) delete[] array;

            size = other.size;
            array = other.array;

            /*Destructor of "other" shouldn't free memory currently owned by "this"*/
            other.array = nullptr;
#else
            size = other.size;
            swap(this->array, other.array);
#endif
        }

        return *this;
    }

    ~Foo()
    {
        if (array) delete[] array;

        cout << "Foo destructed at " << this << endl;
    }

};

// A tricky function that prevents the chain of constructors from collapsing
Foo cp()
{
    int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    if (time(0) % 2)
    {
        Foo f(a, 10);
        return f;
    }

    return Foo (a, 10);
}

int main()
{
    int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    size_t size = sizeof(a) / sizeof(int);

    Foo _1(a, size);

    Foo _2(_1);

    Foo _3(cp());

    Foo _4;
    Foo _5;

    _4 = _2;
    _5 = Foo(a, size);

    return 0;
}
