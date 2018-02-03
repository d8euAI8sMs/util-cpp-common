#pragma once

#include <cmath>
#include <type_traits>

namespace math
{

    /*****************************************************/
    /*                     scalar                        */
    /*****************************************************/

    template < typename _data_t >
    inline double norm(const _data_t & d)
    {
        return std::abs(d);
    }

    template < typename _data_t >
    inline double sqnorm(const _data_t & d)
    {
        return norm(d) * norm(d);
    }

    template < typename _data_t >
    inline _data_t conjugate(const _data_t & d)
    {
        return d;
    }
}
