/*
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

/**
 * @file
 *
 * Simple implementation of wma UDP Emulator.
 * The messages supposed to be received from JSR205Tool.jar:
 */

#include "javacall_network.h"
#include "javacall_datagram.h"
#include "javacall_mms.h"
#include "javacall_logging.h"
#include "javacall_defs.h"

#include <string.h>
#include <stdio.h>

#include <windows.h>
#include "lcd.h" //WM_NETWORK

int getValue(const char* header, const char* fieldName, char* ptr1) {
    const char* ptr;
    int len, headerLen;

    if (header == NULL || fieldName == NULL || ptr1 == NULL) {
        return 0;
    }

    len = strlen(fieldName);
    headerLen = strlen(header);

    for (ptr = header; *ptr; ptr++) {
        if (len > headerLen--) {
            break;
        }

        if (0 == strncmp(fieldName, ptr, len)) {
            ptr += len;
            do { *ptr1++ = *ptr++; } while (*ptr && *ptr != ';');
            *ptr1 = 0;
            return 1;
        }
    }

    *ptr1 = 0;

    return 0;
}

int
getUTFField(int headerLen, const char* header, char* fieldName, char* ptr1,
            int ptr1BufSize) {
    int fieldNameLength;
    const char* ptr = header;

    if (fieldName == NULL || header == NULL || ptr1 == NULL) {
        return 0;
    }

    if (ptr1BufSize <= 1) {
        return 0;
    }
    ptr1BufSize--; /* 1 byte for terminating NULL */

    fieldNameLength = strlen(fieldName);

    for(; ptr < header + headerLen - fieldNameLength; ptr++) {
        if (0 == strncmp(ptr, fieldName, fieldNameLength)) {
            const unsigned char* p = (const unsigned char*)ptr;  
            int b1 = *(p+fieldNameLength);
            int b2 = *(p+fieldNameLength+1);
            int length = (b1 << 8) + b2; // java.io.DataInputStream.readUnsignedShort()

            if (length > ptr1BufSize) {
                length = ptr1BufSize;
            }
	    
	    if (fieldNameLength + 2 + length > headerLen) {
                length = headerLen - (fieldNameLength + 2);
		if (length < 0) {
		    length = 0;
		}
            }

            memcpy(ptr1, ptr + fieldNameLength + 2, length);
            ptr1[length] = 0;

	    return length;
        }
    }

    return 0;
}

/*
 *  Helper function. Extracts appID, replyToAppID and fromAddress from the header.
 *
 *  Note! These parameters seemed to be excessive in UDP header at all,
 *  the function should be moved to JSR code and javacall should not concern 
 *  about the content of the header.
 */
void parseHeader(int headerLen, const char* header,
                 char* appID, char* replyToAppID,
                 char* fromAddress) {
    char fromPhone[128]; 
    char contentType[256];

    if (header == NULL || appID == NULL || replyToAppID == NULL ||
            fromAddress == NULL) {
        return;
    }

    getUTFField(headerLen, header, "Content-Type", contentType,
                sizeof(contentType));
    getUTFField(headerLen, header, "From", fromPhone, sizeof(fromPhone));

    getValue(contentType, "Application-ID = ", appID);
    getValue(contentType, "Reply-To-Application-ID = ", replyToAppID);

    sprintf(fromAddress, "mms://%s%s%s", fromPhone, (*replyToAppID ? ":" : ""),
            replyToAppID);
}

/* temporary buffers for input and output SMS */
#define MMS_RECV_BUFF_LENGTH 50000
#define MMS_SEND_BUFF_LENGTH 50000

static char decode_mms_buffer[MMS_RECV_BUFF_LENGTH];
static char encode_mms_buffer[MMS_SEND_BUFF_LENGTH];

/* definitions for UDP packets */
#define PACKET_MAX_SIZE 1500
#define PACKET_HEADER_SIZE 10
#define PACKET_MAX_CAPACITY (PACKET_MAX_SIZE-PACKET_HEADER_SIZE)

static char recentPackNum = 0;

void decodeMmsBuffer(
        char** fromAddress, char** appID, char** replyToAppID,
        int* bodyLen, char** body) {
    char* ptr = decode_mms_buffer;

    if (fromAddress == NULL || appID == NULL || replyToAppID == NULL ||
            bodyLen == NULL || body == NULL) {
        return;
    }

    recentPackNum = 0;

    *fromAddress  = ptr;           while(*(ptr++) != 0);
    *appID        = ptr;           while(*(ptr++) != 0);
    *replyToAppID = ptr;           while(*(ptr++) != 0);
    *bodyLen      = *((int*)ptr);  ptr += sizeof(int);
    *body         = ptr;           ptr += *bodyLen;

    if (ptr - decode_mms_buffer >= MMS_RECV_BUFF_LENGTH) {
        javacall_print("!!! ERROR: decodeMmsBuffer(): invalid packet!\n");
    }
}

int recvMMS_readPacket(char *ptr, int bytesRead) {
    short packNum;
    short numPackets;
    //short count;
    //int totalLen;

    if (ptr == NULL) {
        return 0;
    }

    packNum     = *((short*) (ptr)  );
    numPackets  = *((short*) (ptr+2));
    //count       = *((short*) (ptr+4));
    //totalLen    = *((int*)   (ptr+6));

    if (++recentPackNum != packNum) {
        javacall_print("### WARNING! UDP packet was lost!\n");
    }

    if (PACKET_MAX_CAPACITY*(packNum-1) + (bytesRead-PACKET_HEADER_SIZE) >=
            MMS_RECV_BUFF_LENGTH) {
        javacall_print("!!! ERROR: recvMMS_readPacket(): "
                       "invalid offset in packet\n");
        return 0;
    }

    memcpy(decode_mms_buffer + (PACKET_MAX_CAPACITY*(packNum-1)), ptr+PACKET_HEADER_SIZE, bytesRead-PACKET_HEADER_SIZE);
    //printf("recv packNum=%i numPackets=%i bytesRead=%i\n", (int) packNum, (int) numPackets, bytesRead);

    return (packNum == numPackets);
}

/* temporary buffer for output datagram packet */
static char datagram_buffer[PACKET_MAX_SIZE];

/* Output UDP packet parameters */
static short packNum;
static short numPackets;
static int   totalLen;
static int   remainLen = 0;

int sendMMS_initBuffer(
        int headerLen, const char* header,
        int bodyLen,   const char* body) {

    char* ptr = encode_mms_buffer;
    int lngth;
    //char* replyToAppID = replyToAppID1 ? replyToAppID1 : "com.sun.mms.MMSTest";
    char appID[128];
    char replyToAppID[128];
    char fromAddress[128];

    if (header == NULL || body == NULL) {
        return 0;
    }

    parseHeader(headerLen, header, appID, replyToAppID, fromAddress);

    if (remainLen > 0) {
        javacall_print("Error: previous MMS send is still in progress!\n");
        return 0;
    }

    totalLen = bodyLen + headerLen + strlen(fromAddress) +
               strlen(appID) + strlen(replyToAppID);
    if (totalLen + PACKET_HEADER_SIZE > MMS_SEND_BUFF_LENGTH) {
        javacall_print("Error: too big MMS!\n");
        return 0;
    }

    lngth = strlen(fromAddress) + 1;  memcpy(ptr, fromAddress, lngth);  ptr += lngth;
    lngth = strlen(appID) + 1;        memcpy(ptr, appID, lngth);        ptr += lngth;
    lngth = strlen(replyToAppID) + 1; memcpy(ptr, replyToAppID, lngth); ptr += lngth;

    *((int*)ptr)   = bodyLen+headerLen;  ptr += sizeof(int);
    lngth = headerLen;                   memcpy(ptr, header, lngth); ptr += lngth;
    lngth = bodyLen /*+headerLen + 1*/;       memcpy(ptr, body,   lngth); ptr += lngth;

    packNum = 1;
    totalLen = ptr - encode_mms_buffer;
    remainLen = totalLen;
    numPackets = 1 + (totalLen - 1)/PACKET_MAX_CAPACITY;

    return 1;
}

int sendMMS_getNextUDPPacket(char** buffer) {
    int count;

    if (remainLen <= 0) {
        return 0;
    }

    count = (remainLen > PACKET_MAX_CAPACITY) ? PACKET_MAX_CAPACITY : remainLen;

    *((short*) (datagram_buffer))   = packNum;
    *((short*) (datagram_buffer+2)) = numPackets;
    *((short*) (datagram_buffer+4)) = count;
    *((int*)   (datagram_buffer+6)) = totalLen;

    memcpy(datagram_buffer + PACKET_HEADER_SIZE,
           encode_mms_buffer + (packNum-1) * PACKET_MAX_CAPACITY,
           count);

    packNum++;
    remainLen -= count;

    *buffer = datagram_buffer;
    //printf("send packNum=%i numPackets=%i count=%i\n", (int) packNum-1, (int) numPackets, count);

    // It is an important problem of UDP emulation.
    // A long sequence of concurrent packets will be lost.
    // It could be easily seen on PACKET_MAX_SIZE=1500 on JDTS 2.0,
    // MultipleMessages#multipleServersAccept test which sends
    // 24 packets and most of them does not reach recepient side.
    if (packNum > 1) {
        MSG msg;
        Sleep(200);
        // For the case if messages are sent to itself
        while (PeekMessage(&msg, NULL, WM_NETWORK, WM_NETWORK, PM_REMOVE)) {
            DispatchMessage(&msg);
        }
    }

    return count+PACKET_HEADER_SIZE;
}

extern javacall_result javacall_is_mms_appID_registered(const char* appID);

javacall_result process_UDPEmulator_mms_incoming(javacall_handle handle) {
    unsigned char pAddress[256];
    int port;
    char buffer[PACKET_MAX_SIZE];
    int length = PACKET_MAX_SIZE;
    int pBytesRead;
    void *pContext = NULL;
    int ok;

    char* fromAddress = "fromAddress";
    char* appID = "appID";
    char* replyToAppID = "replyToAppID";
    int bodyLen;
    unsigned char* body;
    int rc;

    ok = javacall_datagram_recvfrom_start(
        handle, pAddress, &port, buffer, length, &pBytesRead, &pContext);
    if (ok != JAVACALL_OK) {
        javacall_print("MMS: Failed to receive a datagram!\n");
        return JAVACALL_FAIL;
    }

    rc = recvMMS_readPacket(buffer, pBytesRead);
    if (!rc) {
        javacall_print("MMS UDP emulation package received\n");
        return JAVACALL_OK;
    }

    decodeMmsBuffer(&fromAddress, &appID, &replyToAppID, &bodyLen, &body);
    //printf("received: %s %s %s %i\n", fromAddress, appID, replyToAppID, bodyLen);

    if (javacall_is_mms_appID_registered(appID) != JAVACALL_OK) {
        javacall_print("MMS on unregistered appID received!\n");
        return JAVACALL_FAIL;
    }

    javanotify_incoming_mms_singlecall(fromAddress, appID, replyToAppID,
        bodyLen, body);

    return JAVACALL_OK;
}
