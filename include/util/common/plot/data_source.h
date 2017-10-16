#pragma once

#include <functional>

#include <util/common/ptr.h>
#include <util/common/plot/shape.h>
#include <util/common/plot/viewport.h>

namespace plot
{

    /*****************************************************/
    /*                 data_source                       */
    /*****************************************************/

    template < typename _container_t >
    using data_source_t = std::function < util::ptr_t < _container_t > (const viewport &) > ;

    template < typename _container_t >
    static inline data_source_t < _container_t > make_data_source
    (
        const _container_t & data
    )
    {
        util::ptr_t < _container_t > copy = util::create < _container_t > (data);
        return [copy] (const viewport &) { return std::move(copy); };
    }

    template < typename _container_t >
    static inline data_source_t < _container_t > make_data_source
    (
        util::ptr_t < _container_t > data
    )
    {
        return [data] (const viewport &) { return std::move(data); };
    }

    /*****************************************************/
    /*                   data_mapper                     */
    /*****************************************************/

    template
    <
        typename _iterator_t,
        typename _mapped_t = typename _iterator_t::value_type
    >
    using data_mapper_t = std::function < _mapped_t (const _iterator_t & pos, size_t idx) > ;
}
