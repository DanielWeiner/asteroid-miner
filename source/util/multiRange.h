#ifndef UTIL_MULTI_RANGE_H_
#define UTIL_MULTI_RANGE_H_

#include "range.h"
#include <initializer_list>
#include <vector>

namespace Util {
    template<typename T>
    class MultiRange {
    public:
        MultiRange(std::initializer_list<Range<T>> ranges) : _ranges(ranges) {};
    private:
        struct MultiRangeIterator {
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using range_iterator = typename Range<T>::RangeIterator;
            using value_type = T;
            using pointer    = value_type*;
            using reference  = value_type&;

            MultiRangeIterator(std::vector<Range<T>> ranges, range_iterator it, unsigned rangeIndex) 
            : _ranges(ranges), _it(it), _rangeIndex(rangeIndex) {}

            reference operator*() { return _it._value; }
            pointer operator->() { return &_it._value; }

            MultiRangeIterator& operator++() {
                _it++;
                if (_it == _ranges[_rangeIndex].end() && _rangeIndex != _ranges.size() - 1) {
                    _it = _ranges[_rangeIndex + 1].begin();
                    _rangeIndex++;
                }
                return *this; 
            }
            MultiRangeIterator operator++(int) { 
                MultiRangeIterator tmp = *this; ++(*this); 
                return tmp; 
            }
            
            friend bool operator== (const MultiRangeIterator& a, const MultiRangeIterator& b) { 
                return a._rangeIndex == b._rangeIndex && a._it._value == b._it._value;
            };
            friend bool operator!= (const MultiRangeIterator& a, const MultiRangeIterator& b) { 
                return a._rangeIndex != b._rangeIndex || a._it._value != b._it._value;
            };
        private:
            std::vector<Range<T>> _ranges;
            range_iterator        _it;
            unsigned              _rangeIndex;
        };
    public:
        MultiRangeIterator begin() const { 
            return MultiRangeIterator(_ranges, _ranges[0].begin(), 0); 
        }
        MultiRangeIterator end() const { 
            return MultiRangeIterator(_ranges, _ranges[_ranges.size() - 1].end(),_ranges.size() - 1);
        }
    private:
        std::vector<Range<T>> _ranges;
    };
};


#endif