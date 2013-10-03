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

package es.tid.cosmos.api.controllers.profile

import com.wordnik.swagger.annotations._
import play.api.libs.json.Json
import play.api.mvc.Action

import es.tid.cosmos.api.controllers.common.{ApiAuthController, JsonController, Message}
import es.tid.cosmos.api.profile.CosmosProfileDao

/**
 * Resource that represents a user profile.
 */
@Api(value = "/cosmos/v1/profile", listingPath = "/doc/cosmos/v1/profile",
  description = "Represents the user profile")
class ProfileResource(override val dao: CosmosProfileDao)
  extends JsonController with ApiAuthController {

  /**
   * Show user profile.
   */
  @ApiOperation(value = "Get the user profile details", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.profile.UserProfile")
  def show = Action { implicit request =>
    withApiAuth(request) { profile =>
      Ok(Json.toJson(UserProfile(profile.handle, profile.keys.toList)))
    }
  }

  /**
   * Update the user profile
   */
  @ApiOperation(value = "Update the user profile details", httpMethod = "PUT",
    responseClass = "es.tid.cosmos.api.controllers.profile.UserProfile")
  @ApiErrors(Array(
    new ApiError(code = 400, reason = "Invalid JSON payload"),
    new ApiError(code = 400, reason = "No defined public key"),
    new ApiError(code = 400, reason = "More than one public key"),
    new ApiError(code = 400, reason = "Trying to modify user handle")
  ))
  @ApiParamsImplicit(Array(
    new ApiParamImplicit(paramType = "body",
      dataType = "es.tid.cosmos.api.controllers.profile.UserProfile")
  ))
  def update() = JsonBodyAction[UserProfile] { (request, targetProfile) =>
    withApiAuth(request) { currentProfile =>
      if (targetProfile.keys.length != 1) badRequest("Only one public key is supported")
      else dao.withTransaction { implicit c =>
        if (currentProfile.handle != targetProfile.handle) {
          badRequest("Handle modification is not supported")
        } else {
          dao.setHandle(currentProfile.id, targetProfile.handle)
          dao.setPublicKeys(currentProfile.id, targetProfile.keys)
          Ok(Json.toJson(targetProfile))
        }
      }
    }
  }

  private def badRequest(message: String) = BadRequest(Json.toJson(Message(message)))
}
