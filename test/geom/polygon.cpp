#include "stdafx.h"

#include "CppUnitTest.h"

#include <util/common/geom/polygon.h>

#include <array>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template<> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<geom::point2d_t> (const geom::point2d_t & p) { RETURN_WIDE_STRING(p); }

namespace geom
{

    using arr4_t  = std::array < point2d_t, 4 > ;
    using vect_t  = std::vector < point2d_t > ;

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

        BEGIN_TEST_METHOD_ATTRIBUTE(_intersection_with_line)
            TEST_DESCRIPTION(L"intersection with line calculation is correct")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_intersection_with_line)
        {
            auto p1 = make_polygon < vect_t > ({ { { 2, 2 }, { 3, 3 }, { 2, 4 }, { 1, 3 } } });

            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p1.intersects(make_line(1, 2, 3, 4)), L"p1 - l1", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p1.intersects(make_line(2, 3, 3, 4)), L"p1 - l2", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.intersects(make_line(4, 1, 5, 2)), L"p1 - l3", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.intersects(make_line(2, 0, 2, 1)), L"p1 - l4", LINE_INFO());

            Assert::AreEqual(status::trusted(status::ok),
                             p1.intersects(make_line(3, 2, 3, 5)), L"p1 - l5", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.intersects(make_line(1, 2, 1, 5)), L"p1 - l6", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p1.intersects(make_line(2.999, 2, 2.999, 5)), L"p1 - l7", LINE_INFO());

            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p1.intersects(make_line(2, 1, 2, 5)), L"p1 - l8", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p1.intersects(make_line(2, 2, 2, 5)), L"p1 - l9", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p1.intersects(make_line(2, 3, 2, 5)), L"p1 - l10", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p1.intersects(make_line(2, 4, 2, 5)), L"p1 - l11", LINE_INFO());

            Assert::AreEqual(status::trusted(status::ok | status::polygon::coincides_with_line) | status::polygon::contains_line,
                             p1.intersects(make_line(1, 1, 4, 4)), L"p1 - l12", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::coincides_with_line) | status::polygon::contains_line,
                             p1.intersects(make_line(2, 2, 4, 4)), L"p1 - l13", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::coincides_with_line) | status::polygon::contains_line,
                             p1.intersects(make_line(3, 3, 4, 4)), L"p1 - l14", LINE_INFO());

            auto p2 = make_polygon < vect_t > ({ { { 2, 2 }, { 2.5, 2.5 }, { 3, 3 }, { 2.5, 3.5 }, { 2, 4 }, { 1.5, 3.5 }, { 1, 3 }, { 1.5, 2.5 } } });

            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p2.intersects(make_line(1, 2, 3, 4)), L"p2 - l1", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p2.intersects(make_line(2, 3, 3, 4)), L"p2 - l2", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p2.intersects(make_line(4, 1, 5, 2)), L"p2 - l3", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p2.intersects(make_line(2, 0, 2, 1)), L"p2 - l4", LINE_INFO());

            Assert::AreEqual(status::trusted(status::ok),
                             p2.intersects(make_line(3, 2, 3, 5)), L"p2 - l5", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p2.intersects(make_line(1, 2, 1, 5)), L"p2 - l6", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p2.intersects(make_line(2.999, 2, 2.999, 5)), L"p2 - l7", LINE_INFO());

            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p2.intersects(make_line(2, 1, 2, 5)), L"p2 - l8", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p2.intersects(make_line(2, 2, 2, 5)), L"p2 - l9", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p2.intersects(make_line(2, 3, 2, 5)), L"p2 - l10", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p2.intersects(make_line(2, 4, 2, 5)), L"p2 - l11", LINE_INFO());

            Assert::AreEqual(status::trusted(status::ok | status::polygon::coincides_with_line) | status::polygon::contains_line,
                             p2.intersects(make_line(1, 1, 4, 4)), L"p2 - l12", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::coincides_with_line) | status::polygon::contains_line,
                             p2.intersects(make_line(2, 2, 4, 4)), L"p2 - l13", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::coincides_with_line) | status::polygon::contains_line,
                             p2.intersects(make_line(3, 3, 4, 4)), L"p2 - l14", LINE_INFO());

            auto p3 = make_polygon < vect_t > ({ { { 2, 2 }, { 2.25, 2.25 }, { 3, 2 }, { 2.75, 2.75 }, { 3, 3 }, { 2, 4 }, { 1, 3 } } });

            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p3.intersects(make_line(1, 2, 3, 4)), L"p3 - l1", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p3.intersects(make_line(2, 3, 3, 4)), L"p3 - l2", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p3.intersects(make_line(4, 1, 5, 2)), L"p3 - l3", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p3.intersects(make_line(2, 0, 2, 1)), L"p3 - l4", LINE_INFO());

            Assert::AreEqual(status::trusted(status::ok),
                             p3.intersects(make_line(3, 2, 3, 5)), L"p3 - l5", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p3.intersects(make_line(1, 2, 1, 5)), L"p3 - l6", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p3.intersects(make_line(2.999, 2, 2.999, 5)), L"p3 - l7", LINE_INFO());

            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p3.intersects(make_line(2, 1, 2, 5)), L"p3 - l8", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p3.intersects(make_line(2, 2, 2, 5)), L"p3 - l9", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p3.intersects(make_line(2, 3, 2, 5)), L"p3 - l10", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p3.intersects(make_line(2, 4, 2, 5)), L"p3 - l11", LINE_INFO());

            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p3.intersects(make_line(1, 1, 4, 4)), L"p3 - l12", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::intersects),
                             p3.intersects(make_line(2, 2, 4, 4)), L"p3 - l13", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::coincides_with_line) | status::polygon::contains_line,
                             p3.intersects(make_line(3, 3, 4, 4)), L"p3 - l14", LINE_INFO());
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

        BEGIN_TEST_METHOD_ATTRIBUTE(_convex_special_cases)
            TEST_DESCRIPTION(L"convexity calculation is correct - special cases")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_convex_special_cases)
        {
            auto p0 = polygon < > ();

            Assert::IsFalse(p0.is_convex(), L"p0", LINE_INFO());
            Assert::IsTrue(p0.convexity() == convex_type::degenerate, L"p0 c", LINE_INFO());

            auto p1 = polygon < > ();
            p1.points.emplace_back(2, 2);
            p1.points.emplace_back(3, 3);

            Assert::IsFalse(p1.is_convex(), L"p1", LINE_INFO());
            Assert::IsTrue(p1.convexity() == convex_type::degenerate, L"p1 c", LINE_INFO());

            auto p2 = p1;
            p2.points.emplace_back(3, 3);

            Assert::IsFalse(p2.is_convex(), L"p2", LINE_INFO());
            Assert::IsTrue(p2.convexity() == convex_type::degenerate, L"p2 c", LINE_INFO());

            auto p3 = p1;
            p3.points.emplace_back(2.5, 2.5);

            Assert::IsFalse(p3.is_convex(), L"p3", LINE_INFO());
            Assert::IsTrue(p3.convexity() == convex_type::degenerate, L"p3 c", LINE_INFO());

            auto p32 = polygon < > ();
            p1.points.emplace_back(2, 2);
            p3.points.emplace_back(2.5, 2.5);
            p1.points.emplace_back(3, 3);

            Assert::IsFalse(p32.is_convex(), L"p32", LINE_INFO());
            Assert::IsTrue(p32.convexity() == convex_type::degenerate, L"p32 c", LINE_INFO());

            auto p4 = p1;
            p4.points.emplace_back(2.5, 2.5 + 1e-12);

            Assert::IsFalse(p4.is_convex(), L"p4", LINE_INFO());
            Assert::IsTrue(p4.convexity() == convex_type::degenerate, L"p4 c", LINE_INFO());

            auto p5 = p1;
            p5.points.emplace_back(2.5, 2.5 - 1e-12);

            Assert::IsFalse(p5.is_convex(), L"p5", LINE_INFO());
            Assert::IsTrue(p5.convexity() == convex_type::degenerate, L"p5 c", LINE_INFO());

            auto p6 = polygon < > ();
            p6.points.emplace_back(2, 2);
            p6.points.emplace_back(2, 2);
            p6.points.emplace_back(3, 3);
            p6.points.emplace_back(3, 3);
            p6.points.emplace_back(2, 4);
            p6.points.emplace_back(2, 4);
            p6.points.emplace_back(2, 4);
            p6.points.emplace_back(2, 2);

            Assert::IsTrue(p6.is_convex(), L"p6", LINE_INFO());
            Assert::IsTrue(p6.convexity() == convex_type::counterclockwise, L"p6 c", LINE_INFO());

            auto p7 = polygon < > ();
            p7.points.emplace_back(2, 2);
            p7.points.emplace_back(2, 2 + 1e-12);
            p7.points.emplace_back(3 - 1e-12, 3);
            p7.points.emplace_back(3, 3 + 1e-12);
            p7.points.emplace_back(2, 4);
            p7.points.emplace_back(2 + 1e-12, 4);
            p7.points.emplace_back(2, 4 - 1e-12);
            p7.points.emplace_back(2 - 1e-12, 2 + 1e-12);

            Assert::IsTrue(p7.is_convex(), L"p7", LINE_INFO());
            Assert::IsTrue(p7.convexity() == convex_type::counterclockwise, L"p7 c", LINE_INFO());

            auto p8 = polygon < > ();
            p8.points.emplace_back(2, 2);
            p8.points.emplace_back(3, 3);
            p8.points.emplace_back(2, 4);
            p8.points.emplace_back(3, 3);

            Assert::IsFalse(p8.is_convex(), L"p8", LINE_INFO());
            Assert::IsTrue(p8.convexity() == convex_type::degenerate, L"p8 c", LINE_INFO());

            auto p9 = polygon < > ();
            p9.points.emplace_back(2, 2);
            p9.points.emplace_back(2, 2);
            p9.points.emplace_back(3, 3);
            p9.points.emplace_back(3, 3);
            p9.points.emplace_back(2, 4);
            p9.points.emplace_back(2, 4);
            p9.points.emplace_back(2, 4);
            p9.points.emplace_back(2.5, 3);

            Assert::IsFalse(p9.is_convex(), L"p9", LINE_INFO());
            Assert::IsTrue(p9.convexity() == convex_type::no, L"p9 c", LINE_INFO());
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

            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p.contains({ 2, 3 }), L"p1 (2,3)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p.contains({ 2, 2.1 }), L"p1 (2,2.1)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p.contains({ 2, 1.9 }), L"p1 (2,1.9)", LINE_INFO());

            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p2.contains({ 2, 3 }), L"p2 (2,3)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p2.contains({ 2, 2.1 }), L"p2 (2,2.1)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p2.contains({ 2, 1.9 }), L"p2 (2,1.9)", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_contains_concave)
            TEST_DESCRIPTION(L"containment calculation is correct - concave polygon")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_contains_concave)
        {
            auto p1 = polygon < > ();
            p1.points.emplace_back(1, 2);
            p1.points.emplace_back(2, 1);
            p1.points.emplace_back(5, 1);
            p1.points.emplace_back(4, 2);
            p1.points.emplace_back(4, 3);
            p1.points.emplace_back(5, 4);
            p1.points.emplace_back(2, 4);
            p1.points.emplace_back(1, 3);

            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 0, 0 }), L"p1 (0,0)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 1, 0 }), L"p1 (1,0)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 2, 0 }), L"p1 (2,0)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 3, 0 }), L"p1 (3,0)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 4, 0 }), L"p1 (4,0)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 5, 0 }), L"p1 (5,0)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 6, 0 }), L"p1 (6,0)", LINE_INFO());

            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 0, 5 }), L"p1 (0,5)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 1, 5 }), L"p1 (1,5)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 2, 5 }), L"p1 (2,5)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 3, 5 }), L"p1 (3,5)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 4, 5 }), L"p1 (4,5)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 5, 5 }), L"p1 (5,5)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 6, 5 }), L"p1 (6,5)", LINE_INFO());

            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 0, 1 }), L"p1 (0,1)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 0, 2 }), L"p1 (0,2)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 0, 3 }), L"p1 (0,3)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 0, 4 }), L"p1 (0,4)", LINE_INFO());

            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 6, 1 }), L"p1 (6,1)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 6, 2 }), L"p1 (6,2)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 6, 3 }), L"p1 (6,3)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p1.contains({ 6, 4 }), L"p1 (6,4)", LINE_INFO());

            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p1.contains({ 2, 2 }), L"p1 (2,2)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p1.contains({ 3, 3 }), L"p1 (3,3)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p1.contains({ 2, 3 }), L"p1 (2,3)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p1.contains({ 3, 2 }), L"p1 (3,2)", LINE_INFO());

            auto p2 = polygon < > ();
            p2.points.emplace_back(0, 0);
            p2.points.emplace_back(7, 0);
            p2.points.emplace_back(7, 3);
            p2.points.emplace_back(1, 3);
            p2.points.emplace_back(1, 4);
            p2.points.emplace_back(7, 4);
            p2.points.emplace_back(7, 7);
            p2.points.emplace_back(0, 7);
            p2.points.emplace_back(0, 6);
            p2.points.emplace_back(6, 6);
            p2.points.emplace_back(6, 5);
            p2.points.emplace_back(0, 5);
            p2.points.emplace_back(0, 2);
            p2.points.emplace_back(6, 2);
            p2.points.emplace_back(6, 1);
            p2.points.emplace_back(0, 1);

            for (size_t i = 0; i < 9; ++i)
            {
                auto info = L"p2 " + std::to_wstring(i) + L" left";
                Assert::AreEqual(status::trusted(status::ok),
                             p2.contains({ -0.5, -0.5 + i }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" right";
                Assert::AreEqual(status::trusted(status::ok),
                             p2.contains({ 7.5, -0.5 + i }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" top";
                Assert::AreEqual(status::trusted(status::ok),
                             p2.contains({ -0.5 + i, 7.5 }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" bottom";
                Assert::AreEqual(status::trusted(status::ok),
                             p2.contains({ -0.5 + i, -0.5 }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" inner-bottom";
                Assert::AreEqual(status::trusted(status::ok),
                             p2.contains({ -2.5 + i, 1.5 }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" inner-center";
                Assert::AreEqual(status::trusted(status::ok),
                             p2.contains({ 2.5 + i, 3.5 }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" inner-top";
                Assert::AreEqual(status::trusted(status::ok),
                             p2.contains({ -2.5 + i, 5.5 }), info.c_str(), LINE_INFO());
            }

            for (size_t i = 0; i < 7; ++i)
            {
                auto info = L"p2 " + std::to_wstring(i) + L" inside-bottom";
                Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p2.contains({ 0.5 + i, 0.5 }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" inside-center-bottom";
                Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p2.contains({ 0.5 + i, 2.5 }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" inside-center-top";
                Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p2.contains({ 0.5 + i, 4.5 }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" inside-top";
                Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p2.contains({ 0.5 + i, 6.5 }), info.c_str(), LINE_INFO());
            }

            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p2.contains({ 6.5, 1.5 }), L"p2 spec1", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p2.contains({ 0.5, 3.5 }), L"p2 spec2", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p2.contains({ 6.5, 5.5 }), L"p2 spec3", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_contains_concave_special_cases)
            TEST_DESCRIPTION(L"containment calculation is correct - concave polygon - special cases")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_contains_concave_special_cases)
        {
            auto p1 = polygon < > ();
            p1.points.emplace_back(1, 2);
            p1.points.emplace_back(2, 1);
            p1.points.emplace_back(5, 1);
            p1.points.emplace_back(4, 2);
            p1.points.emplace_back(4, 3);
            p1.points.emplace_back(5, 4);
            p1.points.emplace_back(2, 4);
            p1.points.emplace_back(1, 3);

            for (size_t i = 0; i < p1.points.size(); ++i)
            {
                auto info = L"p1 " + std::to_wstring(i);
                Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | status::polygon::edge_contains_point,
                             p1.contains(p1.points[i]), info.c_str(), LINE_INFO());
            }

            for (size_t i = 0; i < p1.points.size(); ++i)
            {
                auto info = L"p1 " + std::to_wstring(i) + L" +0";
                Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | status::polygon::edge_contains_point,
                             p1.contains(p1.points[i] + point2d_t{ 1e-12, 1e-12 }), info.c_str(), LINE_INFO());
            }

            for (size_t i = 0; i < p1.points.size(); ++i)
            {
                auto info = L"p1 " + std::to_wstring(i) + L" -0";
                Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | status::polygon::edge_contains_point,
                             p1.contains(p1.points[i] + point2d_t{ -1e-12, -1e-12 }), info.c_str(), LINE_INFO());
            }

            auto p2 = polygon < > ();
            p2.points.emplace_back(0, 0);
            p2.points.emplace_back(7, 0);
            p2.points.emplace_back(7, 3);
            p2.points.emplace_back(1, 3);
            p2.points.emplace_back(1, 4);
            p2.points.emplace_back(7, 4);
            p2.points.emplace_back(7, 7);
            p2.points.emplace_back(0, 7);
            p2.points.emplace_back(0, 6);
            p2.points.emplace_back(6, 6);
            p2.points.emplace_back(6, 5);
            p2.points.emplace_back(0, 5);
            p2.points.emplace_back(0, 2);
            p2.points.emplace_back(6, 2);
            p2.points.emplace_back(6, 1);
            p2.points.emplace_back(0, 1);

            for (int i = 0; i <= 7; ++i)
            {
                auto info = L"p2 " + std::to_wstring(i) + L" left";
                Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | (((i == 3) || (i == 4)) ? status::trusted(status::polygon::edge_contains_point) : status::polygon::edge_contains_point),
                             p2.contains({ 0, i }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" right";
                Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | (((i == 1) || (i == 2) || (i == 5) || (i == 6)) ? status::trusted(status::polygon::edge_contains_point) : status::polygon::edge_contains_point),
                             p2.contains({ 7, i }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" top";
                Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | (((i == 0) || (i == 7)) ? status::polygon::edge_contains_point : status::trusted(status::polygon::edge_contains_point)),
                             p2.contains({ i, 7 }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" bottom";
                Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | (((i == 0) || (i == 7)) ? status::polygon::edge_contains_point : status::trusted(status::polygon::edge_contains_point)),
                             p2.contains({ i, 0 }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" inner-bottom-1";
                Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | (((i == 0) || (i == 6)) ? status::polygon::edge_contains_point : status::trusted(status::polygon::edge_contains_point)),
                             p2.contains({ i, 1 }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" inner-bottom-2";
                Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | (((i == 0) || (i == 6)) ? status::polygon::edge_contains_point : status::trusted(status::polygon::edge_contains_point)),
                             p2.contains({ i, 2 }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" inner-center-1";
                Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | (((i == 1) || (i == 7)) ? status::polygon::edge_contains_point : status::trusted(status::polygon::edge_contains_point)),
                             p2.contains({ i, 3 }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" inner-center-2";
                Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | (((i == 1) || (i == 7)) ? status::polygon::edge_contains_point : status::trusted(status::polygon::edge_contains_point)),
                             p2.contains({ i, 4 }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" inner-top-1";
                Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | (((i == 0) || (i == 6)) ? status::polygon::edge_contains_point : status::trusted(status::polygon::edge_contains_point)),
                             p2.contains({ i, 5 }), info.c_str(), LINE_INFO());
                info = L"p2 " + std::to_wstring(i) + L" inner-top-2";
                Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | (((i == 0) || (i == 6)) ? status::polygon::edge_contains_point : status::trusted(status::polygon::edge_contains_point)),
                             p2.contains({ i, 6 }), info.c_str(), LINE_INFO());
            }

            auto p3 = polygon < > ();
            p3.points.emplace_back(-2, 0);
            p3.points.emplace_back(0, 0);
            p3.points.emplace_back(2, 0);
            p3.points.emplace_back(0, 2);

            Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | status::polygon::edge_contains_point,
                             p3.contains({ -2, 0 }), L"p3 (-2,0)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p3.contains({ -3, 0 }), L"p3 (-3,0)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | status::polygon::edge_contains_point,
                             p3.contains({ 2, 0 }), L"p3 (2,0)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p3.contains({ 3, 0 }), L"p3 (3,0)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | status::polygon::edge_contains_point,
                             p3.contains({ 0, 2 }), L"p3 (0,2)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p3.contains({ 0, 3 }), L"p3 (0,3)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p3.contains({ 0, 1 }), L"p3 (0,1)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p3.contains({ 0, 1.5 }), L"p3 (0,1.5)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p3.contains({ 0, 1.9 }), L"p3 (0,1.9)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p3.contains({ 0, 1.99 }), L"p3 (0,1.99)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p3.contains({ 0, 1.999 }), L"p3 (0,1.999)", LINE_INFO());

            auto p4 = polygon < > ();
            p4.points.emplace_back(-2, 0);
            p4.points.emplace_back(-2, 0);
            p4.points.emplace_back(0, 0);
            p4.points.emplace_back(2, 0);
            p4.points.emplace_back(2, 0);
            p4.points.emplace_back(0, 2);
            p4.points.emplace_back(0, 2);
            p4.points.emplace_back(-2, 0);
            p4.points.emplace_back(-2, 0);
            p4.points.emplace_back(-2, 0);
            p4.points.emplace_back(-2, 0);

            Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | status::polygon::edge_contains_point,
                             p4.contains({ -2, 0 }), L"p4 (-2,0)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p4.contains({ -3, 0 }), L"p4 (-3,0)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | status::polygon::edge_contains_point,
                             p4.contains({ 2, 0 }), L"p4 (2,0)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p4.contains({ 3, 0 }), L"p4 (3,0)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok) | status::polygon::contains_point | status::polygon::edge_contains_point,
                             p4.contains({ 0, 2 }), L"p4 (0,2)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             p4.contains({ 0, 3 }), L"p4 (0,3)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p4.contains({ 0, 1 }), L"p4 (0,1)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p4.contains({ 0, 1.5 }), L"p4 (0,1.5)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p4.contains({ 0, 1.9 }), L"p4 (0,1.9)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p4.contains({ 0, 1.99 }), L"p4 (0,1.99)", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::polygon::contains_point),
                             p4.contains({ 0, 1.999 }), L"p4 (0,1.999)", LINE_INFO());
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

        BEGIN_TEST_METHOD_ATTRIBUTE(_clockwise_sort)
            TEST_DESCRIPTION(L"clockwise sort calculation is correct")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_clockwise_sort)
        {
            auto p1 = make_polygon < arr4_t > ({ { { 2, 2 }, { 3, 3 }, { 2, 4 }, { 1, 3 } } });
            auto p2 = make_polygon < arr4_t > ({ { { 0, 0 }, { 2, 2 }, { 2, 1 }, { 0, 3 } } });

            p1.sort();
            p2.sort();

            Assert::AreEqual(2, p1.points[0].x, 1e-8, L"p1[0].x ccw", LINE_INFO());
            Assert::AreEqual(2, p1.points[0].y, 1e-8, L"p1[0].y ccw", LINE_INFO());
            Assert::AreEqual(3, p1.points[1].x, 1e-8, L"p1[1].x ccw", LINE_INFO());
            Assert::AreEqual(3, p1.points[1].y, 1e-8, L"p1[1].y ccw", LINE_INFO());
            Assert::AreEqual(2, p1.points[2].x, 1e-8, L"p1[2].x ccw", LINE_INFO());
            Assert::AreEqual(4, p1.points[2].y, 1e-8, L"p1[2].y ccw", LINE_INFO());
            Assert::AreEqual(1, p1.points[3].x, 1e-8, L"p1[3].x ccw", LINE_INFO());
            Assert::AreEqual(3, p1.points[3].y, 1e-8, L"p1[3].y ccw", LINE_INFO());

            Assert::AreEqual(0, p2.points[0].x, 1e-8, L"p2[0].x ccw", LINE_INFO());
            Assert::AreEqual(0, p2.points[0].y, 1e-8, L"p2[0].y ccw", LINE_INFO());
            Assert::AreEqual(2, p2.points[1].x, 1e-8, L"p2[1].x ccw", LINE_INFO());
            Assert::AreEqual(1, p2.points[1].y, 1e-8, L"p2[1].y ccw", LINE_INFO());
            Assert::AreEqual(2, p2.points[2].x, 1e-8, L"p2[2].x ccw", LINE_INFO());
            Assert::AreEqual(2, p2.points[2].y, 1e-8, L"p2[2].y ccw", LINE_INFO());
            Assert::AreEqual(0, p2.points[3].x, 1e-8, L"p2[3].x ccw", LINE_INFO());
            Assert::AreEqual(3, p2.points[3].y, 1e-8, L"p2[3].y ccw", LINE_INFO());

            p1.sort(false);
            p2.sort(false);

            Assert::AreEqual(2, p1.points[0].x, 1e-8, L"p1[0].x cw", LINE_INFO());
            Assert::AreEqual(2, p1.points[0].y, 1e-8, L"p1[0].y cw", LINE_INFO());
            Assert::AreEqual(1, p1.points[1].x, 1e-8, L"p1[1].x cw", LINE_INFO());
            Assert::AreEqual(3, p1.points[1].y, 1e-8, L"p1[1].y cw", LINE_INFO());
            Assert::AreEqual(2, p1.points[2].x, 1e-8, L"p1[2].x cw", LINE_INFO());
            Assert::AreEqual(4, p1.points[2].y, 1e-8, L"p1[2].y cw", LINE_INFO());
            Assert::AreEqual(3, p1.points[3].x, 1e-8, L"p1[3].x cw", LINE_INFO());
            Assert::AreEqual(3, p1.points[3].y, 1e-8, L"p1[3].y cw", LINE_INFO());

            Assert::AreEqual(0, p2.points[0].x, 1e-8, L"p2[0].x cw", LINE_INFO());
            Assert::AreEqual(0, p2.points[0].y, 1e-8, L"p2[0].y cw", LINE_INFO());
            Assert::AreEqual(0, p2.points[1].x, 1e-8, L"p2[1].x cw", LINE_INFO());
            Assert::AreEqual(3, p2.points[1].y, 1e-8, L"p2[1].y cw", LINE_INFO());
            Assert::AreEqual(2, p2.points[2].x, 1e-8, L"p2[2].x cw", LINE_INFO());
            Assert::AreEqual(2, p2.points[2].y, 1e-8, L"p2[2].y cw", LINE_INFO());
            Assert::AreEqual(2, p2.points[3].x, 1e-8, L"p2[3].x cw", LINE_INFO());
            Assert::AreEqual(1, p2.points[3].y, 1e-8, L"p2[3].y cw", LINE_INFO());
        }
    };
}
