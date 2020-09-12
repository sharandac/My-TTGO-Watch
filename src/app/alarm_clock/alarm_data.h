/****************************************************************************
 *   Copyright  2020  Jakub Vesely
 *   Email: jakub_vesely@seznam.cz
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#pragma once
#include <stdint.h>

void alarm_set_term(uint8_t hour, uint8_t minute);
uint8_t alarm_get_hour();
uint8_t alarm_get_minute();
void alarm_set_enabled(bool enable);
bool alarm_is_enabled();
void alarm_set_vibe_allowed(bool _vibe);
bool alarm_is_vibe_allowed();
void alarm_set_fade_allowed(bool _fade);
bool alarm_is_fade_allowed();
bool alarm_is_time();
void alarm_setup();
void alarm_data_store_data(bool enabled);
