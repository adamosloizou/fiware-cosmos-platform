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

package es.tid.cosmos.api.profile.dao.sql

import es.tid.cosmos.api.profile.dao.{CosmosProfileDaoComponent, CosmosDao}

trait PlayDbCosmosDaoComponent extends CosmosProfileDaoComponent {
  lazy val cosmosProfileDao: CosmosDao = new PlayDbCosmosDao
}
