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

package es.tid.cosmos.infinity.server.permissions

/** A permission class (owner, group, others). */
case class PermissionClass(read: Boolean, write: Boolean, execute: Boolean) {
  override def toString: String = {
    val bin = (if (read) 0x4 else 0) | (if (write) 0x2 else 0) | (if (execute) 0x1 else 0)
    bin.toString
  }
}

object PermissionClass {

  val OctalPattern = """([0-7])""".r

  /** Obtain the permission class from its octal representation. */
  def fromOctal(octal: String): PermissionClass = octal match {
    case OctalPattern(_) =>
      val bin = octal.toByte
      PermissionClass(
        read = (bin & 0x4) == 0x4,
        write = (bin & 0x2) == 0x2,
        execute = (bin & 0x1) == 0x1
      )
    case _ =>
      throw new IllegalArgumentException(s"invalid octal UNIX class permissions in $octal")
  }
}

