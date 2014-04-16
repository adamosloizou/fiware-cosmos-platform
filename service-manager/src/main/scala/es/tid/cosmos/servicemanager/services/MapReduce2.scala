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

package es.tid.cosmos.servicemanager.services

import es.tid.cosmos.servicemanager.services.dependencies.ServiceDependencies

/** Representation of the Map Reduce 2 service.
  *
  * The service needs to run along with YARN.
  * @see [[Yarn]]
  */
object MapReduce2 extends Service with NoParametrization {
  override val name: String = "MAPREDUCE2"
  override val dependencies = ServiceDependencies.required(Yarn)
}
