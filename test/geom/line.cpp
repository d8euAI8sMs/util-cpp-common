#include "stdafx.h"

#include "CppUnitTest.h"

#include <util/common/geom/line.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


template<> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<geom::point2d_t> (const geom::point2d_t & p) { RETURN_WIDE_STRING(p); }
template<> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<geom::line> (const geom::line & l) { RETURN_WIDE_STRING(l); }

namespace geom
{

    TEST_CLASS(line_test)
    {
    public:

        BEGIN_TEST_METHOD_ATTRIBUTE(_factory)
            TEST_DESCRIPTION(L"line factory works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_factory)
        {
            auto l = make_line(make_point(1, 2.0), make_point(3, 4.0));
            Assert::AreEqual(make_point(1.0, 2.0), l.p1, L"p1", LINE_INFO());
            Assert::AreEqual(make_point(3.0, 4.0), l.p2, L"p2", LINE_INFO());

            l = make_line(1, 2.0, 3, 4.0);
            Assert::AreEqual(make_point(1.0, 2.0), l.p1, L"p1", LINE_INFO());
            Assert::AreEqual(make_point(3.0, 4.0), l.p2, L"p2", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_equality)
            TEST_DESCRIPTION(L"equality checking works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_equality)
        {
            Assert::IsTrue(make_line(1, 2.0, 3, 4.0) == make_line(1, 2.0, 3, 4.0),
                           L"same types", LINE_INFO());
            Assert::IsTrue(make_line(1u, 2.0, 3u, 4.0) == make_line(1, 2.0, 3, 4.0),
                           L"different types", LINE_INFO());

            Assert::IsFalse(make_line(1, 2.0, 3, 4.0) != make_line(1, 2.0, 3, 4.0),
                           L"same types", LINE_INFO());
            Assert::IsFalse(make_line(1u, 2.0, 3u, 4.0) != make_line(1, 2.0, 3, 4.0),
                           L"different types", LINE_INFO());

            Assert::IsTrue(make_line(1, 2.0, 3, 4.0) != make_line(2, 2.0, 3, 4.0),
                           L"same types - neq", LINE_INFO());
            Assert::IsTrue(make_line(1u, 2.0, 3u, 4.0) != make_line(2, 2.0, 3, 4.0),
                           L"different types - neq", LINE_INFO());
            Assert::IsTrue(make_line(1, 2.0, 3, 4.0) != make_line(1, 2.0, 4, 4.0),
                           L"same types - neq", LINE_INFO());
            Assert::IsTrue(make_line(1u, 2.0, 3u, 4.0) != make_line(2, 2.0, 4, 4.0),
                           L"different types - neq", LINE_INFO());

            Assert::IsFalse(make_line(1, 2.0, 3, 4.0) == make_line(2, 2.0, 3, 4.0),
                           L"same types - neq", LINE_INFO());
            Assert::IsFalse(make_line(1u, 2.0, 3u, 4.0) == make_line(2, 2.0, 3, 4.0),
                           L"different types - neq", LINE_INFO());
            Assert::IsFalse(make_line(1, 2.0, 3, 4.0) == make_line(1, 2.0, 4, 4.0),
                           L"same types - neq", LINE_INFO());
            Assert::IsFalse(make_line(1u, 2.0, 3u, 4.0) == make_line(2, 2.0, 4, 4.0),
                           L"different types - neq", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_norm)
            TEST_DESCRIPTION(L"norm (length) is calculated correctly")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_norm)
        {
            Assert::AreEqual(5, make_line(0, 0, 3, 4).length(), 1e-8,
                             L"length (0,0)-(3,4)", LINE_INFO());
            Assert::AreEqual(5, make_line(1, 1, 4, 5).length(), 1e-8,
                             L"length (1,1)-(4,5)", LINE_INFO());
            Assert::AreEqual(5, math::norm(make_line(1, 1, 4, 5)), 1e-8,
                             L"norm (1,1)-(4,5)", LINE_INFO());
            Assert::AreEqual(25, math::sqnorm(make_line(1, 1, 4, 5)), 1e-8,
                             L"norm (1,1)-(4,5)", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_distance)
            TEST_DESCRIPTION(L"distance is calculated correctly")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_distance)
        {
            auto l = make_line(1, 2, 3, 4);

            Assert::AreEqual(1, l.sqdistance({ 3, 2 }).confidence,
                             L"p 1", LINE_INFO());
            Assert::AreEqual(2, l.sqdistance({ 3, 2 }), 1e-8,
                             L"p 1 val", LINE_INFO());
            Assert::AreEqual(1, l.segment_sqdistance({ 3, 2 }).confidence,
                             L"p 1 seg", LINE_INFO());
            Assert::AreEqual(2, l.segment_sqdistance({ 3, 2 }), 1e-8,
                             L"p 1 val seg", LINE_INFO());

            Assert::AreEqual(1, l.sqdistance({ 0, 3 }).confidence,
                             L"p 2", LINE_INFO());
            Assert::AreEqual(2, l.sqdistance({ 0, 3 }), 1e-8,
                             L"p 2 val", LINE_INFO());
            Assert::AreEqual(-1, l.segment_sqdistance({ 0, 3 }).confidence,
                             L"p 2 seg", LINE_INFO());
            Assert::AreEqual(0, l.segment_sqdistance({ 0, 3 }), 1e-8,
                             L"p 2 val seg", LINE_INFO());

            Assert::AreEqual(1, l.sqdistance(l.p1).confidence,
                             L"p 3", LINE_INFO());
            Assert::AreEqual(0, l.sqdistance(l.p1), 1e-8,
                             L"p 3 val", LINE_INFO());
            Assert::AreEqual(0, l.segment_sqdistance(l.p1).confidence,
                             L"p 3 seg", LINE_INFO());
            Assert::AreEqual(0, l.segment_sqdistance(l.p1), 1e-8,
                             L"p 3 val seg", LINE_INFO());

            Assert::AreEqual(1, l.sqdistance(l.p2).confidence,
                             L"p 4", LINE_INFO());
            Assert::AreEqual(0, l.sqdistance(l.p2), 1e-8,
                             L"p 4 val", LINE_INFO());
            Assert::AreEqual(0, l.segment_sqdistance(l.p2).confidence,
                             L"p 4 seg", LINE_INFO());
            Assert::AreEqual(0, l.segment_sqdistance(l.p2), 1e-8,
                             L"p 4 val seg", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_contains)
            TEST_DESCRIPTION(L"containment is calculated correctly")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_contains)
        {
            auto l = make_line(1, 2, 3, 4);

            Assert::AreEqual(1, l.contains(l.inner_point(0.5)),
                             L"p 1", LINE_INFO());
            Assert::AreEqual(1, l.segment_contains(l.inner_point(0.5)),
                             L"p 1", LINE_INFO());

            Assert::AreEqual(1, l.contains(l.inner_point(2)),
                             L"p 2", LINE_INFO());
            Assert::AreEqual(-1, l.segment_contains(l.inner_point(2)),
                             L"p 2", LINE_INFO());

            Assert::AreEqual(1, l.contains(l.p1),
                             L"p 3", LINE_INFO());
            Assert::AreEqual(0, l.segment_contains(l.p1),
                             L"p 3", LINE_INFO());

            Assert::AreEqual(1, l.contains(l.p2),
                             L"p 4", LINE_INFO());
            Assert::AreEqual(0, l.segment_contains(l.p2),
                             L"p 4", LINE_INFO());

            Assert::AreEqual(-1, l.contains({ 3, 2 }),
                             L"p 5", LINE_INFO());
            Assert::AreEqual(-1, l.segment_contains({ 3, 2 }),
                             L"p 5", LINE_INFO());

            Assert::AreEqual(-1, l.contains({ 1, 4 }),
                             L"p 6", LINE_INFO());
            Assert::AreEqual(-1, l.segment_contains({ 1, 4 }),
                             L"p 6", LINE_INFO());

            Assert::AreEqual(-1, l.contains({ 2, 1 }),
                             L"p 7", LINE_INFO());
            Assert::AreEqual(-1, l.segment_contains({ 2, 1 }),
                             L"p 7", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_angle)
            TEST_DESCRIPTION(L"angle is calculated correctly")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_angle)
        {
            const double pi_4 = std::atan(1);

            Assert::AreEqual(pi_4, make_line(1, 1, 4, 4).angle(), 1e-8,
                             L"pi/4", LINE_INFO());
            Assert::AreEqual(0, make_line(1, 1, 4, 1).angle(), 1e-8,
                             L"0", LINE_INFO());
            Assert::AreEqual(2 * pi_4, make_line(1, 1, 1, 4).angle(), 1e-8,
                             L"pi/2", LINE_INFO());
            Assert::AreEqual(- 3 * pi_4, make_line(1, 1, -4, -4).angle(), 1e-8,
                             L"-3pi/4", LINE_INFO());
            Assert::AreEqual(4 * pi_4, make_line(1, 1, -4, 1).angle(), 1e-8,
                             L"pi", LINE_INFO());
            Assert::AreEqual(- 2 * pi_4, make_line(1, 1, 1, -4).angle(), 1e-8,
                             L"-pi/2", LINE_INFO());
            Assert::AreEqual(0, make_line(1, 1, 1, 1).angle(), 1e-8,
                             L"0 - 1", LINE_INFO());
            Assert::AreEqual(0, make_line(0, 0, 0, 0).angle(), 1e-8,
                             L"0 - 2", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_empty)
            TEST_DESCRIPTION(L"empty check is correct")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_empty)
        {
            using namespace math;

            Assert::IsFalse(make_line(0, 0, 4, 6).empty(),
                             L"non-empty p2", LINE_INFO());
            Assert::IsFalse(make_line(4, 6, 0, 0).empty(),
                             L"non-empty p1", LINE_INFO());

            Assert::IsTrue(make_line(0, 0, 0, 0).empty(),
                           L"empty int", LINE_INFO());
            Assert::IsTrue(make_line(4, 6, 4, 6).empty(),
                           L"empty int - 2", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_inner_point)
            TEST_DESCRIPTION(L"inner point and center calculation is correct")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_inner_point)
        {
            auto p1 = make_line(1, 2, 3, 4).inner_point(0);
            auto p2 = make_line(1, 2, 3, 4).inner_point(1);
            auto p3 = make_line(1, 2, 3, 4).inner_point(0.5);
            auto p4 = make_line(1, 2, 3, 4).inner_point(2);

            Assert::AreEqual(1, p1.x, 1e-8,
                             L"p1.x", LINE_INFO());
            Assert::AreEqual(2, p1.y, 1e-8,
                             L"p1.y", LINE_INFO());
            Assert::AreEqual(3, p2.x, 1e-8,
                             L"p2.x", LINE_INFO());
            Assert::AreEqual(4, p2.y, 1e-8,
                             L"p2.y", LINE_INFO());
            Assert::AreEqual(2, p3.x, 1e-8,
                             L"p3.x", LINE_INFO());
            Assert::AreEqual(3, p3.y, 1e-8,
                             L"p3.y", LINE_INFO());
            Assert::AreEqual(5, p4.x, 1e-8,
                             L"p4.x", LINE_INFO());
            Assert::AreEqual(6, p4.y, 1e-8,
                             L"p4.y", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_intersection)
            TEST_DESCRIPTION(L"intersection calculation is correct")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_intersection)
        {
            auto l1 = make_line(1, 2, 3, 4);
            auto l2 = make_line(1, 4, 3, 2);
            auto l3 = make_line(0, 4, 2, 2);
            auto l4 = make_line(-1, 2, 1, 0);
            auto l5 = make_line(2, 1, 0, 3);
            // real case test; lines are almost parallel; fail if l6.p1 *= ~10000
            // coincidence check would be nice in such case
            auto l6 = make_line(-2, 2, -3.9395141601562496e-007, 3.9297485351562499e-007);
            auto l7 = make_line(2, -2, -3.9395141601562496e-007, 3.9297485351562499e-007);

            double q1, q2;
            status_t r;

            r = l1.intersection(l2, q1, q2);

            Assert::AreEqual(status::trusted(status::ok | status::line::intersects),
                             r, L"l1-l2", LINE_INFO());
            Assert::AreEqual(0.5, q1, 1e-8,
                             L"l1-l2 q1", LINE_INFO());
            Assert::AreEqual(0.5, q2, 1e-8,
                             L"l1-l2 q2", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::line::intersects),
                             l1.intersects(l2), L"l1-l2 int", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::line::intersects | status::line::both_segments),
                             l1.segment_intersects(l2), L"l1-l2 seg", LINE_INFO());

            r = l1.intersection(l3, q1, q2);

            Assert::AreEqual(status::trusted(status::ok | status::line::intersects),
                             r, L"l1-l3", LINE_INFO());
            Assert::AreEqual(0.25, q1, 1e-8,
                             L"l1-l3 q1", LINE_INFO());
            Assert::AreEqual(0.75, q2, 1e-8,
                             L"l1-l3 q2", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::line::intersects),
                             l1.intersects(l3), L"l1-l3 int", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::line::intersects | status::line::both_segments),
                             l1.segment_intersects(l3), L"l1-l3 seg", LINE_INFO());

            r = l1.intersection(l4, q1, q2);

            Assert::AreEqual(status::trusted(status::ok | status::line::intersects),
                             r, L"l1-l4", LINE_INFO());
            Assert::AreEqual(-0.5, q1, 1e-8,
                             L"l1-l4 q1", LINE_INFO());
            Assert::AreEqual(0.5, q2, 1e-8,
                             L"l1-l4 q2", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::line::intersects),
                             l1.intersects(l4), L"l1-l4 int", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::line::intersects | status::line::other_segment),
                             l1.segment_intersects(l4), L"l1-l4 seg", LINE_INFO());

            r = l2.intersection(l4, q1, q2);

            Assert::AreEqual(status::trusted(status::ok),
                             r, L"l2-l4", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             l2.intersects(l4), L"l2-l4 int", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             l2.segment_intersects(l4), L"l2-l4 seg", LINE_INFO());

            r = l1.intersection(l1, q1, q2);

            Assert::AreEqual(status::trusted(status::ok),
                             r, L"l1-l1", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             l1.intersects(l1), L"l1-l1 int", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok),
                             l1.segment_intersects(l1), L"l1-l1 seg", LINE_INFO());

            r = l1.intersection(l1, q1, q2, flags::line::check_coincidence);

            Assert::AreEqual(status::trusted(status::ok | status::line::coincides),
                             r, L"l1-l1", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::line::coincides),
                             l1.intersects(l1, flags::line::check_coincidence), L"l1-l1 int", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::line::coincides),
                             l1.segment_intersects(l1, flags::line::check_coincidence), L"l1-l1 seg", LINE_INFO());

            r = l1.intersection(l5, q1, q2);

            Assert::AreEqual(status::trusted(status::ok | status::line::intersects),
                             r, L"l1-l5", LINE_INFO());
            Assert::AreEqual(0, q1, 1e-8,
                             L"l1-l5 q1", LINE_INFO());
            Assert::AreEqual(0.5, q2, 1e-8,
                             L"l1-l5 q2", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::line::intersects),
                             l1.intersects(l5), L"l1-l5 int", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::line::intersects | status::line::other_segment) | status::line::self_segment,
                             l1.segment_intersects(l5), L"l1-l5 seg", LINE_INFO());

            r = l6.intersection(l7, q1, q2);

            Assert::AreEqual(status::trusted(status::ok | status::line::intersects),
                             r, L"l6-l7", LINE_INFO());
            Assert::AreEqual(1, q1, 1e-4,
                             L"l6-l7 q1", LINE_INFO());
            Assert::AreEqual(1, q2, 1e-4,
                             L"l6-l7 q2", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::line::intersects),
                             l6.intersects(l7), L"l6-l7 int", LINE_INFO());
            Assert::AreEqual(status::trusted(status::ok | status::line::intersects) | status::line::both_segments,
                             l6.segment_intersects(l7), L"l6-l7 seg", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_convexity)
            TEST_DESCRIPTION(L"orientation calculation is correct")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_convexity)
        {
            Assert::IsTrue(make_line(1, 2, 3, 4).convexity({ 1, 4 }) == convex_type::counterclockwise,
                             L"ccw", LINE_INFO());
            Assert::IsTrue(make_line(1, 2, 3, 4).convexity({ 3, 2 }) == convex_type::clockwise,
                             L"cw", LINE_INFO());
            Assert::IsTrue(make_line(1, 2, 3, 4).convexity({ 1, 2 }) == convex_type::degenerate,
                             L"no - 1", LINE_INFO());
            Assert::IsTrue(make_line(1, 2, 3, 4).convexity({ 1.5, 2.5 }) == convex_type::degenerate,
                             L"no - 2", LINE_INFO());
            Assert::IsTrue(make_line(1, 2, 3, 4).convexity({ 0, 1 }) == convex_type::degenerate,
                             L"no - 3", LINE_INFO());
            Assert::IsTrue(make_line(1, 2, 3, 4).convexity({ 0, 1 + 1e-12 }) == convex_type::degenerate,
                             L"no - 4", LINE_INFO());
            Assert::IsTrue(make_line(1, 2, 3, 4).convexity({ 0, 1 - 1e-12 }) == convex_type::degenerate,
                             L"no - 5", LINE_INFO());
            Assert::IsTrue(make_line(1, 2, 1 + 1e-12, 2).convexity({ 1, 3 }) == convex_type::degenerate,
                             L"no - 6", LINE_INFO());
            Assert::IsTrue(make_line(1, 2, 1, 2 + 1e-12).convexity({ 2, 2 }) == convex_type::degenerate,
                             L"no - 7", LINE_INFO());
        }
    };
}
