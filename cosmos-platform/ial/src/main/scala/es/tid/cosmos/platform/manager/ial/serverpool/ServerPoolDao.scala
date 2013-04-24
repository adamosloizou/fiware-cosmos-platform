package es.tid.cosmos.platform.manager.ial.serverpool

import es.tid.cosmos.platform.manager.ial._

/**
 * A data access object for server pool state.
 *
 * @author apv
 */
trait ServerPoolDao {

  /**
   * Obtain the list of available machines which satisfies the given predicate.
   *
   * @param f the predicate the available machines must satisfy¡
   * @return the sequence of machines which are available and satisfy the predicate
   */
  def getAvailableMachinesWith(f: MachineState => Boolean) : Seq[MachineState]

  /**
   * Obtain the list of available machines.
   *
   * @return a sequence of available machines
   */
  def getAvailableMachines: Seq[MachineState] = getAvailableMachinesWith(m => true)

  /**
   * Get status of the machine with given identifier.
   *
   * @param machineId the identifier of the machine which status is to be obtained
   * @return the machine state
   */
  def getMachine(machineId: Id[MachineState]): Option[MachineState]

  /**
   * Set the availability of the machine with given identifier.
   *
   * @param machineId the identifier of the machine which availability is to be modified
   * @param available the new availability of the machine (true is available, false is unavailable)
   * @return the new state of the machine
   */
  def setMachineAvailability(machineId: Id[MachineState], available: Boolean): Option[MachineState]

  /**
   * Set the name of the machine with given identifier.
   *
   * @param machineId the identifier of the machine which name is to be modified
   * @param name the new name of the machine
   * @return the new state of the machine
   */
  def setMachineName(machineId: Id[MachineState], name: String): Option[MachineState]
}
