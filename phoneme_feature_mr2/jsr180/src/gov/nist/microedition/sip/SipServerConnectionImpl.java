/*
 * Portions Copyright  2000-2007 Sun Microsystems, Inc. All Rights
 * Reserved.  Use is subject to license terms.
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
/*
 *   
 *
 * Created on Jan 29, 2004
 *
 */
package gov.nist.microedition.sip;

import java.io.IOException;
import java.io.InputStream;
import java.io.InterruptedIOException;
import java.io.OutputStream;
import java.io.ByteArrayInputStream;

import java.util.Vector;
import java.util.Enumeration;

import javax.microedition.sip.SipDialog;
import javax.microedition.sip.SipException;
import javax.microedition.sip.SipServerConnection;
import javax.microedition.sip.SipConnection;

import gov.nist.core.HostPort;
import gov.nist.core.ParseException;
import gov.nist.siplite.SIPConstants;
import gov.nist.siplite.SipStack;
import gov.nist.siplite.SipProvider;
import gov.nist.siplite.TransactionAlreadyExistsException;
import gov.nist.siplite.TransactionUnavailableException;
import gov.nist.siplite.message.*;
import gov.nist.siplite.stack.Dialog;
import gov.nist.siplite.stack.ServerTransaction;
import gov.nist.siplite.stack.Subscription;
import gov.nist.siplite.header.ContactHeader;
import gov.nist.siplite.header.ContactList;
import gov.nist.siplite.header.ExpiresHeader;
import gov.nist.siplite.header.Header;
import gov.nist.siplite.header.HeaderList;
import gov.nist.siplite.header.ToHeader;
import gov.nist.siplite.header.SubscriptionStateHeader;
import gov.nist.siplite.header.ContentLengthHeader;
import gov.nist.siplite.address.*;
import gov.nist.siplite.stack.Transaction;

import com.sun.midp.log.Logging;
import com.sun.midp.log.LogChannels;

/**
 * SIP ServerConnection implementation.
 *
 * <a href="{@docRoot}/uncopyright.html">This code is in the public domain.</a>
 */
public class SipServerConnectionImpl implements SipServerConnection {
    // Server Transaction States
    /**
     * Terminated, the final state, in which the SIP connection has
     * been terminated by error or closed
     */
    public static final int TERMINATED = 0;
    /**
     * Request Received, SipServerConnection returned from
     * SipConnectionNotifier or provisional response(s) (1xx) sent.
     */
    public static final int REQUEST_RECEIVED = 1;
    /**
     * Initialized, response initialized calling initResponse()
     */
    public static final int INITIALIZED = 2;
    /**
     * Stream Open, OutputStream opened with openContentOutputStream().
     * Opening InputStream for received request does not trigger state
     * transition.
     */
    public static final int STREAM_OPEN = 3;
    /**
     * Completed, transaction completed with sending final response
     * (2xx, 3xx, 4xx, 5xx, 6xx)
     */
    public static final int COMPLETED = 4;
    /**
     * Attribute keeping the actual state of this server transaction
     */
    private int state;
    /**
     * the sip dialog this client transaction belongs to
     */
    private SipDialog sipDialog = null;
    /**
     * the request for this server transaction
     */
    private Request request = null;
    /**
     * the response to the actual request
     */
    private Response response = null;
    /**
     * content of the response body
     */
    private SDPOutputStream contentOutputStream = null;
    /**
     * content from the request body
     */
    private InputStream contentInputStream = null;

    /**
     * Receiver of incoming messages
     */
    private SipConnectionNotifierImpl sipConnectionNotifierImpl;
    /**
     * Flag indicating which SIP message (request or response)
     * should be used in getHeader()/getHeaders()/setHeader()/removeHeader().
     */
    private boolean useResponse = false;

    /**
     * Boolean flag used to indicate if 2xx is allowed to resend
     * This flag is set to true if 2xx is sent and SipServerConnectionImpl
     * transitions to COMPLETED state
     */
    private boolean resend2xxAllowed = false;

    /**
     * Constructor.
     * @param request the protocol connection request
     * @param sipDialog the current transaction state
     * @param sipConnectionNotifierImpl the notification handler
     */
    protected SipServerConnectionImpl(
            Request request,
            SipDialog sipDialog,
            SipConnectionNotifierImpl sipConnectionNotifierImpl) {
        this.request   = request;
        this.sipDialog = sipDialog;
        this.sipConnectionNotifierImpl = sipConnectionNotifierImpl;
        if (request.getMethod() == Request.ACK) {
            state = COMPLETED;
        } else {
            state = REQUEST_RECEIVED;
        }
    }

    /**
     * Initializes SipServerConnection with a specific SIP response to the
     * received request.
     * The default headers and reason phrase will be initialized automatically.
     * After this the SipServerConnection is in Initialized state.
     * The response can be sent. The procedure of generating the response and
     * header fields is defined in RFC 3261 [1] p. 49-50. At least following
     * information is set by the method:
     * From MUST equal the From header field of the request
     * Call-ID  MUST equal the Call-ID header field of the request
     * CSeq MUST equal the CSeq field of the request
     * Via MUST equal the Via header field values in the request
     * and MUST maintain the same ordering
     * To MUST Copy if exists in the original request,
     * 'tag' MUST be added if not present
     * Furthermore, if the system has automatically sent the 100 Trying
     * response, the 100 response initialized and sent by the user
     *  is just ignored.
     * @param code - Response status code 1xx - 6xx
     * @throws IllegalArgumentException - if the status code is out of
     * range 100-699 (RFC 3261 p.28-29)
     * @throws SipException - INVALID_STATE if the response can not be
     * initialized, because of wrong state.
     */
    public void initResponse(int code)
            throws IllegalArgumentException, SipException {
        // Check if the code is not out of range
        if (code < 100 || code > 699)
            throw new
                IllegalArgumentException("the response code is out of range.");

        // Check if we are in a good state to init the response
        if (state != REQUEST_RECEIVED)
            throw new SipException("the response can not be initialized,"
                    + " because of wrong state.",
                    SipException.INVALID_STATE);

        // Generating the response to the request
        try {
            response = StackConnector
                    .messageFactory.createResponse(code, request);
        } catch (ParseException pe) {
            if (Logging.REPORT_LEVEL <= Logging.ERROR) {
                Logging.report(Logging.ERROR, LogChannels.LC_JSR180,
                    "Exception in SSC.initResponse(): " + pe);
                pe.printStackTrace();
            }
        }

        // Set the toTag in the ToHeader if not already present
        ToHeader toHeader = (ToHeader) response.getHeader(ToHeader.NAME);
        if (toHeader.getTag() == null)
            toHeader.setTag(StackConnector.generateTag());

        // if we don't have any contact headers we add one
        ContactList contactList = response.getContactHeaders();
        if (contactList == null || contactList.isEmpty()) {
            ContactHeader contactHeader = null;
            String transport = sipConnectionNotifierImpl.
                getSipProvider().getListeningPoint().getTransport();
            try {
                Address address = StackConnector
                        .addressFactory
                        .createAddress(
                        "<sip:"
                        + sipConnectionNotifierImpl.getLocalAddress()
                        + ":"
                        + sipConnectionNotifierImpl.getLocalPort()
                        + ";transport="
                        + transport
                        + ">");
                contactHeader = StackConnector.headerFactory
                        .createContactHeader(address);
                response.addHeader(contactHeader);
            } catch (IOException ioe) {
                ioe.printStackTrace();
            } catch (ParseException pe) {
                pe.printStackTrace();
            }
        }

        state = INITIALIZED;
        useResponse = true;

        // System.out.println("The following response has been initialized:\n"+
        //                    response.toString());
    }

    /**
     * Changes the default reason phrase.
     * @param phrase the default reason phrase.
     * @throws SipException INVALID_STATE if the response can not
     * be initialized, because of wrong state.
     * INVALID_OPERATION if the reason phrase can not be set.
     * @throws IllegalArgumentException if the reason phrase is illegal.
     */
    public void setReasonPhrase(String phrase)
    throws SipException, IllegalArgumentException {
        if (state != INITIALIZED)
            throw new SipException("the Reason Phrase can not be set,"
                    + " because of wrong state.",
                    SipException.INVALID_STATE);

        if (phrase == null) {
            throw new
                IllegalArgumentException("The reason phrase can not be null.");
        }

        // RFC 3261, section 7.2: No CR or LF is allowed
        // (in the Status Line) except in the final CRLF sequence.
        if ((phrase.indexOf("\n") != -1) || (phrase.indexOf("\r") != -1)) {
            throw new
                IllegalArgumentException("Invalid reason phrase.");
        }

        response.setReasonPhrase(phrase);
    }

    /**
     * (non-Javadoc)
     * @see javax.microedition.sip.SipConnection#send()
     */
    public void send()
    throws IOException, InterruptedIOException, SipException {
        if (state == REQUEST_RECEIVED) {
            throw new SipException("can not send response"
                    + " because of wrong state.",
                    SipException.INVALID_STATE);
        }

        if ((state == COMPLETED) && !resend2xxAllowed) {
            throw new SipException("COMPLETED state allows"
                    + " only resend of 2xx responses",
                    SipException.INVALID_STATE);
        }

        if (state == TERMINATED) {
            throw new SipException("can not send response"
                    + " because SipServerConnection is TERMINATED",
                    SipException.INVALID_STATE);
        }

        if (Logging.REPORT_LEVEL <= Logging.INFORMATION) {
            Logging.report(Logging.INFORMATION, LogChannels.LC_JSR180,
                "Actual request for the response we want to send:\n" +
                request.toString());
        }

        ServerTransaction serverTransaction =
                (ServerTransaction)request.getTransaction();

        if (Logging.REPORT_LEVEL <= Logging.INFORMATION) {
            Logging.report(Logging.INFORMATION, LogChannels.LC_JSR180,
                "SipServerTransaction :" + serverTransaction);
        }

        // Set the sdp body of the message
        if (contentOutputStream != null) {
            contentOutputStream.setOpen(false);
            response.setContent(
                contentOutputStream.getByteArrayOutputStream().toByteArray());
            contentOutputStream = null;
        }

        String method = request.getMethod();
        final int statusCode = response.getStatusCode();
        final int statusGroup = statusCode / 100;

        // send the response
        if (!resend2xxAllowed) { // don't create new transaction on resending
            SipStack sipStack =
                sipConnectionNotifierImpl.getStackConnector().getSipStack();

            if (serverTransaction == null || sipStack.isDialogCreated(method)) {
                try {
                    SipProvider sipProvider =
                        sipConnectionNotifierImpl.getSipProvider();
                    if (serverTransaction == null) {
                        serverTransaction =
                            sipProvider.getNewServerTransaction(request);
                    } else {
                        /*
                         * 12.1 Creation of a Dialog
                         * Dialogs are created through the generation of
                         * non-failure responses to requests with specific
                         * methods.  Within this specification, only 2xx and
                         * 101-199 responses with a To tag, where the request
                         * was INVITE, will establish a dialog.
                         */
                        if (statusCode > 100 && statusCode < 300) {
                            // Equip a dialog in case of dialog is null
                            // or its state is INITIALIZED only prevent
                            // changing contact property of dialog by
                            // sending 200 OK response for INVITE
                            // after sending 200 OK for UPDATE
                            boolean equipDialog = false;
                            if (sipDialog == null) {
                                equipDialog = true;
                            } else if (sipDialog.getState() ==
                                    Dialog.INITIAL_STATE) {
                                equipDialog = true;
                            }

                            if (equipDialog) {
                                sipProvider.equipADialogForTransaction(
                                    serverTransaction, request);
                            }
                        } else if ((sipDialog != null) &&
                            (statusGroup > 2 && statusGroup < 7)) {
                            // JSR180, p.42
                            // Terminated state: error response (3xx-6xx)
                            // received (or sent).
                            ((SipDialogImpl)sipDialog).setState
                                    (SipDialog.TERMINATED);
                        }
                    }
                } catch (TransactionAlreadyExistsException taee) {
                    if (Logging.REPORT_LEVEL <= Logging.ERROR) {
                        Logging.report(Logging.ERROR, LogChannels.LC_JSR180,
                            "Exception in SSC.send(): " + taee);
                        taee.printStackTrace();
                    }
                    // return;
                } catch (TransactionUnavailableException tue) {
                    if (Logging.REPORT_LEVEL <= Logging.ERROR) {
                        Logging.report(Logging.ERROR, LogChannels.LC_JSR180,
                            "Exception in SSC.send(): " + tue);
                        tue.printStackTrace();
                    }
                    // return;
                }
            }
        }

        SipConnectionNotifierImpl newNotifier = sipConnectionNotifierImpl;

        if (sipDialog != null) {
            Dialog dialog = ((SipDialogImpl)sipDialog).getDialog();

            if (dialog != null &&
                    (statusCode > 199 && statusCode < 300)) {
                if (method.equals(Request.UPDATE)) {
                    // processing UPDATE - change dialog contact property
                    dialog.addRoute(serverTransaction.getOriginalRequest());
                }

                newNotifier = findNotifier();
                Transaction transaction = (Transaction)
                    dialog.getFirstTransaction();
                if (transaction instanceof ServerTransaction) {
                    transaction.setApplicationData(newNotifier);
                }

                if (newNotifier != null) {
                    sipConnectionNotifierImpl = newNotifier;
                }
            }
        }

        // Set the application data so that when the request comes in,
        // it will retrieve this SipConnectionNotifier
        serverTransaction.setApplicationData(newNotifier);

        if (Logging.REPORT_LEVEL <= Logging.INFORMATION) {
            Logging.report(Logging.INFORMATION, LogChannels.LC_JSR180,
                "response to send : " + response);
        }

        // May throw IOException and SipException
        serverTransaction.sendResponse(response);

        if (Logging.REPORT_LEVEL <= Logging.INFORMATION) {
            Logging.report(Logging.INFORMATION, LogChannels.LC_JSR180,
                "response sent");
        }

        // Change the state of SipServerConnection
        int responseClass = response.getStatusCode()/100;
        if (responseClass == 1) { // 1xx responses
            state = REQUEST_RECEIVED;
        } else {
            state = COMPLETED;
            if (responseClass == 2) {
                // Allow 2xx response to be resent
                resend2xxAllowed = true;
            }
        }

        // Change the dialog state
        changeDialogState(serverTransaction);
    }

    /**
     * Finds a connection notifier that listens on the port given in the
     * Contact header of the response. When a 2xx response contains a new
     * Contact with the port that is different from one that is used by the
     * connection notifier associated with this server connection object
     * (sipConnectionNotifierImpl), the notifier must be changed accordingly.
     * @returns SipConnectionNotifier listening on the new Contact or
     * null if such notifier was not found.
     */
    private SipConnectionNotifierImpl findNotifier() throws SipException {
        // IMPL_NOTE: handle shared connections
        int localPort = SIPConstants.DEFAULT_NONTLS_PORT;
        String localTransport = SIPConstants.TRANSPORT_UDP;

        ContactList contactList = response.getContactHeaders();
        if (contactList != null) {
            ContactHeader contact = (ContactHeader)contactList.getFirst();
            HostPort hp = contact.getHostPort();
            if (hp != null) {
                localPort = hp.getPort();
            }

            URI uri = contact.getAddress().getURI();
            String transport = uri.isSipURI() ?
                ((SipURI)uri).getTransportParam() : null;
            if (transport != null) {
                localTransport = transport;
            }
        }

        // Find a SipConnectionNotifier listening on the given port.
        StackConnector stackConnector =
            sipConnectionNotifierImpl.getStackConnector();

        SipConnectionNotifierImpl newNotifier = (SipConnectionNotifierImpl)
            stackConnector.getSipConnectionNotifier(localPort,
                sipConnectionNotifierImpl.getMIMEType());

        return newNotifier;
    }


    /**
     * Sets header value in SIP message. If the header does not exist
     * it will be added to the message, otherwise the existing header is
     * overwritten. If multiple header field values exist the topmost is
     * overwritten. The implementations MAY restrict the access to some headers
     * according to RFC 3261.
     * @param name - name of the header, either in full or compact form.
     * RFC 3261 p.32
     * @param value - the header value
     * @throws SipException - INVALID_STATE if header can not be set in
     * this state. <br> INVALID_OPERATION if the system does not allow to set
     * this header.
     * @throws IllegalArgumentException - MAY be thrown if the header or
     * value is invalid
     */
    public void setHeader(String name, String value)
    throws SipException, IllegalArgumentException {
        if (state != INITIALIZED)
            throw new SipException("the Header can not be set,"
                    + " because of wrong state.",
                    SipException.INVALID_STATE);

        if (name == null)
            throw new
                IllegalArgumentException("The header name can not be null");

        if (value == null)
            throw new
                IllegalArgumentException("The header value can not be null");

        Header header = null;

        try {
            header = StackConnector.headerFactory.createHeader(name, value);
        } catch (ParseException pe) {
            throw new IllegalArgumentException(pe.getMessage());
        }

        if (header == null)
            throw new IllegalArgumentException("null header!");

        // response.attachHeader(header, false, true);
        Message currentMessage = useResponse ? (Message)response :
            (Message)request;

        if (currentMessage == null) {
            throw new SipException("Failure in setHeader(),"
                    + " associated request or response is null",
                    SipException.INVALID_MESSAGE);
        }

        currentMessage.attachHeader(header, true, true);
    }

    /**
     * Adds a header to the SIP message. If multiple header field values exist
     * the header value is added topmost of this type of headers.
     * The implementations MAY restrict the access to some headers
     * according to RFC 3261.
     * @param name - name of the header, either in full or compact form.
     * RFC 3261 p.32
     * @param value - the header value
     * @throws SipException - INVALID_STATE if header can not be added in
     * this state. <br> INVALID_OPERATION if the system does not allow to add
     * this header.
     * @throws IllegalArgumentException - MAY be thrown if the header or
     * value is invalid
     */
    public void addHeader(String name, String value)
    throws SipException, IllegalArgumentException {
        if (state != INITIALIZED)
            throw new SipException("the Header can not be add,"
                    + " because of wrong state.",
                    SipException.INVALID_STATE);
        if (name == null)
            throw new
                IllegalArgumentException("The header name can not be null");
        if (value == null)
            throw new
                IllegalArgumentException("The header value can not be null");

        Header header = null;
        try {
            header = StackConnector.headerFactory.createHeader(name, value);
        } catch (ParseException pe) {
            throw new IllegalArgumentException("The header can not be created,"
                    + " check if it is correct");
        }

        Message currentMessage = useResponse ? (Message)response :
            (Message)request;

        if (currentMessage == null) {
            throw new SipException("Failure in addHeader(),"
                    + " associated request or response is null",
                    SipException.INVALID_MESSAGE);
        }

        currentMessage.addHeader(header);
    }

    /**
     * Removes header from the SIP message. If multiple header field
     * values exist the topmost is removed.
     * The implementations MAY restrict the access to some headers
     * according to RFC 3261.
     * If the named header is not found this method does nothing.
     * @param name - name of the header to be removed, either int
     * full or compact form RFC 3261 p.32.
     * @throws SipException - INVALID_STATE if header can not be removed in
     * this state. <br> INVALID_OPERATION if the system does not allow to remove
     * this header.
     */
    public void removeHeader(String name)
            throws SipException, IllegalArgumentException {
        if (state != INITIALIZED) {
            throw new SipException("the Header can not be removed,"
                    + " because of wrong state.",
                    SipException.INVALID_STATE);
        }

        if (name == null) {
            throw new
                    IllegalArgumentException("The header name can not be null");
        }

        Message currentMessage = useResponse ? (Message)response :
            (Message)request;

        if (currentMessage == null) {
            throw new SipException("Failure in removeHeader(),"
                    + " associated request or response is null",
                    SipException.INVALID_MESSAGE);
        }

        currentMessage.removeHeader(name, true);
    }

    /**
     * Gets the header field value(s) of specified header type
     * @param name - name of the header, either in full or compact form.
     * RFC 3261 p.32
     * @return array of header field values (topmost first), or null if the
     * current message does not have such a header or the header is for other
     * reason not available (e.g. message not initialized).
     */
    public String[] getHeaders(String name) {
        Message currentMessage = useResponse ? (Message)response :
            (Message)request;

        // Return null if associated request or response is null
        if (currentMessage == null) {
            return null;
        }

        HeaderList nameList = currentMessage.getHeaderList(name);

        if (nameList == null) {
            return null;
        }

        int size = nameList.size();

        if (size < 1) {
            return null;
        }

        String[] headerValues = new String[size];

        for (int count = 0; count < size; count++) {
            headerValues[count] =
                    ((Header)nameList.elementAt(count)).getHeaderValue();
        }

        return headerValues;
    }

    /**
     * Gets the header field value of specified header type.
     * @param name - name of the header type, either in full or compact form.
     * RFC 3261 p.32
     * @return topmost header field value, or null if the
     * current message does not have such a header or the header is for other
     * reason not available (e.g. message not initialized).
     */
    public String getHeader(String name) {
        Message currentMessage = useResponse ? (Message)response :
            (Message)request;

        // Return null if associated request or response is null
        if (currentMessage == null) {
            return null;
        }

        Header header = currentMessage.getHeader(name);

        if (header == null) {
            return null;
        }

        return header.getHeaderValue();
    }

    /**
     * Gets the SIP method. Applicable when a message has been
     * initialized or received.
     * @return SIP method name REGISTER, INVITE, NOTIFY, etc. Returns null if
     * the method is not available.
     */
    public String getMethod() {
        if (TERMINATED == state) {
            return null;
        } else {
            return request.getMethod();
        }
    }

    /**
     * Gets Request-URI. Available when SipClientConnection is in Initialized
     * state or when SipServerConnection is in Request Received state.
     * Built from the original URI given in Connector.open().
     * See RFC 3261 p.35 (8.1.1.1 Request-URI)
     * @return Request-URI of the message. Returns null if the Request-URI
     * is not available.
     */
    public String getRequestURI() {
        // from the JSR180 spec:
        // "Returns null if the Request-URI is not available...
        // Available when... SipServerConnection is in Request Received state."
        // Most likely, because only in this case there's no ambiguity whether
        // the request or the response was meant.
        if (REQUEST_RECEIVED != state) {
            return null;
        } else {
            return request.getRequestURI().toString();
        }
    }

    /**
     * Gets SIP response status code. Available when SipClientConnection is in
     * Proceeding or Completed state or when SipServerConnection is in
     * Initialized state.
     * @return status code 1xx, 2xx, 3xx, 4xx, ... Returns 0 if the status code
     * is not available.
     */
    public int getStatusCode() {
        if (state != INITIALIZED || response == null) {
            return 0;
        } else {
            return response.getStatusCode();
        }
    }

    /**
     * Gets SIP response reason phrase. Available when SipClientConnection is in
     * Proceeding or Completed state or when SipServerConnection is in
     * Initialized state.
     * @return reason phrase. Returns null if the reason phrase is
     *  not available.
     */
    public String getReasonPhrase() {
        if (state != INITIALIZED || response == null) {
            return null;
        } else {
            return response.getReasonPhrase();
        }
    }

    /**
     * Returns the current SIP dialog. This is available when the SipConnection
     * belongs to a created SipDialog and the system has received (or sent)
     * provisional (101-199) or final response (200).
     * @return SipDialog object if this connection belongs to a dialog,
     * otherwise returns null.
     */
    public SipDialog getDialog() {
        if (sipDialog != null) {
            byte dialogState = this.sipDialog.getState();
            if ((dialogState != SipDialog.EARLY) &&
                (dialogState != SipDialog.CONFIRMED)) {
                return null;
            }
        }

        return sipDialog;
    }

    /**
     * Returns InputStream to read SIP message body content.
     * @return InputStream to read body content
     * @throws java.io.IOException - if the InputStream can not be opened,
     * because of an I/O error occurred.
     * @throws SipException - INVALID_STATE the InputStream can not be opened
     * in this state (e.g. no message received).
     */
    public InputStream openContentInputStream()
    throws IOException, SipException {
        if (state != REQUEST_RECEIVED)
            throw new SipException("the content input stream can not be open,"
                    + " because of wrong state: " + state,
                    SipException.INVALID_STATE);

        if (request == null) {
            throw new IOException("Request is null.");
        }

        ContentLengthHeader contentLengthHeader =
            request.getContentLengthHeader();
        if (contentLengthHeader == null) {
            throw new
                IOException("Request contains no content length header.");
        }

        int bodyLength = contentLengthHeader.getContentLength();
        if (bodyLength == 0) {
            throw new IOException("Request's body has zero length.");
        }

        byte[] buf = request.getRawContent();
        if (buf == null) { // body is empty
            throw new IOException("Body of SIP request is empty.");
        }

        contentInputStream = new ByteArrayInputStream(buf);
        return contentInputStream;
    }

    /**
     * Returns OutputStream to fill the SIP message body content.
     * When calling close() on OutputStream the message will be sent
     * to the network. So it is equivalent to call send(). Again send() must
     * not be called after closing the OutputStream, since it will throw
     * Exception because of calling the method in wrong state.
     * Before opening OutputStream the Content-Length and Content-Type headers
     * has to se set. If not SipException.UNKNOWN_LENGTH or
     * SipException.UNKNOWN_TYPE will be thrown respectively.
     * @return OutputStream to write body content
     * @throws IOException if the OutputStream can not be opened,
     * because of an I/O error occurred.
     * @throws SipException INVALID_STATE the OutputStream can not be opened
     * in this state (e.g. no message initialized).
     * UNKNOWN_LENGTH Content-Length header not set.
     * UNKNOWN_TYPE Content-Type header not set.
     */
    public OutputStream openContentOutputStream()
    throws IOException, SipException {
        if (state != INITIALIZED)
            throw new SipException("the content output strean can not be open,"
                    + " because of wrong state.",
                    SipException.INVALID_STATE);
        if (state == TERMINATED) {
            throw new IOException("can not open content output stream"
                    + " because SipServerConnection is TERMINATED");
        }

        if (response.getHeader(Header.CONTENT_TYPE) == null)
            throw new SipException("Content-Type unknown, set the"
                    + " content-type header first",
                    SipException.UNKNOWN_TYPE);
        if (response.getHeader(Header.CONTENT_LENGTH) == null)
            throw new SipException("Content-Length unknown, set the "
                    + "content-length header first",
                    SipException.UNKNOWN_LENGTH);
        contentOutputStream = new SDPOutputStream(this);
        state = STREAM_OPEN;
        return contentOutputStream;
    }

    /**
     * Closes the connection.
     * @exception IOException if an I/O error occurs
     * @see javax.microedition.io.Connection#close()
     */
    public void close() throws IOException {
        state = TERMINATED;
    }

    /**
     * Change the state of the dialog after sending a response.
     * @param serverTransaction current transaction
     */
    private void changeDialogState(ServerTransaction serverTransaction) {
        // System.out.println(">>> SERVER: changing state, " +
        //     response.getCSeqHeader().getMethod());

        int statusCode = response.getStatusCode();

        if (statusCode == 100 || sipDialog == null) {
            return;
        }

        String cseqMethod = response.getCSeqHeader().getMethod();
        SipDialogImpl sipDialogImpl = (SipDialogImpl)sipDialog;

        if (cseqMethod.equals(Request.NOTIFY)) {
            // System.out.println(">>> SERVER: NOTIFY!!!");
            sipDialogImpl.handleNotify(request,
                serverTransaction.getDialog(), null);
            return;
        }

        // Default is to not create a dialog.
        // The dialog only is created for the methods that are known
        // to establish a dialog.
        int statusGroup = statusCode / 100;

        if (sipConnectionNotifierImpl.getStackConnector().getSipStack().
                isDialogCreated(cseqMethod) || cseqMethod.equals(Request.BYE)) {
            if (statusGroup == 2) {
                // RFC 3261, section 13.2.2.4:
                // If the dialog identifier in the 2xx response matches the
                // dialog identifier of an existing dialog, the dialog MUST
                // be transitioned to the "confirmed" state.
                sipDialogImpl.setDialog(serverTransaction.getDialog());
                sipDialogImpl.setState(SipDialog.CONFIRMED);

                if (statusCode == 200) {
                    if (cseqMethod.equals(Request.SUBSCRIBE) ||
                            cseqMethod.equals(Request.REFER)) {
                        sipDialogImpl.addSubscription(new Subscription(
                            sipDialogImpl.getDialog(), request));
                    } else if (cseqMethod.equals(Request.BYE)) {
                        sipDialogImpl.setWaitForBye(false);
                        sipDialogImpl.terminateIfNoSubscriptions();
                    }
                }
            } else if (statusGroup == 1) {
                // provisional response
                if (sipDialog.getState() == SipDialogImpl.INITIALIZED) {
                   // switch to EARLY state
                   sipDialogImpl.setState(SipDialog.EARLY);
                }
            } else { // another response code - switch to TERMINATED state
                sipDialogImpl.terminateIfNoSubscriptions();
            }

            // set dialog ID if need
            if (sipDialogImpl.getDialogID() == null) {
                int state = sipDialog.getState();
                if ((state == SipDialog.EARLY) ||
                        (state == SipDialog.CONFIRMED)) {
                    sipDialogImpl.setDialog(serverTransaction.getDialog());
                    sipDialogImpl.setDialogID(response.getDialogId(true));
                }
            }
        }
    }

    /**
     * Return the state of SIP server connection
     *
     * @return state of the SIP Server Connection
     */
    public int getState() {
        return state;
    }
}
