/* (C) 2009 by Simon Busch <morphis@gravedo.de>
 *
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#ifndef GLOFIISH_H_
#define GLOFIISH_H_

#define GLOFIISH_M800_MODEM_RST S3C_GPA(15)
#define GLOFIISH_M800_MODEM_PWRON S3C_GPB(4)
#define GLOFIISH_M800_MODEM_GPA11 S3C_GPA(11)

struct glofiish_cpld_state;
struct glofiish_modem_state;

struct glofiish_cpld* glofiish_cpld_init(target_phys_addr_t base);
void glofiish_cpld_reset(struct glofiish_cpld_state *cpld);

struct CharDriverState* glofiish_modem_init();
void glofiish_modem_reset(struct glofiish_modem_state *s, int level);
void glofiish_modem_enable(struct CharDriverState *opaque, int enable);
void glofiish_modem_gpa11(struct glofiish_modem_state *s, int level);

#endif

