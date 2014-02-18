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

package es.tid.cosmos.tests.e2e

import scala.language.postfixOps
import scala.sys.process._

import net.liftweb.json._
import org.scalatest.{Assertions, Informer}
import org.scalatest.matchers.MustMatchers
import org.scalatest.verb.MustVerb

class Cluster(clusterSize: Int, owner: User, services: Seq[String] = Seq())
             (implicit info: Informer)
  extends MustVerb with MustMatchers with Patience with CommandLineMatchers {

  private implicit val Formats = net.liftweb.json.DefaultFormats
  private val sshFlags = "-o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no"

  private var _id: Option[String] = None

  def id(): String = _id match {
    case Some(id) => id
    case None =>
      _id = Some(create())
      _id.get
  }

  def isListed(executedBy: User = owner): Boolean =
    (s"cosmos -c ${executedBy.cosmosrcPath} list" lines_!).exists(_.contains(id()))

  def describe(executedBy: User = owner) = parse(
    s"cosmos -c ${executedBy.cosmosrcPath} show ${id()}" !! ProcessLogger(info(_))
  )

  def state(executedBy: User = owner): Option[String] =
    (describe(executedBy) \ "state").extractOpt[String]

  def users(executedBy: User = owner): Seq[String] =
    (describe(executedBy) \ "users").toOpt match {
      case Some(users) =>
        for {
          JObject(fields) <- users
          JField("username", JString(handle)) <- fields
        } yield handle
      case None => Seq.empty
    }

  def addUser(clusterUser: String, executedBy: User = owner): Int = {
    val command = s"cosmos -c ${executedBy.cosmosrcPath} adduser ${id()} $clusterUser"
    command ! ProcessLogger(info(_))
  }

  def removeUser(clusterUser: String, executedBy: User = owner): Int = {
    val command = s"cosmos -c ${executedBy.cosmosrcPath} rmuser ${id()} $clusterUser"
    command ! ProcessLogger(info(_))
  }

  def ensureState(expectedState: String, executedBy: User = owner) {
    eventually {
      assert(
        state(executedBy) == Some(expectedState),
        s"ExpectedState [${Some(expectedState)}] not reached. Actual: [${state(executedBy)}]." +
          s"Cluster info:\n${pretty(render(describe(executedBy)))}"
      )
    }
  }

  def terminate(executedBy: User = owner) = _id match {
    case Some(clusterId) =>
      info(s"Calling terminate on cluster $clusterId")
      s"cosmos -c ${executedBy.cosmosrcPath} terminate $clusterId" ! ProcessLogger(info(_))
    case _ => ()
  }

  /** Upload a file to the cluster master using SCP */
  def scp(localFile: String, remotePath: String = "", recursive: Boolean = false, executedBy: User = owner) {
    val hostname = masterHostname(executedBy).getOrElse(fail("No master to scp to"))
    val scpFlags = if (recursive) "-r" else ""
    val command = s"scp $sshFlags $scpFlags $localFile ${executedBy.handle}@$hostname:$remotePath"
    command.! must runSuccessfully
  }

  /** Executes a command on the master node through SSH.
    * An exception is thrown if the command has nonzero return status.
    */
  def sshCommand(command: String, executedBy: User = owner) {
    val hostname = masterHostname(executedBy).getOrElse(fail("No master to ssh to"))
    s"""ssh $sshFlags ${executedBy.handle}@$hostname "$command""".! must runSuccessfully
  }

  /** Creates the cluster and returns its ID in case of success. */
  private def create(): String = {
    val availableServices = listServices(owner)
    val optionalServices = services.filter(availableServices.contains)
    val nameFlag = "--name default-services"
    val sizeFlag = s"--size $clusterSize"
    val servicesFlag = if (optionalServices.nonEmpty) s"--services ${services.mkString(" ")}" else ""
    val command = s"cosmos -c ${owner.cosmosrcPath} create $nameFlag $sizeFlag $servicesFlag"
    info(s"Calling create cluster with command '$command'")
    val commandOutput = command.lines_!.toList
    commandOutput.foreach(info(_))
    val expectedPrefix = "Provisioning new cluster "
    val id = commandOutput
      .find(_.startsWith(expectedPrefix))
      .getOrElse(fail(s"unexpected create command output: ${commandOutput.mkString("\n")}"))
      .substring(expectedPrefix.length)
    info(s"Cluster created with id $id")
    id
  }

  private def listServices(executedBy: User = owner): Seq[String] = {
    val commandOutput = s"cosmos -c ${executedBy.handle} list-services".lines_!.toSeq
    val optionalServices = commandOutput.tail.map(_.trim)
    optionalServices
  }

  private def masterHostname(executedBy: User = owner): Option[String] = (for {
    JObject(children) <- describe(executedBy)
    JField("master", JObject(masterFields)) <- children
    JField("hostname", JString(hostname)) <- masterFields
  } yield hostname).headOption
}
