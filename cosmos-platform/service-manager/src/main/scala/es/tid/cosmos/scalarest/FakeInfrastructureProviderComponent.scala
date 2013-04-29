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

package es.tid.cosmos.scalarest

import es.tid.cosmos.platform.manager.ial.{InfrastructureProviderComponent, InfrastructureProvider}

/**
 * @author sortega
 */
trait FakeInfrastructureProviderComponent extends InfrastructureProviderComponent {
  def infrastructureProvider: InfrastructureProvider = new FakeInfrastructureProvider
}
