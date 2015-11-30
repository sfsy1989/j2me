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

#include "javacall_mms.h"
#include "javacall_memory.h"
#include "javacall_logging.h"
#include "javacall_datagram.h"
#include "javacall_network.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>

extern int sendMMS_initBuffer(
        int headerLen, const char* header,
        int bodyLen,   const char* body);
extern int sendMMS_getNextUDPPacket(char** buffer);

extern char* getIPBytes_nonblock(char *hostname);

 /**
  * checks if the Multimedia Message Service (MMS) is available, and MMS 
  * messages can be sent and received
  * 
  * @return <tt>JAVACALL_OK</tt> if MMS service is avaialble 
  *         <tt>JAVACALL_FAIL</tt> or negative value otherwise
  */
javacall_result javacall_mms_is_service_available(void) {

    return JAVACALL_OK;
}

extern char* getProp(const char* propName, char* defaultValue);
extern int getIntProp(const char* propName, int defaultValue);

/**
 * sends an MMS message
 *
 * The MMS message header and body have to conforms to the message 
 * structure in D.1 and D.2 of JSR205 spec.
 *
 * @param toAddress The recipient's MMS address.
 * @param fromAddress The sender's MMS address.
 * @param appID The application ID associated with the message.
 * @param replyToAppID The application ID to which replies can be sent.
 *
 * @param headerLen The length of the message header.
 * @param header The message header should include Subject, DeliveryData, 
 *          Priority, From, To, Cc and Bcc.
 *          If the MMS message is for Java applications, Application-ID 
 *          and Reply-To-Application-ID are added to the Content-Type 
 *          header field as additional Content-Type parameters.
 * @param bodyLen The length of the message body.
 * @param body The message body.
 *        The MMS message body is composed of one or more message parts.
 *        The following fields are in the message part structure:
 *          MIME-Type - the MIME Content-Type for the Message Part [RFC 2046]
 *          content-ID - the content-id header field value for the Message Part [RFC 2045]. 
 *              The content-id is unique over all Message Parts of an MMS and must always be set
 *          content-Location - the content location which specifies the  name of the 
 *              file that is attached. If  the content location is set to null, 
 *              no content location will be set for this message part.
 *          contents -  the message contents of the message part
 * @param pHandle of sent mms 
 *
 * @return <tt>JAVACALL_OK</tt> if send request success
 *         <tt>JAVACALL_FAIL</tt> or negative value otherwise
 * 
 * Note: javanotify_mms_send_completed() needs to be called to notify
 *       completion of sending operation.
 *       The returned handle will be passed to javanotify_mms_send_completed( ) upon completion
 */
javacall_result javacall_mms_send(int headerLen, const char* header, 
                                  int bodyLen,   const unsigned char* body,
                                  javacall_handle* /*OUT*/pHandle) {

    javacall_handle datagramHandle;
    javacall_result ok;
    int pBytesWritten = 0;
    void *pContext;
    unsigned char *pAddress;
    static int mmsID = 0;

    javacall_int64 timeStamp = 0;
    int encodedMMSLength;
    char* encodedMMS;

    char* IP_text = getProp("JSR_205_DATAGRAM_HOST", "127.0.0.1");
    //JSR205Tool listens on 33301 port, but sends to 33300 port
    int mmsRemotePortNumber = getIntProp("JSR_205_MMS_OUT_PORT", 33301);

    javacall_network_init_start();
    pAddress = getIPBytes_nonblock(IP_text);

    if (!sendMMS_initBuffer(headerLen, header, bodyLen, body)) {
        return JAVACALL_FAIL;
    }

    ok = javacall_datagram_open(0, &datagramHandle);
    if (ok == JAVACALL_OK) {
      while(encodedMMSLength = sendMMS_getNextUDPPacket(&encodedMMS)) {
        ok = javacall_datagram_sendto_start(datagramHandle, pAddress, mmsRemotePortNumber,
            encodedMMS, encodedMMSLength, &pBytesWritten, &pContext);
        if (ok != JAVACALL_OK) {
            javacall_print("Error: MMS sending - datagram blocked.\n");
        }
      }
    }

    printf("## javacall: MMS sending... (IP=%s, port=%i)\n", IP_text, mmsRemotePortNumber);

    *pHandle = javacall_malloc(sizeof(int));
    if (*pHandle == NULL) {
        javacall_print("MMS: OUT OF MEMORY\n");
        return JAVACALL_FAIL;
    }

    **((int**)pHandle) = mmsID++;
    javanotify_mms_send_completed(JAVACALL_MMS_SENDING_RESULT_SUCCESS,
                                  (javacall_handle)*pHandle);

    return JAVACALL_OK;
}
    
/**
 * Requests to fetch the incoming MMS message.
 *
 * This function requests to fetch MMS message and should return quickly.
 * After a MMS indication was notified, this API requests the platform to retrieve the MMS message body.
 *
 * @param handle of available MMS message
 * @param fetch if JAVACALL_TRUE, the platform should fetch the MMS message 
 *          body from the network and call javanotify_incoming_mms().
 *          Otherwise, the MMS message body should be discarded.
 *
 * @return <tt>JAVACALL_OK</tt> if fetch request success
 *         <tt>JAVACALL_FAIL</tt> or negative value otherwise
 */
javacall_result javacall_mms_fetch(javacall_handle handle, javacall_bool fetch) {

    return JAVACALL_OK;
}


#define APP_ID_MAX 8
static char* appIDList[APP_ID_MAX] = {0,0,0,0,0,0,0,0};

/**
 * The platform must have the ability to identify the target application of incoming 
 * MMS messages, and delivers messages with application ID to the WMA implementation.
 * If this application ID has already been registered either by a native 
 * application or by another WMA application, then the API should return an error code.
 * 
 * @param appID The application ID associated with the message.
 * @return <tt>JAVACALL_OK</tt> if started listening, or 
 *         <tt>JAVACALL_FAIL</tt> or negative value if unsuccessful
 */
javacall_result javacall_mms_add_listening_appID(const char* appID) {
    int i;
    int free = -1;
    char* pNewAppId;

    for (i = 0; i < APP_ID_MAX; i++) {
        if (appIDList[i] == NULL) {
            free = i;
            continue;
        }
        if (0 == strcmp(appIDList[i], appID)) {
            return JAVACALL_FAIL;
        }
    }

    if (free == -1) {
        javacall_print("appID amount exceeded");
        return JAVACALL_FAIL;
    }

    pNewAppId = (char*)javacall_malloc(strlen(appID));
    if (pNewAppId == NULL) {
        javacall_print("MMS: OUT OF MEMORY\n");
        return JAVACALL_FAIL;
    }

    appIDList[free] = strcpy(pNewAppId, appID);

    //printf("mms.c: javacall_mms_add_listening_appID(%s) \n", appID);

    return JAVACALL_OK;
}
    
/**
 * Stops listening to an application ID.
 * After unregistering an application ID, MMS messages received by the device 
 * for the specified application ID should not be delivered to the WMA 
 * implementation.  
 * If this API specifies an application ID which is not registered, then it 
 * should return an error code.
 *
 * @param appID The application ID to stop listening to
 * @return <tt>JAVACALL_OK </tt> if stopped listening to the application ID, 
 *          or <tt>0</tt> if failed, or the application ID not registered
 */
javacall_result javacall_mms_remove_listening_appID(const char* appID) {
    int i;

    for (i = 0; i < APP_ID_MAX; i++) {
        if (appIDList[i] == NULL) {
            continue;
        }
        if (0 == strcmp(appIDList[i], appID)) {
            javacall_free(appIDList[i]);
            appIDList[i] = NULL;
            return JAVACALL_OK;
        }
    }

    return JAVACALL_FAIL;
}

javacall_result javacall_is_mms_appID_registered(const char* appID) {
    int i;

    for (i = 0; i < APP_ID_MAX; i++) {
        if (appIDList[i] == NULL) {
            continue;
        }
        if (0 == strcmp(appIDList[i], appID)) {
            //printf("mms.c: javacall_is_mms_appID_registered() called with appID=%s - OK\n", appID);
            return JAVACALL_OK;
        }
    }

    //printf("mms.c: javacall_is_mms_appID_registered() called with appID=%s - FAIL\n", appID);
    return JAVACALL_FAIL;
}
    
/**
 * Computes the number of transport-layer segments that would be required to
 * send the given message.
 *
 * @param msgBuffer The message to be sent.
 * @param msgLen The length of the message.
 * @return The number of transport-layer segments required to send the message.
 */
int javacall_mms_get_number_of_segments(unsigned char msgBuffer[], int msgLen) {

    return 1;
}

/**
 * Gets the phone number of device
 *
 * @return The phone number of device.
 */
javacall_utf16_string javacall_mms_get_internal_phone_number() {

    static javacall_utf16 phone[4] = {'9','1','1',0};
    return phone;
}
