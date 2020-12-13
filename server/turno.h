/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2001 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@corest.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#pragma once

#include "../common/common.h"
#include "player.h"

namespace teg::server
{

/*! give turn to next player */
TEG_STATUS turno_next(void);

/*! initialize the turn */
TEG_STATUS turno_init(void);

/*! give the turn to the next player */
TEG_STATUS turno_2nextplayer(PSPLAYER *j);

/*! ends the turn for a player */
TEG_STATUS turno_end(PSPLAYER pJ);

/*! says if the round is completed */
bool turno_is_round_complete(void);

/*! called when a round is complete */
void turno_initialize_new_round(void);

struct FindNextPlayer {
	FindNextPlayer(unsigned current): current{current} {}
	SPLAYER* next = nullptr;
	unsigned const current;
	static void search(void* self, SPLAYER* player);
};

}
