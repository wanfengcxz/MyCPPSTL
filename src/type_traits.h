//
// Created by 晚风吹行舟 on 2023/4/9.
//

#ifndef MYCPPSTL_TYPE_TRAITS_H
#define MYCPPSTL_TYPE_TRAITS_H

#include <type_traits>

namespace stl {

    template<class T, T v>
    struct m_integral_constant {
        static constexpr T value = v;
    };

    template<bool b>
    using m_bool_constant = m_integral_constant<bool, b>;

    typedef m_bool_constant<true> m_true_type;
    typedef m_bool_constant<false> m_false_type;

    /*********************************************************/
    // type traits

    // is_int
    template<class T>
    struct is_int : m_false_type {
    };

    // specializations
    template<>
    struct is_int<int> : m_true_type {
    };

    // is_pair

    // --- forward declaration begin
    template<class T1, class T2>
    struct pair {
    };
    // --- forward declaration end

    template<class T>
    struct is_pair : m_false_type {
    };

    // specializations
    template<class T1, class T2>
    struct is_pair<pair<T1, T2>> : m_true_type {
    };

}

#endif //MYCPPSTL_TYPE_TRAITS_H
