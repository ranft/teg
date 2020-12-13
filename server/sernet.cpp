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

#include "sernet.h"

#include <stdio.h>
#include <stdarg.h>

#include "../common/net.h"
#include "../common/protocol.h"
#include "player.h"

namespace teg::server
{

void netall_printf(char const *format, ...)
{
	va_list args;
	char buf[PROT_MAX_LEN];

	va_start(args, format);
	vsnprintf(buf, sizeof(buf) -1, format, args);
	va_end(args);

	buf[ sizeof(buf) -1 ] = 0;

	player_map(buf, [](void* user, SPLAYER* player) {
		char const* tx = static_cast<char const*>(user);
		if(player->fd > 0) {
			net_print(player->fd, tx);
		}
	});
}

}
