/****************************************************************************
 *   Oct 05 23:37:31 2020
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
#ifndef _MSG_CHAIN_H
    #define _MSG_CHAIN_H

    #include <stdint.h>

    struct msg_chain_entry_t {
        msg_chain_entry_t *prev_msg;
        const char *msg;
        msg_chain_entry_t *next_msg;
    };

    struct msg_chain_t {
        int32_t entrys;
        msg_chain_entry_t *first_msg_chain_entry;
        int32_t current_entry;
        msg_chain_entry_t *current_msg_chain_entry;
    };

    /**
     * @brief add a message to the msg_chain
     * 
     * @param   msg_chain   pointer to the msg_chain, if NULL a new msg_chain is make. don't forget to save the return pointer
     * @param   msg         the message to store
     * 
     * @return  pointer     to the msg_chain structure, NULL if failed
     */
    msg_chain_t *msg_chain_add_msg( msg_chain_t *msg_chain, const char *msg );
    /**
     * @brief delete an msg from the msg_chain
     * 
     * @param   msg_chain   pointer to the msg_chain
     * @param   entry       entry number to delete
     * 
     * @return  true if success, false if failed
     */
    bool msg_chain_delete_msg_entry( msg_chain_t *msg_chain, int32_t entry );
    /**
     * @brief get an messges entry from the msg_chain
     * 
     * @param   msg_chain   pointer to the msg_chain
     * @param   entry       entry numger to get
     * 
     * @return  pointer to the messages of NULL if failed
     */
    const char* msg_chain_get_msg_entry( msg_chain_t *msg_chain, int32_t entry );
    /**
     * @brief get the number of entry in the msg_chain
     * 
     * @return  number of entrys
     */
    int32_t msg_chain_get_entrys( msg_chain_t *msg_chain );
    /**
     * @brief printf all messages from the chain
     * 
     * @param   msg_chain   pointer to the msg_chain
     */
    void msg_chain_printf_msg_chain( msg_chain_t *msg_chain );

#endif // _MSG_CHAIN_H