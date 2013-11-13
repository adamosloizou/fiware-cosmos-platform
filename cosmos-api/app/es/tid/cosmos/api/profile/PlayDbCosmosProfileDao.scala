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

import java.sql.{SQLException, Connection}
import java.util.Date

import anorm._
import anorm.SqlParser._
import play.api.db.DB
import play.api.Play.current

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.servicemanager.ClusterId

trait PlayDbCosmosProfileDaoComponent extends CosmosProfileDaoComponent {
  lazy val cosmosProfileDao: CosmosProfileDao = new PlayDbCosmosProfileDao
}

class PlayDbCosmosProfileDao extends CosmosProfileDao {
  type Conn = Connection

  def withConnection[A](block: (Conn) => A): A = DB.withTransaction[A](block)
  def withTransaction[A](block: (Conn) => A): A = DB.withTransaction[A](block)

  private val toGroup: PartialFunction[Row, Group] = {
    case Row(name: String, minimumQuota: Option[_]) =>
      GuaranteedGroup(name, Quota(minimumQuota.asInstanceOf[Option[Int]]))
  }

  override def registerUserInDatabase(userId: UserId, reg: Registration, group: Group, quota: Quota)(implicit c: Conn): CosmosProfile = {
    val apiCredentials = ApiCredentials.random()
    val defaultKey = NamedKey("default", reg.publicKey)
    val cosmosId = SQL(
      """INSERT INTO user(auth_realm, auth_id, handle, api_key, api_secret, group_name)
        | VALUES ({auth_realm}, {auth_id}, {handle}, {api_key}, {api_secret}, {group_name})"""
        .stripMargin)
      .on(
        "auth_realm" -> userId.realm,
        "auth_id" -> userId.id,
        "handle" -> reg.handle,
        "api_key" -> apiCredentials.apiKey,
        "api_secret" -> apiCredentials.apiSecret,
        "group_name" -> dbGroupName(group)
      ).executeInsert(scalar[Long].single)
    addPublicKey(cosmosId, defaultKey)
    CosmosProfile(
      id = cosmosId,
      handle = reg.handle,
      group,
      quota,
      apiCredentials = apiCredentials,
      keys = Seq(defaultKey)
    )
  }

  override def getCosmosId(userId: UserId)(implicit c: Conn): Option[Long] =
    SQL("SELECT cosmos_id FROM user WHERE auth_realm = {realm} AND auth_id = {id}")
      .on("realm" -> userId.realm, "id" -> userId.id)
      .as(scalar[Long].singleOpt)

  override def getMachineQuota(cosmosId: Long)(implicit c: Conn): Quota =
    SQL("SELECT machine_quota FROM user WHERE cosmos_id = {cosmos_id}")
      .on("cosmos_id" -> cosmosId)
      .apply()
      .collectFirst {
        case Row(machineQuota: Option[_]) => Quota(machineQuota.asInstanceOf[Option[Int]])
      }
      .getOrElse(EmptyQuota)

  override def setMachineQuota(cosmosId: Long, quota: Quota)(implicit c: Conn): Boolean = {
    val quotaValue = quota match {
      case UnlimitedQuota => "NULL"
      case EmptyQuota => "0"
      case FiniteQuota(limit) => limit.toString
    }
    SQL("UPDATE user SET machine_quota = {machine_quota}")
      .on("machine_quota" -> quotaValue)
      .executeUpdate() > 0
  }

  override def handleExists(handle: String)(implicit c: Conn): Boolean = {
    val usersWithThatHandle = SQL("SELECT count(*) from user WHERE handle = {handle}")
      .on("handle" -> handle)
      .as(scalar[Long].single)
    usersWithThatHandle > 0
  }

  override def lookupByUserId(userId: UserId)(implicit c: Conn): Option[CosmosProfile] =
    lookup(SQL("""SELECT u.cosmos_id, u.handle, u.machine_quota, u.api_key, u.api_secret,
                 | u.group_name,
                 | p.name, p.signature
                 | FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                 | WHERE u.auth_realm = {realm} AND u.auth_id = {id}""".stripMargin)
      .on("realm" -> userId.realm, "id" -> userId.id))

  override def lookupByApiCredentials(creds: ApiCredentials)(implicit c: Conn): Option[CosmosProfile] =
    lookup(SQL("""SELECT u.cosmos_id, u.handle, u.machine_quota, u.api_key, u.api_secret,
                 | u.group_name,
                 | p.name, p.signature
                 | FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                 | WHERE u.api_key = {key} AND u.api_secret = {secret}""".stripMargin)
      .on("key" -> creds.apiKey, "secret" -> creds.apiSecret))

  override def lookupByGroup(group: Group)(implicit c: Conn): Set[CosmosProfile] =
    lookupMany(SQL(s"""SELECT u.cosmos_id, u.handle, u.machine_quota, u.api_key, u.api_secret,
                       | u.group_name,
                       | p.name, p.signature
                       | FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                       | WHERE u.group_name ${equalsOp(dbGroupName(group))} {groupName}"""
      .stripMargin).on("groupName" -> dbGroupName(group))).toSet


  override def getGroups(implicit c: Conn): Set[Group] = {
    val registeredGroups = SQL("SELECT name, min_quota FROM user_group").apply().collect(toGroup)
    Set[Group](NoGroup) ++ registeredGroups
  }

  private def getGroup(maybeName: Option[String])(implicit c: Conn): Group = {
    val maybeGroup = maybeName.flatMap(name =>
      SQL("""SELECT name, min_quota FROM user_group WHERE name = {name}""".stripMargin)
        .on("name" -> name).apply().collectFirst(toGroup))

    maybeGroup.getOrElse(NoGroup)
  }

  override def registerGroup(group: Group)(implicit c: Conn) {
    SQL("""INSERT INTO user_group(name, min_quota)
          | VALUES ({name}, {min_quota})""".stripMargin).on(
      "name" -> group.name,
      "min_quota" -> group.minimumQuota.toInt
    ).execute()
  }

  override def assignCluster(assignment: ClusterAssignment)(implicit c: Conn) {
    SQL("""INSERT INTO cluster(cluster_id, owner, creation_date)
          | VALUES ({cluster_id}, {owner}, {creation_date})""".stripMargin).on(
      "cluster_id" -> assignment.clusterId.toString,
      "owner" -> assignment.ownerId,
      "creation_date" -> assignment.creationDate
    ).execute()
  }

  override def clustersOf(cosmosId: Long)(implicit c: Conn): Seq[ClusterAssignment] =
    SQL("SELECT cluster_id, creation_date FROM cluster WHERE owner = {owner}")
      .on("owner" -> cosmosId)
      .apply() collect {
        case Row(clusterId: String, creationDate: Date) =>
          ClusterAssignment(ClusterId(clusterId), cosmosId, creationDate)
      }

  /**
   * Lookup Cosmos profile by a custom query.
   *
   * @param query Query with the following output columns: cosmos id, handle, apiKey, apiSecret,
   *              name and signature
   * @return      A cosmos profile or nothing
   */
  private def lookup(query: SimpleSql[Row])(implicit c: Conn): Option[CosmosProfile] = {
    val rowsForOneProfile = query().toList
    toCosmosProfile(rowsForOneProfile)
  }

  private def lookupMany(query: SimpleSql[Row])(implicit c: Conn): Seq[CosmosProfile] = {
    val allRows = query().toList
    val rowsByProfileId = allRows.groupBy(row => row[Int]("cosmos_id"))
    val maybeProfiles = for (rows <- rowsByProfileId.values) yield toCosmosProfile(rows)
    maybeProfiles.flatten.toSeq
  }

  private def toCosmosProfile(rows: List[Row])(implicit c: Conn): Option[CosmosProfile] =
    rows.headOption.map {
      case Row(id: Int, handle: String, machineQuota: Option[_], apiKey: String,
      apiSecret: String, groupName: Option[_], _, _ ) => {
        val namedKeys = rows.map(row => NamedKey(row[String]("name"), row[String]("signature")))
        CosmosProfile(
          id, handle,
          getGroup(groupName.asInstanceOf[Option[String]]),
          Quota(machineQuota.asInstanceOf[Option[Int]]),
          ApiCredentials(apiKey, apiSecret), namedKeys.toSeq)
      }
    }

  override def setHandle(id: Long, handle: String)(implicit c: Conn) {
    val updatedRows = try {
      SQL("UPDATE user SET handle = {handle} WHERE cosmos_id = {id}")
        .on(
          "handle" -> handle,
          "id" -> id
        ).executeUpdate()
    } catch {
      case ex: SQLException => throw new IllegalArgumentException("Cannot change handle", ex)
    }
    require(updatedRows > 0, s"No handle was updated for user with id=$id")
  }

  override def setPublicKeys(id: Long, publicKeys: Seq[NamedKey])(implicit c: Conn) {
    try {
      SQL("DELETE FROM public_key WHERE cosmos_id = {id}").on("id" -> id).executeUpdate()
      publicKeys.foreach(key => addPublicKey(id, key))
    } catch {
      case ex: SQLException => throw new IllegalArgumentException(
        s"Cannot change public keys. Does user with id=$id exists?", ex)
    }
  }

  private def addPublicKey(cosmosId: Long, publicKey: NamedKey)(implicit c: Conn) =
    SQL("""INSERT INTO public_key(cosmos_id, name, signature)
         | VALUES ({cosmos_id}, {name}, {signature})""".stripMargin)
    .on(
      "cosmos_id" -> cosmosId,
      "name" -> publicKey.name,
      "signature" -> publicKey.signature
    )
    .executeInsert()

  private def dbGroupName(group: Group): Option[String] = group match {
    case NoGroup => None
    case _ => Some(group.name)
  }

  /**
   * Utility method for changing the operator in SQL statements when using optional values
   * that convert None to NULL in SQL.
   * <br/>
   * e.g
   *
   * {{{select * from user where group_name IS NULL}}}
   *
   * @param maybe the optional value. In case of None the operator will be 'IS', '=' otherwise
   * @return the operator to be used
   */
  private def equalsOp(maybe: Option[_]): String = maybe match {
    case None => "IS"
    case _ => "="
  }
}
