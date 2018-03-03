#include "stdafx.h"

#include "CppUnitTest.h"

#include <util/common/geom/circle.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace geom
{

    TEST_CLASS(circle_test)
    {
    public:

        BEGIN_TEST_METHOD_ATTRIBUTE(_factory)
            TEST_DESCRIPTION(L"factory works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_factory)
        {
            auto c = make_circle(2, 3, 1);

            Assert::AreEqual(2, c.center.x, 1e-8, L"c.x", LINE_INFO());
            Assert::AreEqual(3, c.center.y, 1e-8, L"c.y", LINE_INFO());
            Assert::AreEqual(1, c.sqradius, 1e-8, L"r", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_radius)
            TEST_DESCRIPTION(L"radius works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_radius)
        {
            auto c = make_circle(2, 3, 25);

            Assert::AreEqual(25, c.sqradius, 1e-8, L"sqr", LINE_INFO());
            Assert::AreEqual(5, c.radius(), 1e-8, L"r", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_contains)
            TEST_DESCRIPTION(L"contains works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_contains)
        {
            auto c = make_circle(2, 3, 1);

            Assert::AreEqual(1, c.contains({ 2, 3 }), L"(2,3)", LINE_INFO());

            Assert::AreEqual(-1, c.contains({ 0, 0 }), L"(0,0)", LINE_INFO());

            Assert::AreEqual(0, c.contains({ 2, 2 }), L"(2,2)", LINE_INFO());

            Assert::AreEqual(0, c.contains({ 2, 2 - 1e-12 }), L"(2,2-0)", LINE_INFO());

            Assert::AreEqual(0, c.contains({ 2, 2 + 1e-12 }), L"(2,2+0)", LINE_INFO());
        }
    };
}
