#ifndef UTIL_RANGE_H_
#define UTIL_RANGE_H_
#include <iterator>
#include <cstddef>
#include <type_traits>

namespace Util {
    template <typename T>
    class MultiRange;

    template<typename T, typename U>
    class Range {
    public:
        Range(T start, U end) : _start(start), _end((std::enable_if_t<std::is_convertible_v<T,U>,U>)end) {};
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