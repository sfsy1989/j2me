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

package com.sun.j2me.payment;

import com.sun.midp.midlet.MIDletStateHandler;
import com.sun.midp.midlet.MIDletSuite;
import com.sun.midp.midletsuite.MIDletSuiteImpl;
import com.sun.midp.util.DummyMIDlet;
import javax.microedition.midlet.MIDlet;
import javax.microedition.payment.*;
import com.sun.midp.security.SecurityToken;
import com.sun.midp.i3test.*;
import com.sun.j2me.payment.*;
import java.io.IOException;


/**
 * TestTransactionStore is a class for Transaction store testing
 *
 * @version 1.0
 */
public class TestTransactionStore extends TestCase {

    /**
     * number of application to be simulated
     */
    private static final int numOfApp = 15;

    /**
     * Store to be tested
     */
    private TransactionStore store;

    /**
     * Dummy transaction processor
     */
    private TransactionProcessor transProc;

    /**
     * Dummy transaction module
     */
    private TransactionModuleImpl transMod;

    /**
     * SecurityToken for file operation
     */
    private SecurityToken token;

    /**
     * Maximum records per application
     */
    private static int maxRecPerApp =
        CldcTransactionStoreImpl.MISSED_TRANSACTIONS_LIMIT;

    /**
     * Stored transaction for verification
     */
    private Transaction[][] transVector =
        new Transaction[numOfApp][maxRecPerApp];

    /**
     * maximum number of simulated application
     */
    private int maxNumOfApp = numOfApp;

    /**
     * Array of application IDs
     */
    private int[] appIDArr = new int[numOfApp];


    /**
     * Try to init CldcTransactionStore
     *
     * @throws Exception if it is impossible to init CldcTransactionStore
     */
    private void PrepareEnv() throws Exception {
        token = getSecurityToken();

        MIDletStateHandler msHandler =
            MIDletStateHandler.getMidletStateHandler();
        MIDletSuite suite = msHandler.getMIDletSuite();
        suite.setTempProperty(token, MIDletSuiteImpl.SUITE_NAME_PROP,
                              "DummySuite");

        // add dummy payment info for CldcTransactionModuleImpl init
        suite.setTempProperty(token, "Pay-Version", "1.0");
        suite.setTempProperty(token, "Pay-Adapters", "PPSMS");
        suite.setTempProperty(token, "MIDlet-Permissions",
                              "javax.microedition.payment.process.jpp");
        suite.setTempProperty(token, "Pay-Update-Stamp",
                              "2004-11-15 02:00+01:00");
        suite.setTempProperty(token, "Pay-Update-URL",
                              "http://127.0.0.1/thisgame.manifest.jpp");
        suite.setTempProperty(token, "Pay-Providers", "SMS1");
        suite.setTempProperty(token, "Pay-Cache", "no");
        suite.setTempProperty(token, "Pay-Feature-0", "0");
        suite.setTempProperty(token, "Pay-SMS1-Info", "PPSMS, EUR,"
                              + System.getProperty("payment.mcc")
                              + ","
                              + System.getProperty("payment.mnc"));
        suite.setTempProperty(token, "Pay-SMS1-Tag-0",
                              "1.20, 9990000, 0x0cba98765400");

        // Start a new instance of DummyMIDlet
        msHandler.startMIDlet("com.sun.midp.util.DummyMIDlet",
                              "DummyMIDlet");

        // Wait for async request to be processed
        int i = 1000;
        while (DummyMIDlet.midlet == null && i > 0) {
            try {
                Thread.sleep(100);
                i--;
            } catch (InterruptedException ie) {}
        }

        if (DummyMIDlet.midlet == null)
            throw new Exception("Cann't create DummyMIDlet");

        // PPSMS adapter accepts null cinfiguration string
        transProc = PPSMSAdapter.getInstance(null);
        transMod = (TransactionModuleImpl)new CldcTransactionModuleImpl(
            DummyMIDlet.midlet);
        store = new CldcTransactionStoreImpl(token);
        store.cleanUp();
    }

    /**
     * Starts N threads to test
     * <code>reserve</code> and <code>addTransaction</code> 
     * functions simultaneously
     *
     * @throws Exception if something goes wrong with store
     */
    private void WriteTest() throws Exception {
        int i = 0;
	TestThread[] write2app = new TestThread[numOfApp];
        for (i = 0; i < numOfApp; i++) {
            /* Start thread to open Transaction store */
            write2app[i] = new TestThread(this, i);
            write2app[i].start();
        }

        // Make sure all thread are done before proceeding to the next test.
        for (i = 0; i < numOfApp; i++) {
            write2app[i].join();
        }
    }

    /**
     * Written data verification
     *
     * @throws Exception if something goes wrong with store
     */
    private void Verification() throws Exception {
        store = null;

        // for future tests. For now it is not important
        System.gc();
        store = new CldcTransactionStoreImpl(token);
        TransactionRecord[] transRec;
        int transID = 0;
        int appID;
        Transaction trans = null;

        // checking for stored records and setDelivering functions
        for (int i = 0; i < maxNumOfApp; i++) {
            appID = appIDArr[i];
            transRec = store.getMissedTransactions(appID);
            assertEquals(
                "Number of stored records is different from expected one",
                transRec.length, maxRecPerApp);
            for (int j = 0; j < maxRecPerApp; j++) {
                trans = transVector[i][j];
                assertEquals(
                    "Critical error: FeatureId is different",
                    transRec[j].getFeatureID(),
                    trans.getFeatureID());
                assertEquals(
                    "Critical error: TransactionID is different",
                    transRec[j].getTransactionID(),
                    trans.getTransactionID());
                assertEquals("Critical error: State is different",
                             transRec[j].getState(),
                             trans.getState());

                // NOTE: max number of record to store is
                // PASSED_TRANSACTIONS_LIMIT
                store.setDelivered(transRec[j].getTransactionID());
            }
        }

        // should we notify about it?
        //  fail("Payload was not stored");

        // checking for passed records and functions
        int passedNum = 0;
        int appSize;
        for (int i = 0; i < maxNumOfApp; i++) {
            appID = appIDArr[i];
            assertNull("Problem with setDelivered",
                       store.getMissedTransactions(appID));
            transRec = store.getPassedTransactions(appID);
            if (transRec != null) {
                passedNum += transRec.length;
                for (int j = 0; j < transRec.length; j++) {
                    assertTrue(
                        "Passed transaction was not delivered",
                        transRec[j].wasMissed() != true);
                }
            }
            appSize = store.getSizeUsedByApplication(appID);
            store.removeApplicationRecords(appID);
            assertTrue("Problem with removeApplicationREcords",
                       appSize ==
                       store.getSizeUsedByApplication(appID));
        }

        if (passedNum > CldcTransactionStoreImpl.PASSED_TRANSACTIONS_LIMIT) {
            fail("passedNum is greater then " +
                 "CldcTransactionStoreImpl.PASSED_TRANSACTIONS_LIMIT");
        } else if (passedNum > maxNumOfApp * maxRecPerApp) {
            fail("passedNum is less then  maxNumOfApp * maxRecPerApp");
        }
        store.cleanUp();
    }

    /**
     * Written data verification. removeMissedTransaction test
     *
     * @throws Exception if something goes wrong with store
     */
    private void Verification2() throws Exception {
        CldcTransactionStoreImpl store = new CldcTransactionStoreImpl(token);
        TransactionRecord[] transRec, transRec2;
        int transID = 0;
        int appID;
        Transaction trans = null;

        // checking for stored records and setDelivering functions
        for (int i = 0; i < maxNumOfApp; i++) {
            appID = appIDArr[i];
            transRec = store.getMissedTransactions(appID);
            assertEquals(
                "Number of stored records is different from expected one",
                transRec.length, maxRecPerApp);
            store.removeMissedTransaction(appID);
            transRec2 = store.getMissedTransactions(appID);
            assertNull("Problem with removeMissedTransaction()", transRec2);
        }
        store.cleanUp();
    }

    /**
     * Local thread to perform addTransaction operation for single application
     */
    private class TestThread extends Thread {
        /** reference to the calling class */
        private TestTransactionStore master;
        /** application Number */
        private int appNum;

        /**
         * Constructs thread for RMS polling
         *
         * @param master the reference to Transaction Store
         */
        TestThread(TestTransactionStore master, int appNum) {
            this.master = master;
            this.appNum = appNum;
        }

        /**
         * Start thread
         */
        public void run() {
            try {
                WriteTest();
            } catch (Exception e) {
            }
        }

        /**
         * <code>reserve</code> and <code>addTransaction</code> test
         *
         * @throws Exception if something goes wrong with store
         */
        private void WriteTest() throws Exception {

            int j = 0;
            Transaction trans = null;
            int transID = 0;
            byte[] payload;
            int appID;
            appID = master.store.getNextApplicationID();
            master.appIDArr[appNum] = appID;
            try {
                for (j = 0; j < TestTransactionStore.maxRecPerApp; j++) {
                    payload = new byte[appNum * j + 1];
                    trans = new Transaction(master.transProc, master.transMod,
                                            j,
                                            "Feature " + j,
                                            "Description " + j,
                                            payload);
                    transID = master.store.reserve(appID, trans);
                    trans.setTransactionID(transID);
                    master.store.addTransaction(trans);
                    master.transVector[appNum][j] = trans;
                }
            } catch (IOException e) {
                if (appNum == 0 && j == 0) {
                    throw e;
                } else {
                    master.maxNumOfApp = appNum;
                }
            }
            // addTransaction without reserve should throw exception
            boolean exc = false;
            try {
                trans = new Transaction(master.transProc, master.transMod,
                                    j,
                                    "Feature " + j,
                                    "Description " + j,
                                    null);
                trans.setTransactionID(transID + 1000);
                master.store.addTransaction(trans);
            } catch (IllegalStateException e) {
                exc = true;
            }
            assertTrue("addTransaction without reserve", exc);
        }

    }

    /**
     * Run tests
     */
    public void runTests() {
        boolean fail;
        try {
            declare("Prepare enviroment");
            PrepareEnv();
            assertTrue(true);

            declare("Write test");
            WriteTest();
            assertTrue(true);

            declare("Store verification");
            Verification();
            assertTrue(true);

            declare("Write test 2");
            WriteTest();
            assertTrue(true);

            declare("Store verification 2");
            Verification2();
            assertTrue(true);

        }
        catch (Exception e) {
            fail("test throws exception");
            e.printStackTrace();
        } finally {
            DummyMIDlet.midlet.notifyDestroyed();
        }
    }
}
