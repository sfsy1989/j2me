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

package examples;

import javax.microedition.io.*;
import javax.microedition.midlet.MIDlet;
import javax.microedition.lcdui.*;
import javax.microedition.sip.*;

/**
 * Attempt various push connection registrations
 * with static push registrations.
 */
public class SipPushTest extends MIDlet 
    implements SipServerConnectionListener, CommandListener {

    /** Array of active connections. */
    SipConnectionNotifier[] scn;
    /** Array of URL connections. */
    String[] connections;

    /** Terminate MIDlet button. */
    Command exitCommand;
    /** Message array for user communication. */
    TextBox textBox;
    /** Current user interface handle. */
    Display display;

    /**
     * Constructs a messgae array and quit  button and
     * displays message to end user.
     * @param s the string to present to the user.
     */
    public void updateScreen(String s) {
	exitCommand = new Command("Exit", Command.EXIT, 1);
	textBox = new TextBox("SipPushTest", s, s.length(),
			      TextField.ANY | TextField.UNEDITABLE);
	textBox.addCommand(exitCommand);
	textBox.setCommandListener(this);

	display = Display.getDisplay(this);
	display.setCurrent(textBox);
    }

    /**
     * Launches the MIDlet.
     */
    public void startApp() {
	
	// Attempt dynamic registration with port
	try {
	    PushRegistry
		.registerConnection("sip:5081",
				    "examples.SipPushTest",
				    "*");
	} catch (Exception ex) { 
	    ex.printStackTrace();
	    updateScreen(ex.getMessage());
	}
	try {
	    PushRegistry
		.unregisterConnection("sip:5081");
	} catch (Exception ex) { 
	    ex.printStackTrace();
	    updateScreen(ex.getMessage());
	}

	// Attempt dynamic registration with port and transport
	try {
	    PushRegistry
		.registerConnection("sip:5081;transport=udp",
				    "examples.SipPushTest",
				    "*");
	} catch (Exception ex) { 
	    ex.printStackTrace();
	    updateScreen(ex.getMessage());
	}
	try {
	    PushRegistry
		.unregisterConnection("sip:5081;transport=udp");
	} catch (Exception ex) { 
	    ex.printStackTrace();
	    updateScreen(ex.getMessage());
	}

	// Verify dynamic registration with port and transport=tcp
	// is blocked.
	try {
	    PushRegistry
		.registerConnection("sip:5081;transport=tcp",
				    "examples.SipPushTest",
				    "*");
	} catch (Exception ex) { 
	    // Should fail - "transport=tcp" not supported
	}

	// Verify dynamic registration for sips is blocked
	try {
	    PushRegistry
		.registerConnection("sips:5081",
				    "examples.SipPushTest",
				    "*");
	} catch (Exception ex) { 
	    // Should fail - "sips" not supported
	}

	// Check the list of pending connections
	connections = PushRegistry.listConnections(false);
	if (connections != null &&
	    connections.length > 0) {
	    scn = new SipConnectionNotifier[connections.length];
	    for (int i = 0; i < connections.length; i++) {
		
		// Open connection for the designated push registration
		try { 
		    scn[i] = (SipConnectionNotifier)
			Connector.open(connections[i]);
		    // Register callback method
		    scn[i].setListener(this);
		} catch (Exception ex) {
		    // handle Exceptions
		    ex.printStackTrace();
		    updateScreen(ex.getMessage());
		}
	    }
	}
    }
    
    /**
     * Callback listener for inbound messages.
     * @param scn the accepted inbound connection handle.
     */
    public void notifyRequest(SipConnectionNotifier scn) {
	try {
	    SipServerConnection ssc;
	    // retrieve the request received
	    ssc = scn.acceptAndOpen();
	    
	    // Check for NOTIFY message
	    if (ssc.getMethod().equals("NOTIFY")) {
		updateScreen(ssc.getMethod());
	    } else {
		updateScreen(ssc.getMethod());
	    }
	} catch (Exception ex) {
	    // handle Exceptions
            ex.printStackTrace();
	    updateScreen(ex.getMessage());
	}
    }
    /** Pauses the MIDlet. */
    public void pauseApp() { }

    /**
     * Destroys the MIDlet and cleans up all opened
     * resources.
     * @param unconditional flag indicating that
     * the MIDlet will be destroyed 
     */
    public void destroyApp(boolean unconditional) {
	for (int i = 0; i < connections.length; i++) {
	    if (scn[i] != null) {
		try {
		    scn[i].close();
		} catch (Exception ex) {
		}
	    }
	}
    }
    /**
     * Callback for user interface interactions.
     * @param command the user interface element
     * @param screen the context for the selected
     * user interface element.
     */
    public void commandAction(Command command, Displayable screen) {
	if (command == exitCommand) {
	    destroyApp(false);
	    notifyDestroyed();
	}
    }
}

