/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
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
/**
 * @file turno.c
 * Manejo de turnos
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../common/missions.h"
#include "server.h"
#include "tegdebug.h"
#include "missions.h"

#define TURNO_DEBUG PDEBUG

namespace teg::server
{

void FindNextPlayer::search(void* user, SPLAYER *player)
{
	auto &self = *static_cast<FindNextPlayer*>(user);

	if((player->numjug != self.current)
	        && player_is_playing(player)) {
		if(self.next) {
			const auto nn = self.next->numjug;
			const auto pn = player->numjug;
			if(nn > pn) {
				/* The current selected next player candidate number is bigger
				 * than the player number in the list ->
				 * check if both numbers are either above or below the active
				 * player number. If so, set the next player candidate to this
				 * record */
				if(((pn>self.current) && (nn>self.current))
				        || ((pn<self.current) && (nn<self.current))) {
					self.next = player;
				}
			} else {
				/* the player record number is larger than the selected next
				 * player. The only valid case for reassignment here is, if the
				 * checked player record is above the active player, and the
				 * currently selected candidate is below. This is the special
				 * case, where a wrap around of the player numbers happened, and
				 * the candidate has to be assigned to the higher number. */
				if((pn>self.current) && (nn<self.current)) {
					self.next = player;
				}
			}
		} else {
			self.next = player;
		}
	}
}

/* Gives turn to the next player */
TEG_STATUS turno_2nextplayer(PSPLAYER *ppJ)
{
	FindNextPlayer p{unsigned((**ppJ).numjug)};
	player_map(&p, FindNextPlayer::search);

	if(p.next) {
		*ppJ = p.next;
		return TEG_STATUS_SUCCESS;
	} else {
		con_text_out_wop(M_ERR, "Abnormal error in turno_2nextplayer\n");
		return TEG_STATUS_PLAYERNOTFOUND;
	}
}

/* Ends the player turn */
TEG_STATUS turno_end(PSPLAYER pJ)
{
	assert(pJ);

	/* FIXME: Creo que igual este chequeo no sirve, porque se chequea cuando
	 * se conquista un country. Solo sirve si uno gana sin conquistar algun country
	 * Puede pasar ???
	 */
	if(mission_chequear(pJ) == TEG_STATUS_GAMEOVER) {
		game_end(pJ);
		return TEG_STATUS_GAMEOVER;
	}

	player_clear_turn(pJ);
	return TEG_STATUS_SUCCESS;
}

/* Gives turn to the next player */
TEG_STATUS turno_next(void)
{
	assert(g_game.turno);

	/* give turn to the next one */
	if(turno_2nextplayer(&g_game.turno) == TEG_STATUS_SUCCESS) {

		/* the one who has the turn is the one that started ? */
		if(turno_is_round_complete()) {

			turno_initialize_new_round();

			aux_token_fichasc(g_game.turno);

		} else {
			g_game.turno->estado = PLAYER_STATUS_ATAQUE;
			netall_printf(TOKEN_TURNO"=%d\n", g_game.turno->numjug);
		}
		return TEG_STATUS_SUCCESS;
	}

	return TEG_STATUS_ERROR;
}

/* initializes the round. Called before the game starts */
TEG_STATUS turno_init(void)
{
	int i;
	int real_i;
	PSPLAYER j;

	i = random_between(0, g_game.playing-1);

	player_from_indice(i, &real_i);
	if(player_whois(real_i, &j) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_ERROR;
	}

	g_game.old_turn = NULL;
	g_game.turno = j;
	g_game.empieza_turno = j;

	return TEG_STATUS_SUCCESS;
}

/* return true if the round is complete */
bool turno_is_round_complete()
{
	/* I want to know if the round is over. It is not enought to know
	 * if newturn == started because if a player with the turn exit the game
	 * he will never receive the turn again, but the started turn will point
	 * to him
	 */

	if(g_game.old_turn == NULL) {
		return false;
	}

	if(g_game.empieza_turno == g_game.turno) {
		return true;
	}

	struct FindOpenTurns {
		bool someone_has_the_turn = false;
	} turns;

	player_map(&turns, [](void* user, SPLAYER* player) {
		auto& turns=*static_cast<FindOpenTurns*>(user);
		turns.someone_has_the_turn |= player == g_game.turno;
	});

	return turns.someone_has_the_turn;

}

/* initialize variables for the new round */
void turno_initialize_new_round(void)
{
	/* So, a new round is started */
	turno_2nextplayer(&g_game.empieza_turno);
	g_game.turno = g_game.empieza_turno;

	g_game.round_number++;

	player_map(nullptr, [](void*, SPLAYER* player) {
		if(player->is_player) {
			unsigned long conts;
			player_listar_conts(player, &conts);
			int i;
			for(i=0; i<CONT_CANT; i++) {
				if(conts & 1) {
					player->player_stats.continents_turn[i]++;
				}
				conts >>= 1;
			}
			/* update the score */
			stats_score(&player->player_stats, g_conts);
		}
	});

	netall_printf(TOKEN_NEW_ROUND"=%d,%d\n", g_game.turno->numjug, g_game.round_number);

}

}
