#pragma once

#include <util/common/plot/shape.h>

namespace plot
{

    // coordinate_translator






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








    // viewport







    class viewport
    {

        using s_to_w_t = coordinate_translator < screen_t::value_type, world_t::value_type > ;
        using w_to_s_t = coordinate_translator < world_t::value_type, screen_t::value_type > ;

    public:

        const screen_t screen;
        const world_t  world;

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
    };
}
