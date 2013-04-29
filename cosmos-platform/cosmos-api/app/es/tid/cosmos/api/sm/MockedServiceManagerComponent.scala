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

package es.tid.cosmos.api.sm

import es.tid.cosmos.servicemanager.{ServiceManagerComponent, ServiceManager}

/**
 * @author sortega
 */
trait MockedServiceManagerComponent extends ServiceManagerComponent {
  val serviceManager: ServiceManager = new MockedServiceManager(10000)
}
