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

package es.tid.cosmos.platform.injection.server;

import java.io.File;

import static junit.framework.Assert.assertTrue;
import org.junit.*;

/**
 * InjectionServerTest
 *
 * @author logc
 * @since 15/05/12
 */
public class InjectionServerTest {
    private InjectionServer instance;

    @Before
    public void setUp() throws Exception {
        Configuration configuration = new Configuration(
                InjectionServerMain.class
                        .getResource("/injection_server.dev.properties"));
        this.instance = new InjectionServer(configuration);
    }

    @Test
    public void testSetupSftpServer() throws Exception {
        assertTrue(this.instance instanceof InjectionServer);
        // No exceptions thrown
        this.instance.setupSftpServer();
    }
}
