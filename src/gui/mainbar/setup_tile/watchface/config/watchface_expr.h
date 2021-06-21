
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
#ifndef _WATCHFACE_EXPR_H
#define _WATCHFACE_EXPR_H

#include "utils/tinyexpr/tinyexpr.h"
#include "time.h"

/**
 * Available variables
 */
extern te_variable watchface_expr_vars[];

/**
 * @brief Update values of global context
 */
void watchface_expr_update( tm &new_info );

/**
 * @brief compile the expression for later evaluation
 */
te_expr * watchface_expr_compile(const char* str, int *error);

/**
 * @brief evaluate a precompiled expression
 */
double watchface_expr_eval( te_expr *expr);

/**
 * @brief setup the watchface expression module
 */
void watchface_expr_setup( void );

#endif // _WATCHFACE_EXPR_H