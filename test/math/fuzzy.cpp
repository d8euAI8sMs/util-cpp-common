#include "stdafx.h"

#include "CppUnitTest.h"

#include <util/common/math/fuzzy.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace math
{

    struct test_traits : fuzzy_traits < double >
    {
        static type tolerance() { return 1e-8; }
    };

    TEST_CLASS(fuzzy_test)
    {
    public:

        BEGIN_TEST_METHOD_ATTRIBUTE(_equals)
            TEST_DESCRIPTION(L"equals works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_equals)
        {
            Assert::AreEqual(1,  fuzzy < test_traits > :: equals(1, 1), L"1 == 1", LINE_INFO());
            Assert::AreEqual(-1, fuzzy < test_traits > :: equals(1, 2), L"1 != 2", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: equals(1, 1 + 1e-12), L"1 ~ 1+0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: equals(1, 1 - 1e-12), L"1 ~ 1-0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: equals(1, 1 + test_traits::tolerance()), L"1 ~ 1+t", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: equals(1, 1 - test_traits::tolerance()), L"1 ~ 1-t", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_equals_override_tolerance)
            TEST_DESCRIPTION(L"equals with overridden tolerance works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_equals_override_tolerance)
        {
            Assert::AreEqual(1,  fuzzy < test_traits > :: equals(1, 1, 0.5), L"1 == 1", LINE_INFO());
            Assert::AreEqual(-1, fuzzy < test_traits > :: equals(1, 2, 0.5), L"1 != 2", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: equals(1, 1 + 1e-12, 0.5), L"1 ~ 1+0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: equals(1, 1 - 1e-12, 0.5), L"1 ~ 1-0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: equals(1, 1 + 0.5, 0.5), L"1 ~ 1+t", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: equals(1, 1 - 0.5, 0.5), L"1 ~ 1-t", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_operator_eq)
            TEST_DESCRIPTION(L"== works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_operator_eq)
        {
            Assert::IsTrue (fuzzy < test_traits > (1) == 1, L"1 == 1", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) == 2, L"1 != 2", LINE_INFO());
            Assert::IsTrue (fuzzy < test_traits > (1) == 1 + 1e-12, L"1 ~ 1+0", LINE_INFO());
            Assert::IsTrue (fuzzy < test_traits > (1) == 1 - 1e-12, L"1 ~ 1-0", LINE_INFO());
            Assert::IsTrue (fuzzy < test_traits > (1) == 1 + test_traits::tolerance(), L"1 ~ 1+t", LINE_INFO());
            Assert::IsTrue (fuzzy < test_traits > (1) == 1 - test_traits::tolerance(), L"1 ~ 1-t", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_operator_neq)
            TEST_DESCRIPTION(L"!= works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_operator_neq)
        {
            Assert::IsFalse(fuzzy < test_traits > (1) != 1, L"1 == 1", LINE_INFO());
            Assert::IsTrue (fuzzy < test_traits > (1) != 2, L"1 != 2", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) != 1 + 1e-12, L"1 ~ 1+0", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) != 1 - 1e-12, L"1 ~ 1-0", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) != 1 + test_traits::tolerance(), L"1 ~ 1+t", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) != 1 - test_traits::tolerance(), L"1 ~ 1-t", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_less)
            TEST_DESCRIPTION(L"less works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_less)
        {
            Assert::AreEqual(1,  fuzzy < test_traits > :: less(1, 2), L"1 < 2", LINE_INFO());
            Assert::AreEqual(-1, fuzzy < test_traits > :: less(2, 1), L"2 !< 1", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: less(1, 1 + 1e-12), L"1 ~ 1+0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: less(1, 1 - 1e-12), L"1 ~ 1-0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: less(1, 1 + test_traits::tolerance()), L"1 ~ 1+t", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: less(1, 1 - test_traits::tolerance()), L"1 ~ 1-t", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_less_override_tolerance)
            TEST_DESCRIPTION(L"less with overridden tolerance works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_less_override_tolerance)
        {
            Assert::AreEqual(1,  fuzzy < test_traits > :: less(1, 2, 0.5), L"1 < 2", LINE_INFO());
            Assert::AreEqual(-1, fuzzy < test_traits > :: less(2, 1, 0.5), L"2 !< 1", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: less(1, 1 + 1e-12, 0.5), L"1 ~ 1+0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: less(1, 1 - 1e-12, 0.5), L"1 ~ 1-0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: less(1, 1 + 0.5, 0.5), L"1 ~ 1+t", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: less(1, 1 - 0.5, 0.5), L"1 ~ 1-t", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_operator_lt)
            TEST_DESCRIPTION(L"< works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_operator_lt)
        {
            Assert::IsTrue (fuzzy < test_traits > (1) < 2, L"1 < 2", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (2) < 1, L"2 !< 1", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) < 1 + 1e-12, L"1 ~ 1+0", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) < 1 - 1e-12, L"1 ~ 1-0", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) < 1 + test_traits::tolerance(), L"1 ~ 1+t", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) < 1 - test_traits::tolerance(), L"1 ~ 1-t", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_operator_le)
            TEST_DESCRIPTION(L"<= works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_operator_le)
        {
            Assert::IsTrue (fuzzy < test_traits > (1) <= 2, L"1 < 2", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (2) <= 1, L"2 !< 1", LINE_INFO());
            Assert::IsTrue (fuzzy < test_traits > (1) <= 1 + 1e-12, L"1 ~ 1+0", LINE_INFO());
            Assert::IsTrue (fuzzy < test_traits > (1) <= 1 - 1e-12, L"1 ~ 1-0", LINE_INFO());
            Assert::IsTrue (fuzzy < test_traits > (1) <= 1 + test_traits::tolerance(), L"1 ~ 1+t", LINE_INFO());
            Assert::IsTrue (fuzzy < test_traits > (1) <= 1 - test_traits::tolerance(), L"1 ~ 1-t", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_greater)
            TEST_DESCRIPTION(L"greater works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_greater)
        {
            Assert::AreEqual(1,  fuzzy < test_traits > :: greater(2, 1), L"2 > 1", LINE_INFO());
            Assert::AreEqual(-1, fuzzy < test_traits > :: greater(1, 2), L"1 !> 2", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: greater(1, 1 + 1e-12), L"1 ~ 1+0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: greater(1, 1 - 1e-12), L"1 ~ 1-0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: greater(1, 1 + test_traits::tolerance()), L"1 ~ 1+t", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: greater(1, 1 - test_traits::tolerance()), L"1 ~ 1-t", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_greater_override_tolerance)
            TEST_DESCRIPTION(L"greater with overridden tolerance works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_greater_override_tolerance)
        {
            Assert::AreEqual(1,  fuzzy < test_traits > :: greater(2, 1, 0.5), L"2 > 1", LINE_INFO());
            Assert::AreEqual(-1, fuzzy < test_traits > :: greater(1, 2, 0.5), L"1 !> 2", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: greater(1, 1 + 1e-12, 0.5), L"1 ~ 1+0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: greater(1, 1 - 1e-12, 0.5), L"1 ~ 1-0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: greater(1, 1 + 0.5, 0.5), L"1 ~ 1+t", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: greater(1, 1 - 0.5, 0.5), L"1 ~ 1-t", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_operator_gt)
            TEST_DESCRIPTION(L"> works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_operator_gt)
        {
            Assert::IsTrue (fuzzy < test_traits > (2) > 1, L"2 > 1", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) > 2, L"1 !> 2", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) > 1 + 1e-12, L"1 ~ 1+0", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) > 1 - 1e-12, L"1 ~ 1-0", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) > 1 + test_traits::tolerance(), L"1 ~ 1+t", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) > 1 - test_traits::tolerance(), L"1 ~ 1-t", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_operator_ge)
            TEST_DESCRIPTION(L">= works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_operator_ge)
        {
            Assert::IsTrue (fuzzy < test_traits > (2) >= 1, L"2 > 1", LINE_INFO());
            Assert::IsFalse(fuzzy < test_traits > (1) >= 2, L"1 !> 2", LINE_INFO());
            Assert::IsTrue (fuzzy < test_traits > (1) >= 1 + 1e-12, L"1 ~ 1+0", LINE_INFO());
            Assert::IsTrue (fuzzy < test_traits > (1) >= 1 - 1e-12, L"1 ~ 1-0", LINE_INFO());
            Assert::IsTrue (fuzzy < test_traits > (1) >= 1 + test_traits::tolerance(), L"1 ~ 1+t", LINE_INFO());
            Assert::IsTrue (fuzzy < test_traits > (1) >= 1 - test_traits::tolerance(), L"1 ~ 1-t", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_compare)
            TEST_DESCRIPTION(L"compare works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_compare)
        {
            Assert::AreEqual(1,  fuzzy < test_traits > :: compare(2, 1), L"2 > 1", LINE_INFO());
            Assert::AreEqual(-1, fuzzy < test_traits > :: compare(1, 2), L"1 < 2", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: compare(1, 1), L"1 = 1", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: compare(1, 1 + 1e-12), L"1 ~ 1+0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: compare(1, 1 - 1e-12), L"1 ~ 1-0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: compare(1, 1 + test_traits::tolerance()), L"1 ~ 1+t", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: compare(1, 1 - test_traits::tolerance()), L"1 ~ 1-t", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(_compare_override_tolerance)
            TEST_DESCRIPTION(L"compare with overridden tolerance works fine")
        END_TEST_METHOD_ATTRIBUTE()

        TEST_METHOD(_compare_override_tolerance)
        {
            Assert::AreEqual(1,  fuzzy < test_traits > :: compare(2, 1, 0.5), L"2 > 1", LINE_INFO());
            Assert::AreEqual(-1, fuzzy < test_traits > :: compare(1, 2, 0.5), L"1 < 2", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: compare(1, 1, 0.5), L"1 = 1", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: compare(1, 1 + 1e-12, 0.5), L"1 ~ 1+0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: compare(1, 1 - 1e-12, 0.5), L"1 ~ 1-0", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: compare(1, 1 + 0.5, 0.5), L"1 ~ 1+t", LINE_INFO());
            Assert::AreEqual(0,  fuzzy < test_traits > :: compare(1, 1 - 0.5, 0.5), L"1 ~ 1-t", LINE_INFO());
        }
    };
}
