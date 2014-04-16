/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.common.scalatest.matchers

import scalaz._

import org.scalatest.Assertions.convertToEqualizer
import org.scalatest.matchers.{MatchResult, Matcher}

/**
 * Mixin trait for asserting Scalaz validations.
 */
trait ValidationMatchers {

  /**
   * Matcher asserts that the validation has been successful.
   * <br/>
   * e.g
   * {{{
   *   val validatedSize: ValidationNel[String, Int] = ???
   *   validatedSize must beSuccessful
   * }}}
   * @return the matcher
   */
  def beSuccessful = Matcher { (left: ValidationNel[_, _]) =>
    MatchResult(left.isSuccess, s"$left has failures", s"$left is successful")
  }

  /**
   * Matcher that both asserts that the validation was successful and that the value produced
   * was as expected.<br/>
   * e.g.
   * {{{
   *   val validatedSize: ValidationNel[String, Int] = ???
   *   validatedSize must haveValidValue(1984)
   * }}}
   *
   * @param expected the expected value produced by the validation
   * @tparam T       the value type
   * @return         the matcher
   */
  def haveValidValue[T](expected: T) = Matcher { (left: ValidationNel[_, T]) =>
    def value(validation: ValidationNel[_, T]) = validation.toOption

    MatchResult(
      left.isSuccess && value(left) == Some(expected),
      s"Value did not match expected. ${Some(expected) === value(left)}",
      "Value matched"
    )
  }

  /**
   * Matcher that both asserts that the validation has failed and that the failures are
   * as expected.<br/>
   * e.g.
   * {{{
   *   val validatedSize: ValidationNel[String, Int] = ???
   *   validatedSize must haveFailures("1st failure message", "2nd failure message")
   * }}}
   *
   * @param expected the expected failures
   * @tparam T       the type of failures
   * @return         the matcher
   */
  def haveFailures[T](expected: T*) = Matcher { (left: ValidationNel[T, _]) =>
    def failures(validation: ValidationNel[T, _]): List[T] =
      validation.swap.map(_.list).getOrElse(Nil)

    MatchResult(
      left.isFailure && failures(left) == expected,
      s"Failures did not match. ${expected === failures(left)}",
      "Failures matched"
    )
  }

  def haveFailure[T](expected: T) = Matcher { (left: Validation[T, _]) =>
    val failure = left.swap.toOption.get

    MatchResult(
      left.isFailure && failure == expected,
      s"Failure did not match. ${expected === failure}",
      "Failures matched"
    )
  }
}
