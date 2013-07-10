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

package es.tid.cosmos.api.profile

import java.sql.Connection

import anorm._
import anorm.SqlParser._

import es.tid.cosmos.api.authorization.ApiCredentials
import es.tid.cosmos.api.controllers.pages.{NamedKey, CosmosProfile, Registration}
import es.tid.cosmos.servicemanager.ClusterId

object CosmosProfileDao {
  def registerUserInDatabase(userId: String, reg: Registration)(implicit c: Connection): Long = {
    val credentials = ApiCredentials.random()
    val cosmosId = SQL("""INSERT INTO user(user_id, handle, api_key, api_secret)
                        | VALUES ({user_id}, {handle}, {api_key}, {api_secret})""".stripMargin).on(
      "user_id" -> userId,
      "handle" -> reg.handle,
      "api_key" -> credentials.apiKey,
      "api_secret" -> credentials.apiSecret
    ).executeInsert(scalar[Long].single)
    SQL("""INSERT INTO public_key(cosmos_id, name, signature)
           VALUES ({cosmos_id}, 'default', {signature})""")
      .on("cosmos_id" -> cosmosId, "signature" -> reg.publicKey)
      .executeInsert()
    cosmosId
  }

  def getCosmosId(userId: String)(implicit c: Connection): Option[Long] =
    SQL("SELECT cosmos_id FROM user WHERE user_id = {user_id}")
      .on("user_id" -> userId)
      .as(scalar[Long].singleOpt)

  def getMachineQuota(cosmosId: Long)(implicit c: Connection): Quota =
    SQL("SELECT machine_quota FROM user WHERE cosmos_id = {cosmos_id}")
      .on("cosmos_id" -> cosmosId)
      .apply()
      .collectFirst {
        case Row(machineQuota: Option[Int]) => Quota(machineQuota)
      }
      .getOrElse(EmptyQuota)

  def lookupByUserId(userId: String)(implicit c: Connection): Option[CosmosProfile] =
    lookup(SQL("""SELECT u.cosmos_id, u.handle, u.machine_quota, u.api_key, u.api_secret, p.name,
                 | p.signature
                 | FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                 | WHERE u.user_id = {user_id}""".stripMargin)
      .on("user_id" -> userId))

  def lookupByApiCredentials(creds: ApiCredentials)(implicit c: Connection): Option[CosmosProfile] =
    lookup(SQL("""SELECT u.cosmos_id, u.handle, u.machine_quota, u.api_key, u.api_secret, p.name,
                 | p.signature
                 | FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                 | WHERE u.api_key = {key} AND u.api_secret = {secret}""".stripMargin)
      .on("key" -> creds.apiKey, "secret" -> creds.apiSecret))

  def assignCluster(clusterId: ClusterId, ownerId: Long)(implicit c: Connection) {
    SQL("INSERT INTO cluster(cluster_id, owner) VALUES ({cluster_id}, {owner})")
      .on("cluster_id" -> clusterId.toString, "owner" -> ownerId)
      .execute()
  }

  def clustersOf(cosmosId: Long)(implicit c: Connection): Seq[ClusterId] =
    SQL("SELECT cluster_id FROM cluster WHERE owner = {owner}")
      .on("owner" -> cosmosId)
      .as(str("cluster_id").map(ClusterId.apply) *)

  /**
   * Lookup Cosmos profile by a custom query.
   *
   * @param query Query with the following output columns: cosmos id, handle, apiKey, apiSecret,
   *              name and signature
   * @return      A cosmos profile or nothing
   */
  private def lookup(query: SimpleSql[Row])(implicit c: Connection): Option[CosmosProfile] = {
    val rows = query().toList
    rows.headOption.map {
      case Row(id: Int, handle: String, machineQuota: Option[Int], apiKey: String,
          apiSecret: String, _, _) => {
        val namedKeys = rows.map(row => NamedKey(row[String]("name"), row[String]("signature")))
        CosmosProfile(
          id, handle, Quota(machineQuota), ApiCredentials(apiKey, apiSecret), namedKeys: _*)
      }
    }
  }
}
