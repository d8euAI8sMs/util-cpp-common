#pragma once

#include <util/common/plot/plot.h>
#include <util/common/math/common.h>
#include <util/common/iterable.h>

namespace plot
{

    template < typename _container_t >
    static inline void sampled_t_to_data(math::sampled_t sampled,
                                         _container_t & points,
                                         double origin = 0,
                                         math::un_op_t mapper = math::identity_un_op())
    {
        points.clear();
        for (size_t i = 0; i < sampled.count; ++i)
        {
            points.emplace_back(origin + i * sampled.period, mapper(i, sampled.samples[i]));
        }
    }

    using sampled_iterable_t = util::generic_iterable
    <
        util::mapping_iterator_t
        <
            math::sampled_t,
            size_t,
            plot::point < double >
        >
    > ;

    static inline sampled_iterable_t::ptr_t make_sampled_iterable(math::sampled_t sampled, double origin = 0)
    {
        return sampled_iterable_t::create
        (
            sampled,
            [origin] (const math::sampled_t & s) { return 0; },
            [origin] (const math::sampled_t & s) { return s.count; },
            [origin] (const math::sampled_t & s, const size_t & pos, size_t) -> plot::point < double > { return { origin + s.period * pos, s.samples[pos] }; },
            [origin] (const math::sampled_t & s, const size_t & pos, size_t) -> plot::point < double > ::ptr_t { return plot::point <double > ::create(origin + s.period * pos, s.samples[pos]); }
        );
    }

    static inline sampled_iterable_t::ptr_t make_sampled_iterable(util::ptr_t < math::sampled_t > sampled, double origin)
    {
        return sampled_iterable_t::create
        (
            *sampled,
            [sampled /* hold ref */, origin] (const math::sampled_t & s) { return 0; },
            [sampled /* hold ref */, origin] (const math::sampled_t & s) { return s.count; },
            [sampled /* hold ref */, origin] (const math::sampled_t & s, const size_t & pos, size_t) -> plot::point < double > { return { origin + s.period * pos, s.samples[pos] }; },
            [sampled /* hold ref */, origin] (const math::sampled_t & s, const size_t & pos, size_t) -> plot::point < double > ::ptr_t { return plot::point <double > ::create(origin + s.period * pos, s.samples[pos]); }
        );
    }
}
