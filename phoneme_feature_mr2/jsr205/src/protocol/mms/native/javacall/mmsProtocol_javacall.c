/*
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

#include <jsr205_mms_protocol.h>
#include <pcsl_memory.h>
#include <javacall_defs.h>
#include <javacall_mms.h>
#include <string.h>


/**
 * Register an application identifier for receiving MMS messages.
 * <p>
 * This API registers a MMS message which matches the registered application
 * identifier. According to the MMS standard, the application identifier is a
 * string and a special field in the MMS message header. Its use is similar to
 * the SMS port number for transferring the MMS to a specified WMA application.
 * After registering the special application ID, the target device should check
 * the MMS header to forward the MMS whose application ID matches the registered
 * application ID to the correct MIDP WMA. If this application ID is used by
 * another WMA or target device native application, then this API should return
 * an error code.
 *
 * @param appID The application identifier associated with the message..
 *
 * @return <code>WMA_OK</code> when successful or
 *      <code>WMA_ERR</code> if an error occurred.
 */
WMA_STATUS jsr205_add_mms_listening_appID(unsigned char* appID) {

    javacall_result result = javacall_mms_add_listening_appID((char*)appID);

    return (result == JAVACALL_OK) ? WMA_OK : WMA_ERR;
}

/**
 * Clear a registered application identifier from receiving MMS messages.
 * <p>
 * This API removes a registered message application identifier. After removing
 * this application ID, MIDP WMA should no longer receive incoming MMS messages
 * matching this application ID. If the specified application ID has not been
 * registered, then this API should return an error code.
 *
 * @param appID The application identifier associated with the message..
 *
 * @return <code>WMA_OK</code> when successful or
 *      <code>WMA_ERR</code> if an error occurred.
 */
WMA_STATUS jsr205_remove_mms_listening_appID(unsigned char* appID) {

    javacall_result result = javacall_mms_remove_listening_appID((char*)appID); 

    return (result == JAVACALL_OK) ? WMA_OK : WMA_ERR;
}


/**
 * Incoming MMS Message.
 * <p>
 * After a WMA application confirms the fetching of an MMS message body, the
 * native protocol layer of the target device should fetch the MMS message body
 * from the network and forward it to MIDP WMA. The message should be forwarded
 * as an asynchronous message. The incoming MMS should be complete and include
 * both the MMS message header and all message parts. For more information
 * about the message header and message parts, refer to Section 2.3.3. The
 * incoming MMS can be sent to WMA by an asynchronous message or by calling a
 * callback function.
 * <P>
 * Note: if There are any WMA MMS notifications in the native mailbox when the
 * VM is launched, those notifications are expected to be delivered by the first
 * WMA application which registers those messages.
 *
 * @param msgBuffer The incoming message.
 */
void jsr205_notify_incoming_mms(void* msgBuffer) {

    /* void javanotify_incoming_mms(javacall_handle handle,
                                    int bodyLen, 
                                    const unsigned char* body); */

    /**
     * Need revisit: (verify if it is correct)
     * javanotify hands over the incoming mms.
     * the mms should be stored until this function is called, and 
     * returns the mms to java - copies the saved mms to the supplied buffer
     */
    (void)msgBuffer;
}

/**
 * Send a Multimedia Message.
 * <p>
 * This API sends an MMS message. According to the JSR 205 specification, one
 * MMS includes a message header and a message body. There can be more than one
 * message part in an MMS message body. MIDP WMA layer. The customer of this API
 * does not have to care about the details of the MMS PDU protocol
 * (WAP-209-MMS-Encapsulation standard). The native protocol of the target
 * device should package the MMS into the MMS PDU and send it to the network.
 * The customer should also tell the MIDP WMA the maximum message length which
 * the target device can support.
 * <p>
 * Sending an MMS message to the network has a long life process. To avoid
 * blocking the JVM for a long time, this API should work in asynchronous mode.
 * It will transfer the parameter to the native software platform and return
 * quickly. The native software platform should take the responsibility for
 * handling the entire communcation session and use an asynchronous message or
 * callback function to notify the MIDP WMA of the result.
 *
 * @param toAddress The recipient's MMS address.
 * @param fromAddress The sender's MMS address.
 * @param appID The application ID associated with the message.
 * @param replyToAppID The application ID to which replies can be sent.
 * @param headerLen The length of the message header.
 * @param header The message header.
 * @param bodyLen The length of the message body.
 * @param body The message body.
 * @param bytesSent The number of bytes that were sent.
 * @param pContext pointer where to save context of asynchronous operation.
 *
 * @return <code>WMA_OK</code>, <code>WMA_NET_WOULDBLOCK</code> or
 * <code>WMA_ERR</code>.
 */
WMA_STATUS jsr205_send_mms(char* toAddr, char* fromAddr, char* appID,
    char* replyToAppID, jint headerLen, char* header, jint bodyLen,
    char* body, /* OUT */jint* bytesSent, /* OUT */void **pContext) {

    javacall_handle pHandle;
    javacall_result result = javacall_mms_send((int)headerLen, 
                                         (const char*)header, 
                                         (int)bodyLen, 
                                         (const unsigned char*)body,
                                         &pHandle);

    return (result == JAVACALL_OK) ? WMA_OK : WMA_ERR;

    (void)toAddr;
    (void)fromAddr;
    (void)appID;
    (void)replyToAppID;
    (void)bytesSent;
    (void)pContext;
}

/**
 * Notify the native software platform that the send operation has completed.
 * This allows the native platform to perform any special operations, too.
 *
 * @param bytesSent Number of bytes sent or <code>-1</code> if there was a problem.
 */
void jsr205_mms_notify_send_completed(/* OUT */ jint *bytesSent) {

	/* void javanotify_mms_send_completed(
                        javacall_mms_sending_result result, 
                        javacall_handle             handle);
    */

    (void)bytesSent;
}

/**
 * When an incoming MMS message arrives in the MMS proxy, the proxy should send
 * a notification to the MMS client (target device). The notification includes
 * only the MMS header. The MMS client decides if it must fetch the message body
 * of this MMS from the network. If the target device finds that the incoming MMS
 * notification is for MIDP WMA, it should send a notification to WMA and wait
 * for a confirmation from WMA. This notification can be sent to WMA by an
 * asynchronous message or by calling a callback function.
 *
 * @param msgHeader The message header contents.
 */
void MMSNotification(void* msgHeader) {

    /* void javanotify_incoming_mms_available(javacall_handle handle,
                                              int headerLen, 
                                              const char* header); */

    /**
     * Need revisit:
     * when a mms is available, javanotify function is called with the msg header.
     * verify what is the use of this api - might not be needed (do nothing)
     */
    
    (void)msgHeader;
}

/**
 * Confirm to fetch the incoming MMS.
 * <p>
 * AFter WMA gets the MMS message notification (message header), this callback
 * function provides a confirmation to native software platform for fetching
 * the MMS message body. If the return value is <code>true</code>, the target
 * device should fetch the MMS message body from the network and forward it to
 * WMA; otherwise, the MMS message body should be discarded.
 *
 * @return <code>true</code> if the target device should fetch the MMS message
 *         body from the network; <code>false</code>, if the message body
 *         should be discarded.
 *
 * @return <code>WMA_OK</code> if the target device should fetch the MMS
 *     message body. <code>WMA_ERR</code> if an error occurred.
 */
WMA_STATUS jsr205_fetch_mms() {

    /* javacall_result result = javacall_mms_fetch(javacall_handle handle, javacall_bool fetch); */
    return WMA_OK;
}

/**
 * Returns the number of segments that would be needed in the underlying
 * protocol to send a specified message. The specified message is included as a
 * parameter of this function. Note that this method does not actually send the
 * message. It will only calculate the number of protocol segments needed for
 * sending the message.
 *
 * @param msgBuffer The message body.
 * @param msgLen Message body length (in bytes).
 * @param msgType Message type: Binary or Text.
 * @param hasPort indicates if the message includes source or destination port number.
 * @param numSegments The number of segments required to send the message.
 *
 * @return <code>WMA_OK</code> when successful or
 *      <code>WMA_ERR</code> if an error occurred.
 */
WMA_STATUS jsr205_number_of_mms_segments(unsigned char msgBuffer[],
    jint msgLen, jint msgType, jboolean hasPort, /* OUT */jint* numSegments) {

    int result = javacall_mms_get_number_of_segments(msgBuffer, msgLen);
    *numSegments = result;

    /**
     * Need revisit:
     * msgType and hasPort are irrelevant.
     * Verify this assumption.
     */
    
    (void)msgType;
    (void)hasPort;

    return (result > 0) ? WMA_OK : WMA_ERR;
}

/**
 * Gets the phone number of device
 *
 * @return The phone number of device.
 */
pcsl_string getInternalPhoneNumber(void) {
    pcsl_string retValue = PCSL_STRING_NULL;
    const char* phoneNumber = "911";
    pcsl_string_status status = PCSL_STRING_ERR;
    //load_var_char_env_prop((char**)&phoneNumber, "JSR_120_PHONE_NUMBER",
    //    "com.sun.midp.io.j2me.sms.PhoneNumber");
    if (phoneNumber != NULL) {
        status = pcsl_string_convert_from_utf8((const jbyte *)phoneNumber,
            strlen(phoneNumber), &retValue);
        if (status != PCSL_STRING_OK) {
            retValue = PCSL_STRING_NULL;
        }
    }
    return retValue;
}
