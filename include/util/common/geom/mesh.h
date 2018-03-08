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
            idx_t tree_idx;
            point2d_t point;
            std::set < idx_t > neighbor_vertices;
            std::set < idx_t > neighbor_triangles;
            dirichlet_cell neighborhood;
        };

        struct triangle_info
        {
            flags_t flags;
            idx_t tree_idx;
            idx_t vertices[3];
            circle enclosing;
        };

    private:

        enum quadrant { NW, NE, SE, SW, NO };

        struct rect
        {
            double xmin, xmax, ymin, ymax;
        };

        struct quad_node
        {
            flags_t flags;
            idx_t nw, ne, se, sw;
            idx_t parent;
            std::set < idx_t > elems;

            quad_node(idx_t _nw, idx_t _ne, idx_t _se, idx_t _sw) : flags(0), parent(0)
            {
                nw = _nw; ne = _ne; sw = _sw; se = _se;
            }
            quad_node() : quad_node(0, 0, 0, 0) { }

            bool leaf() const
            {
                return (nw == 0) && (ne == 0) && (sw == 0) && (se == 0);
            }
            bool empty() const
            {
                return elems.empty() && leaf();
            }
            void clear()
            {
                elems.clear();
                nw = ne = sw = se = 0;
                flags = 0;
                parent = 0;
            }
        };

        struct quad_tree
        {
            idx_t root_node;
            rect bounds;
        };

    private:

        std::vector < vertex_info >   _vertices;
        std::vector < triangle_info > _triangles;
        std::vector < idx_t >         _phantom_triangles;
        std::vector < quad_node >     _quad_trees;
        std::vector < idx_t >         _phantom_tree_nodes;
        quad_tree                     _vertices_tree;
        quad_tree                     _enclosing_circles_tree;
        quad_tree                     _dirichlet_cell_tree;

    public:

        const bool autocalculate_neighborhoods;
        const bool autocalculate_neighbor_vertices;
        const size_t max_vertex_tree_deep;
        const size_t max_circle_tree_deep;

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
            , max_vertex_tree_deep(6)
            , max_circle_tree_deep(6)
        {
        }

        mesh(bool autocalculate_neighbor_vertices,
             bool autocalculate_neighborhoods,
             size_t max_vertex_tree_deep = 6,
             size_t max_circle_tree_deep = 6)
            : autocalculate_neighbor_vertices(autocalculate_neighbor_vertices)
            , autocalculate_neighborhoods(autocalculate_neighborhoods)
            , max_vertex_tree_deep(max_vertex_tree_deep)
            , max_circle_tree_deep(max_circle_tree_deep)
        {
        }

    public:

        mesh & clear()
        {
            _triangles.clear();
            _vertices.clear();
            _quad_trees.clear();
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
                0,
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

                /* check if no other vertex is in enclosing
                   circle of the current triangle */

                auto cs = _tree_circle_contains_point(info.enclosing,
                                                      orphans[i], orphans[j], orphans[k]);
                bool satisfies = cs <= 0;
                bool circle_collision = cs == 0;

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
                        _tree_add_c(idx);
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

            auto f = _tree_find_v(p);
            if (f != SIZE_T_MAX) return f;

            std::set < idx_t > orphans;
            std::vector < idx_t > circle_collision_triangles;

            _tree_remove_circles_containing_point(p, orphans, circle_collision_triangles);

            if (orphans.empty()) return SIZE_T_MAX;

            _vertices.push_back({ flags, 0, p });

            orphans.insert(_vertices.size() - 1);

            _tree_add_v(_vertices.size() - 1);

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

            _tree_init();

            _vertices_tree.bounds.xmin = _vertices_tree.bounds.xmax = ps[0].x;
            _vertices_tree.bounds.ymin = _vertices_tree.bounds.ymax = ps[0].y;

            _vertices.resize(ps.size());
            for (idx_t i = 0; i < ps.size(); ++i)
            {
                _vertices[i].point = ps[i];
                _vertices[i].flags = flags | superstruct;
                orphans[i] = i;
                if (_vertices_tree.bounds.xmin > ps[i].x)
                    _vertices_tree.bounds.xmin = ps[i].x;
                if (_vertices_tree.bounds.ymin > ps[i].y)
                    _vertices_tree.bounds.ymin = ps[i].y;
                if (_vertices_tree.bounds.xmax < ps[i].x)
                    _vertices_tree.bounds.xmax = ps[i].x;
                if (_vertices_tree.bounds.ymax < ps[i].y)
                    _vertices_tree.bounds.ymax = ps[i].y;
            }

            _enclosing_circles_tree.bounds = _vertices_tree.bounds;
            _dirichlet_cell_tree.bounds = _vertices_tree.bounds;

            for (idx_t i = 0; i < ps.size(); ++i)
            {
                _tree_add_v(orphans[i]);
            }

            _triangulate(orphans, {});
        }

        void _tree_init()
        {
            _quad_trees.resize(3);
            _vertices_tree.root_node = 0;
            _enclosing_circles_tree.root_node = 1;
            _dirichlet_cell_tree.root_node = 2;
        }

        void _tree_add_v(idx_t v)
        {
            rect bounds = _vertices_tree.bounds, old_bounds;
            idx_t node = _vertices_tree.root_node, node2, old_node;
            idx_t v2 = SIZE_T_MAX;
            for (size_t d = 1; ; ++d)
            {
                if (_quad_trees[node].elems.empty() && (v2 == SIZE_T_MAX) ||
                    (d == max_vertex_tree_deep))
                {
                    if (_quad_trees[node].leaf() || (d == max_vertex_tree_deep))
                    {
                        _quad_trees[node].elems.insert(v);
                        _vertices[v].tree_idx = node;
                        if (v2 != SIZE_T_MAX)
                        {
                            _quad_trees[node2].elems.insert(v2);
                            _vertices[v2].tree_idx = node2;
                        }
                        return;
                    }
                    node = _tree_move_down_v(v, node, bounds);
                    continue;
                }
                /* need to distribute existing node */
                if ((_quad_trees[node].elems.size() == 1) && (v2 == SIZE_T_MAX))
                {
                    v2 = *_quad_trees[node].elems.begin();
                    _quad_trees[node].elems.clear();
                }
                if (v2 != SIZE_T_MAX)
                {
                    old_bounds = bounds;
                    old_node = node;
                    node = _tree_move_down_v(v, old_node, bounds);
                    node2 = _tree_move_down_v(v2, old_node, old_bounds);
                    if (node != node2)
                    {
                        _quad_trees[node].elems.insert(v);
                        _quad_trees[node2].elems.insert(v2);
                        _vertices[v].tree_idx = node;
                        _vertices[v2].tree_idx = node2;
                        return;
                    }
                }
            }
        }

        idx_t _tree_move_down_v(idx_t v, idx_t old_node, rect & b)
        {
            idx_t node;
            switch (_tree_distribute_v(v, b))
            {
            case NW:
                node = _quad_trees[old_node].nw = _tree_ensure_exists(_quad_trees[old_node].nw, old_node);
                b.xmax -= (b.xmax - b.xmin) / 2; b.ymin += (b.ymax - b.ymin) / 2;
                break;
            case SW:
                node = _quad_trees[old_node].sw = _tree_ensure_exists(_quad_trees[old_node].sw, old_node);
                b.xmax -= (b.xmax - b.xmin) / 2; b.ymax -= (b.ymax - b.ymin) / 2;
                break;
            case NE:
                node = _quad_trees[old_node].ne = _tree_ensure_exists(_quad_trees[old_node].ne, old_node);
                b.xmin += (b.xmax - b.xmin) / 2; b.ymin += (b.ymax - b.ymin) / 2;
                break;
            case SE:
                node = _quad_trees[old_node].se = _tree_ensure_exists(_quad_trees[old_node].se, old_node);
                b.xmin += (b.xmax - b.xmin) / 2; b.ymax -= (b.ymax - b.ymin) / 2;
                break;
            case NO:
            default:
                node = _vertices_tree.root_node;
                break;
            }
            return node;
        }

        quadrant _tree_distribute_v(idx_t v, const rect & b)
        {
            auto & p = point_at(v);
            if (((p.x - b.xmin) < (b.xmax - b.xmin) / 2) &&
                ((p.y - b.ymin) < (b.ymax - b.ymin) / 2))
                return SW;
            if (((p.x - b.xmin) < (b.xmax - b.xmin) / 2) &&
                ((p.y - b.ymin) >= (b.ymax - b.ymin) / 2))
                return NW;
            if (((p.x - b.xmin) >= (b.xmax - b.xmin) / 2) &&
                ((p.y - b.ymin) < (b.ymax - b.ymin) / 2))
                return SE;
            if (((p.x - b.xmin) >= (b.xmax - b.xmin) / 2) &&
                ((p.y - b.ymin) >= (b.ymax - b.ymin) / 2))
                return NE;
            return NO;
        }

        idx_t _tree_make_node(idx_t parent)
        {
            if (!_phantom_tree_nodes.empty())
            {
                idx_t i = _phantom_tree_nodes.back();
                _phantom_tree_nodes.pop_back();
                _quad_trees[i].clear();
                _quad_trees[i].parent = parent;
                return i;
            }
            _quad_trees.emplace_back();
            _quad_trees.back().parent = parent;
            return _quad_trees.size() - 1;
        }

        idx_t _tree_ensure_exists(idx_t i, idx_t parent)
        {
            if (i == 0) return _tree_make_node(parent);
            return i;
        }

        void _tree_delete_unused(idx_t i, idx_t root)
        {
            while ((i != root) && _quad_trees[i].empty())
            {
                _quad_trees[i].flags |= phantom;
                _phantom_tree_nodes.push_back(i);
                idx_t p = _quad_trees[i].parent;
                if (_quad_trees[p].se == i) _quad_trees[p].se = 0;
                else if (_quad_trees[p].sw == i) _quad_trees[p].sw = 0;
                else if (_quad_trees[p].ne == i) _quad_trees[p].ne = 0;
                else if (_quad_trees[p].nw == i) _quad_trees[p].nw = 0;
                i = p;
            }
        }

        int _tree_circle_contains_point(const circle & c,
                                         idx_t ign1, idx_t ign2, idx_t ign3)
        {
            return _tree_circle_contains_point(
                c, _vertices_tree.root_node, _vertices_tree.bounds, ign1, ign2, ign3);
        }

        int _tree_circle_contains_point(const circle & c, idx_t node, rect b,
                                        idx_t ign1, idx_t ign2, idx_t ign3)
        {
            bool collision = false;
            if (_quad_trees[node].empty()) return -1;
            if (_quad_trees[node].leaf())
            {
                for (auto it = _quad_trees[node].elems.begin();
                     it != _quad_trees[node].elems.end();
                     ++it)
                {
                    if ((*it == ign1) || (*it == ign2) || (*it == ign3)) continue;
                    auto conf = c.contains(point_at(*it));
                    if (conf > 0) return 1;
                    if (conf == 0) collision = true;
                }
                return collision ? 0 : -1;
            }
            double w = b.xmax - b.xmin, h = b.ymax - b.ymin;
            double d1 = math::sqnorm(c.center.x - (b.xmin + w / 2)) - c.sqradius;
            double d2 = math::sqnorm(c.center.y - (b.ymin + h / 2)) - c.sqradius;
            bool north = true, east = true, south = true, west = true;
            if (fuzzy_t::gt(d1, 0))
            {
                east = ((c.center.x - b.xmin) > w / 2);
                west = !east;
            }
            if (fuzzy_t::gt(d2, 0))
            {
                north = ((c.center.y - b.ymin) > h / 2);
                south = !north;
            }
            if (north && east)
            {
                if (_quad_trees[node].ne != 0)
                {
                    auto r = _tree_circle_contains_point(
                        c, _quad_trees[node].ne,
                        { b.xmin + w / 2, b.xmax, b.ymin + h / 2, b.ymax },
                        ign1, ign2, ign3);
                    if (r > 0) return 1;
                    collision |= (r == 0);
                }
            }
            if (north && west)
            {
                if (_quad_trees[node].nw != 0)
                {
                    auto r = _tree_circle_contains_point(
                        c, _quad_trees[node].nw,
                        { b.xmin, b.xmax - w / 2, b.ymin + h / 2, b.ymax },
                        ign1, ign2, ign3);
                    if (r > 0) return 1;
                    collision |= (r == 0);
                }
            }
            if (south && east)
            {
                if (_quad_trees[node].se != 0)
                {
                    auto r = _tree_circle_contains_point(
                        c, _quad_trees[node].se,
                        { b.xmin + w / 2, b.xmax, b.ymin, b.ymax - h / 2 },
                        ign1, ign2, ign3);
                    if (r > 0) return 1;
                    collision |= (r == 0);
                }
            }
            if (south && west)
            {
                if (_quad_trees[node].sw != 0)
                {
                    auto r = _tree_circle_contains_point(
                        c, _quad_trees[node].sw,
                        { b.xmin, b.xmax - w / 2, b.ymin, b.ymax - h / 2 },
                        ign1, ign2, ign3);
                    if (r > 0) return 1;
                    collision |= (r == 0);
                }
            }
            return collision ? 0 : -1;
        }

        idx_t _tree_find_v(const point2d_t & p)
        {
            return _tree_find_v(
                p, _vertices_tree.root_node, _vertices_tree.bounds);
        }

        idx_t _tree_find_v(const point2d_t & p, idx_t node, rect b)
        {
            if (_quad_trees[node].empty()) return SIZE_T_MAX;
            if (_quad_trees[node].leaf())
            {
                for (auto it = _quad_trees[node].elems.begin();
                     it != _quad_trees[node].elems.end();
                     ++it)
                {
                    if (fuzzy_t::eq(p.x, _vertices[*it].point.x) &&
                        fuzzy_t::eq(p.y, _vertices[*it].point.y))
                        return *it;
                }
                return SIZE_T_MAX;
            }
            bool north = true, east = true, south = true, west = true;
            double w = b.xmax - b.xmin, h = b.ymax - b.ymin;
            if (fuzzy_t::gt(p.x, b.xmin + w / 2))
            {
                east = true; west = false;
            }
            if (fuzzy_t::gt(p.y, b.ymin + h / 2))
            {
                north = true; south = false;
            }
            if (north && east)
            {
                if (_quad_trees[node].ne != 0)
                {
                    auto r = _tree_find_v(
                        p, _quad_trees[node].ne,
                        { b.xmin + w / 2, b.xmax, b.ymin + h / 2, b.ymax });
                    if (r != SIZE_T_MAX) return r;
                }
            }
            if (north && west)
            {
                if (_quad_trees[node].nw != 0)
                {
                    auto r = _tree_find_v(
                        p, _quad_trees[node].nw,
                        { b.xmin, b.xmax - w / 2, b.ymin + h / 2, b.ymax });
                    if (r != SIZE_T_MAX) return r;
                }
            }
            if (south && east)
            {
                if (_quad_trees[node].se != 0)
                {
                    auto r = _tree_find_v(
                        p, _quad_trees[node].se,
                        { b.xmin + w / 2, b.xmax, b.ymin, b.ymax - h / 2 });
                    if (r != SIZE_T_MAX) return r;
                }
            }
            if (south && west)
            {
                if (_quad_trees[node].sw != 0)
                {
                    auto r = _tree_find_v(
                        p, _quad_trees[node].sw,
                        { b.xmin, b.xmax - w / 2, b.ymin, b.ymax - h / 2 });
                    if (r != SIZE_T_MAX) return r;
                }
            }
            return SIZE_T_MAX;
        }

        void _tree_add_c(idx_t t)
        {
            auto & c = _triangles[t].enclosing;
            rect b = _enclosing_circles_tree.bounds;
            idx_t node = _enclosing_circles_tree.root_node;
            for (size_t d = 1; ; ++d)
            {
                double w = b.xmax - b.xmin, h = b.ymax - b.ymin;
                double d1 = math::sqnorm(c.center.x - (b.xmin + w / 2)) - c.sqradius;
                double d2 = math::sqnorm(c.center.y - (b.ymin + h / 2)) - c.sqradius;
                if (!(d == max_circle_tree_deep) &&
                    fuzzy_t::gt(d1, 0) && fuzzy_t::gt(d2, 0))
                {
                    bool east = ((c.center.x - b.xmin) > w / 2);
                    bool north = ((c.center.y - b.ymin) > h / 2);
                    if (north && east)
                    {
                        node = _quad_trees[node].ne = _tree_ensure_exists(_quad_trees[node].ne, node);
                        b.xmin += w / 2; b.ymin += h / 2;
                    }
                    else if (north && !east)
                    {
                        node = _quad_trees[node].nw = _tree_ensure_exists(_quad_trees[node].nw, node);
                        b.xmax -= w / 2; b.ymin += h / 2;
                    }
                    else if (!north && east)
                    {
                        node = _quad_trees[node].se = _tree_ensure_exists(_quad_trees[node].se, node);
                        b.xmin += w / 2; b.ymax -= h / 2;
                    }
                    else // if (!north && !east)
                    {
                        node = _quad_trees[node].sw = _tree_ensure_exists(_quad_trees[node].sw, node);
                        b.xmax -= w / 2; b.ymax -= h / 2;
                    }
                }
                else
                {
                    _quad_trees[node].elems.insert(t);
                    _triangles[t].tree_idx = node;
                    return;
                }
            }
        }

        void _tree_remove_circles_containing_point(
            const point2d_t & p,
            std::set < idx_t > & orphans,
            std::vector < idx_t > & circle_collision_triangles)
        {
            _tree_remove_circles_containing_point(
                p, orphans, circle_collision_triangles,
                _enclosing_circles_tree.root_node,
                _enclosing_circles_tree.bounds);
        }

        void _tree_remove_circles_containing_point(
            const point2d_t & p,
            std::set < idx_t > & orphans,
            std::vector < idx_t > & circle_collision_triangles,
            idx_t node, rect b)
        {
            for (auto it = _quad_trees[node].elems.begin();
                 it != _quad_trees[node].elems.end();)
            {
                auto & t = _triangles[*it];
                auto c = t.enclosing.contains(p);
                if (c >= 0)
                {
                    if (c > 0)
                    {
                        _delete_triangle(*it);
                        it = _quad_trees[node].elems.erase(it);
                    }
                    else if (c == 0)
                    {
                        circle_collision_triangles.push_back(*it);
                        ++it;
                    }
                    orphans.insert(t.vertices[0]);
                    orphans.insert(t.vertices[1]);
                    orphans.insert(t.vertices[2]);
                }
                else
                {
                    ++it;
                }
            }
            if (_quad_trees[node].leaf())
            {
                _tree_delete_unused(node, _enclosing_circles_tree.root_node);
                return;
            }
            double w = b.xmax - b.xmin, h = b.ymax - b.ymin;
            if (((p.x - b.xmin) >= w / 2) && ((p.y - b.ymin) >= h / 2))
            {
                if (_quad_trees[node].ne != 0)
                {
                    _tree_remove_circles_containing_point(
                        p, orphans, circle_collision_triangles,
                        _quad_trees[node].ne,
                        { b.xmin + w / 2, b.xmax, b.ymin + h / 2, b.ymax });
                }
            }
            else if (((p.x - b.xmin) >= w / 2) && ((p.y - b.ymin) < h / 2))
            {
                if (_quad_trees[node].se != 0)
                {
                    _tree_remove_circles_containing_point(
                        p, orphans, circle_collision_triangles,
                        _quad_trees[node].se,
                        { b.xmin + w / 2, b.xmax, b.ymin, b.ymax - h / 2 });
                }
            }
            else if (((p.x - b.xmin) < w / 2) && ((p.y - b.ymin) >= h / 2))
            {
                if (_quad_trees[node].nw != 0)
                {
                    _tree_remove_circles_containing_point(
                        p, orphans, circle_collision_triangles,
                        _quad_trees[node].nw,
                        { b.xmin, b.xmax - w / 2, b.ymin + h / 2, b.ymax });
                }
            }
            else // if (((p.x - b.xmin) < w / 2) && ((p.y - b.ymin) < h / 2))
            {
                if (_quad_trees[node].sw != 0)
                {
                    _tree_remove_circles_containing_point(
                        p, orphans, circle_collision_triangles,
                        _quad_trees[node].sw,
                        { b.xmin, b.xmax - w / 2, b.ymin, b.ymax - h / 2 });
                }
            }
        }

        rect _dirichlet_cell_bounds(idx_t c) const
        {
            rect b;
            auto & dc = _vertices[c].neighborhood;
            b.xmin = b.xmax = _triangles[dc.path.front()].enclosing.center.x;
            b.ymin = b.ymax = _triangles[dc.path.front()].enclosing.center.y;
            for (size_t i = 0; i < dc.path.size(); ++i)
            {
                if (_triangles[dc.path[i]].enclosing.center.x < b.xmin)
                    b.xmin = _triangles[dc.path[i]].enclosing.center.x;
                if (_triangles[dc.path[i]].enclosing.center.x > b.xmax)
                    b.xmax = _triangles[dc.path[i]].enclosing.center.x;
                if (_triangles[dc.path[i]].enclosing.center.y < b.ymin)
                    b.ymin = _triangles[dc.path[i]].enclosing.center.y;
                if (_triangles[dc.path[i]].enclosing.center.y > b.ymax)
                    b.ymax = _triangles[dc.path[i]].enclosing.center.y;
            }
            return b;
        }

        /* adapt addition algorithm of enclosing circle */
        void _tree_add_dc(idx_t t)
        {
            if (_vertices[t].neighborhood.path.empty())
                _vertices[t].neighborhood = _make_dirichlet_cell(t);
            auto & dc = _vertices[t].neighborhood;
            rect b = _enclosing_circles_tree.bounds;
            rect dcb = _dirichlet_cell_bounds(t);
            point2d_t c = { (dcb.xmin + dcb.xmax) / 2, (dcb.ymin + dcb.ymax) / 2 };
            point2d_t s = { (dcb.xmax - dcb.xmin) / 2, (dcb.ymax - dcb.ymin) / 2 };
            idx_t node = _dirichlet_cell_tree.root_node;
            for (size_t d = 1; ; ++d)
            {
                double w = b.xmax - b.xmin, h = b.ymax - b.ymin;
                double d1 = math::sqnorm(c.x - (b.xmin + w / 2)) - s.x * s.x;
                double d2 = math::sqnorm(c.y - (b.ymin + h / 2)) - s.y * s.y;
                if (!(d == max_circle_tree_deep) &&
                    fuzzy_t::gt(d1, 0) && fuzzy_t::gt(d2, 0))
                {
                    bool east = ((c.x - b.xmin) > w / 2);
                    bool north = ((c.y - b.ymin) > h / 2);
                    if (north && east)
                    {
                        node = _quad_trees[node].ne = _tree_ensure_exists(_quad_trees[node].ne, node);
                        b.xmin += w / 2; b.ymin += h / 2;
                    }
                    else if (north && !east)
                    {
                        node = _quad_trees[node].nw = _tree_ensure_exists(_quad_trees[node].nw, node);
                        b.xmax -= w / 2; b.ymin += h / 2;
                    }
                    else if (!north && east)
                    {
                        node = _quad_trees[node].se = _tree_ensure_exists(_quad_trees[node].se, node);
                        b.xmin += w / 2; b.ymax -= h / 2;
                    }
                    else // if (!north && !east)
                    {
                        node = _quad_trees[node].sw = _tree_ensure_exists(_quad_trees[node].sw, node);
                        b.xmax -= w / 2; b.ymax -= h / 2;
                    }
                }
                else
                {
                    _quad_trees[node].elems.insert(t);
                    return;
                }
            }
        }

        /* adapt search algorithm of enclosing circle */
        idx_t _tree_find_dc(const point2d_t & p) const
        {
            return _tree_find_dc(
                p,
                _dirichlet_cell_tree.root_node,
                _dirichlet_cell_tree.bounds);
        }

        idx_t _tree_find_dc(
            const point2d_t & p,
            idx_t node, rect b) const
        {
            for (auto it = _quad_trees[node].elems.begin();
                 it != _quad_trees[node].elems.end();
                 ++it)
            {
                auto t = dirichlet_cell_at(*it);
                auto c = t.contains(p);
                if (status::is(c, status::polygon::contains_point))
                    return *it;
            }
            double w = b.xmax - b.xmin, h = b.ymax - b.ymin;
            if (((p.x - b.xmin) >= w / 2) && ((p.y - b.ymin) >= h / 2))
            {
                if (_quad_trees[node].ne != 0)
                {
                    return _tree_find_dc(
                        p, _quad_trees[node].ne,
                        { b.xmin + w / 2, b.xmax, b.ymin + h / 2, b.ymax });
                }
            }
            else if (((p.x - b.xmin) >= w / 2) && ((p.y - b.ymin) < h / 2))
            {
                if (_quad_trees[node].se != 0)
                {
                    return _tree_find_dc(
                        p, _quad_trees[node].se,
                        { b.xmin + w / 2, b.xmax, b.ymin, b.ymax - h / 2 });
                }
            }
            else if (((p.x - b.xmin) < w / 2) && ((p.y - b.ymin) >= h / 2))
            {
                if (_quad_trees[node].nw != 0)
                {
                    return _tree_find_dc(
                        p, _quad_trees[node].nw,
                        { b.xmin, b.xmax - w / 2, b.ymin + h / 2, b.ymax });
                }
            }
            else // if (((p.x - b.xmin) < w / 2) && ((p.y - b.ymin) < h / 2))
            {
                if (_quad_trees[node].sw != 0)
                {
                    return _tree_find_dc(
                        p, _quad_trees[node].sw,
                        { b.xmin, b.xmax - w / 2, b.ymin, b.ymax - h / 2 });
                }
            }
            return SIZE_T_MAX;
        }

        void _tree_build_dc_tree()
        {
            for (idx_t i = 0; i < _vertices.size(); ++i)
            {
                _tree_add_dc(i);
            }
        }
    };
}
