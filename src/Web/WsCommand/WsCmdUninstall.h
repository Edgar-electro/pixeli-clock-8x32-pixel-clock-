/* MIT License
 *
 * Copyright (c) 2019 - 2022 Andreas Merkle <web@blue-andi.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*******************************************************************************
    DESCRIPTION
*******************************************************************************/
/**
 * @brief  Websocket command to uninstall a plugin
 * @author Andreas Merkle <web@blue-andi.de>
 *
 * @addtogroup web
 *
 * @{
 */

#ifndef __WSCMDUNINSTALL_H__
#define __WSCMDUNINSTALL_H__

/******************************************************************************
 * Compile Switches
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "WsCmd.h"
#include "SlotList.h"

/******************************************************************************
 * Macros
 *****************************************************************************/

/******************************************************************************
 * Types and Classes
 *****************************************************************************/

/**
 * Websocket command to uninstall a plugin.
 */
class WsCmdUninstall: public WsCmd
{
public:

    /**
     * Constructs the websocket command.
     */
    WsCmdUninstall() :
        WsCmd("UNINSTALL"),
        m_isError(false),
        m_slotId(SlotList::SLOT_ID_INVALID)
    {
    }

    /**
     * Destroys websocket command.
     */
    ~WsCmdUninstall()
    {
    }

    /**
     * Execute command.
     *
     * @param[in] server    Websocket server
     * @param[in] client    Websocket client
     */
    void execute(AsyncWebSocket* server, AsyncWebSocketClient* client) final;

    /**
     * Set command parameter. Call this for each parameter, until executing it.
     *
     * @param[in] par   Parameter string
     */
    void setPar(const char* par) final;

private:

    bool    m_isError;  /**< Any error happened during parameter reception? */
    uint8_t m_slotId;   /**< Slot id, where the plugin is installed. */

    WsCmdUninstall(const WsCmdUninstall& cmd);
    WsCmdUninstall& operator=(const WsCmdUninstall& cmd);
};

/******************************************************************************
 * Functions
 *****************************************************************************/

#endif  /* __WSCMDUNINSTALL_H__ */

/** @} */