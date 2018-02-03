#include "stdafx.h"

#include "CppUnitTest.h"

#include <util/common/geom/point.h>
#include <util/common/plot/shape.h>
#include <util/common/math/complex.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template<> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<geom::point<int>> (const geom::point<int> & p) { RETURN_WIDE_STRING(p); }
template<> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<geom::point<math::complex<>>> (const geom::point<math::complex<>> & p) { RETURN_WIDE_STRING(p); }

namespace geom
{

	TEST_CLASS(point_test)
	{
	public:

        BEGIN_TEST_METHOD_ATTRIBUTE(_factory)
            TEST_DESCRIPTION(L"point factory works fine")
        END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(_factory)
		{
            auto p = make_point(3, 4.0);
            Assert::AreEqual(p.x, 3, L"x", LINE_INFO());
            Assert::AreEqual(p.y, 4.0, L"y", LINE_INFO());
		}

        BEGIN_TEST_METHOD_ATTRIBUTE(_equality)
            TEST_DESCRIPTION(L"equality checking works fine")
        END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(_equality)
		{
            Assert::IsTrue(make_point(3, 4) == make_point(3, 4),
                           L"same types", LINE_INFO());
            Assert::IsTrue(make_point(3u, 4u) == make_point(3, 4),
                           L"different types", LINE_INFO());

            Assert::IsFalse(make_point(3, 4) != make_point(3, 4),
                            L"same types", LINE_INFO());
            Assert::IsFalse(make_point(3u, 4u) != make_point(3, 4),
                            L"different types", LINE_INFO());

            Assert::IsFalse(make_point(3, 4) == make_point(3, 5),
                            L"same types - neq", LINE_INFO());
            Assert::IsFalse(make_point(3, 4) == make_point(4, 4),
                            L"same types - neq", LINE_INFO());
            Assert::IsFalse(make_point(3u, 4u) == make_point(3, 5),
                            L"different types - neq", LINE_INFO());
            Assert::IsFalse(make_point(3u, 4u) == make_point(4, 4),
                            L"different types - neq", LINE_INFO());

            Assert::IsTrue(make_point(3, 4) != make_point(3, 5),
                            L"same types - neq", LINE_INFO());
            Assert::IsTrue(make_point(3, 4) != make_point(4, 4),
                            L"same types - neq", LINE_INFO());
            Assert::IsTrue(make_point(3u, 4u) != make_point(3, 5),
                            L"different types - neq", LINE_INFO());
            Assert::IsTrue(make_point(3u, 4u) != make_point(4, 4),
                            L"different types - neq", LINE_INFO());
		}

        BEGIN_TEST_METHOD_ATTRIBUTE(_norm)
            TEST_DESCRIPTION(L"norm (length) is calculated correctly")
        END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(_norm)
		{
            Assert::AreEqual(5, make_point(3, 4).length(), 1e-8,
                             L"length", LINE_INFO());
            Assert::AreEqual(5, norm(make_point(3, 4)), 1e-8,
                             L"different types", LINE_INFO());
            Assert::AreEqual(25, sqnorm(make_point(3, 4)), 1e-8,
                             L"different types", LINE_INFO());
		}

        BEGIN_TEST_METHOD_ATTRIBUTE(_angle)
            TEST_DESCRIPTION(L"angle is calculated correctly")
        END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(_angle)
		{
            const double pi_4 = std::atan(1);

            Assert::AreEqual(pi_4, make_point(4, 4).angle(), 1e-8,
                             L"pi/4", LINE_INFO());
            Assert::AreEqual(0, make_point(4, 0).angle(), 1e-8,
                             L"0", LINE_INFO());
            Assert::AreEqual(2 * pi_4, make_point(0, 4).angle(), 1e-8,
                             L"pi/2", LINE_INFO());
            Assert::AreEqual(- 3 * pi_4, make_point(-4, -4).angle(), 1e-8,
                             L"-3pi/4", LINE_INFO());
            Assert::AreEqual(4 * pi_4, make_point(-4, 0).angle(), 1e-8,
                             L"pi", LINE_INFO());
            Assert::AreEqual(- 2 * pi_4, make_point(0, -4).angle(), 1e-8,
                             L"-pi/2", LINE_INFO());
		}

        BEGIN_TEST_METHOD_ATTRIBUTE(_conjugate)
            TEST_DESCRIPTION(L"conjugation operation is correct")
        END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(_conjugate)
		{
            using namespace math;
            Assert::AreEqual(make_point(2 - 3 * _i, - 6 * _i),
                             conjugate(make_point(2 + 3 * _i, + 6 * _i)),
                             L"conjugate", LINE_INFO());
		}

        BEGIN_TEST_METHOD_ATTRIBUTE(_add)
            TEST_DESCRIPTION(L"add and subtract operations are correct")
        END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(_add)
		{
            Assert::AreEqual(make_point(4, 6),
                             make_point(1, 2) + make_point(3, 4),
                             L"add", LINE_INFO());
            Assert::AreEqual(make_point(4, 6),
                             make_point(1, 2) - make_point(-3, -4),
                             L"add", LINE_INFO());

            auto p = make_point(1, 2);
            Assert::AreEqual(make_point(4, 6),
                             p += make_point(3, 4),
                             L"add", LINE_INFO());
            p = make_point(1, 2);
            Assert::AreEqual(make_point(4, 6),
                             p -= make_point(-3, -4),
                             L"add", LINE_INFO());
		}

        BEGIN_TEST_METHOD_ATTRIBUTE(_empty)
            TEST_DESCRIPTION(L"empty check is correct")
        END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(_empty)
		{
            using namespace math;

            Assert::IsFalse(make_point(4, 6).empty(),
                             L"non-empty int", LINE_INFO());
            Assert::IsFalse(make_point(2 - 3 * _i, - 6 * _i).empty(),
                             L"non-empty complex", LINE_INFO());

            Assert::IsTrue(make_point(0, 0).empty(),
                           L"empty int", LINE_INFO());
            Assert::IsTrue(make_point(0 * _i, 0 * _i).empty(),
                           L"empty complex", LINE_INFO());
		}

        BEGIN_TEST_METHOD_ATTRIBUTE(_distance)
            TEST_DESCRIPTION(L"distance calculation is correct")
        END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(_distance)
		{
            Assert::AreEqual(5.0, make_point(0, 0).distance(3, 4), 1e-8,
                             L"from (0,0)", LINE_INFO());
            Assert::AreEqual(5.0, make_point(1, 1).distance(4, 5), 1e-8,
                             L"from (1,1)", LINE_INFO());
		}
	};
}
