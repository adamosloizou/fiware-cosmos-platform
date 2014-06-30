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

package es.tid.cosmos.infinity.common.credentials

/** The credentials used by a Infinity client to authenticate a request. */
sealed trait Credentials

/** The credentials used by users to authenticate their Infinity FS requests. */
case class UserCredentials(apiKey: String, apiSecret: String) extends Credentials

/** The credentials used by clusters to authenticate their Infinity FS requests. */
case class ClusterCredentials(clusterSecret: String) extends Credentials
