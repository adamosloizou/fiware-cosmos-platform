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

package es.tid.cosmos.servicemanager.clusters

import java.net.URI

/**
 * An immutable description of a cluster
 */
case class ImmutableClusterDescription(
    override val id: ClusterId,
    override val name: String,
    override val size: Int,
    override val state: ClusterState,
    override val nameNode: Option[URI],
    override val master: Option[HostDetails],
    override val slaves: Seq[HostDetails]) extends ClusterDescription
