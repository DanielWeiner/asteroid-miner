#ifndef UTIL_POINTER_SPAN_H_
#define UTIL_POINTER_SPAN_H_

#include <iterator>
#include <cstddef>
#include <type_traits>

namespace Util {

template<typename _Wrapper>
class PointerSpan {
public:
    PointerSpan(_Wrapper& wrapper) : _wrapper(wrapper) {}
private:
    struct PointerIterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::conditional_t<
            std::is_pointer_v<typename _Wrapper::value_type>,
            std::remove_pointer_t<typename _Wrapper::value_type>,
            std::enable_if_t<
                std::is_pointer_v<typename _Wrapper::value_type::pointer>,
                std::remove_pointer_t<typename _Wrapper::value_type::pointer>
            >
        >;
        using wrapper_iterator = typename _Wrapper::iterator;

        PointerIterator(wrapper_iterator it) : _it(it) {}

        value_type& operator*() { return **_it; }
        value_type* operator->() { return &*_it; }

        PointerIterator& operator++() { _it++; return *this; }
        PointerIterator operator++(int) { PointerIterator tmp = *this; ++(*this); return tmp; }
        
        bool operator==(const PointerIterator& rhs) const { return _it == rhs._it; }
        bool operator!=(const PointerIterator& rhs) const { return _it != rhs._it; }

        wrapper_iterator _it;
    };
public:
    PointerIterator begin() { return std::begin(_wrapper); }
    PointerIterator end() { return std::end(_wrapper); }
private:
    _Wrapper& _wrapper;
};

}

#endif