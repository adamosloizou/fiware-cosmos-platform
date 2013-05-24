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

package es.tid.cosmos.api.controllers.pages

import dispatch.{Future => _, _}, Defaults._
import play.Logger
import play.api.Play.current
import play.api.data.Form
import play.api.data.Forms._
import play.api.data.validation.Constraints._
import play.api.db.DB
import play.api.libs.json.Json
import play.api.mvc.{RequestHeader, Action, Controller}

import _root_.controllers.{routes => rootRoutes}
import es.tid.cosmos.api.controllers.common.Message
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.oauth2.OAuthError.UnauthorizedClient
import es.tid.cosmos.api.oauth2.{UserProfile, OAuthClient, OAuthError, OAuthException}

/**
 * Controller for the web pages of the service.
 */
class Pages(oauthClient: OAuthClient) extends Controller {
  private val registrationForm = Form(mapping(
    "handle" -> text.verifying(minLength(3), pattern("^[a-zA-Z][a-zA-Z0-9]*$".r,
      error="Not a valid unix handle, please use a-z letters and numbers " +
        "in a non-starting position")),
    "publicKey" -> text.verifying(nonEmpty)
  )(Registration.apply)(Registration.unapply))

  def index = Action { implicit request =>
    if (!session.isAuthenticated) landingPage
    else if (!session.isRegistered) registrationPage(session.userProfile.get, registrationForm)
    else userProfile
  }

  def swaggerUI = Action { implicit request =>
    val docBaseUrl = rootRoutes.ApiHelpController.getResources().absoluteURL(secure = false)
    Ok(views.html.swaggerUI(docBaseUrl))
  }

  def authorize(maybeCode: Option[String], maybeError: Option[String]) = Action { implicit request =>
    (maybeCode, maybeError.flatMap(OAuthError.valueOf)) match {
      case (_, Some(error)) => unauthorizedPage(OAuthException(error))
      case (Some(code), _) => Async {
        (for {
          token <- oauthClient.requestAccessToken(code)
          userProfile <- oauthClient.requestUserProfile(token)
          maybeCosmosId = DB.withConnection { implicit c => ProfileDao.getCosmosId(userProfile.id) }
        } yield {
          Logger.info(s"Authorized with token $token")
          Redirect(routes.Pages.index()).withSession(session
            .setToken(token)
            .setUserProfile(userProfile)
            .setCosmosId(maybeCosmosId))
        }) recover {
          case Wrapped(Wrapped(ex: OAuthException)) => unauthorizedPage(ex)
        }
      }
      case _ => BadRequest(Json.toJson(Message("Missing code")))
    }
  }

  def registerUser = Action { implicit request =>
    session.userProfile.map(userProfile =>
      registrationForm.bindFromRequest().fold(
        formWithErrors => registrationPage(userProfile, formWithErrors),
        registration => {
          val newUserId: Long = DB.withConnection { implicit c =>
            ProfileDao.registerUserInDatabase(userProfile.id, registration)
          }
          Redirect(routes.Pages.index()).withSession(session.setCosmosId(newUserId))
        })
    ).getOrElse(
      Forbidden(Json.toJson(Message("Not authenticated")))
    )
  }

  def logout() = Action { request =>
    Redirect(routes.Pages.index()).withNewSession
  }

  private def landingPage(implicit request: RequestHeader) = {
    Ok(views.html.landingPage(oauthClient.signUpUrl, authenticateUrl))
  }

  private def registrationPage(profile: UserProfile, form: Form[Registration]) = {
    val contents = views.html.registration(profile, form)
    if (form.hasErrors) BadRequest(contents)
    else Ok(contents)
  }

  private def userProfile(implicit request: RequestHeader) = {
    DB.withConnection { implicit c =>
      val userProfile = session.userProfile.get
      Ok(views.html.profile(userProfile, ProfileDao.lookupByUserId(userProfile.id).get))
    }
  }

  private def unauthorizedPage(ex: OAuthException)(implicit request: RequestHeader) = {
    val formattedDescription = ex.description.map(d => s", description: $d").getOrElse("")
    Logger.error("OAuth request failed" + formattedDescription, ex)
    val message = if (ex.error == UnauthorizedClient) "Access denied" else "Authorization failed"
    val body = views.html.landingPage(oauthClient.signUpUrl, authenticateUrl, Some(message))
    Unauthorized(body).withNewSession
  }

  private def authenticateUrl(implicit request: RequestHeader) =
    oauthClient.authenticateUrl(routes.Pages.authorize(None, None).absoluteURL(secure = false))

  /**
   * Extractor of exception causes.
   */
  private object Wrapped {
    def unapply(ex: Throwable): Option[Throwable] =
      if (ex == null) None
      else if (ex.getCause == null) None
      else Some(ex.getCause)
  }
}
