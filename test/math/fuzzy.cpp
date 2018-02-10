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
    };
}
