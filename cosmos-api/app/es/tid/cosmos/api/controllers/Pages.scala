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

package es.tid.cosmos.api.controllers

import play.api.mvc.{Action, Controller}

import es.tid.cosmos.api.controllers.cosmos.CosmosResource

/**
 * Controller for the web pages of the service.
 */
trait Pages extends Controller {
  def index = Action { implicit request =>
    val baseUrl: String = routes.Application.version().absoluteURL(secure = false)
    Ok(views.html.index(baseUrl, CosmosResource.apiVersion))
  }
}
