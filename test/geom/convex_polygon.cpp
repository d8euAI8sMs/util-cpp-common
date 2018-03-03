#include "stdafx.h"

#include "CppUnitTest.h"

#include <util/common/geom/convex_polygon.h>

#include <array>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace geom
{

    using arr4_t  = std::array < point2d_t, 4 > ;

    static const arr4_t points =
    {
        { { 2, 2 }, { 3, 3 }, { 2, 4 }, { 1, 3 } }
    };

    TEST_CLASS(convex_polygon_test)
    {
    public:

        BEGIN_TEST_METHOD_ATTRIBUTE(_compiles)
            TEST_DESCRIPTION(L"code compiles")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_compiles)
        {
            // test that compilation succeeds :D
            auto p = make_convex_polygon(points); // array
            auto p2 = convex_polygon < > (std::vector < point2d_t > (points.begin(), points.end())); // vector
            auto p3 = rotate(p, 0);
            auto p4 = scale(p, 1);
            auto p5 = move(p, {});
        }
    };
}
