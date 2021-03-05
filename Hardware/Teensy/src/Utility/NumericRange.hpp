

#ifndef _NUMERIC_RANGE_
#define _NUMERIC_RANGE_

template<typename T>
class NumericRange
{

public:

    class iterator
    {
    public:

        constexpr iterator(T value) : value_(value)
        {}

        ~iterator() = default;

        constexpr T operator*() const
        {
            return value_;
        }

        constexpr bool operator==(iterator other) const
        {
            return other.value_ == value_;
        }

        constexpr bool operator!=(iterator other) const
        {
            return other.value_ != value_;
        }

        constexpr iterator& operator++()
        {
            value_++;
            return *this;
        }

        constexpr iterator operator++(int) const
        {
            iterator old(*this);
            operator++();
            return old;
        }

        // TODO: this should eventually be a random access iterator

    private:
        T value_;
    };

    constexpr NumericRange() = default;

    constexpr NumericRange(T beginVal, T endVal) : beginVal_(beginVal), endVal_(endVal)
    {}

    ~NumericRange() = default;

    constexpr operator bool()
    {
        return isEmpty();
    }

    constexpr T operator[](T index)
    {
        return beginVal_ + index;
    }

    constexpr T getBeginVal() const
    {
        return beginVal_;
    }

    constexpr T getEndVal() const
    {
        return endVal_;
    }

    constexpr iterator begin() const
    {
        return iterator(beginVal_);
    }

    constexpr iterator end() const
    {
        return iterator(endVal_);
    }

    constexpr T size() const
    {
        return endVal_ - beginVal_;
    }

    constexpr bool isEmpty() const
    {
        return beginVal_ == endVal_;
    }

private:

    T beginVal_{};
    T endVal_{};

};

#endif