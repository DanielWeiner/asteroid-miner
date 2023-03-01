#ifndef UTIL_STRING_H_
#define UTIL_STRING_H_

#include "range.h"
#include <glm/glm.hpp>
#include <string>

namespace Util::String {
    template<glm::length_t L, typename T, glm::qualifier Q = glm::defaultp>
    std::string toString(glm::vec<L,T,Q> vector){
        std::string result = "(";

        for (auto i : Util::Range(0, L)) {
            if (i != 0) {
                result += ", ";
            }
            result += to_string(vector[i]);
        }

        result += ")";

        return result;
    }


    template<typename T>
    class String : public std::basic_string<T> {
    public:
        template<typename U>
        String(String<U>& otherString) : std::basic_string<T>(otherString.begin(), otherString.end()) {};
        template<typename U>
        String(std::basic_string<U>& otherString) : std::basic_string<T>(otherString.begin(), otherString.end()) {};
        template<typename U>
        String(String<U> otherString) : std::basic_string<T>(otherString.begin(), otherString.end()) {};
        template<typename U>
        String(std::basic_string<U> otherString) : std::basic_string<T>(otherString.begin(), otherString.end()) {};
        template<typename U>
        String(const U* chars) : String(std::basic_string<U>(chars)) {};
    };
}

#endif