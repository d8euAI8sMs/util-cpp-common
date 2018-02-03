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
		}

        BEGIN_TEST_METHOD_ATTRIBUTE(_conjugate)
            TEST_DESCRIPTION(L"conjugation operation is correct")
        END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(_conjugate)
		{
            using namespace math;
            Assert::AreEqual(make_line(2, - 6, 8, 1),
                             conjugate(make_line(2, -6, 8, 1)),
                             L"conjugate", LINE_INFO());
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
		}
	};
}
