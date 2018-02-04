#include "stdafx.h"

#include "CppUnitTest.h"

#include <util/common/geom/polygon.h>

#include <array>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template<> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<geom::point2d_t> (const geom::point2d_t & p) { RETURN_WIDE_STRING(p); }

namespace geom
{

    using arr4_t  = std::array < point2d_t, 4 > ;

    static const arr4_t points =
    {
        { { 2, 2 }, { 3, 3 }, { 2, 4 }, { 1, 3 } }
    };

    TEST_CLASS(polygon_test)
    {
    public:

        BEGIN_TEST_METHOD_ATTRIBUTE(_factory)
            TEST_DESCRIPTION(L"factory and constructor works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_factory)
        {
            auto p = make_polygon(points); // array
            auto p2 = polygon < > (points); // vector

            Assert::AreEqual(points[0], p.points[0], L"p1[0]", LINE_INFO());
            Assert::AreEqual(points[1], p.points[1], L"p1[1]", LINE_INFO());
            Assert::AreEqual(points[2], p.points[2], L"p1[2]", LINE_INFO());
            Assert::AreEqual(points[3], p.points[3], L"p1[3]", LINE_INFO());

            Assert::AreEqual(points[0], p2.points[0], L"p2[0]", LINE_INFO());
            Assert::AreEqual(points[1], p2.points[1], L"p2[1]", LINE_INFO());
            Assert::AreEqual(points[2], p2.points[2], L"p2[2]", LINE_INFO());
            Assert::AreEqual(points[3], p2.points[3], L"p2[3]", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_equality)
            TEST_DESCRIPTION(L"equality checking works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_equality)
        {
            auto p1 = make_polygon < arr4_t > ({ { { 2, 2 }, { 3, 3 }, { 2, 4 }, { 1, 3 } } });
            auto p2 = make_polygon < arr4_t > ({ { { 2, 2 }, { 3, 3 }, { 2, 4 }, { 1, 3 } } });
            auto p3 = make_polygon < arr4_t > ({ { { 1, 2 }, { 3, 3 }, { 2, 4 }, { 1, 3 } } });
            auto p4 = make_polygon < arr4_t > ({ { { 2, 2 }, { 1, 3 }, { 2, 4 }, { 1, 3 } } });
            auto p5 = make_polygon < arr4_t > ({ { { 2, 2 }, { 3, 3 }, { 1, 4 }, { 1, 3 } } });
            auto p6 = make_polygon < arr4_t > ({ { { 2, 2 }, { 3, 3 }, { 2, 4 }, { 1, 1 } } });

            Assert::IsTrue(p1 == p2, L"p1 == p2", LINE_INFO());
            Assert::IsFalse(p1 != p2, L"p1 != p2", LINE_INFO());

            Assert::IsTrue(p1 != p3, L"p1 != p3", LINE_INFO());
            Assert::IsFalse(p1 == p3, L"p1 == p3", LINE_INFO());
            Assert::IsTrue(p1 != p4, L"p1 != p4", LINE_INFO());
            Assert::IsFalse(p1 == p4, L"p1 == p4", LINE_INFO());
            Assert::IsTrue(p1 != p5, L"p1 != p5", LINE_INFO());
            Assert::IsFalse(p1 == p5, L"p1 == p5", LINE_INFO());
            Assert::IsTrue(p1 != p6, L"p1 != p6", LINE_INFO());
            Assert::IsFalse(p1 == p6, L"p1 == p6", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_empty)
            TEST_DESCRIPTION(L"empty check is correct")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_empty)
        {
            Assert::IsFalse(make_polygon(points).empty(),
                             L"non-empty", LINE_INFO());
            Assert::IsFalse(make_polygon < arr4_t > ({}).empty(),
                             L"empty", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_intersection)
            TEST_DESCRIPTION(L"intersection calculation is correct")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_intersection)
        {
            auto p1 = make_polygon < arr4_t > ({ { { 2, 2 }, { 3, 3 }, { 2, 4 }, { 1, 3 } } });
            auto p2 = make_polygon < arr4_t > ({ { { 2, 1 }, { 3, 2 }, { 2, 3 }, { 1, 2 } } });
            auto p3 = make_polygon < arr4_t > ({ { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } } });
            auto p4 = make_polygon < arr4_t > ({ { { 3, 1 }, { 3, 3 }, { 1, 3 }, { 1, 1 } } });
            auto l1 = make_line(1, 2, 3, 4);
            auto l2 = make_line(2, 3, 3, 4);

            auto i = p1.intersections(l1);

            Assert::AreEqual(size_t(2), i.size(), L"p1 - l1", LINE_INFO());
            // counterclockwise polygon side iteration
            Assert::AreEqual(2.5, i[0].x, 1e-8, L"p1 - l1 x1", LINE_INFO());
            Assert::AreEqual(3.5, i[0].y, 1e-8, L"p1 - l1 y1", LINE_INFO());
            Assert::AreEqual(1.5, i[1].x, 1e-8, L"p1 - l1 x2", LINE_INFO());
            Assert::AreEqual(2.5, i[1].y, 1e-8, L"p1 - l1 y2", LINE_INFO());
            Assert::IsTrue(p1.intersects(l1), L"p1 - l1 int", LINE_INFO());
            Assert::IsTrue(intersects(p1, l1), L"p1 - l1 int glob", LINE_INFO());
            Assert::IsTrue(intersects(l1, p1), L"l1 - p1 int glob", LINE_INFO());

            i = p1.intersections(l2);

            Assert::AreEqual(size_t(1), i.size(), L"p1 - l2", LINE_INFO());
            Assert::AreEqual(2.5, i[0].x, 1e-8, L"p1 - l2 x", LINE_INFO());
            Assert::AreEqual(3.5, i[0].y, 1e-8, L"p1 - l2 y", LINE_INFO());
            Assert::IsTrue(p1.intersects(l2), L"p1 - l2 int", LINE_INFO());
            Assert::IsTrue(intersects(p1, l2), L"p1 - l2 int glob", LINE_INFO());
            Assert::IsTrue(intersects(l2, p1), L"l2 - p1 int glob", LINE_INFO());

            Assert::IsTrue(p1.intersects(p2), L"p1 - p2 int", LINE_INFO());
            Assert::IsTrue(intersects(p1, p2), L"p1 - p2 int glob", LINE_INFO());
            Assert::IsTrue(intersects(p2, p1), L"p2 - p1 int glob", LINE_INFO());

            Assert::IsFalse(p1.intersects(p3), L"p1 - p3 int", LINE_INFO());
            Assert::IsFalse(intersects(p1, p3), L"p1 - p3 int glob", LINE_INFO());
            Assert::IsFalse(intersects(p3, p1), L"p3 - p1 int glob", LINE_INFO());

            Assert::IsTrue(p1.intersects(p4), L"p1 - p4 int", LINE_INFO());
            Assert::IsTrue(intersects(p1, p4), L"p1 - p4 int glob", LINE_INFO());
            Assert::IsTrue(intersects(p4, p1), L"p4 - p1 int glob", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_convex)
            TEST_DESCRIPTION(L"convexity calculation is correct")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_convex)
        {
            auto p0 = polygon < > ();
            p0.points.emplace_back(2, 2);
            p0.points.emplace_back(3, 3);
            p0.points.emplace_back(2, 4);
            auto p1 = polygon < > ();
            p1.points.emplace_back(2, 2);
            p1.points.emplace_back(3, 3);
            p1.points.emplace_back(2, 4);
            p1.points.emplace_back(1, 3);
            auto p2 = polygon < > ();
            p2.points.emplace_back(2.5, 2.5);
            p2.points.emplace_back(3, 3);
            p2.points.emplace_back(2, 4);
            p2.points.emplace_back(1, 3);
            p2.points.emplace_back(1.5, 2.5);
            p2.points.emplace_back(2, 3);
            auto p3 = p2;
            p3.points.back() = { 2, 0 };
            auto p4 = p1;
            std::reverse(p4.points.begin(), p4.points.end());
            auto p5 = p3;
            std::reverse(p5.points.begin(), p5.points.end());
            auto p6 = p0;
            std::reverse(p6.points.begin(), p6.points.end());

            Assert::IsTrue(p0.is_convex(), L"p0", LINE_INFO());
            Assert::IsTrue(p0.convexity() == convex_type::counterclockwise, L"p0 c", LINE_INFO());
            Assert::IsTrue(p1.is_convex(), L"p1", LINE_INFO());
            Assert::IsTrue(p1.convexity() == convex_type::counterclockwise, L"p1 c", LINE_INFO());
            Assert::IsFalse(p2.is_convex(), L"p2", LINE_INFO());
            Assert::IsTrue(p2.convexity() == convex_type::no, L"p2 c", LINE_INFO());
            Assert::IsFalse(p3.is_convex(), L"p3", LINE_INFO());
            Assert::IsTrue(p3.convexity() == convex_type::no, L"p3 c", LINE_INFO());
            Assert::IsTrue(p4.is_convex(), L"p4", LINE_INFO());
            Assert::IsTrue(p4.convexity() == convex_type::clockwise, L"p4 c", LINE_INFO());
            Assert::IsFalse(p5.is_convex(), L"p5", LINE_INFO());
            Assert::IsTrue(p5.convexity() == convex_type::no, L"p5 c", LINE_INFO());
            Assert::IsTrue(p6.is_convex(), L"p6", LINE_INFO());
            Assert::IsTrue(p6.convexity() == convex_type::clockwise, L"p6 c", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_rotate)
            TEST_DESCRIPTION(L"rotation calculation is correct")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_rotate)
        {
            const double pi = 4 * std::atan(1);

            auto p0 = make_polygon < arr4_t > ({ { { 2, 2 }, { 3, 3 }, { 2, 4 }, { 1, 3 } } });
            auto p1 = rotate(p0, pi);
            auto p2 = rotate(p0, pi/2, { 2, 3 });

            Assert::AreEqual(-2, p1.points[0].x, 1e-8, L"p1[0].x", LINE_INFO());
            Assert::AreEqual(-2, p1.points[0].y, 1e-8, L"p1[0].y", LINE_INFO());
            Assert::AreEqual(-3, p1.points[1].x, 1e-8, L"p1[1].x", LINE_INFO());
            Assert::AreEqual(-3, p1.points[1].y, 1e-8, L"p1[1].y", LINE_INFO());
            Assert::AreEqual(-2, p1.points[2].x, 1e-8, L"p1[2].x", LINE_INFO());
            Assert::AreEqual(-4, p1.points[2].y, 1e-8, L"p1[2].y", LINE_INFO());
            Assert::AreEqual(-1, p1.points[3].x, 1e-8, L"p1[3].x", LINE_INFO());
            Assert::AreEqual(-3, p1.points[3].y, 1e-8, L"p1[3].y", LINE_INFO());

            Assert::AreEqual(3, p2.points[0].x, 1e-8, L"2[0].x", LINE_INFO());
            Assert::AreEqual(3, p2.points[0].y, 1e-8, L"2[0].y", LINE_INFO());
            Assert::AreEqual(2, p2.points[1].x, 1e-8, L"2[1].x", LINE_INFO());
            Assert::AreEqual(4, p2.points[1].y, 1e-8, L"2[1].y", LINE_INFO());
            Assert::AreEqual(1, p2.points[2].x, 1e-8, L"2[2].x", LINE_INFO());
            Assert::AreEqual(3, p2.points[2].y, 1e-8, L"2[2].y", LINE_INFO());
            Assert::AreEqual(2, p2.points[3].x, 1e-8, L"2[3].x", LINE_INFO());
            Assert::AreEqual(2, p2.points[3].y, 1e-8, L"2[3].y", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_scale)
            TEST_DESCRIPTION(L"scale calculation is correct")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_scale)
        {
            auto p0 = make_polygon < arr4_t > ({ { { 2, 2 }, { 3, 3 }, { 2, 4 }, { 1, 3 } } });
            auto p1 = scale(p0, 10);
            auto p2 = scale(p0, 10, { 2, 3 });

            Assert::AreEqual(20, p1.points[0].x, 1e-8, L"p1[0].x", LINE_INFO());
            Assert::AreEqual(20, p1.points[0].y, 1e-8, L"p1[0].y", LINE_INFO());
            Assert::AreEqual(30, p1.points[1].x, 1e-8, L"p1[1].x", LINE_INFO());
            Assert::AreEqual(30, p1.points[1].y, 1e-8, L"p1[1].y", LINE_INFO());
            Assert::AreEqual(20, p1.points[2].x, 1e-8, L"p1[2].x", LINE_INFO());
            Assert::AreEqual(40, p1.points[2].y, 1e-8, L"p1[2].y", LINE_INFO());
            Assert::AreEqual(10, p1.points[3].x, 1e-8, L"p1[3].x", LINE_INFO());
            Assert::AreEqual(30, p1.points[3].y, 1e-8, L"p1[3].y", LINE_INFO());

            Assert::AreEqual(2, p2.points[0].x, 1e-8, L"2[0].x", LINE_INFO());
            Assert::AreEqual(-7, p2.points[0].y, 1e-8, L"2[0].y", LINE_INFO());
            Assert::AreEqual(12, p2.points[1].x, 1e-8, L"2[1].x", LINE_INFO());
            Assert::AreEqual(3, p2.points[1].y, 1e-8, L"2[1].y", LINE_INFO());
            Assert::AreEqual(2, p2.points[2].x, 1e-8, L"2[2].x", LINE_INFO());
            Assert::AreEqual(13, p2.points[2].y, 1e-8, L"2[2].y", LINE_INFO());
            Assert::AreEqual(-8, p2.points[3].x, 1e-8, L"2[3].x", LINE_INFO());
            Assert::AreEqual(3, p2.points[3].y, 1e-8, L"2[3].y", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_move)
            TEST_DESCRIPTION(L"move calculation is correct")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_move)
        {
            auto p0 = make_polygon < arr4_t > ({ { { 2, 2 }, { 3, 3 }, { 2, 4 }, { 1, 3 } } });
            auto p1 = move(p0, { 2, 3 });

            Assert::AreEqual(4, p1.points[0].x, 1e-8, L"p1[0].x", LINE_INFO());
            Assert::AreEqual(5, p1.points[0].y, 1e-8, L"p1[0].y", LINE_INFO());
            Assert::AreEqual(5, p1.points[1].x, 1e-8, L"p1[1].x", LINE_INFO());
            Assert::AreEqual(6, p1.points[1].y, 1e-8, L"p1[1].y", LINE_INFO());
            Assert::AreEqual(4, p1.points[2].x, 1e-8, L"p1[2].x", LINE_INFO());
            Assert::AreEqual(7, p1.points[2].y, 1e-8, L"p1[2].y", LINE_INFO());
            Assert::AreEqual(3, p1.points[3].x, 1e-8, L"p1[3].x", LINE_INFO());
            Assert::AreEqual(6, p1.points[3].y, 1e-8, L"p1[3].y", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_contains)
            TEST_DESCRIPTION(L"containment calculation is correct")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_contains)
        {
            auto p = make_polygon < arr4_t > ({ { { 2, 2 }, { 3, 3 }, { 2, 4 }, { 1, 3 } } });
            auto p2 = p;
            std::reverse(p2.points.begin(), p2.points.end());

            Assert::IsTrue(p.contains({ 2, 3 }), L"p1 (2,3)", LINE_INFO());
            Assert::IsTrue(p.contains({ 2, 2.1 }), L"p1 (2,2.1)", LINE_INFO());
            Assert::IsFalse(p.contains({ 2, 1.9 }), L"p1 (2,1.9)", LINE_INFO());

            Assert::IsTrue(p2.contains({ 2, 3 }), L"p2 (2,3)", LINE_INFO());
            Assert::IsTrue(p2.contains({ 2, 2.1 }), L"p2 (2,2.1)", LINE_INFO());
            Assert::IsFalse(p2.contains({ 2, 1.9 }), L"p2 (2,1.9)", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_center)
            TEST_DESCRIPTION(L"center calculation is correct")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_center)
        {
            auto p1 = make_polygon < arr4_t > ({ { { 2, 2 }, { 3, 3 }, { 2, 4 }, { 1, 3 } } });
            auto p2 = make_polygon < arr4_t > ({ { { 2, 1 }, { 4, 3 }, { 2, 5 }, { 1, 3 } } });

            auto c1 = center(p1);
            auto c2 = center(p2);

            Assert::AreEqual(2, c1.x, 1e-8, L"c1.x", LINE_INFO());
            Assert::AreEqual(3, c1.y, 1e-8, L"c1.y", LINE_INFO());
            Assert::AreEqual(2.25, c2.x, 1e-8, L"c2.x", LINE_INFO());
            Assert::AreEqual(3, c2.y, 1e-8, L"c2.y", LINE_INFO());
        }
    };
}
