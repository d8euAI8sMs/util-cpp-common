#pragma once

#include <memory>
#include <functional>
#include <vector>

#include <util/common/plot/shape.h>
#include <util/common/plot/viewport.h>

namespace plot
{

    using points_t = std::vector < point < double > > ;
    using points_source_t = std::function < std::shared_ptr < points_t > (const viewport &) > ;

    static inline points_source_t make_points_source
    (
        const points_t & points
    )
    {
        std::shared_ptr < points_t > copy = std::make_shared < points_t > (points);
        return [copy] (const viewport &) { return std::move(copy); };
    }

    static inline points_source_t make_points_source
    (
        std::shared_ptr < points_t > points
    )
    {
        return [points] (const viewport &) { return std::move(points); };
    }
}
