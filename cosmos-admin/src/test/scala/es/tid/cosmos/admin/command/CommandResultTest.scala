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

package es.tid.cosmos.admin.command

import scala.concurrent.{Promise, Future}
import scala.concurrent.duration._
import scalaz.{Failure, Success}

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class CommandResultTest extends FlatSpec with MustMatchers {

  "A command result" must "be get from a successful command" in {
    CommandResult.await(Future.successful(CommandResult.success("ok"))) must
      be (CommandResult(0, "ok"))
  }

  it must "be get from a command finished with error" in {
    CommandResult.await(Future.successful(CommandResult.error(":("))) must
      be (CommandResult(-1, ":("))
  }

  it must "be get from a command with unhandled exceptions" in {
    val result = CommandResult.await(Future.failed(new Error("cannot handle something")))
    result.exitStatus must not be 0
    result.message must include ("cannot handle something")
  }

  it must "be get form a timing out command" in {
    val timeout = 1.second
    CommandResult.await(Promise().future, timeout) must
      be (CommandResult(-1, s"Command timed out after $timeout"))
  }

  it must "be get from a validation result" in {
    CommandResult.fromValidation(Success(CommandResult.success())) must be ('success)
    CommandResult.fromValidation(Failure("ko")) must not be 'success
  }
}
