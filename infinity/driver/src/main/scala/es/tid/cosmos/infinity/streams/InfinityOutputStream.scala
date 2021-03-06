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

package es.tid.cosmos.infinity.streams

import java.io.OutputStream

import org.apache.hadoop.util.Progressable

private[infinity] class InfinityOutputStream(stream: OutputStream, progress: Option[Progressable])
  extends OutputStream {

  override def write(b: Int): Unit = withProgress(stream.write(b))

  override def write(b: Array[Byte], off: Int, len: Int): Unit =
    withProgress(stream.write(b, off, len))

  override def flush(): Unit = withProgress(stream.flush())

  override def close(): Unit = withProgress(stream.close())

  private def withProgress[T](body: => T): T = {
    progress.foreach(_.progress())
    body
  }
}
