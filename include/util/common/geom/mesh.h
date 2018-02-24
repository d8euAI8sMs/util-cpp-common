#pragma once

#include <vector>
#include <set>
#include <bitset>

#include <util/common/geom/point.h>
#include <util/common/geom/circle.h>
#include <util/common/geom/triangle.h>

namespace geom
{

    /*****************************************************/
    /*                     mesh                          */
    /*****************************************************/

    class mesh
    {

    public:

        using flags_t = size_t;
        using idx_t   = size_t;

        enum /* flags */ : flags_t
        {
            no_flags     = 0 << 0,
            phantom      = 1 << 0,
            superstruct  = 1 << 1
        };

        struct dirichlet_cell
        {
            std::vector < idx_t > path;
        };

        struct vertex_info
        {
            flags_t flags;
            point2d_t point;
            std::set < idx_t > neighbor_vertices;
            std::set < idx_t > neighbor_triangles;
            dirichlet_cell neighborhood;
        };

        struct triangle_info
        {
            flags_t flags;
            idx_t vertices[3];
            circle enclosing;
        };

    private:

        std::vector < vertex_info >   _vertices;
        std::vector < triangle_info > _triangles;
        std::vector < idx_t >         _phantom_triangles;

    public:

        const bool autocalculate_neighborhoods;
        const bool autocalculate_neighbor_vertices;

    public:

        const std::vector < vertex_info > & vertices() const
        {
            return _vertices;
        }

        const std::vector < triangle_info > & triangles() const
        {
            return _triangles;
        }

        const point2d_t & point_at(idx_t i) const
        {
            return _vertices[i].point;
        }

        triangle triangle_at(idx_t i) const
        {
            auto & info = _triangles[i];
            return
            {
                point_at(info.vertices[0]),
                point_at(info.vertices[1]),
                point_at(info.vertices[2])
            };
        }

        convex_polygon < > dirichlet_cell_at(idx_t i) const
        {
            auto & v = _vertices[i];
            if (v.neighborhood.path.empty())
            {
                return _make_dirichlet_polygon(i);
            }
            return _dirichlet_cell_to_polygon(v.neighborhood);
        }

        flags_t flags_at(idx_t i) const
        {
            return _vertices[i].flags;
        }

        flags_t flags_at(idx_t i, flags_t flags)
        {
            auto old = _vertices[i].flags;
            _vertices[i].flags = _safe_flags(old, flags);
            return old;
        }

    public:

        mesh()
            : autocalculate_neighbor_vertices(false)
            , autocalculate_neighborhoods(false)
        {
        }

        mesh(bool autocalculate_neighbor_vertices,
             bool autocalculate_neighborhoods)
            : autocalculate_neighbor_vertices(autocalculate_neighbor_vertices)
            , autocalculate_neighborhoods(autocalculate_neighborhoods)
        {
        }

    public:

        mesh & clear()
        {
            _triangles.clear();
            _vertices.clear();
            return *this;
        }

        mesh & calculate_neighbor_vertices()
        {
            _finish(true, false);
            return *this;
        }

        mesh & calculate_neighborhoods()
        {
            _finish(false, true);
            return *this;
        }

        mesh & finish_mesh()
        {
            _finish(true, true);
            return *this;
        }

        mesh & init(const std::vector < point2d_t > & points,
                    flags_t flags = no_flags)
        {
            clear();
            _init(points, flags);
            return *this;
        }

        template < typename _C >
        mesh & init(const _C & c,
                    flags_t flags = no_flags)
        {
            std::vector < point2d_t > points;
            points.insert(points.end(), std::begin(c), std::end(c));
            return init(points, flags);
        }

        template < typename _InputIter >
        mesh & init(_InputIter begin, _InputIter end,
                    flags_t flags = no_flags)
        {
            std::vector < point2d_t > points;
            points.insert(points.end(), begin, end);
            return init(points, flags);
        }

        idx_t add(const point2d_t & p,
                  flags_t flags = no_flags)
        {
            return _add(p, flags);
        }

        template < typename _C >
        std::vector < idx_t > add(const _C & c,
                   flags_t flags = no_flags)
        {
            std::vector < idx_t > remapped(c.size());
            for (size_t i = 0; i < c.size(); ++i)
            {
                remapped[i] = add(c[i], flags);
            }
            return remapped;
        }

        template < typename _InputIter >
        std::vector < idx_t > add(_InputIter begin, _InputIter end,
                   flags_t flags = no_flags)
        {
            std::vector < idx_t > remapped;
            for (; begin != end; ++begin)
            {
                remapped.push_back(add(*begin, flags));
            }
            return remapped;
        }

    private:

        static flags_t _system_flags()
        {
            return phantom | superstruct;
        }

        static flags_t _safe_flags(flags_t f)
        {
            return f & ~_system_flags();
        }

        static flags_t _safe_flags(flags_t old, flags_t f)
        {
            return _safe_flags(f) | (old & _system_flags());
        }

        convex_polygon < >
        _dirichlet_cell_to_polygon(const dirichlet_cell & dc) const
        {
            convex_polygon < > p;
            p.points.resize(dc.path.size());
            for (size_t i = 0; i < dc.path.size(); ++i)
            {
                p.points[i] = _triangles[dc.path[i]].enclosing.center;
            }
            return p;
        }

        dirichlet_cell _angle_map_to_dirichlet_cell(
            const std::vector < std::pair < double, idx_t > > & angles) const
        {
            dirichlet_cell dc;
            dc.path.resize(angles.size());
            for (size_t i = 0; i < angles.size(); ++i)
            {
                dc.path[i] = angles[i].second;
            }

            return dc;
        }

        convex_polygon < > _angle_map_to_polygon(
            const std::vector < std::pair < double, idx_t > > & angles) const
        {
            convex_polygon < > p;
            p.points.resize(angles.size());
            for (size_t i = 0; i < angles.size(); ++i)
            {
                p.points[i] = _triangles[angles[i].second].enclosing.center;
            }

            return p;
        }

        std::vector < std::pair < double, idx_t > >
        _make_dirichlet_cell_angle_map(idx_t i) const
        {
            auto nt = _vertices[i].neighbor_triangles;

            if (nt.empty()) return {};

            std::vector < std::pair < double, idx_t > >
                angles(nt.size());

            auto j = nt.begin();
            auto & p0 = point_at(i);
            for (idx_t i = 0; j != nt.end(); ++j, ++i)
            {
                angles[i] = { angle(p0, _triangles[*j].enclosing.center), *j };
            }
            auto cmp = []
            (const std::pair < double, idx_t > & p1,
             const std::pair < double, idx_t > & p2)
            {
                return p1.first < p2.first;
            };
            std::sort(angles.begin(), angles.end(), cmp);

            return angles;
        }

        dirichlet_cell _make_dirichlet_cell(idx_t v) const
        {
            return _angle_map_to_dirichlet_cell(
                _make_dirichlet_cell_angle_map(v));
        }

        convex_polygon < > _make_dirichlet_polygon(idx_t v) const
        {
            return _angle_map_to_polygon(
                _make_dirichlet_cell_angle_map(v));
        }

        void _finish(bool neighbor_vertices,
                     bool neighborhoods)
        {
            if (!neighbor_vertices && !neighborhoods) return;

            for (idx_t i = 0; i < _vertices.size(); ++i)
            {
                if (neighbor_vertices)
                {
                    _vertices[i].neighbor_vertices.clear();
                    for (auto j = _vertices[i].neighbor_triangles.begin();
                         j != _vertices[i].neighbor_triangles.end();
                         ++j)
                    {
                        if (_triangles[*j].vertices[0] != i)
                            _vertices[i].neighbor_vertices.emplace(_triangles[*j].vertices[0]);
                        if (_triangles[*j].vertices[1] != i)
                            _vertices[i].neighbor_vertices.emplace(_triangles[*j].vertices[1]);
                        if (_triangles[*j].vertices[2] != i)
                            _vertices[i].neighbor_vertices.emplace(_triangles[*j].vertices[2]);
                    }
                }
                if (neighborhoods)
                {
                    _vertices[i].neighborhood = _make_dirichlet_cell(i);
                }
            }
        }

        idx_t _add_triangle(const triangle_info & info)
        {
            idx_t idx = _triangles.size();
            if (!_phantom_triangles.empty())
            {
                idx = _phantom_triangles.back();
                _phantom_triangles.pop_back();
                _triangles[idx] = info;
            }
            else
            {
                _triangles.push_back(info);
            }

            if (autocalculate_neighbor_vertices)
            {
                _vertices[info.vertices[0]].neighbor_vertices.insert(info.vertices[1]);
                _vertices[info.vertices[0]].neighbor_vertices.insert(info.vertices[2]);
                _vertices[info.vertices[1]].neighbor_vertices.insert(info.vertices[0]);
                _vertices[info.vertices[1]].neighbor_vertices.insert(info.vertices[2]);
                _vertices[info.vertices[2]].neighbor_vertices.insert(info.vertices[0]);
                _vertices[info.vertices[2]].neighbor_vertices.insert(info.vertices[1]);
            }

            _vertices[info.vertices[0]].neighbor_triangles.insert(idx);
            _vertices[info.vertices[1]].neighbor_triangles.insert(idx);
            _vertices[info.vertices[2]].neighbor_triangles.insert(idx);

            return idx;
        }

        void _delete_triangle(idx_t idx)
        {
            auto & info = _triangles[idx];
            info.flags |= phantom;
            _phantom_triangles.push_back(idx);

            if (autocalculate_neighbor_vertices)
            {
                _vertices[info.vertices[0]].neighbor_vertices.erase(info.vertices[1]);
                _vertices[info.vertices[0]].neighbor_vertices.erase(info.vertices[2]);
                _vertices[info.vertices[1]].neighbor_vertices.erase(info.vertices[0]);
                _vertices[info.vertices[1]].neighbor_vertices.erase(info.vertices[2]);
                _vertices[info.vertices[2]].neighbor_vertices.erase(info.vertices[0]);
                _vertices[info.vertices[2]].neighbor_vertices.erase(info.vertices[1]);
            }

            if (autocalculate_neighborhoods)
            {
                _vertices[info.vertices[0]].neighborhood.path.clear();
                _vertices[info.vertices[1]].neighborhood.path.clear();
                _vertices[info.vertices[2]].neighborhood.path.clear();
            }

            _vertices[info.vertices[0]].neighbor_triangles.erase(idx);
            _vertices[info.vertices[1]].neighbor_triangles.erase(idx);
            _vertices[info.vertices[2]].neighbor_triangles.erase(idx);
        }

        triangle_info _make_triangle_info(idx_t i1, idx_t i2, idx_t i3)
        {
            auto & v1 = _vertices[i1],
                 & v2 = _vertices[i2],
                 & v3 = _vertices[i3];

            auto flags = (v1.flags | v2.flags | v3.flags) & superstruct;

            return
            {
                flags,
                { i1, i2, i3 },
                enclosing_circle(v1.point, v2.point, v3.point)
            };
        }

        bool _intersects(const triangle_info & info, idx_t t)
        {
            for (size_t i = 0, j = 1; i < 3; ++i, j = (i + 1) % 3)
            for (size_t k = 0, l = 1; k < 3; ++k, l = (k + 1) % 3)
            {
                auto s = make_line_view(_vertices[info.vertices[i]].point,
                              _vertices[info.vertices[j]].point)
                    .segment_intersects(make_line_view(_vertices[_triangles[t].vertices[k]].point,
                                            _vertices[_triangles[t].vertices[l]].point));
                if (status::is_trusted(s, status::line::intersects | status::line::both_segments))
                    return true;
            }
            return false;
        }

        void _triangulate(const std::vector < idx_t > & orphans,
                          std::vector < idx_t > circle_collision_triangles,
                          idx_t bound_to = SIZE_T_MAX)
        {
            ASSERT(orphans.size() >= 3);

            triangle_info info;

            for (idx_t i = 0;     i < orphans.size(); ++i)
            for (idx_t j = i + 1; j < orphans.size(); ++j)
            for (idx_t k = j + 1; k < orphans.size(); ++k)
            {
                if ((bound_to != SIZE_T_MAX) &&
                    (orphans[i] != bound_to) &&
                    (orphans[j] != bound_to) &&
                    (orphans[k] != bound_to))
                    continue;

                info = _make_triangle_info(orphans[i],
                                           orphans[j],
                                           orphans[k]);

                if (info.enclosing.sqradius == 0) continue;

                bool satisfies = true;
                bool circle_collision = false;

                /* check if no other vertex is in enclosing
                   circle of the current triangle */

                for (idx_t l = 0; l < _vertices.size(); ++l)
                {
                    if ((l == orphans[i]) || (l == orphans[j]) || (l == orphans[k])) continue;
                    if (info.enclosing.contains(_vertices[l].point) > 0)
                    {
                        satisfies = false;
                        break;
                    }
                    if (info.enclosing.contains(_vertices[l].point) == 0)
                    {
                        circle_collision = true;
                        continue;
                    }
                }

                if (satisfies)
                {
                    bool can_insert = true;

                    /* check that the current triangle does not
                       intersect other triangles on the circle
                       on which the collision occurred */
                    if (circle_collision)
                    {
                        auto & c1 = info.enclosing;
                        for (idx_t l = 0;
                             l < circle_collision_triangles.size();
                             ++l)
                        {
                            auto & c2 = _triangles[circle_collision_triangles[l]].enclosing;
                            {
                                if (_intersects(info, circle_collision_triangles[l]))
                                {
                                    can_insert = false;
                                    break;
                                }
                            }
                        }
                    }
                    if (can_insert)
                    {
                        idx_t idx = _add_triangle(info);
                        if (circle_collision)
                        {
                            circle_collision_triangles.push_back(idx);
                        }
                    }
                }
            }

            if (autocalculate_neighborhoods)
            {
                for (size_t i = 0; i < orphans.size(); ++i)
                {
                    _vertices[orphans[i]].neighborhood =
                        _make_dirichlet_cell(orphans[i]);
                }
            }
        }

        idx_t _add(const point2d_t & p,
                   flags_t flags)
        {
            flags = _safe_flags(flags);

            for (idx_t i = 0; i < _vertices.size(); ++i)
            {
                if (fuzzy_t::eq(p.x, _vertices[i].point.x) &&
                    fuzzy_t::eq(p.y, _vertices[i].point.y))
                {
                    return i;
                }
            }

            std::set < idx_t > orphans;
            std::vector < idx_t > circle_collision_triangles;

            for (idx_t i = 0; i < _triangles.size(); ++i)
            {
                if (_triangles[i].flags & phantom) continue;
                auto c = _triangles[i].enclosing.contains(p);
                auto & t = _triangles[i];
                if (c >= 0)
                {
                    if (c > 0)
                        _delete_triangle(i);
                    else if (c == 0)
                        circle_collision_triangles.push_back(i);
                    orphans.insert(t.vertices[0]);
                    orphans.insert(t.vertices[1]);
                    orphans.insert(t.vertices[2]);
                }
            }

            if (orphans.empty()) return SIZE_T_MAX;

            _vertices.push_back({ flags, p });

            orphans.insert(_vertices.size() - 1);

            _triangulate(std::vector < idx_t > (orphans.begin(), orphans.end()),
                         std::move(circle_collision_triangles),
                         _vertices.size() - 1);

            return _vertices.size() - 1;
        }

        void _init(const std::vector < point2d_t > & ps,
                   flags_t flags)
        {
            flags = _safe_flags(flags);

            std::vector < idx_t > orphans(ps.size());

            _vertices.resize(ps.size());
            for (idx_t i = 0; i < ps.size(); ++i)
            {
                _vertices[i].point = ps[i];
                _vertices[i].flags = flags | superstruct;
                orphans[i] = i;
            }

            _triangulate(orphans, {});
        }
    };
}
