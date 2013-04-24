package es.tid.cosmos.platform.manager.ial.serverpool

import java.sql.{DriverManager, Connection}
import scala.util.Try

import org.squeryl._
import org.squeryl.adapters.MySQLAdapter
import org.squeryl.internals.DatabaseAdapter
import org.squeryl.PrimitiveTypeMode._

import es.tid.cosmos.platform.manager.ial._
import java.util.UUID

class Machine(var machineId: UUID,
              var name: String,
              var available: Boolean,
              var profile: MachineProfile.Value,
              var status: MachineStatus.Value,
              var hostname: String,
              var ipAddress: String) extends KeyedEntity[Long] {

  override val id: Long = 0

  override def toString: String =
      s"{ name: '$name', available: $available, profile: '$profile', " +
          s"status: '$status', hostname:'$hostname', ipAddress: '$ipAddress' }"

  def this() = this(UUID.randomUUID(), "unknown", false, MachineProfile.M, MachineStatus.Provisioning, "", "")

  def toMachineState: MachineState = {
      new MachineState(Id(machineId), name, profile, status, hostname, ipAddress)
  }

  def == (state: MachineState) = (machineId.toString == state.id.toString) && (name == state.name) &&
      (profile == state.profile) && (status == state.status) && (hostname == state.hostname) &&
      (ipAddress == state.ipAddress)
}

object Machine {
  def apply(name: String,
            available: Boolean,
            profile: MachineProfile.Value,
            status: MachineStatus.Value,
            hostname: String,
            ipAddress: String) = new Machine(UUID.randomUUID(), name, available, profile, status, hostname, ipAddress)
}

object InfraDb extends Schema {
    val machines = table[Machine]
}

trait SqlDatabase extends SessionFactory {

  val adapter: DatabaseAdapter

  def connect: Try[Connection]

  def newSession: Session = Session.create(connect.get, adapter)
}

class MySqlDatabase(val host: String,
                    val port: Int,
                    val username: String,
                    val password: String,
                    val dbName: String) extends SqlDatabase {

  Class.forName("com.mysql.jdbc.Driver")

  def connect: Try[Connection] =
      Try(DriverManager.getConnection(s"jdbc:mysql://$host:$port/$dbName", username, password))

  override val adapter = new MySQLAdapter
}

/**
 * A data access object implementation for MySQL database.
 *
 * @author apv
 */
class SqlServerPoolDao(val db: SqlDatabase) extends ServerPoolDao {

  import InfraDb._

  def getAvailableMachinesWith(f: MachineState => Boolean): Seq[MachineState] = transaction(db.newSession) {
    from(machines)(s => where(s.available === true) select(s)).map(_.toMachineState).toArray.filter(f)
  }

  def getMachine(machineId: Id[MachineState]): Option[MachineState] =
    getMachineInternal(machineId).map(_.toMachineState)

  def setMachineAvailability(machineId: Id[MachineState], available: Boolean): Option[MachineState] =
    modifyMachineInternal(machineId)(m => m.available = available)

  def setMachineName(machineId: Id[MachineState], name: String): Option[MachineState] =
    modifyMachineInternal(machineId)(m => m.name = name)

  def getMachineInternal(machineId: Id[MachineState]): Option[Machine] = transaction(db.newSession) {
      from(machines)(s => where(s.machineId.toString === machineId.toString) select(s)).headOption
  }

  def modifyMachineInternal(machineId: Id[MachineState])(mod: Machine => Unit): Option[MachineState] = {
    getMachineInternal(machineId) match {
      case Some(m) => transaction(db.newSession) {
        mod(m)
        machines.update(m)
        return Some(m.toMachineState)
      }
      case None => None
    }
  }
}

class MySqlServerPoolDao(host: String, port: Int, username: String, password: String, dbName: String)
  extends SqlServerPoolDao(new MySqlDatabase(host, port, username, password, dbName))
