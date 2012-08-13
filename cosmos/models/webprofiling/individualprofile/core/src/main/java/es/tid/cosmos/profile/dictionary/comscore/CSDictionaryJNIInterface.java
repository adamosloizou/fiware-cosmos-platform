/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.profile.dictionary.comscore;

/**
 * Class that defines the native methods to access the comScore dictionary API
 * via JNI.
 *
 * @author dmicol, sortega
 */
public class CSDictionaryJNIInterface implements NativeCSDictionary {
    public static final String[] DEFAULT_COMSCORE_LIBS = new String[] {
        "/opt/hadoop/lib/native/Linux-amd64-64/libcsCFD.so.1",
        "/opt/hadoop/lib/native/Linux-amd64-64/libCategories.so"
    };

    /*
     * {@inheritDoc}
     */
    @Override
    public native boolean initFromTermsInDomainFlatFile(int iMode,
            String szTermsInDomainFlatFileName);

    /*
     * {@inheritDoc}
     */
    @Override
    public native boolean loadCSDictionary(int iMode,
            String szTermsInDomainFlatFileName, String szDictionaryName);

    /*
     * {@inheritDoc}
     */
    @Override
    public native long applyDictionaryUsingUrl(String szURL);
}