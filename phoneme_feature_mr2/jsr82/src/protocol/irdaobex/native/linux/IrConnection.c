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

#include "IrConnection.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <linux/irda.h>
#include <fcntl.h>
#include <sys/poll.h>

#include <midp_logging.h>

#define DISC_BUF_LEN (sizeof(struct irda_device_list) + \
    sizeof(struct irda_device_info) * MAX_DEVICES)

/** @def PERROR Prints diagnostic message. */
#define PERROR(msg) \
    REPORT_WARN2(0, "%s: %s", msg, strerror(errno))

/**
 * Creates a new connection handle. Only resources are allocated, and no
 * connection is established by this function.
 *
 * @param handle variable to receive a newly created handle
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure
 */
ir_result_t ir_create(ir_handle_t *handle)
{
    int sockfd = socket(AF_IRDA, SOCK_STREAM, 0);
    if (sockfd == INVALID_IR_HANDLE) {
        PERROR("socket");
        return IR_ERROR;
    }
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK)) {
        PERROR("fcntl");
        close(sockfd);
        return IR_ERROR;
    }
    *handle = sockfd;
    return IR_SUCCESS;
}

/**
 * Discovers nearby infrared devices.
 *
 * @param hints hint bits used for filtering
 * @param ias IAS class name used for filtering
 * @param addr array to receive addresses of discovered devices
 * @param count contains the number of entries in the <code>addr</code> array
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure
 */
ir_result_t ir_discover(ir_hints_t hints, ir_ias_t ias,
        ir_addr_t *addr, size_t *count)
{
    int i, sockfd, len;
    struct irda_device_list *list;
    static char disc[DISC_BUF_LEN];
    struct irda_ias_set ias_set;
    unsigned char hbits[4];

    sockfd = socket(AF_IRDA, SOCK_STREAM, 0);
    if (sockfd == -1) {
        PERROR("socket");
        return IR_ERROR;
    }
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK)) {
        PERROR("fcntl");
        close(sockfd);
        return IR_ERROR;
    }
    hbits[0] = hints;
    hbits[1] = hints >> 8;
    hbits[2] = hints >> 16;
    hbits[3] = hints >> 24;
    if (hbits[3]) hbits[2] |= 0x80;
    if (hbits[2]) hbits[1] |= 0x80;
    if (hbits[1]) hbits[0] |= 0x80;
    if (setsockopt(sockfd, SOL_IRLMP, IRLMP_HINT_MASK_SET, hbits,
            sizeof(hbits))) {
        PERROR("setsockopt");
        close(sockfd);
        return IR_ERROR;
    }
    list = (struct irda_device_list *)disc;
    len = sizeof(disc);
    if (getsockopt(sockfd, SOL_IRLMP, IRLMP_ENUMDEVICES,
            disc, &len)) {
        PERROR("getsockopt");
        close(sockfd);
        return IR_ERROR;
    }
    if (len == 0 || list->len == 0) {
        close(sockfd);
        *count = 0;
        return IR_SUCCESS;
    }
    strncpy(ias_set.irda_class_name, (const char *)ias, IAS_MAX_CLASSNAME);
    ias_set.irda_class_name[IAS_MAX_CLASSNAME + 1] = 0;
    strcpy(ias_set.irda_attrib_name, "IrDA:TinyTP:LsapSel");
    len = 0;
    for (i = 0; i < (int)list->len; ++i) {
        int daddr = list->dev[i].daddr;
        char *dhints = list->dev[i].hints;
        if ((dhints[0] & hbits[0]) != hbits[0] ||
                (dhints[1] & hbits[1]) != hbits[1]) {
            continue;
        }
        len = sizeof(ias);
        ias_set.daddr = daddr;
        if (getsockopt(sockfd, SOL_IRLMP, IRLMP_IAS_QUERY,
                &ias_set, &len)) {
            continue;
        }
        *addr++ = daddr;
        if (++len >= (int)*count) {
            break;
        }
    }
    *count = len;
    close(sockfd);
    return IR_SUCCESS;
}

/**
 * Opens connection to a remote device.
 *
 * @param handle connection handle to be associated with a connection
 * @param addr address of a remote device
 * @param ias IAS class name
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure,
 *         <code>IR_INPROGRESS</code> if the operation was not completed
 */
ir_result_t ir_connect(ir_handle_t handle, ir_addr_t addr, ir_ias_t ias)
{
    struct sockaddr_irda sa;
    sa.sir_family = AF_IRDA;
    sa.sir_addr = addr;
    strncpy(sa.sir_name, (const char *)ias, 24);
    sa.sir_name[24] = 0;
    if (connect(handle, (struct sockaddr *)&sa, sizeof(sa))) {
        if (errno == EINPROGRESS || errno == EALREADY) {
            return IR_INPROGRESS;
        } else {
            PERROR("connect");
            return IR_ERROR;
        }
    }
    return IR_SUCCESS;
}

/**
 * Completes opening connection to a remote device.
 *
 * @param handle connection handle to be associated with a connection
 * @param addr address of a remote device
 * @param ias IAS class name
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure,
 *         <code>IR_AGAIN</code> if the operation needs to be repeated
 */
ir_result_t ir_connect_complete(ir_handle_t handle)
{
    int ret, opt, len;
    struct pollfd pfd;
    pfd.fd = handle;
    pfd.events = POLLOUT | POLLERR;
    ret = poll(&pfd, 1, 10);
    if (ret < 0) {
        PERROR("poll");
        return IR_ERROR;
    }
    if (ret == 0) {
        return IR_AGAIN;
    }
    if (pfd.revents & POLLERR) {
        return IR_ERROR;
    }
    /* Before the call, it's good to supply in 'len' the size of 'opt' */
    len = sizeof(opt);
    if (getsockopt(handle, SOL_SOCKET, SO_ERROR, &opt, &len)) {
        PERROR("getsockopt");
        return IR_ERROR;
    }
    if (opt != 0) {
        errno = opt;
        PERROR("getsockopt unsuccessful");
        return IR_ERROR;
    }
    return IR_SUCCESS;
}

/**
 * Closes connection associated with a connection handle and releases
 * resources associated with the handle.
 *
 * @param handle connection handle
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure
 */
ir_result_t ir_close(ir_handle_t handle)
{
    close(handle);
    return IR_SUCCESS;
}

/**
 * Receives data from an open connection.
 *
 * @param handle handle to an open connection
 * @param buf buffer to receive the data
 * @param count number of bytes to be received / actually received
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure,
 *         <code>IR_AGAIN</code> if the operation needs to be repeated
 */
ir_result_t ir_receive(ir_handle_t handle, void *buf, size_t *count)
{
    size_t len = 0;
    while (len < *count) {
        ssize_t ret = read(handle, (char *)buf + len, *count - len);
        if (ret == -1) {
            if (errno == EAGAIN) {
                break;
            }
            PERROR("read");
            return IR_ERROR;
        }
        if (ret == 0) {
            break;
        }
        len += ret;
    }
    *count = len;
    return IR_SUCCESS;
}

/**
 * Transmits data over an open connection.
 *
 * @param handle handle to an open connection
 * @param buf buffer containing the data to send
 * @param count number of bytes to be sent / actually sent
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure,
 *         <code>IR_AGAIN</code> if the operation needs to be repeated
 */
ir_result_t ir_send(ir_handle_t handle, const void *buf, size_t *count)
{
    size_t len = 0;
    while (len < *count) {
        ssize_t ret = write(handle, (const char *)buf + len, *count - len);
        if (ret == -1) {
            if (errno == EAGAIN) {
                break;
            }
            PERROR("write");
            return IR_ERROR;
        }
        if (ret == 0) {
            break;
        }
        len += ret;
    }
    *count = len;
    return IR_SUCCESS;
}

/**
 * Prepares to listen for incoming connections.
 *
 * @param handle connection handle
 * @param hints hint bits to be advertised
 * @param ias IAS class name
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure
 */
ir_result_t ir_listen(ir_handle_t handle, ir_hints_t hints, ir_ias_t ias)
{
    struct sockaddr_irda sa;
    unsigned char hbits[4];

    sa.sir_family = AF_IRDA;
    sa.sir_addr = 0;
    sa.sir_lsap_sel = LSAP_ANY;
    strncpy(sa.sir_name, (const char *)ias, 24);
    sa.sir_name[24] = 0;
    if (bind(handle, (struct sockaddr *)&sa, sizeof(sa))) {
        PERROR("bind");
        close(handle);
        return IR_ERROR;
    }
    hbits[0] = hints;
    hbits[1] = hints >> 8;
    hbits[2] = hints >> 16;
    hbits[3] = hints >> 24;
    if (hbits[3]) hbits[2] |= 0x80;
    if (hbits[2]) hbits[1] |= 0x80;
    if (hbits[1]) hbits[0] |= 0x80;
    if (setsockopt(handle, SOL_IRLMP, IRLMP_HINTS_SET, hbits, sizeof(hbits))) {
        PERROR("setsockopt");
        close(handle);
        return IR_ERROR;
    }
    if (listen(handle, 1)) {
        PERROR("listen");
        close(handle);
        return IR_ERROR;
    }
    return IR_SUCCESS;
}

/**
 * Accepts an incoming connection.
 *
 * @param handle connection handle in listening state
 * @param peer variable to receive peer connection handle
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure,
 *         <code>IR_AGAIN</code> if the operation needs to be repeated
 */
ir_result_t ir_accept(ir_handle_t handle, ir_handle_t *peer)
{
    int peerfd;
    struct sockaddr_irda sa;
    socklen_t sl = sizeof(sa);
    peerfd = accept(handle, (struct sockaddr *)&sa, &sl);
    if (peerfd == -1) {
        if (errno == EAGAIN) {
            return IR_AGAIN;
        } else {
            PERROR("accept");
            return IR_ERROR;
        }
    }
    if (fcntl(peerfd, F_SETFL, fcntl(peerfd, F_GETFL) | O_NONBLOCK)) {
        PERROR("fcntl");
        close(peerfd);
        return IR_ERROR;
    }
    *peer = peerfd;
    return IR_SUCCESS;
}
