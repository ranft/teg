/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2002 Ricardo Quesada
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
/* Fog of War */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "../common/net.h"
#include "server.h"
#include "fow.h"

namespace teg::server
{

TEG_STATUS fow_set_mode(bool b)
{
	if(JUEGO_EMPEZADO) {
		return TEG_STATUS_ERROR;
	}

	if(b!=0) {
		b=1;
	}
	g_game.fog_of_war = b;
	return TEG_STATUS_SUCCESS;
}

bool fow_can_player_see_country(PSPLAYER pJ, PCOUNTRY pP)
{
	PLIST_ENTRY pL;
	PCOUNTRY pcountry;

	if(! pJ  || ! pP) {
		return false;
	}

	pL = pJ->countries.Flink;
	while(!IsListEmpty(&pJ->countries) && (pL != &pJ->countries)) {
		pcountry = (PCOUNTRY) pL;

		if(countries_eslimitrofe(pP->id, pcountry->id)) {
			return true;
		}

		if(pP->id == pcountry->id) {
			return true;
		}

		pL = LIST_NEXT(pL);
	}

	return false;
}

TEG_STATUS fow_fill_with_boundaries(int country, char *buffer, int buf_len)
{
	int i;
	int first_time=1;

	if(! g_game.player_fow) {
		return TEG_STATUS_ERROR;
	}

	for(i=0; i < COUNTRIES_CANT; i++) {
		if(i != country && g_countries[i].numjug != g_game.player_fow->numjug && countries_eslimitrofe(country, i)) {
			char buf[1024];

			if(first_time) {
				snprintf(buf, sizeof(buf)-1, "%s=%d,%d,%d", TOKEN_COUNTRY, i, g_countries[i].numjug, g_countries[i].ejercitos);
				first_time = 0;
			} else {
				snprintf(buf, sizeof(buf)-1, ";%s=%d,%d,%d", TOKEN_COUNTRY, i, g_countries[i].numjug, g_countries[i].ejercitos);
			}

			strncat(buffer, buf, buf_len);
		}
	}


	/* dont send, since I didnt modify the buffer */
	if(first_time) {
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
}

int fow_netall_printf(int country, char const *format, ...)
{
	if(country < 0 || country >= COUNTRIES_CANT) {
		return -1;
	}

	struct Transmitter {
		char buf[PROT_MAX_LEN];
		int const country;
	} tx{.country = country};

	va_list args;
	va_start(args, format);
	vsnprintf(tx.buf, sizeof(tx.buf) -1, format, args);
	va_end(args);

	tx.buf[sizeof(tx.buf)-1] = 0;


	player_map(&country, [](void* user, SPLAYER* pJ) {
		auto &tx = *static_cast<Transmitter*>(user);
		if(pJ->fd > 0 && pJ->is_player) {
			PLIST_ENTRY	pLcountry = pJ->countries.Flink;

			/* check if he has a boundry country wih 'country' */
			while(!IsListEmpty(&pJ->countries) && (pLcountry != &pJ->countries)) {
				PCOUNTRY pP = (PCOUNTRY) pLcountry;

				if(countries_eslimitrofe(pP->id, tx.country)
				        || (pP->id == tx.country)) {
					net_print(pJ->fd, tx.buf);
					break;
				}

				pLcountry = LIST_NEXT(pLcountry);
			}

		}
	});
	return 0;
}

/*
 * IMPORTANT: it is very important the user can't control the format
 * in this function to prevent a format string vulnerability
 *
 * After format is parsed, 2 "%d" should be present, because they will
 * be filled with src & dst
 */
int fow_2_netall_printf(int src, int dst, char const *format, ...)
{
	if(src<0 || src>=COUNTRIES_CANT || dst<0 || dst>=COUNTRIES_CANT) {
		return -1;
	}

	char buf[PROT_MAX_LEN];
	va_list args;
	va_start(args, format);
	vsnprintf(buf, sizeof(buf) -1, format, args);
	va_end(args);
	buf[sizeof(buf)-1] = 0;

	struct Transmitter {
		int src, dst;
		char const*const buf;
	} tx{.src=src, .dst=dst, .buf=buf};

	player_map(&tx, [](void* user, SPLAYER *pJ) {
		auto &tx = *static_cast<Transmitter*>(user);
		if(pJ->fd > 0 && pJ->is_player) {
			PLIST_ENTRY pLcountry = pJ->countries.Flink;
			/* check if he has a boundry country wih src && dst */

			int src_country = -1;
			int dst_country = -1;
			while(!IsListEmpty(&pJ->countries) && (pLcountry != &pJ->countries)) {
				PCOUNTRY pP = (PCOUNTRY) pLcountry;

				if(countries_eslimitrofe(pP->id, tx.src) || pP->id == tx.src) {
					src_country = tx.src;
				}

				if(countries_eslimitrofe(pP->id, tx.dst) || pP->id == tx.dst) {
					dst_country = tx.dst;
				}

				if(dst_country >= 0 && src_country >=0) {
					break;
				}

				pLcountry = LIST_NEXT(pLcountry);
			}

			net_printf(pJ->fd, tx.buf, src_country, dst_country);
		}
	});
	return 0;
}

}
