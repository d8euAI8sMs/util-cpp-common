#pragma once

#include <util/common/plot/plot.h>
#include <util/common/math/common.h>

namespace plot
{

    template < typename _points_t >
    static inline void sampled_t_to_data(math::sampled_t sampled,
                                         _points_t & points,
                                         double origin = 0,
                                         math::un_op_t mapper = common::identity_un_op())
    {
        points.clear();
        for (size_t i = 0; i < sampled.count; ++i)
        {
            points.emplace_back(origin + i * sampled.period, mapper(i, sampled.samples[i]));
        }
    }
}
