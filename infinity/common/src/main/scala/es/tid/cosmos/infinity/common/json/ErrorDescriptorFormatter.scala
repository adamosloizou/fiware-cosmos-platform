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

package es.tid.cosmos.infinity.common.json

import net.liftweb.json._

import es.tid.cosmos.infinity.common.messages.ErrorDescriptor

class ErrorDescriptorFormatter extends JsonFormatter[ErrorDescriptor] {

  /** Formats error descriptions as JSON.
    *
    * @param error  Value to be formatted
    * @return       JSON representation of the value
    */
  override def format(error: ErrorDescriptor): String = pretty(render(Extraction.decompose(error)))
}
