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
package es.tid.cosmos.servicemanager.ambari

import dispatch.url
import net.liftweb.json.JsonDSL._
import org.mockito.Mockito._
import scala.concurrent.Future
import net.liftweb.json.JsonAST.JValue

class AmbariRequestTest extends AmbariTestBase {

  it must "handle instant completions" in {
    val request = new AmbariRequest(url("http://www.some.request.url.com/api/v1/request/1"))
      with FakeAmbariRestReplies with MockedRestResponsesComponent {
        addMock(
          responses.getRequest("1"),
          ("tasks" -> List(
            "Tasks" -> ("status" -> "COMPLETED")
        )))
    }
    get(request.ensureFinished)
    verify(request.responses).getRequest("1")
  }

  it must "handle waterfalled completions of multiple tasks" in {
    val request = new AmbariRequest(url("http://www.some.request.url.com/api/v1/request/1"))
      with FakeAmbariRestReplies with MockedRestResponsesComponent {
      when(responses.getRequest("1"))
        .thenReturn(
          Future.successful[JValue]("tasks" -> List(
            "Tasks" -> ("status" -> "QUEUED"),
            "Tasks" -> ("status" -> "PENDING"),
            "Tasks" -> ("status" -> "PENDING"))),
          Future.successful[JValue]("tasks" -> List(
            "Tasks" -> ("status" -> "PENDING"),
            "Tasks" -> ("status" -> "COMPLETED"),
            "Tasks" -> ("status" -> "IN_PROGRESS"))),
          Future.successful[JValue]("tasks" -> List(
            "Tasks" -> ("status" -> "COMPLETED"),
            "Tasks" -> ("status" -> "COMPLETED"),
            "Tasks" -> ("status" -> "COMPLETED"))))
    }
    get(request.ensureFinished)
    verify(request.responses, times(3)).getRequest("1")
  }

  it must "error if a single task ends up in a failed state" in {
    val request = new AmbariRequest(url("http://www.some.request.url.com/api/v1/request/1"))
      with FakeAmbariRestReplies with MockedRestResponsesComponent {
      when(responses.getRequest("1"))
        .thenReturn(
        Future.successful[JValue]("tasks" -> List(
          "Tasks" -> ("status" -> "QUEUED"),
          "Tasks" -> ("status" -> "PENDING"),
          "Tasks" -> ("status" -> "PENDING"))),
        Future.successful[JValue]("tasks" -> List(
          "Tasks" -> ("status" -> "PENDING"),
          "Tasks" -> ("status" -> "COMPLETED"),
          "Tasks" -> ("status" -> "IN_PROGRESS"))),
        Future.successful[JValue]("tasks" -> List(
          "Tasks" -> ("status" -> "PENDING"),
          "Tasks" -> ("status" -> "COMPLETED"),
          "Tasks" -> ("status" -> "TIMEDOUT"))))
    }
    evaluating {
      get(request.ensureFinished)
    } must produce [ServiceException]
    verify(request.responses, times(3)).getRequest("1")

    val request2 = new AmbariRequest(url("http://www.some.request.url.com/api/v1/request/1"))
      with FakeAmbariRestReplies with MockedRestResponsesComponent {
      when(responses.getRequest("1"))
        .thenReturn(
        Future.successful[JValue]("tasks" -> List(
          "Tasks" -> ("status" -> "QUEUED"),
          "Tasks" -> ("status" -> "PENDING"),
          "Tasks" -> ("status" -> "PENDING"))),
        Future.successful[JValue]("tasks" -> List(
          "Tasks" -> ("status" -> "PENDING"),
          "Tasks" -> ("status" -> "COMPLETED"),
          "Tasks" -> ("status" -> "IN_PROGRESS"))),
        Future.successful[JValue]("tasks" -> List(
          "Tasks" -> ("status" -> "FAILED"),
          "Tasks" -> ("status" -> "COMPLETED"),
          "Tasks" -> ("status" -> "TIMEDOUT"))))
    }
    evaluating {
      get(request2.ensureFinished)
    } must produce [ServiceException]
    verify(request2.responses, times(3)).getRequest("1")
  }

}
