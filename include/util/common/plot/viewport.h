#pragma once

#include <vector>

#include <util/common/plot/ptr.h>
#include <util/common/plot/shape.h>

namespace plot
{

    /*****************************************************/
    /*                  -typedefs-                       */
    /*****************************************************/

    using world_t  = rect < double > ;
    using screen_t = rect < int > ;

    /*****************************************************/
    /*               coordinate_translator               */
    /*****************************************************/

    template
    <
        typename NS,
        typename NT,
        typename PS = rect < NS >,
        typename PT = rect < NT >
    >
    class coordinate_translator
    {

        const PS &source;
        const PT &target;

    public:

        coordinate_translator(const PS &source, const PT &target) : source(source), target(target)
        {
        }

    private:

        inline double scale_x() const
        {
            return
                double(target.width()) /
                double(source.width());
        }

        inline double translation_x() const
        {
            return
                double(target.xmin) - scale_x() * double(source.xmin);
        }

        inline double scale_y() const
        {
            return -
                double(target.height()) /
                double(source.height());
        }

        inline double translation_y() const
        {
            return
                double(target.ymax) - scale_y() * double(source.ymin);
        }

    public:

        inline NT x(NS x) const
        {
            return NT(scale_x() * x + translation_x());
        }

        inline NT y(NS y) const
        {
            return NT(scale_y() * y + translation_y());
        }

        inline point<NT> xy(const point<NS> &xy) const
        {
            return{
                x(xy.x),
                y(xy.y)
            };
        }
    };

    /*****************************************************/
    /*                     viewport                      */
    /*****************************************************/

    class viewport
    {

    public:

        using ptr_t = plot::ptr_t < viewport > ;

        template < class ... T > static ptr_t create(T && ... t) { return plot::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    private:

        using s_to_w_t = coordinate_translator < screen_t::value_type, world_t::value_type > ;
        using w_to_s_t = coordinate_translator < world_t::value_type, screen_t::value_type > ;

    public:

        screen_t screen;
        world_t  world;

    private:

        const s_to_w_t s_to_w;
        const w_to_s_t w_to_s;

    public:

        viewport(const screen_t &screen, const world_t &world)
            : screen(screen), world(world)
            , s_to_w(this->screen, this->world)
            , w_to_s(this->world, this->screen)
        {
        }

        viewport()
            : viewport({ }, { })
        {
        }

        viewport(const viewport &other)
            : viewport(other.screen, other.world)
        {
        }

        const s_to_w_t & screen_to_world() const
        {
            return s_to_w;
        }

        const w_to_s_t & world_to_screen() const
        {
            return w_to_s;
        }

        bool empty() const
        {
            return screen.empty();
        }
    };

    /*****************************************************/
    /*                   world_mapper                    */
    /*****************************************************/

    using world_mapper_t = std::function < world_t (const world_t &) > ;

    static inline world_mapper_t make_world_mapper(const world_t & w)
    {
        return [w] (const world_t & vp) { return w; };
    }

    static inline world_mapper_t make_world_mapper(world_t::ptr_t w)
    {
        return [w] (const world_t & vp) { return *w; };
    }

    static inline world_mapper_t make_maximizing_world_mapper(std::vector < world_mapper_t > ws, bool include_parameter = false)
    {
        return [ws, include_parameter] (const world_t & vp)
        {
            bool has_any = false;

            world_t resulting =
            {
                (std::numeric_limits < double > :: max)   (),
                (std::numeric_limits < double > :: lowest)(),
                (std::numeric_limits < double > :: max)   (),
                (std::numeric_limits < double > :: lowest)()
            };

            for each (auto & m in ws)
            {
                world_t w = m(vp);
                if (w.empty()) continue;

                has_any = true;

                if (resulting.xmin > w.xmin) resulting.xmin = w.xmin;
                if (resulting.xmax < w.xmax) resulting.xmax = w.xmax;
                if (resulting.ymin > w.ymin) resulting.ymin = w.ymin;
                if (resulting.ymax < w.ymax) resulting.ymax = w.ymax;
            }

            if (include_parameter && (!vp.empty()))
            {
                has_any = true;

                if (resulting.xmin > vp.xmin) resulting.xmin = vp.xmin;
                if (resulting.xmax < vp.xmax) resulting.xmax = vp.xmax;
                if (resulting.ymin > vp.ymin) resulting.ymin = vp.ymin;
                if (resulting.ymax < vp.ymax) resulting.ymax = vp.ymax;
            }

            return (has_any ? resulting : world_t());
        };
    }

    /*****************************************************/
    /*                  viewport_mapper                  */
    /*****************************************************/

    using viewport_mapper_t = std::function < viewport (const viewport &) > ;

    static inline viewport_mapper_t make_viewport_mapper(const viewport & vp)
    {
        return [vp] (const viewport &) { return vp; };
    }

    static inline viewport_mapper_t make_viewport_mapper(const world_t & w)
    {
        return [w] (const viewport & vp) { return viewport(vp.screen, w); };
    }

    static inline viewport_mapper_t make_viewport_mapper(const screen_t & s)
    {
        return [s] (const viewport & vp) { return viewport(s, vp.world); };
    }

    static inline viewport_mapper_t make_viewport_mapper(plot::ptr_t < viewport > vp)
    {
        return [vp] (const viewport &) { return *vp; };
    }

    static inline viewport_mapper_t make_viewport_mapper(plot::ptr_t < world_t > w)
    {
        return [w] (const viewport & vp) { return viewport(vp.screen, *w); };
    }

    static inline viewport_mapper_t make_viewport_mapper(plot::ptr_t < screen_t > s)
    {
        return [s] (const viewport & vp) { return viewport(*s, vp.world); };
    }

    static inline viewport_mapper_t make_viewport_mapper(world_mapper_t wm)
    {
        return [wm] (const viewport & vp) { return viewport(vp.screen, wm(vp.world)); };
    }
}
