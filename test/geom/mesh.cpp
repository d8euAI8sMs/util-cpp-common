#include "stdafx.h"

#include "CppUnitTest.h"

#include <util/common/geom/mesh.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template<> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<geom::point2d_t> (const geom::point2d_t & p) { RETURN_WIDE_STRING(p); }

namespace geom
{

    template < typename _C, typename ... _Ty >
    static bool contains(const _C & c, const _Ty & ... vs);

    template < typename _C, typename _Ty >
    static bool contains(const _C & c, const _Ty & v)
    {
        return std::find(std::begin(c), std::end(c), v) != std::end(c);
    }

    template < typename _C, typename _Ty0, typename ... _Ty >
    static bool contains(const _C & c, const _Ty0 & v, const _Ty & ... vs)
    {
        return contains(c, v) && contains(c, vs ...);
    }

    TEST_CLASS(mesh_test)
    {
    public:

        BEGIN_TEST_METHOD_ATTRIBUTE(_init_1)
            TEST_DESCRIPTION(L"mesh initialization works fine - triangle")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_init_1)
        {
            mesh m(true, false);

            m.init(std::vector < point2d_t > ({ { 0, 0 }, { 1, 0 }, { 0, 1 } }));

            Assert::AreEqual(size_t(3), m.vertices().size(), L"vertices size", LINE_INFO());
            Assert::AreEqual(size_t(1), m.triangles().size(), L"triangles size", LINE_INFO());

            Assert::AreEqual(point2d_t(0, 0), m.vertices()[0].point, L"vertices[0]", LINE_INFO());
            Assert::AreEqual(point2d_t(1, 0), m.vertices()[1].point, L"vertices[1]", LINE_INFO());
            Assert::AreEqual(point2d_t(0, 1), m.vertices()[2].point, L"vertices[2]", LINE_INFO());

            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[0].flags, L"vertices[0].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[1].flags, L"vertices[1].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[2].flags, L"vertices[2].flags", LINE_INFO());

            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.triangles()[0].flags, L"triangles[0].flags", LINE_INFO());

            auto t = m.triangle_at(0);
            t.sort(true, false);

            auto te = make_triangle(0, 0, 1, 0, 0, 1);
            te.sort(true, false);

            Assert::AreEqual(te.points[0], t.points[0], L"t[0]", LINE_INFO());
            Assert::AreEqual(te.points[1], t.points[1], L"t[1]", LINE_INFO());
            Assert::AreEqual(te.points[2], t.points[2], L"t[2]", LINE_INFO());

            Assert::AreEqual(size_t(1), m.vertices()[0].neighbor_triangles.size(), L"v0 neighbor triangles", LINE_INFO());
            Assert::AreEqual(size_t(2), m.vertices()[0].neighbor_vertices.size(),  L"v0 neighbor vertices", LINE_INFO());
            Assert::AreEqual(size_t(1), m.vertices()[1].neighbor_triangles.size(), L"v1 neighbor triangles", LINE_INFO());
            Assert::AreEqual(size_t(2), m.vertices()[1].neighbor_vertices.size(),  L"v1 neighbor vertices", LINE_INFO());
            Assert::AreEqual(size_t(1), m.vertices()[2].neighbor_triangles.size(), L"v2 neighbor triangles", LINE_INFO());
            Assert::AreEqual(size_t(2), m.vertices()[2].neighbor_vertices.size(),  L"v2 neighbor vertices", LINE_INFO());

            Assert::IsTrue(contains(m.vertices()[0].neighbor_triangles, 0),   L"v0 neighbor triangle 0", LINE_INFO());
            Assert::IsTrue(contains(m.vertices()[0].neighbor_vertices, 1, 2), L"v0 neighbor vertices 1, 2", LINE_INFO());

            Assert::IsTrue(contains(m.vertices()[1].neighbor_triangles, 0),   L"v1 neighbor triangle 0", LINE_INFO());
            Assert::IsTrue(contains(m.vertices()[1].neighbor_vertices, 0, 2), L"v1 neighbor vertices 0, 2", LINE_INFO());

            Assert::IsTrue(contains(m.vertices()[2].neighbor_triangles, 0),   L"v2 neighbor triangle 0", LINE_INFO());
            Assert::IsTrue(contains(m.vertices()[2].neighbor_vertices, 0, 1), L"v2 neighbor vertices 0, 1", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_init_2)
            TEST_DESCRIPTION(L"mesh initialization works fine - square")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_init_2)
        {
            mesh m(true, false);

            m.init(std::vector < point2d_t > ({ { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }));

            Assert::AreEqual(size_t(4), m.vertices().size(), L"vertices size", LINE_INFO());
            Assert::AreEqual(size_t(2), m.triangles().size(), L"triangles size", LINE_INFO());

            Assert::AreEqual(point2d_t(0, 0), m.vertices()[0].point, L"vertices[0]", LINE_INFO());
            Assert::AreEqual(point2d_t(1, 0), m.vertices()[1].point, L"vertices[1]", LINE_INFO());
            Assert::AreEqual(point2d_t(1, 1), m.vertices()[2].point, L"vertices[2]", LINE_INFO());
            Assert::AreEqual(point2d_t(0, 1), m.vertices()[3].point, L"vertices[3]", LINE_INFO());

            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[0].flags, L"vertices[0].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[1].flags, L"vertices[1].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[2].flags, L"vertices[2].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[3].flags, L"vertices[3].flags", LINE_INFO());

            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.triangles()[0].flags, L"triangles[0].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.triangles()[1].flags, L"triangles[1].flags", LINE_INFO());

            auto t1 = m.triangle_at(0);
            t1.sort(true, false);

            auto t1e = make_triangle(0, 0, 1, 0, 1, 1);
            t1e.sort(true, false);

            auto t2 = m.triangle_at(1);
            t2.sort(true, false);

            auto t2e = make_triangle(1, 1, 0, 1, 0, 0);
            t2e.sort(true, false);

            Assert::AreEqual(t1e.points[0], t1.points[0], L"t1[0]", LINE_INFO());
            Assert::AreEqual(t1e.points[1], t1.points[1], L"t1[1]", LINE_INFO());
            Assert::AreEqual(t1e.points[2], t1.points[2], L"t1[2]", LINE_INFO());

            Assert::AreEqual(t2e.points[0], t2.points[0], L"t2[0]", LINE_INFO());
            Assert::AreEqual(t2e.points[1], t2.points[1], L"t2[1]", LINE_INFO());
            Assert::AreEqual(t2e.points[2], t2.points[2], L"t2[2]", LINE_INFO());

            Assert::AreEqual(size_t(2), m.vertices()[0].neighbor_triangles.size(), L"v0 neighbor triangles", LINE_INFO());
            Assert::AreEqual(size_t(3), m.vertices()[0].neighbor_vertices.size(),  L"v0 neighbor vertices", LINE_INFO());
            Assert::AreEqual(size_t(1), m.vertices()[1].neighbor_triangles.size(), L"v1 neighbor triangles", LINE_INFO());
            Assert::AreEqual(size_t(2), m.vertices()[1].neighbor_vertices.size(),  L"v1 neighbor vertices", LINE_INFO());
            Assert::AreEqual(size_t(2), m.vertices()[2].neighbor_triangles.size(), L"v2 neighbor triangles", LINE_INFO());
            Assert::AreEqual(size_t(3), m.vertices()[2].neighbor_vertices.size(),  L"v2 neighbor vertices", LINE_INFO());
            Assert::AreEqual(size_t(1), m.vertices()[3].neighbor_triangles.size(), L"v3 neighbor triangles", LINE_INFO());
            Assert::AreEqual(size_t(2), m.vertices()[3].neighbor_vertices.size(),  L"v3 neighbor vertices", LINE_INFO());

            Assert::IsTrue(contains(m.vertices()[0].neighbor_triangles, 0, 1), L"v0 neighbor triangle 0, 1", LINE_INFO());
            Assert::IsTrue(contains(m.vertices()[0].neighbor_vertices, 1, 2, 3),  L"v0 neighbor vertices 1, 2, 3", LINE_INFO());

            Assert::IsTrue(contains(m.vertices()[1].neighbor_triangles, 0), L"v1 neighbor triangle 0", LINE_INFO());
            Assert::IsTrue(contains(m.vertices()[1].neighbor_vertices, 0, 2),  L"v1 neighbor vertices 0, 2", LINE_INFO());

            Assert::IsTrue(contains(m.vertices()[2].neighbor_triangles, 0, 1), L"v2 neighbor triangle 0, 1", LINE_INFO());
            Assert::IsTrue(contains(m.vertices()[2].neighbor_vertices, 0, 1, 3),  L"v2 neighbor vertices 0, 1, 3", LINE_INFO());

            Assert::IsTrue(contains(m.vertices()[3].neighbor_triangles, 1), L"v3 neighbor triangle 1", LINE_INFO());
            Assert::IsTrue(contains(m.vertices()[3].neighbor_vertices, 0, 2),  L"v3 neighbor vertices 0, 2", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_init_3)
            TEST_DESCRIPTION(L"mesh initialization works fine - hexagon")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_init_3)
        {
            mesh m(true, false);

            double sqrt32 = std::sqrt(3) / 2;

            m.init(std::vector < point2d_t > ({
                    { -1, 0 },
                    { -0.5, sqrt32 },
                    { 0.5, sqrt32 },
                    { 1, 0 },
                    { 0.5, -sqrt32 },
                    { -0.5, -sqrt32 }
            }));

            Assert::AreEqual(size_t(6), m.vertices().size(), L"vertices size", LINE_INFO());
            Assert::AreEqual(size_t(4), m.triangles().size(), L"triangles size", LINE_INFO());

            Assert::AreEqual(point2d_t({ -1, 0 }), m.vertices()[0].point, L"vertices[0]", LINE_INFO());
            Assert::AreEqual(point2d_t({ -0.5, sqrt32 }), m.vertices()[1].point, L"vertices[1]", LINE_INFO());
            Assert::AreEqual(point2d_t({ 0.5, sqrt32 }), m.vertices()[2].point, L"vertices[2]", LINE_INFO());
            Assert::AreEqual(point2d_t({ 1, 0 }), m.vertices()[3].point, L"vertices[3]", LINE_INFO());
            Assert::AreEqual(point2d_t({ 0.5, -sqrt32 }), m.vertices()[4].point, L"vertices[4]", LINE_INFO());
            Assert::AreEqual(point2d_t({ -0.5, -sqrt32 }), m.vertices()[5].point, L"vertices[5]", LINE_INFO());

            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[0].flags, L"vertices[0].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[1].flags, L"vertices[1].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[2].flags, L"vertices[2].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[3].flags, L"vertices[3].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[4].flags, L"vertices[4].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[5].flags, L"vertices[5].flags", LINE_INFO());

            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.triangles()[0].flags, L"triangles[0].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.triangles()[1].flags, L"triangles[1].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.triangles()[2].flags, L"triangles[2].flags", LINE_INFO());

            for (mesh::idx_t i = 0; i < m.triangles().size(); ++i)
            {
                if (m.triangles()[i].flags & mesh::phantom) continue;
                auto t1 = m.triangle_at(i);
                for (mesh::idx_t j = 0; j < m.triangles().size(); ++j)
                {
                    if (j == i) continue;
                    if (m.triangles()[j].flags & mesh::phantom) continue;
                    auto t2 = m.triangle_at(j);
                    auto info = std::to_wstring(i) + L"-" + std::to_wstring(j) + L" intersection";
                    Assert::IsFalse(status::is(t1.intersects(t2), status::polygon::intersects), info.c_str(), LINE_INFO());
                }
            }
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_init_4)
            TEST_DESCRIPTION(L"mesh initialization works fine - random mesh")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_init_4)
        {
            mesh m(true, true);

            std::vector < point2d_t > points;
            for (size_t i = 0; i < 100; ++i)
            {
                points.emplace_back((double) rand() / RAND_MAX * 100,
                                    (double) rand() / RAND_MAX * 100);
            }

            m.init(points);

            Assert::AreEqual(size_t(100), m.vertices().size(), L"vertices size", LINE_INFO());
            Assert::IsTrue((m.triangles().size() >= 100) && (m.triangles().size() <= 200), L"triangles size", LINE_INFO());

            std::set < mesh::idx_t > vertices;

            for (mesh::idx_t i = 0; i < m.triangles().size(); ++i)
            {
                if (m.triangles()[i].flags & mesh::phantom) continue;
                vertices.emplace(m.triangles()[i].vertices[0]);
                vertices.emplace(m.triangles()[i].vertices[1]);
                vertices.emplace(m.triangles()[i].vertices[2]);
                auto t1 = m.triangle_at(i);
                for (mesh::idx_t j = 0; j < m.triangles().size(); ++j)
                {
                    if (j == i) continue;
                    if (m.triangles()[j].flags & mesh::phantom) continue;
                    auto t2 = m.triangle_at(j);
                    auto info = std::to_wstring(i) + L"-" + std::to_wstring(j) + L" intersection";
                    Assert::IsFalse(status::is(t1.intersects(t2), status::polygon::intersects), info.c_str(), LINE_INFO());
                }
            }

            Assert::AreEqual(size_t(100), vertices.size(), L"collected vertices size", LINE_INFO());

            size_t degenerate_count = 0;

            for (mesh::idx_t i = 0; i < m.vertices().size(); ++i)
            {
                auto p = m.dirichlet_cell_at(i);
                auto c = polygon < > (p.points).convexity();
                auto conv_info = std::to_wstring(i) + L" convexity";
                Assert::IsFalse(c == convex_type::no, conv_info.c_str(), LINE_INFO());
                if (c == convex_type::degenerate) ++degenerate_count;
            }

            Assert::IsTrue(degenerate_count < 100 / 2, L"degenerate Dirichlet cell count is small", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_add_1)
            TEST_DESCRIPTION(L"mesh update works fine - square")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_add_1)
        {
            mesh m(true, false);

            m.init(std::vector < point2d_t > ({ { 0, 0 }, { 1, 0 }, { 1, 1 } }));
            m.add(point2d_t(0, 1));

            Assert::AreEqual(size_t(4), m.vertices().size(), L"vertices size", LINE_INFO());
            Assert::AreEqual(size_t(2), m.triangles().size(), L"triangles size", LINE_INFO());

            Assert::AreEqual(point2d_t(0, 0), m.vertices()[0].point, L"vertices[0]", LINE_INFO());
            Assert::AreEqual(point2d_t(1, 0), m.vertices()[1].point, L"vertices[1]", LINE_INFO());
            Assert::AreEqual(point2d_t(1, 1), m.vertices()[2].point, L"vertices[2]", LINE_INFO());
            Assert::AreEqual(point2d_t(0, 1), m.vertices()[3].point, L"vertices[3]", LINE_INFO());

            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[0].flags, L"vertices[0].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[1].flags, L"vertices[1].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[2].flags, L"vertices[2].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(0), m.vertices()[3].flags, L"vertices[3].flags", LINE_INFO());

            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.triangles()[0].flags, L"triangles[0].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.triangles()[1].flags, L"triangles[1].flags", LINE_INFO());

            auto t1 = m.triangle_at(0);
            t1.sort(true, false);

            auto t1e = make_triangle(0, 0, 1, 0, 1, 1);
            t1e.sort(true, false);

            auto t2 = m.triangle_at(1);
            t2.sort(true, false);

            auto t2e = make_triangle(1, 1, 0, 1, 0, 0);
            t2e.sort(true, false);

            Assert::AreEqual(t1e.points[0], t1.points[0], L"t1[0]", LINE_INFO());
            Assert::AreEqual(t1e.points[1], t1.points[1], L"t1[1]", LINE_INFO());
            Assert::AreEqual(t1e.points[2], t1.points[2], L"t1[2]", LINE_INFO());

            Assert::AreEqual(t2e.points[0], t2.points[0], L"t2[0]", LINE_INFO());
            Assert::AreEqual(t2e.points[1], t2.points[1], L"t2[1]", LINE_INFO());
            Assert::AreEqual(t2e.points[2], t2.points[2], L"t2[2]", LINE_INFO());

            Assert::AreEqual(size_t(2), m.vertices()[0].neighbor_triangles.size(), L"v0 neighbor triangles", LINE_INFO());
            Assert::AreEqual(size_t(3), m.vertices()[0].neighbor_vertices.size(),  L"v0 neighbor vertices", LINE_INFO());
            Assert::AreEqual(size_t(1), m.vertices()[1].neighbor_triangles.size(), L"v1 neighbor triangles", LINE_INFO());
            Assert::AreEqual(size_t(2), m.vertices()[1].neighbor_vertices.size(),  L"v1 neighbor vertices", LINE_INFO());
            Assert::AreEqual(size_t(2), m.vertices()[2].neighbor_triangles.size(), L"v2 neighbor triangles", LINE_INFO());
            Assert::AreEqual(size_t(3), m.vertices()[2].neighbor_vertices.size(),  L"v2 neighbor vertices", LINE_INFO());
            Assert::AreEqual(size_t(1), m.vertices()[3].neighbor_triangles.size(), L"v3 neighbor triangles", LINE_INFO());
            Assert::AreEqual(size_t(2), m.vertices()[3].neighbor_vertices.size(),  L"v3 neighbor vertices", LINE_INFO());

            Assert::IsTrue(contains(m.vertices()[0].neighbor_triangles, 0, 1), L"v0 neighbor triangle 0, 1", LINE_INFO());
            Assert::IsTrue(contains(m.vertices()[0].neighbor_vertices, 1, 2, 3),  L"v0 neighbor vertices 1, 2, 3", LINE_INFO());

            Assert::IsTrue(contains(m.vertices()[1].neighbor_triangles, 0), L"v1 neighbor triangle 0", LINE_INFO());
            Assert::IsTrue(contains(m.vertices()[1].neighbor_vertices, 0, 2),  L"v1 neighbor vertices 0, 2", LINE_INFO());

            Assert::IsTrue(contains(m.vertices()[2].neighbor_triangles, 0, 1), L"v2 neighbor triangle 0, 1", LINE_INFO());
            Assert::IsTrue(contains(m.vertices()[2].neighbor_vertices, 0, 1, 3),  L"v2 neighbor vertices 0, 1, 3", LINE_INFO());

            Assert::IsTrue(contains(m.vertices()[3].neighbor_triangles, 1), L"v3 neighbor triangle 1", LINE_INFO());
            Assert::IsTrue(contains(m.vertices()[3].neighbor_vertices, 0, 2),  L"v3 neighbor vertices 0, 2", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_add_2)
            TEST_DESCRIPTION(L"mesh update works fine - hexagon")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_add_2)
        {
            mesh m(true, false);

            double sqrt32 = std::sqrt(3) / 2;

            m.init(std::vector < point2d_t > ({
                    { -1, 0 },
                    { -0.5, sqrt32 },
                    { 0.5, sqrt32 },
                    { 1, 0 },
                    { 0.5, -sqrt32 }
            }));

            m.add({ -0.5, -sqrt32 });

            Assert::AreEqual(size_t(6), m.vertices().size(), L"vertices size", LINE_INFO());
            Assert::AreEqual(size_t(4), m.triangles().size(), L"triangles size", LINE_INFO());

            Assert::AreEqual(point2d_t({ -1, 0 }), m.vertices()[0].point, L"vertices[0]", LINE_INFO());
            Assert::AreEqual(point2d_t({ -0.5, sqrt32 }), m.vertices()[1].point, L"vertices[1]", LINE_INFO());
            Assert::AreEqual(point2d_t({ 0.5, sqrt32 }), m.vertices()[2].point, L"vertices[2]", LINE_INFO());
            Assert::AreEqual(point2d_t({ 1, 0 }), m.vertices()[3].point, L"vertices[3]", LINE_INFO());
            Assert::AreEqual(point2d_t({ 0.5, -sqrt32 }), m.vertices()[4].point, L"vertices[4]", LINE_INFO());
            Assert::AreEqual(point2d_t({ -0.5, -sqrt32 }), m.vertices()[5].point, L"vertices[5]", LINE_INFO());

            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[0].flags, L"vertices[0].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[1].flags, L"vertices[1].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[2].flags, L"vertices[2].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[3].flags, L"vertices[3].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.vertices()[4].flags, L"vertices[4].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(0), m.vertices()[5].flags, L"vertices[5].flags", LINE_INFO());

            for (mesh::idx_t i = 0; i < m.triangles().size(); ++i)
            {
                if (m.triangles()[i].flags & mesh::phantom) continue;
                auto t1 = m.triangle_at(i);
                for (mesh::idx_t j = 0; j < m.triangles().size(); ++j)
                {
                    if (j == i) continue;
                    if (m.triangles()[j].flags & mesh::phantom) continue;
                    auto t2 = m.triangle_at(j);
                    auto info = std::to_wstring(i) + L"-" + std::to_wstring(j) + L" intersection";
                    Assert::IsFalse(status::is(t1.intersects(t2), status::polygon::intersects), info.c_str(), LINE_INFO());
                }
            }
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_add_3)
            TEST_DESCRIPTION(L"mesh update works fine - random mesh")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_add_3)
        {
            mesh m(true, true);

            m.init(std::vector < point2d_t > ({
                    { -50, -50 }, { 150, -50 },
                    { 150, 150 }, { -50, 150 }
            }));

            std::vector < point2d_t > points;
            for (size_t i = 0; i < 100; ++i)
            {
                points.emplace_back((double) rand() / RAND_MAX * 100,
                                    (double) rand() / RAND_MAX * 100);
            }

            m.add(points);

            Assert::AreEqual(size_t(104), m.vertices().size(), L"vertices size", LINE_INFO());
            Assert::IsTrue((104 <= m.triangles().size()) && (m.triangles().size() <= 2 * 104), L"triangles size", LINE_INFO());

            std::set < mesh::idx_t > vertices;

            for (mesh::idx_t i = 0; i < m.triangles().size(); ++i)
            {
                if (m.triangles()[i].flags & mesh::phantom) continue;
                vertices.emplace(m.triangles()[i].vertices[0]);
                vertices.emplace(m.triangles()[i].vertices[1]);
                vertices.emplace(m.triangles()[i].vertices[2]);
                auto t1 = m.triangle_at(i);
                for (mesh::idx_t j = 0; j < m.triangles().size(); ++j)
                {
                    if (j == i) continue;
                    if (m.triangles()[j].flags & mesh::phantom) continue;
                    auto t2 = m.triangle_at(j);
                    auto info = std::to_wstring(i) + L"-" + std::to_wstring(j) + L" intersection";
                    Assert::IsFalse(status::is(t1.intersects(t2), status::polygon::intersects), info.c_str(), LINE_INFO());
                }
            }

            Assert::AreEqual(size_t(104), vertices.size(), L"collected vertices size", LINE_INFO());

            size_t degenerate_count = 0;

            for (mesh::idx_t i = 0; i < m.vertices().size(); ++i)
            {
                auto p = m.dirichlet_cell_at(i);
                auto c = polygon <> (p.points).convexity();
                auto conv_info = std::to_wstring(i) + L" convexity";
                Assert::IsFalse(c == convex_type::no, conv_info.c_str(), LINE_INFO());
                if (c == convex_type::degenerate) ++degenerate_count;
            }

            Assert::IsTrue(degenerate_count < 100 / 2, L"degenerate Dirichlet cell count is small", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_add_4)
            TEST_DESCRIPTION(L"mesh update works fine - special cases")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_add_4)
        {
            mesh m(true, true);

            m.init(std::vector < point2d_t > ({ { 0, 0 }, { 1, 0 }, { 0, 1 } }));

            auto r = m.add(point2d_t { 0, 1 });

            Assert::AreEqual(size_t(3), m.vertices().size(), L"vertices size", LINE_INFO());
            Assert::AreEqual(size_t(1), m.triangles().size(), L"triangles size", LINE_INFO());
            Assert::AreEqual(size_t(2), r, L"remapped", LINE_INFO());

            r = m.add(point2d_t { 0 - 1e-12, 1 + 1e-12 });

            Assert::AreEqual(size_t(3), m.vertices().size(), L"vertices size - 2", LINE_INFO());
            Assert::AreEqual(size_t(1), m.triangles().size(), L"triangles size - 2", LINE_INFO());
            Assert::AreEqual(size_t(2), r, L"remapped - 2", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_add_5)
            TEST_DESCRIPTION(L"mesh update works fine - test triangulation invariants")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_add_5)
        {
            std::vector < point2d_t > super = { { -2, -2 }, { 2, -2 }, { 2, 2 }, { -2, 2 } };
            for (size_t i = 0; i < 10000; ++i)
            {
                mesh m(false, false);

                double t = rand() / (RAND_MAX + 1.) - 0.5;

                std::vector < point2d_t > points = { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } };
                for (size_t j = 0; j < points.size(); ++j)
                {
                    auto r1 = rand() % 4, r2 = rand() % 4;
                    double d1 = ((r1 == 0) ? 0 : ((r1 == 1) ? 1e-4 : ((r1 == 2) ? 1e-6 : 1e-8)));
                    double d2 = ((r2 == 0) ? 0 : ((r2 == 1) ? 1e-4 : ((r2 == 2) ? 1e-6 : 1e-8)));
                    points[j].x += t + ((rand() % 2) * 2 - 1) * d1 * rand() / (RAND_MAX + 1.);
                    points[j].y += t + ((rand() % 2) * 2 - 1) * d2 * rand() / (RAND_MAX + 1.);
                }
                std::random_shuffle(points.begin(), points.end());

                m.init(super);
                m.add(points);

                CString fmt; fmt.Format(L"%d: ", i);

                Assert::AreEqual(size_t(8), m.vertices().size(), fmt + L"vertices size", LINE_INFO());
                Assert::AreEqual(size_t(10), m.triangles().size(), fmt + L"triangles size", LINE_INFO());

                for (size_t t = 0; t < m.triangles().size(); ++t)
                {
                    for (size_t t2 = 0; t2 < m.triangles().size(); ++t2)
                    {
                        if (t == t2) continue;
                        fmt.Format(L"%d: intersection (%d-%d)", i, t, t2);

                        auto intersects = m.triangle_at(t).intersects(m.triangle_at(t2));

                        Assert::IsFalse(status::is(intersects, status::polygon::intersects),
                                        fmt, LINE_INFO());
                    }
                }
            }
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_user_flags)
            TEST_DESCRIPTION(L"user flags work fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_user_flags)
        {
            mesh m(true, false);

            const mesh::flags_t flags = (1 << 20) | (1 << 6);
            const mesh::flags_t flags2 = (1 << 18) | (1 << 4);
            const mesh::flags_t flags3 = flags2 | mesh::superstruct | mesh::phantom;

            m.init(std::vector < point2d_t > ({ { 0, 0 }, { 1, 0 }, { 0, 1 } }), flags);

            Assert::AreEqual(mesh::flags_t(mesh::superstruct | flags), m.vertices()[0].flags, L"vertices[0].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct | flags), m.vertices()[1].flags, L"vertices[1].flags", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct | flags), m.vertices()[2].flags, L"vertices[2].flags", LINE_INFO());

            Assert::AreEqual(mesh::flags_t(mesh::superstruct | flags), m.flags_at(0), L"vertices[0].flags_at", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct | flags), m.flags_at(1), L"vertices[1].flags_at", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct | flags), m.flags_at(2), L"vertices[2].flags_at", LINE_INFO());

            Assert::AreEqual(mesh::flags_t(mesh::superstruct), m.triangles()[0].flags, L"triangles[0].flags", LINE_INFO());

            auto old = m.flags_at(1, flags2);

            Assert::AreEqual(mesh::flags_t(mesh::superstruct | flags), old, L"vertices[1].flags_at old", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct | flags2), m.flags_at(1), L"vertices[1].flags_at new", LINE_INFO());

            m.clear();

            m.init(std::vector < point2d_t > ({ { 0, 0 }, { 1, 0 }, { 0, 1 } }), flags3);

            Assert::AreEqual(mesh::flags_t(mesh::superstruct | flags2), m.flags_at(0), L"vertices[0].flags_at 3", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct | flags2), m.flags_at(1), L"vertices[1].flags_at 3", LINE_INFO());
            Assert::AreEqual(mesh::flags_t(mesh::superstruct | flags2), m.flags_at(2), L"vertices[2].flags_at 3", LINE_INFO());

            m.add(point2d_t(1, 1), flags3);

            // system flags not set
            Assert::AreEqual(mesh::flags_t(flags2), m.flags_at(3), L"vertices[3].flags_at 3", LINE_INFO());

            m.flags_at(3, flags3);

            // system flags not set
            Assert::AreEqual(mesh::flags_t(flags2), m.flags_at(3), L"vertices[3].flags_at new 3", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_dirichlet_cells)
            TEST_DESCRIPTION(L"Dirichlet cell in-time and post calculation work fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_dirichlet_cells)
        {
            auto super  = std::vector < point2d_t > ({ { -10, -10 }, { 10, -10 }, { 10, 10 }, { -10, 10 } });
            auto points = std::vector < point2d_t > ({ { -1, -1 }, { 1, -1 }, { 1, 1 }, { -1, 1 } });

            mesh m(true, true);
            m.init(super);
            m.add(points);

            auto dc = m.dirichlet_cell_at(4);
            auto p = polygon <> (dc.points);

            Assert::IsFalse(m.vertices()[4].neighborhood.path.empty(), L"auto calculated", LINE_INFO());
            Assert::IsTrue(p.is_convex(), L"auto", LINE_INFO());

            mesh m2(true, false);
            m2.init(super);
            m2.add(points);

            dc = m2.dirichlet_cell_at(4);
            auto p2 = polygon <> (dc.points);

            Assert::IsTrue(m2.vertices()[4].neighborhood.path.empty(), L"in-time not calculated", LINE_INFO());
            Assert::IsTrue(p2.is_convex(), L"in-time", LINE_INFO());
            Assert::IsTrue(p2 == p, L"in-time equals auto", LINE_INFO());
            Assert::IsTrue(m2.vertices()[4].neighborhood.path.empty(), L"in-time after not calculated", LINE_INFO());

            m2.calculate_neighborhoods();

            dc = m2.dirichlet_cell_at(4);
            auto p3 = polygon <> (dc.points);

            Assert::IsFalse(m2.vertices()[4].neighborhood.path.empty(), L"post calculated", LINE_INFO());
            Assert::IsTrue(p3.is_convex(), L"post", LINE_INFO());
            Assert::IsTrue(p3 == p, L"post equals auto", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_neighbor_vertices)
            TEST_DESCRIPTION(L"neightbor vertice auto and post calculation work fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_neighbor_vertices)
        {
            auto super  = std::vector < point2d_t > ({ { -10, -10 }, { 10, -10 }, { 10, 10 }, { -10, 10 } });
            auto points = std::vector < point2d_t > ({ { -1, -1 }, { 1, -1 }, { 1, 1 }, { -1, 1 } });

            mesh m(true, false);
            m.init(super);
            m.add(points);

            Assert::IsFalse(m.vertices()[4].neighbor_vertices.empty(), L"auto calculated", LINE_INFO());

            mesh m2(false, false);
            m2.init(super);
            m2.add(points);

            Assert::IsTrue(m2.vertices()[4].neighbor_vertices.empty(), L"post not calculated", LINE_INFO());

            m2.calculate_neighbor_vertices();

            Assert::IsFalse(m2.vertices()[4].neighbor_vertices.empty(), L"post after calculated", LINE_INFO());

            Assert::IsTrue(m.vertices()[4].neighbor_vertices == m2.vertices()[4].neighbor_vertices, L"post equals auto", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_find_vertex)
            TEST_DESCRIPTION(L"neighborhood tree works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_find_vertex)
        {
            mesh m(true, true);

            m.init(std::vector < point2d_t > ({
                    { -50, -50 }, { 150, -50 },
                    { 150, 150 }, { -50, 150 }
            }));

            std::vector < point2d_t > points;
            for (size_t i = 0; i < 100; ++i)
            {
                points.emplace_back((double) rand() / RAND_MAX * 100,
                                    (double) rand() / RAND_MAX * 100);
            }

            auto r = m.add(points);

            for (size_t i = 0; i < points.size(); ++i)
            {
                CString fmt; fmt.Format(L"%d: ", i);

                Assert::AreEqual(r[i], m.find_vertex(points[i]),
                                 fmt + L"find_vertex", LINE_INFO());
            }

            Assert::AreEqual(SIZE_T_MAX, m.find_vertex({ 1000, 1000 }),
                             L"find_vertex of outer point", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_neighborhood_tree)
            TEST_DESCRIPTION(L"neighborhood tree works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_neighborhood_tree)
        {
            mesh m(true, true);

            m.init(std::vector < point2d_t > ({
                    { -50, -50 }, { 150, -50 },
                    { 150, 150 }, { -50, 150 }
            }));

            std::vector < point2d_t > points;
            for (size_t i = 0; i < 100; ++i)
            {
                points.emplace_back((double) rand() / RAND_MAX * 100,
                                    (double) rand() / RAND_MAX * 100);
            }

            auto r = m.add(points);

            m.build_neighborhood_tree();

            for (size_t i = 0; i < points.size(); ++i)
            {
                CString fmt; fmt.Format(L"%d: ", i);

                Assert::AreEqual(r[i], m.find_nearest(points[i]),
                                 fmt + L"find_nearest", LINE_INFO());
            }

            for (size_t i = 0; i < 10000; ++i)
            {
                point2d_t p = { (double) rand() / RAND_MAX * 100,
                                (double) rand() / RAND_MAX * 100 };

                CString fmt; fmt.Format(L"%d (%lf, %lf): ", i, p.x, p.y);

                auto v = m.find_nearest(p);

                Assert::IsTrue(v != SIZE_T_MAX,
                               fmt + L"nearest exists", LINE_INFO());
                Assert::IsTrue(status::is(m.dirichlet_cell_at(v).contains(p), status::polygon::contains_point),
                               fmt + L"contains", LINE_INFO());
            }
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_find_triangle)
            TEST_DESCRIPTION(L"triangle search works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_find_triangle)
        {
            mesh m(true, true);

            m.init(std::vector < point2d_t > ({
                    { -50, -50 }, { 150, -50 },
                    { 150, 150 }, { -50, 150 }
            }));

            std::vector < point2d_t > points;
            for (size_t i = 0; i < 100; ++i)
            {
                points.emplace_back((double) rand() / RAND_MAX * 100,
                                    (double) rand() / RAND_MAX * 100);
            }

            m.add(points);

            for (mesh::idx_t i = 0; i < m.triangles().size(); ++i)
            {
                if (m.triangles()[i].flags & mesh::phantom) continue;

                CString fmt; fmt.Format(L"%d: ", i);

                auto t = m.triangle_at(i);

                Assert::AreEqual(i, m.find_triangle((t.points[0] + t.points[1] + t.points[2]) / 3),
                                 fmt + L"find_triangle", LINE_INFO());
            }

            for (mesh::idx_t i = 0; i < m.vertices().size(); ++i)
            {
                CString fmt; fmt.Format(L"%d: ", i);

                auto & p = m.point_at(i);

                auto t = m.find_triangle(p);

                Assert::IsTrue(t != SIZE_T_MAX,
                               fmt + L"triangle found for vertex", LINE_INFO());
                Assert::IsTrue(status::is(m.triangle_at(t).contains(p), status::polygon::contains_point),
                               fmt + L"contains vertex", LINE_INFO());
            }

            for (size_t i = 0; i < 10000; ++i)
            {
                point2d_t p = { (double) rand() / RAND_MAX * 100,
                                (double) rand() / RAND_MAX * 100 };

                CString fmt; fmt.Format(L"%d (%lf, %lf): ", i, p.x, p.y);

                auto v = m.find_triangle(p);

                Assert::IsTrue(v != SIZE_T_MAX,
                               fmt + L"triangle found", LINE_INFO());
                Assert::IsTrue(status::is(m.triangle_at(v).contains(p), status::polygon::contains_point),
                               fmt + L"contains", LINE_INFO());
            }
        }
    };
}
