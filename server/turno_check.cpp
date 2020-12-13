// SPDX-License-Identifier: GPL-2.0-or-later

#include "turno.h"

#include <gtest/gtest.h>

namespace teg::server
{

void turn_sub_check(FindNextPlayer& p, SPLAYER* records,
                    std::size_t selected_record, SPLAYER *expected_record,
                    unsigned origin)
{
	p.search(&p, records+selected_record);
	EXPECT_EQ(expected_record, p.next) << "Origin: " << origin;

	p.search(&p, records+0);
	EXPECT_EQ(expected_record, p.next) << "Origin: " << origin;

	p.search(&p, records+3);
	EXPECT_EQ(expected_record, p.next) << "Origin: " << origin;

	p.search(&p, records+selected_record);
	EXPECT_EQ(expected_record, p.next) << "Origin: " << origin;
}

TEST(Turno, find_next_player)
{
	FindNextPlayer p{3};

	SPLAYER players[] = {
		{.numjug=-1, .is_player=false, .estado=PLAYER_STATUS_FICHAS2},
		{.numjug=1, .is_player=true, .estado=PLAYER_STATUS_FICHAS2},
		{.numjug=2, .is_player=true, .estado=PLAYER_STATUS_FICHAS2},
		{.numjug=3, .is_player=true, .estado=PLAYER_STATUS_FICHAS2},
		{.numjug=4, .is_player=true, .estado=PLAYER_STATUS_FICHAS2},
		{.numjug=5, .is_player=true, .estado=PLAYER_STATUS_FICHAS2},
		{.numjug=6, .is_player=true, .estado=PLAYER_STATUS_FICHAS2},
	};

	turn_sub_check(p, players, 0, nullptr, __LINE__);
	turn_sub_check(p, players, 3, nullptr, __LINE__);
	turn_sub_check(p, players, 2, players+2, __LINE__);
	turn_sub_check(p, players, 1, players+1, __LINE__);
	turn_sub_check(p, players, 2, players+1, __LINE__);

	turn_sub_check(p, players, 3, players+1, __LINE__);

	turn_sub_check(p, players, 6, players+6, __LINE__);
	turn_sub_check(p, players, 2, players+6, __LINE__);
	turn_sub_check(p, players, 4, players+4, __LINE__);
	turn_sub_check(p, players, 5, players+4, __LINE__);
	turn_sub_check(p, players, 6, players+4, __LINE__);
}

}
