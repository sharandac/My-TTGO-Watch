/****************************************************************************
 *  FindPhone.h
 *  Neuroplant  11/2020
 *  Email: m.roych@gmail.com
 *
 *
 *  Based on the work of Dirk Brosswick,  sharandac / My-TTGO-Watch  Example_App"
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
#ifndef _FindPhone_H
    #define _FindPhone_H

//    #define EXAMPLE_WIDGET    // uncomment if an widget need

    void FindPhone_setup( void );
    //uint32_t FindPhone_get_app_setup_tile_num( void );//No use just yet
    uint32_t FindPhone_get_app_main_tile_num( void );

#endif // _FindPhone_H
