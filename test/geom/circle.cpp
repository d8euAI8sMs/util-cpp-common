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

            Assert::IsTrue(c.contains({ 2, 3 }), L"(2,3)", LINE_INFO());
            Assert::IsTrue(contains(c, { 2, 3 }), L"(2,3) glob", LINE_INFO());
            Assert::IsFalse(c.border_contains({ 2, 3 }), L"(2,3) border", LINE_INFO());
            Assert::IsTrue(c.inner_contains({ 2, 3 }), L"(2,3) inn", LINE_INFO());
            Assert::IsTrue(c.outer_contains({ 2, 3 }), L"(2,3) out", LINE_INFO());

            Assert::IsFalse(c.contains({ 0, 0 }), L"(0,0)", LINE_INFO());
            Assert::IsFalse(contains(c, { 0, 0 }), L"(0,0) glob", LINE_INFO());
            Assert::IsFalse(c.border_contains({ 0, 0 }), L"(0,0) border", LINE_INFO());
            Assert::IsFalse(c.inner_contains({ 0, 0 }), L"(0,0) inn", LINE_INFO());
            Assert::IsFalse(c.outer_contains({ 0, 0 }), L"(0,0) out", LINE_INFO());

            Assert::IsFalse(c.contains({ 2, 2 }), L"(2,2)", LINE_INFO());
            Assert::IsFalse(contains(c, { 2, 2 }), L"(2,2) glob", LINE_INFO());
            Assert::IsTrue(c.border_contains({ 2, 2 }), L"(2,2) border", LINE_INFO());
            Assert::IsFalse(c.inner_contains({ 2, 2 }), L"(2,2) inn", LINE_INFO());
            Assert::IsTrue(c.outer_contains({ 2, 2 }), L"(2,2) out", LINE_INFO());

            Assert::IsFalse(c.contains({ 2, 2 - 1e-12 }), L"(2,2-0)", LINE_INFO());
            Assert::IsFalse(contains(c, { 2, 2 - 1e-12 }), L"(2,2-0) glob", LINE_INFO());
            Assert::IsTrue(c.border_contains({ 2, 2 - 1e-12 }), L"(2,2-0) border", LINE_INFO());
            Assert::IsFalse(c.inner_contains({ 2, 2 - 1e-12 }), L"(2,2-0) inn", LINE_INFO());
            Assert::IsTrue(c.outer_contains({ 2, 2 - 1e-12 }), L"(2,2-0) out", LINE_INFO());

            Assert::IsTrue(c.contains({ 2, 2 + 1e-12 }), L"(2,2+0)", LINE_INFO());
            Assert::IsTrue(contains(c, { 2, 2 + 1e-12 }), L"(2,2+0) glob", LINE_INFO());
            Assert::IsTrue(c.border_contains({ 2, 2 + 1e-12 }), L"(2,2+0) border", LINE_INFO());
            Assert::IsFalse(c.inner_contains({ 2, 2 + 1e-12 }), L"(2,2+0) inn", LINE_INFO());
            Assert::IsTrue(c.outer_contains({ 2, 2 + 1e-12 }), L"(2,2+0) out", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_rotate)
            TEST_DESCRIPTION(L"rotate works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_rotate)
        {
            auto c = make_circle(2, 3, 1);
            auto r = rotate(c, 1);

            Assert::AreEqual(2, r.center.x, 1e-8, L"c.x", LINE_INFO());
            Assert::AreEqual(3, r.center.y, 1e-8, L"c.y", LINE_INFO());
            Assert::AreEqual(1, r.sqradius, 1e-8, L"r", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_scale)
            TEST_DESCRIPTION(L"scale works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_scale)
        {
            auto c = make_circle(2, 3, 1);
            auto r = scale(c, 10, { 2, 0 });

            Assert::AreEqual(2,  r.center.x, 1e-8, L"c.x", LINE_INFO());
            Assert::AreEqual(30, r.center.y, 1e-8, L"c.y", LINE_INFO());
            Assert::AreEqual(100, r.sqradius, 1e-8, L"r", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_move)
            TEST_DESCRIPTION(L"move works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_move)
        {
            auto c = make_circle(2, 3, 1);
            auto r = move(c, { 2, 3 });

            Assert::AreEqual(4, r.center.x, 1e-8, L"c.x", LINE_INFO());
            Assert::AreEqual(6, r.center.y, 1e-8, L"c.y", LINE_INFO());
            Assert::AreEqual(1, r.sqradius, 1e-8, L"r", LINE_INFO());
        }
    };
}
