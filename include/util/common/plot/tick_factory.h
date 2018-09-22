#pragma once

#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <functional>

#include <util/common/ptr.h>
#include <util/common/plot/viewport.h>

namespace plot
{

    /*****************************************************/
    /*                    -typedefs-                     */
    /*****************************************************/

    using tick_label_t     = std::string;
    using tick_t           = std::pair < double, tick_label_t > ;
    using tick_formatter_t = std::function < tick_label_t(double) > ;

    /*****************************************************/
    /*                  -tick formatters-                */
    /*****************************************************/

    static inline tick_formatter_t make_simple_tick_formatter
    (
        int precision, size_t max_symbols = 10
    )
    {
        return [precision, max_symbols] (double tick)
        {
            std::ostringstream oss;
            oss.precision(precision);
            oss << std::fixed << tick;
            std::string str = oss.str();
            if (str.length() > max_symbols)
            {
                oss = std::ostringstream();
                oss << tick;
                return oss.str();
            }
            return std::move(str);
        };
    }

    /*****************************************************/
    /*                     tick_factory                  */
    /*****************************************************/

    class tick_factory
    {

    public:

        using ptr_t = util::ptr_t < tick_factory > ;

    protected:

        tick_formatter_t fmt;

    public:

        tick_factory(tick_formatter_t fmt)
            : fmt(std::move(fmt))
        {
        }

        virtual util::ptr_t < std::vector < tick_t > >
            produce(const viewport & bounds) = 0;

        virtual ~tick_factory()
        {
        }
    };

    /*****************************************************/
    /*                     axe                           */
    /*****************************************************/

    enum class axe
    {
        x, y
    };

    /*****************************************************/
    /*            const_interval_tick_factory            */
    /*****************************************************/

    template < axe x_or_y >
    class const_interval_tick_factory : public tick_factory
    {

    public:

        using ptr_t = util::ptr_t < const_interval_tick_factory > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    private:

        double origin;
        double interval;

    public:

        const_interval_tick_factory(tick_formatter_t fmt,
                                    double origin,
                                    double interval)
            : tick_factory(std::move(fmt))
            , origin(origin)
            , interval(interval)
        {
        }

        virtual ~const_interval_tick_factory()
        {
        }

        virtual util::ptr_t < std::vector < tick_t > >
            produce(const viewport &bounds) override
        {
            double size, min, max;

            if (axe::x == x_or_y)
            {
                size = bounds.world.width();
                min = bounds.world.xmin;
                max = bounds.world.xmax;
            }
            else
            {
                size = bounds.world.height();
                min = bounds.world.ymin;
                max = bounds.world.ymax;
            }

            util::ptr_t < std::vector < tick_t > > ticks
                = util::create < std::vector < tick_t > > ();

            if (size < 1e-8) return ticks;

            int n = int(size / interval);

            ticks->reserve(n + 2);

            int d = (int) std::ceil((origin - min) / interval);

            double start = origin - d * interval;

            int i = 0;
            for (double tick = start; tick < max; tick += interval, ++i)
            {
                if (tick <= min) continue;
                ticks->emplace_back(tick, fmt(tick));
            }

            return std::move(ticks);
        }
    };

    /*****************************************************/
    /*             const_n_tick_factory                  */
    /*****************************************************/

    template < axe x_or_y >
    class const_n_tick_factory : public tick_factory
    {

    public:

        using ptr_t = util::ptr_t < const_n_tick_factory > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    private:

        double origin;
        int n_intervals;

    public:

        const_n_tick_factory(tick_formatter_t fmt,
                             double origin,
                             int n_intervals)
            : tick_factory(std::move(fmt))
            , origin(origin)
            , n_intervals(n_intervals)
        {
        }

        virtual ~const_n_tick_factory()
        {
        }

        virtual util::ptr_t < std::vector < tick_t > >
            produce(const viewport &bounds) override
        {
            double size, min, max;

            if (axe::x == x_or_y)
            {
                size = bounds.world.width();
                min = bounds.world.xmin;
                max = bounds.world.xmax;
            }
            else
            {
                size = bounds.world.height();
                min = bounds.world.ymin;
                max = bounds.world.ymax;
            }

            double interval = size / n_intervals;

            util::ptr_t < std::vector < tick_t > > ticks
                = std::make_shared < std::vector < tick_t > > ();

            if (size < 1e-8) return ticks;

            ticks->reserve(n_intervals + 2);

            double _d = (origin - min) / interval;
            if ((_d + 1) > INT_MAX || (_d - 1) < INT_MIN)
            {
                return ticks;
            }
            int d = (int) std::ceil(_d);

            double start = origin - d * interval;

            int i = 0;
            for (double tick = start; tick < max; tick += interval, ++i)
            {
                if (tick <= min) continue;
                ticks->emplace_back(tick, fmt(tick));
            }

            return std::move(ticks);
        }
    };
}
