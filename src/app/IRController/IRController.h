/****************************************************************************
 *   Aug 3 12:17:11 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
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
#ifndef _IRCONTROLLER_H
    #define _IRCONTROLLER_H

    #ifdef NATIVE_64BIT
        void IRController_setup( void );
    #else
        #if defined( M5PAPER ) || defined( M5CORE2 ) || defined( LILYGO_WATCH_2021 )    
            void IRController_setup( void );
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            enum IRControlSettingsAction {
                Ignore,
                Load,
                Save
            };

            struct InfraButton;

            void IRController_setup( void );
            void IRController_build_UI(IRControlSettingsAction settingsAction);
            void execute_ir_cmd(InfraButton* config);
        #else
            /**
             * NEW_HARDWARE_TAG or not defined
             */
            #warning "No destination hardware defined"
        #endif
    #endif
#endif // _IRCONTROLLER_H
