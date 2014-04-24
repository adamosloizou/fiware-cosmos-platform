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

package es.tid.cosmos.infinity.common.messages.json

import net.liftweb.json.{Serializer, DefaultFormats}

/** Configuration to make the Lift JSON API understand non-primitive types used
  * on our domain classes.
  *
  * It should be made available as an implicit value.
  */
private[json] object JsonFormats extends DefaultFormats {

  override def dateFormatter = Rfc822DateFormat

  override val customSerializers: List[Serializer[_]] = List(
    new PathSerializer,
    new PathTypeSerializer,
    new PermissionsMaskSerializer,
    new UrlSerializer
  )
}
