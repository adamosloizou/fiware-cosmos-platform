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

package es.tid.cosmos.platform.common.scalatest.matchers

import scala.concurrent.{Await, Future}
import scala.concurrent.duration.Duration

import org.scalatest.matchers.{Matcher, MatchResult}
import java.util.concurrent.TimeoutException

trait FutureMatchers {

  def runUnder[T](timeout: Duration): Matcher[Future[T]] = new Matcher[Future[T]] {
    def apply(left: Future[T]) = MatchResult(
      matches = try {
          Await.result(left, timeout)
          true
        } catch {
          case ex: TimeoutException => false
        },
      failureMessage = s"future didn't complete in $timeout",
      negatedFailureMessage= s"future completed in $timeout"
    )
  }

  def eventually[T](matcher: Matcher[T]): Matcher[Future[T]] = new Matcher[Future[T]] {
    def apply(left: Future[T]): MatchResult = matcher.apply(Await.result(left, Duration.Inf))
  }
}
