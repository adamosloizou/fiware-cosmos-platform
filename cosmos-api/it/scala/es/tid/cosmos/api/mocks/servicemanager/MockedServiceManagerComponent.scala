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

package es.tid.cosmos.api.mocks.servicemanager

import es.tid.cosmos.servicemanager.{ServiceManager, ServiceManagerComponent}

trait MockedServiceManagerComponent extends ServiceManagerComponent {
  lazy val serviceManager: ServiceManager = new MockedServiceManager(transitionDelay = 10000)
}
