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

#include "BlueZStack.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sdp_lib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <btCommon.h>

#include <midp_logging.h>

/** @def BLUEZ_TIMEOUT default timeout value for blocking bluez calls */
#define BLUEZ_TIMEOUT 1000

/** @def PERROR Prints diagnostic message. */
#define PERROR(msg) \
    REPORT_WARN2(0, "%s: %s", msg, strerror(errno))

int CBlueZStack::g_devId = -1;

CBlueZStack::CBlueZStack():
CGenericBluetoothStack(), m_fd(-1)
{
    if (g_devId < 0) {
        // retrieve local bluetooth address from environment variable
        const char *addr = getenv("JSR82BTADDR");
        if (addr != NULL) {
            g_devId = hci_devid(addr);
        } else {
            g_devId = hci_get_route(NULL);
        }
        if (g_devId < 0) {
            return;
        }
        hci_dev_info di;
        hci_devinfo(g_devId, &di);
    }
    m_fd = hci_open_dev(g_devId);
    if (m_fd < 0) {
        return;
    }
    fcntl(m_fd, F_SETFL, O_NONBLOCK);
    hci_filter flt;
    hci_filter_clear(&flt);
    hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
    hci_filter_set_event(EVT_CMD_COMPLETE, &flt);
    hci_filter_set_event(EVT_CMD_STATUS, &flt);
    hci_filter_set_event(EVT_INQUIRY_COMPLETE, &flt);
    hci_filter_set_event(EVT_INQUIRY_RESULT, &flt);
    hci_filter_set_event(EVT_INQUIRY_RESULT, &flt);
    hci_filter_set_event(EVT_REMOTE_NAME_REQ_COMPLETE, &flt);
    hci_filter_set_event(EVT_AUTH_COMPLETE, &flt);
    hci_filter_set_event(EVT_ENCRYPT_CHANGE, &flt);
    if (setsockopt(m_fd, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0) {
        return;
    }
    m_poll.fd = m_fd;
    m_poll.events = POLLIN | POLLHUP | POLLERR;
    m_poll.revents = 0;
    m_bInitialized = true;
}

CBlueZStack::~CBlueZStack()
{
    if (m_fd >= 0) {
        hci_close_dev(m_fd);
    }
}

bool CBlueZStack::IsEnabled()
{
    struct hci_dev_info di;
    if (hci_devinfo(g_devId, &di) < 0) {
        return false;
    }
    return hci_test_bit(HCI_UP, &(di.flags));
}

bool CBlueZStack::Enable()
{
    int sk = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (sk < 0) {
        return false;
    }

    /*
     * Due a particular implementation
     * device can not be upped if it's in down state.
     * On the 'ioctl' call as well as on 'hciconfig hci0 up' console command
     * it sends 'HCI Command: Read Local Supported Features (0x04|0x0003)'
     * and failed with 'Connection timed out (110)'.
     * If device is already up, the 'ioctl' correctly returns EALREADY.
     */

    /*
     * Note: root permissions are required to set power on otherwise
     * EPERM (Operation not permitted) error is raised.
     */
    if (ioctl(sk, HCIDEVUP, g_devId) < 0) {
      if (errno != EALREADY) {
          close(sk);
          return false;
      }
    }

    close(sk);
    return true;
}

const char *CBlueZStack::GetLocalAddress()
{
    static char address[13];
    bdaddr_t ba;
    if (hci_read_bd_addr(m_fd, &ba, BLUEZ_TIMEOUT) < 0) {
        return NULL;
    }
    sprintf(address, "%02X%02X%02X%02X%02X%02X", ba.b[5], ba.b[4], ba.b[3],
            ba.b[2], ba.b[1], ba.b[0]);
    return address;
}

const char *CBlueZStack::GetLocalName(char *name, int size)
{
    if (hci_read_local_name(m_fd, size, name, 0) < 0) {
        PERROR("error reading local name");
        return NULL;
    }
    return name;
}

int CBlueZStack::GetDeviceClass()
{
    unsigned char cls[3];
    if (hci_read_class_of_dev(m_fd, cls, BLUEZ_TIMEOUT) < 0) {
        PERROR("error reading device class");
        return -1;
    }
    return (cls[0] << 16) | (cls[1] << 8) | cls[2];
}

bool CBlueZStack::SetServiceClasses(int classes)
{
    unsigned char cls[3];
    if (hci_read_class_of_dev(m_fd, cls, BLUEZ_TIMEOUT) < 0) {
        PERROR("error reading device class");
        return false;
    }
    classes |= cls[1] << 8;
    classes |= cls[2];
    if (hci_write_class_of_dev(m_fd, classes, BLUEZ_TIMEOUT) < 0) {
        PERROR("error writing device class");
        return false;
    }
    return true;
}

int CBlueZStack::GetAccessCode()
{
    unsigned char iac[3];
    unsigned char num = 1;
    if (hci_read_current_iac_lap(m_fd, &num, iac, BLUEZ_TIMEOUT) < 0) {
        PERROR("error reading access code");
        return -1;
    }
    return (iac[0] << 16) | (iac[1] << 8) | iac[2];
}

bool CBlueZStack::SetAccessCode(int accessCode)
{
    unsigned char iac[3];
    iac[0] = (accessCode & 0xff0000) >> 16;
    iac[1] = (accessCode & 0x00ff00) >> 8;
    iac[2] = accessCode & 0x0000ff;
    if (hci_write_current_iac_lap(m_fd, 1, iac, BLUEZ_TIMEOUT) < 0) {
        PERROR("error writing access code");
        return false;
    }
    return true;
}

bool CBlueZStack::CheckEvents()
{
    poll(&m_poll, 1, 0);
    if (!m_poll.revents) {
        return false;
    }
    m_poll.revents = 0;
    return true;
}

int CBlueZStack::ReadData(void *data, unsigned short len)
{
    int nread = read(m_fd, data, len);
    if (nread < 0 || nread == 0) {
        return nread;
    }
    memmove(data, (unsigned char *)data + 1, --nread);
    return nread;
}

bool CBlueZStack::SendHCICommand(unsigned short ogf, unsigned short ocf,
        unsigned char len, const void *param)
{
    return hci_send_cmd(m_fd, ogf, ocf, len, (void *)param) >= 0;
}

bool CBlueZStack::WaitHCICommandComplete()
{
    struct {
        uint8_t event_code, param_length;
        uint8_t ncmd;
        uint16_t opcode;
    } __PPACKED__ event;
    while (!CheckEvents()) usleep(10);
    ReadData(&event, sizeof(event));
    return event.event_code == 0x0e;
}

bool CBlueZStack::WaitHCICommandStatus()
{
    struct {
        uint8_t event_code, param_length;
        uint8_t status, ncmd;
        uint16_t opcode;
    } __PPACKED__ event;
    while (!CheckEvents()) usleep(10);
    ReadData(&event, sizeof(event));
    return event.event_code == 0x0f && event.status == 0;
}
