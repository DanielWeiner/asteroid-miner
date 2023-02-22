#ifndef UTIL_RANGE_H_
#define UTIL_RANGE_H_
#include <iterator>
#include <cstddef>

namespace Util {
    template <typename T>
    class MultiRange;

    template<typename T>
    class Range {
    public:
        Range(T start, T end) : _start(start), _end(end) {};
    private:
        friend MultiRange<T>;
        struct RangeIterator {
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer    = value_type*;
            using reference  = value_type&;

            RangeIterator(value_type value, bool reverse = false) : _value(value), _reverse(reverse) {}

            reference operator*() { return _value; }
            pointer operator->() { return &_value; }

            RangeIterator& operator++() { 
                _reverse ? _value-- : _value++; 
                return *this; 
            }
            RangeIterator operator++(int) { RangeIterator tmp = *this; ++(*this); return tmp; }
            
            friend bool operator== (const RangeIterator& a, const RangeIterator& b) { 
                return a._value == b._value; 
            };
            friend bool operator!= (const RangeIterator& a, const RangeIterator& b) { 
                return a._value != b._value; 
            };
            value_type _value;
            bool       _reverse;
        };
    public:
        RangeIterator begin() const { 
            return RangeIterator(_start, _start > _end); 
        }
        RangeIterator end() const { 
            return RangeIterator(_end, _start > _end);
        }
        
    private:
        T _start;
        T _end;
    };
}


#endif