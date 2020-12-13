// SPDX-License-Identifier: GPL-2.0-or-later

#include "container.hpp"

#include <gtest/gtest.h>

namespace teg::common
{

TEST(Container, find_and_copy_item)
{
	int const haystack[] = {1, 2, 3, 4};

	auto const gt2 = [](int i) {
		return i>2;
	};

	auto const eq3 = [](int i) {
		return i==3;
	};

	auto const eq4 = [](int i) {
		return i==4;
	};

	auto const eq23 = [](int i) {
		return i==23;
	};


	{
		int const *result{nullptr};
		EXPECT_TRUE(find_and_copy_pointer(haystack, result, gt2));
		EXPECT_EQ(haystack+2, result);
	}

	{
		int const *result{nullptr};
		EXPECT_TRUE(find_and_copy_pointer(haystack, result, eq3));
		EXPECT_EQ(haystack+2, result);
	}

	{
		int const *result{nullptr};
		EXPECT_TRUE(find_and_copy_pointer(haystack, result, eq4));
		EXPECT_EQ(haystack+3, result);
	}

	{
		int const *result{haystack};
		EXPECT_FALSE(find_and_copy_pointer(haystack, result, eq23));
		EXPECT_EQ(nullptr, result);
	}
}

}
