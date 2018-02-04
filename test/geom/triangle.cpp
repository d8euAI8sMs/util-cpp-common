#include "stdafx.h"

#include "CppUnitTest.h"

#include <util/common/geom/triangle.h>

#include <array>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace geom
{

    using arr3_t = std::array < point2d_t, 3 > ;

    static const arr3_t points =
    {
        { { 2, 2 }, { 3, 3 }, { 2, 4 } }
    };

    TEST_CLASS(triangle_test)
    {
    public:

        BEGIN_TEST_METHOD_ATTRIBUTE(_compiles)
            TEST_DESCRIPTION(L"code compiles")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_compiles)
        {
            // test that compilation succeeds :D
            auto t = make_triangle(points);
            auto t2 = make_triangle(2, 2, 3.0, 3.0, 2u, 4u);
            auto t3 = rotate(t, 0);
            auto t4 = scale(t, 1);
            auto t5 = move(t, {});
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_constructor)
            TEST_DESCRIPTION(L"triangle cannot be created from collection with size > 3")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_constructor)
        {
            Assert::ExpectException < std::domain_error > ([] () {
                auto t = make_triangle(std::vector < point2d_t > (4));
            }, L"4", LINE_INFO());

            auto t2 = make_triangle(std::vector < point2d_t > ({ { 1, 2 }, { 3, 4 } }));

            Assert::AreEqual(1, t2.points[0].x, 1e-8, L"t2[0].x", LINE_INFO());
            Assert::AreEqual(2, t2.points[0].y, 1e-8, L"t2[0].y", LINE_INFO());
            Assert::AreEqual(3, t2.points[1].x, 1e-8, L"t2[1].x", LINE_INFO());
            Assert::AreEqual(4, t2.points[1].y, 1e-8, L"t2[1].y", LINE_INFO());
            Assert::AreEqual(0, t2.points[2].x, 1e-8, L"t2[2].x", LINE_INFO());
            Assert::AreEqual(0, t2.points[2].y, 1e-8, L"t2[2].y", LINE_INFO());
        }
    };
}
