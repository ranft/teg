// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <algorithm>
#include <array>

namespace teg::common
{

/**
 * Find the first element matching predicate \p p in the container \p c, and
 * assign its contents to \p dest.
 *
 * \return True if the element was found and \p dest was assigned,
 *         false otherwiese.
 */
template<typename Predicate, typename Item, typename C>
bool find_and_copy_pointer(C& c, Item* (&dest), Predicate p)
{
	const auto item = std::find_if(std::begin(c), std::end(c), p);
	if(item != std::end(c)) {
		dest = &(*item);
		return true;
	} else {
		dest = nullptr;
		return false;
	}
}

}
