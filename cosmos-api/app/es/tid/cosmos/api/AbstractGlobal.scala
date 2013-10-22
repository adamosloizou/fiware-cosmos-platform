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

package es.tid.cosmos.api

import scala.concurrent.Future

import play.api.{Logger, GlobalSettings}
import play.api.http.MimeTypes.{JSON, HTML}
import play.api.libs.json.Json
import play.api.mvc.Results._
import play.api.mvc.{SimpleResult, RequestHeader, Controller}

import es.tid.cosmos.api.controllers.Application
import es.tid.cosmos.api.controllers.common.ErrorMessage
import es.tid.cosmos.api.controllers.cosmos.routes

/**
 * Custom global Play! settings to override controller instantiation.
 */
abstract class AbstractGlobal(val application: Application) extends GlobalSettings {
  override def getControllerInstance[A](controllerClass: Class[A]): A = {
    application.controllers(controllerClass.asInstanceOf[Class[Controller]]).asInstanceOf[A]
  }

  override def onError(request: RequestHeader, ex: Throwable): Future[SimpleResult] = {
    val message = ErrorMessage("Uncaught exception", ex)
    Logger.error(message.error, ex)
    val result = responseType(request) match {
      case HTML => InternalServerError(views.html.error(message))
      case JSON => InternalServerError(Json.toJson(message))
    }
    Future.successful(result)
  }

  private def responseType(request: RequestHeader) = {
    val defaultType = defaultResponseType(request.path)
    if (request.accepts(defaultType)) defaultType
    else if (request.accepts(HTML)) HTML
    else JSON
  }

  private def defaultResponseType(path: String) = {
    val apiBase = routes.CosmosResource.version().url
    if (path.startsWith(apiBase)) JSON else HTML
  }
}
