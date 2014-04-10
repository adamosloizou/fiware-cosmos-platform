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

package es.tid.cosmos.servicemanager.clusters.sql.migrations

import com.imageworks.migration.Migration

class Migrate_20140409112535_AddBlockedPortsEntity extends Migration {

  val tableName = "cluster_blocked_ports"

  override def up() {
    execute(s"""CREATE TABLE $tableName (
              |  id INT NOT NULL AUTO_INCREMENT,
              |  cluster_id VARCHAR(45) NOT NULL,
              |  port INT NOT NULL,
              |  PRIMARY KEY (id),
              |  KEY fk_cluster_blocked_ports (cluster_id),
              |  CONSTRAINT fk_cluster_blocked_ports
              |     FOREIGN KEY (cluster_id) REFERENCES cluster_state (id) ON DELETE CASCADE
              |);""".stripMargin)
  }

  override def down() {
    execute(s"DROP TABLE IF EXISTS $tableName;")
  }
}
