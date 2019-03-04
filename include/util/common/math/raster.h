#pragma once

#include <util/common/geom/geom.h>

namespace math
{
    
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    inline void _bresenham_rasterize_low(
        const geom::point < int > & p1,
        const geom::point < int > & p2,
        const std::function < void(const geom::point < int > &) > & callback)
    {
        auto d = p2 - p1;
        int yi = 1;
        if (d.y < 0)
        {
            yi = -1;
            d.y = -d.y;
        }
        int D = 2 * d.y - d.x;
        auto p = p1;

        for (; p.x <= p2.x; ++p.x)
        {
            callback(p);
            if (D > 0)
            {
                p.y += yi;
                D -= 2 * d.x;
            }
            D += 2 * d.y;
        }
    }

    inline void _bresenham_rasterize_high(
        const geom::point < int > & p1,
        const geom::point < int > & p2,
        const std::function < void(const geom::point < int > &) > & callback)
    {
        auto d = p2 - p1;
        int xi = 1;
        if (d.x < 0)
        {
            xi = -1;
            d.x = -d.x;
        }
        int D = 2 * d.x - d.y;
        auto p = p1;

        for (; p.y <= p2.y; ++p.y)
        {
            callback(p);
            if (D > 0)
            {
                p.x += xi;
                D -= 2 * d.y;
            }
            D += 2 * d.x;
        }
    }

    inline void bresenham_rasterize(
        geom::point < int > p1,
        geom::point < int > p2,
        const std::function < void(const geom::point < int > &) > & callback)
    {
        if (std::abs(p2.y - p1.y) < std::abs(p2.x - p1.x))
        {
            if (p1.x > p2.x) _bresenham_rasterize_low(p2, p1, callback);
            else             _bresenham_rasterize_low(p1, p2, callback);
        }
        else
        {
            if (p1.y > p2.y) _bresenham_rasterize_high(p2, p1, callback);
            else             _bresenham_rasterize_high(p1, p2, callback);
        }
    }

    // https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm

    // integer part of x
    inline int _ipart(double x) { return (int) std::floor(x); }
    inline int _round(double x) { return _ipart(x + 0.5); }
    // fractional part of x
    inline double _fpart(double x) { return x - std::floor(x); }
    inline double _rfpart(double x) { return 1.0 - _fpart(x); }

    inline void wu_rasterize(
        geom::point < int > p1,
        geom::point < int > p2,
        const std::function < void(const geom::point < int > &, double c) > & callback)
    {
        bool steep = std::abs(p2.y - p1.y) > std::abs(p2.x - p1.x);
        if (steep) { std::swap(p1.x, p1.y); std::swap(p2.x, p2.y); }
        if (p1.x > p2.x) { std::swap(p1, p2); }

        auto d = p2 - p1;
        double grad = (double) d.y / d.x;
        if (d.x == 0) grad = 1;

        int xend = _round(p1.x);
        double yend = p1.y + grad * (xend - p1.x);
        double xgap = _rfpart(p1.x + 0.5);
        int xpxl1 = xend;
        int ypxl1 = _ipart(yend);

        // handle first endpoint
        if (steep)
        {
            callback({ ypxl1, xpxl1 }, _rfpart(yend) * xgap);
            callback({ ypxl1 + 1, xpxl1 },  _fpart(yend) * xgap);
        }
        else
        {
            callback({ xpxl1, ypxl1 }, _rfpart(yend) * xgap);
            callback({ xpxl1, ypxl1 + 1 }, _fpart(yend) * xgap);
        }
        double intery = yend + grad;
    
        // handle second endpoint
        xend = _round(p2.x);
        yend = p2.y + grad * (xend - p2.x);
        xgap = _fpart(p2.x + 0.5);
        int xpxl2 = xend;
        int ypxl2 = _ipart(yend);

        if (steep)
        {
            callback({ ypxl2, xpxl2 }, _rfpart(yend) * xgap);
            callback({ ypxl2 + 1, xpxl2 }, _fpart(yend) * xgap);
        }
        else
        {
            callback({ xpxl2, ypxl2 }, _rfpart(yend) * xgap);
            callback({ xpxl2, ypxl2 + 1 }, _fpart(yend) * xgap);
        }
    
        // main loop
        if (steep)
        {
            for (int x = xpxl1 + 1; x <= xpxl2 - 1; ++x)
            {
                callback({ _ipart(intery), x }, _rfpart(intery));
                callback({ _ipart(intery) + 1, x }, _fpart(intery));
                intery += grad;
            }
        }
        else
        {
            for (int x = xpxl1 + 1; x <= xpxl2 - 1; ++x)
            {
                callback({ x, _ipart(intery) }, _rfpart(intery));
                callback({ x, _ipart(intery) + 1 }, _fpart(intery));
                intery += grad;
            }
        }
    }
}