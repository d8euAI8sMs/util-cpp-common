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

    /*****************************************************/
    /*                     complex                       */
    /*****************************************************/

    template < typename _data_t = double,
               typename = std::enable_if_t < std::is_arithmetic < _data_t > :: value > >
    struct complex
    {
        _data_t re, im;

        template < typename _second_t >
        complex(complex < _second_t > other)
            : re(static_cast < _data_t > (other.re))
            , im(static_cast < _data_t > (other.im))
        {
        }

        template < typename _second_t,
                   typename = std::enable_if_t < std::is_arithmetic < _data_t > :: value > >
        complex(_second_t re, _second_t im = {})
            : re(static_cast < _data_t > (re))
            , im(static_cast < _data_t > (im))
        {
        }

        complex()
            : re({}), im({})
        {
        }
    };

    template < typename _data_t, typename _second_t >
    inline complex < _data_t > operator+(const complex < _data_t > &first, const complex < _second_t > &second)
    {
        return{ first.re + second.re, first.im + second.im };
    }

    template < typename _data_t, typename _second_t >
    inline complex < _data_t > operator-(const complex < _data_t > &first, const complex < _second_t > &second)
    {
        return{ first.re - second.re, first.im - second.im };
    }

    template < typename _data_t >
    inline complex < _data_t > operator-(const complex < _data_t > &first)
    {
        return{ -first.re, -first.im };
    }

    template < typename _data_t, typename _second_t,
               typename = std::enable_if_t < std::is_arithmetic < _second_t > :: value >  >
    inline complex < _data_t > operator*(const complex < _data_t > &first, _second_t n)
    {
        return{ n * first.re, n * first.im };
    }

    template < typename _data_t, typename _second_t,
               typename = std::enable_if_t < std::is_arithmetic < _second_t > :: value >  >
    inline complex < _data_t > operator*(_second_t n, const complex < _data_t > &first)
    {
        return first * n;
    }

    template < typename _data_t, typename _second_t,
               typename = std::enable_if_t < std::is_arithmetic < _second_t > :: value >  >
    inline complex < _data_t > operator+(const complex < _data_t > &first, _second_t n)
    {
        return{ n + first.re, first.im };
    }

    template < typename _data_t, typename _second_t,
               typename = std::enable_if_t < std::is_arithmetic < _second_t > :: value >  >
    inline complex < _data_t > operator+(_second_t n, const complex < _data_t > &first)
    {
        return first + n;
    }

    template < typename _data_t, typename _second_t,
               typename = std::enable_if_t < std::is_arithmetic < _second_t > :: value >  >
    inline complex < _data_t > operator-(const complex < _data_t > &first, _second_t n)
    {
        return{ n - first.re, first.im };
    }

    template < typename _data_t, typename _second_t,
               typename = std::enable_if_t < std::is_arithmetic < _second_t > :: value >  >
    inline complex < _data_t > operator-(_second_t n, const complex < _data_t > &first)
    {
        return first - n;
    }

    template < typename _data_t, typename _second_t >
    inline complex < _data_t > operator*(const complex < _data_t > &first, const complex < _second_t > &second)
    {
        return{
            first.re * second.re - first.im * second.im,
            first.re * second.im + first.im * second.re
        };
    }

    template < typename _data_t, typename _second_t,
               typename = std::enable_if_t < std::is_arithmetic < _second_t > :: value >  >
    inline complex < _data_t > operator/(const complex < _data_t > &first, _second_t n)
    {
        return{ first.re / n, first.im / n };
    }

    template < typename _data_t >
    inline double norm(const complex < _data_t > &first)
    {
        return std::sqrt(norm(first.re * first.re + first.im * first.im));
    }

    template < typename _data_t >
    inline double sqnorm(const complex < _data_t > &first)
    {
        return norm(first.re * first.re + first.im * first.im);
    }

    template < typename _data_t >
    inline complex < _data_t > conjugate(const complex < _data_t > &first)
    {
        return { first.re, - first.im };
    }

    template < typename _data_t, typename _second_t >
    inline complex < _data_t > operator/(const complex < _data_t > &first, const complex < _second_t > &second)
    {
        return first * conjugate(second) / norm(second) / norm(second);
    }

    template < typename _data_t, typename _second_t,
               typename = std::enable_if_t < std::is_arithmetic < _second_t > :: value >  >
    inline complex < _data_t > operator/(_second_t n, const complex < _data_t > &first)
    {
        return first * conjugate(second) / norm(second) / norm(second);
    }
}
