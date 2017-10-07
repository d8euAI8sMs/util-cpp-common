#pragma once

#include <memory>
#include <functional>
#include <vector>

#include <util/common/plot/shape.h>
#include <util/common/plot/viewport.h>

namespace plot
{

    template < typename _points_t >
    using points_source_t = std::function < std::shared_ptr < _points_t > (const viewport &) > ;

    template < typename _points_t >
    static inline points_source_t < _points_t > make_points_source
    (
        const _points_t & points
    )
    {
        std::shared_ptr < _points_t > copy = std::make_shared < _points_t > (points);
        return [copy] (const viewport &) { return std::move(copy); };
    }

    template < typename _points_t >
    static inline points_source_t < _points_t > make_points_source
    (
        std::shared_ptr < _points_t > points
    )
    {
        return [points] (const viewport &) { return std::move(points); };
    }
}
