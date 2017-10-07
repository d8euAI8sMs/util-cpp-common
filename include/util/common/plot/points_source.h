#pragma once

#include <functional>
#include <vector>

#include <util/common/plot/ptr.h>
#include <util/common/plot/shape.h>
#include <util/common/plot/viewport.h>

namespace plot
{

    template < typename _points_t >
    using points_source_t = std::function < plot::ptr_t < _points_t > (const viewport &) > ;

    template < typename _points_t >
    static inline points_source_t < _points_t > make_points_source
    (
        const _points_t & points
    )
    {
        plot::ptr_t < _points_t > copy = create < _points_t > (points);
        return [copy] (const viewport &) { return std::move(copy); };
    }

    template < typename _points_t >
    static inline points_source_t < _points_t > make_points_source
    (
        plot::ptr_t < _points_t > points
    )
    {
        return [points] (const viewport &) { return std::move(points); };
    }
}
