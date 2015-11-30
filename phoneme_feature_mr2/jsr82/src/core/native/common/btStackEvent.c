/*
 *   
 *
 * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */

#include "btStackEvent.h"

#ifdef BT_USE_EVENT_API

#include <btCommon.h>
#include <string.h>

#define EVT_INQUIRY_COMPLETE        0x01
#define EVT_INQUIRY_RESULT          0x02
#define EVT_AUTHENTICATION_COMPLETE 0x06
#define EVT_REMOTE_NAME_COMPLETE    0x07
#define EVT_ENCRYPTION_CHANGE       0x08

#define MAX_INQUIRY_RESPONSES 18

#define MAX_HCI_QUEUE_SIZE 4
#define MAX_HCI_EVENT_SIZE 257

static unsigned char hci_queue[MAX_HCI_QUEUE_SIZE][MAX_HCI_EVENT_SIZE];
static int hci_queue_head, hci_queue_tail, hci_queue_size;

bt_result_t bt_stack_check_events(bt_bool_t *retval)
{
    *retval = hci_queue_size > 0 ? BT_BOOL_TRUE : BT_BOOL_FALSE;
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_stack_read_data(void *data, int len, int *retval)
{
    void *event;
    int size;
    if (hci_queue_size == 0) {
        return BT_RESULT_FAILURE;
    }
    event = hci_queue[hci_queue_head++];
    if (hci_queue_head == MAX_HCI_QUEUE_SIZE) {
        hci_queue_head = 0;
    }
    size = *((uint8_t *)event + 1) + 2;
    if (size > len) {
        size = len;
    }
    memcpy(data, event, size);
    *retval = size;
    hci_queue_size--;
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_stack_on_hci_event(void *event)
{
    if (hci_queue_size >= MAX_HCI_QUEUE_SIZE) {
        return BT_RESULT_FAILURE;
    }
    memcpy(hci_queue[hci_queue_tail++], event, *((uint8_t *)event + 1) + 2);
    if (hci_queue_tail == MAX_HCI_QUEUE_SIZE) {
        hci_queue_tail = 0;
    }
    hci_queue_size++;
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_stack_on_inquiry_complete(bt_bool_t success)
{
    struct {
        uint8_t event_type;
        uint8_t param_length;
        uint8_t status;
    }  __PPACKED__ event;
    event.event_type = EVT_INQUIRY_COMPLETE;
    event.param_length = sizeof(event) - 2;
    event.status = success == BT_BOOL_TRUE ? 0x00 : 0xff;
    return bt_stack_on_hci_event(&event);
}

bt_result_t bt_stack_on_inquiry_result(bt_inquiry_t result[], int count)
{
    typedef struct {
        bt_bdaddr_t bdaddr;
        uint8_t pscan_rep_mode;
        uint8_t pscan_period_mode;
        uint8_t pscan_mode;
        uint8_t dev_class[3];
        uint16_t clock_offset;
    } __PPACKED__ inquiry_response;
    struct {
        uint8_t event_type;
        uint8_t param_length;
        uint8_t num_responses;
        inquiry_response response[MAX_INQUIRY_RESPONSES];
    } __PPACKED__ event;
    int i;
    event.event_type = EVT_INQUIRY_RESULT;
    event.param_length = 1 + count * sizeof(inquiry_response);
    event.num_responses = count;
    for (i = 0; i < count; i++) {
        bt_inquiry_t *src = &result[i];
        inquiry_response *dst = &event.response[i];
        memcpy(dst->bdaddr, src->bdaddr, BT_ADDRESS_SIZE);
        dst->pscan_rep_mode = 0;
        dst->pscan_period_mode = 0;
        dst->pscan_mode = 0;
        dst->dev_class[0] = src->cod >> 16;
        dst->dev_class[1] = src->cod >> 8;
        dst->dev_class[2] = src->cod;
        dst->clock_offset = 0;
    }
    return bt_stack_on_hci_event(&event);
}

bt_result_t bt_stack_on_authentication_complete(int handle, bt_bool_t success)
{
    struct {
        uint8_t event_type;
        uint8_t param_length;
        uint8_t status;
        uint16_t handle;
    } __PPACKED__ event;
    event.event_type = EVT_AUTHENTICATION_COMPLETE;
    event.param_length = sizeof(event) - 2;
    event.status = success == BT_BOOL_TRUE ? 0x00 : 0xff;
    event.handle = handle;
    return bt_stack_on_hci_event(&event);
}

bt_result_t bt_stack_on_remote_name_complete(const bt_bdaddr_t bdaddr,
        const char *name)
{
    struct {
        uint8_t event_type;
        uint8_t param_length;
        uint8_t status;
        bt_bdaddr_t bdaddr;
        char name[248];
    } __PPACKED__ event;
    
    event.event_type = EVT_REMOTE_NAME_COMPLETE;
    event.param_length = sizeof(event) - 2;
    event.status = name != NULL ? 0x00 : 0xff;
    if (name != NULL) {
        memcpy(event.bdaddr, bdaddr, BT_ADDRESS_SIZE);
        strncpy(event.name, name, sizeof(event.name));
    }
    return bt_stack_on_hci_event(&event);
}

bt_result_t bt_stack_on_encryption_change(int handle, bt_bool_t success,
        bt_bool_t on)
{
    struct {
        uint8_t event_type;
        uint8_t param_length;
        uint8_t status;
        uint16_t handle;
        uint8_t encrypt;
    } __PPACKED__ event;
    event.event_type = EVT_ENCRYPTION_CHANGE;
    event.param_length = sizeof(event) - 2;
    event.status = success == BT_BOOL_TRUE ? 0x00 : 0xff;
    event.handle = handle;
    event.encrypt = on == BT_BOOL_TRUE ? 0x01 : 0x00;
    return bt_stack_on_hci_event(&event);
}

#endif /* BT_USE_EVENT_API */
