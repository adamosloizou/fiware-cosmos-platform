package es.tid.cosmos.platform.manager.ial.serverpool

import scala.Some

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.manager.ial._

/**
 * @author apv
 */
class SqlServerPoolDaoIT extends FlatSpec with MySqlTest with MustMatchers {

    "The SQL server pool DAO" must "list available machines" in {
        val machines = dao.getAvailableMachines
        machines must have size (5)
        machines must containMachines(availableMachines)
    }

    it must "list available machines of given profile" in {
        val machines = dao.getAvailableMachinesWith(_.profile == MachineProfile.M)
        machines must have size (2)
        machines must containMachines(List(availableMachines(1), availableMachines(2)))
    }

    it must "not list assigned machines as available" in {
        val machines = dao.getAvailableMachines
        machines must have size (5)
        machines must not (containMachines(assignedMachines))
    }

    it must "get existing machine" in {
        val machines = dao.getAvailableMachines
        val machine = dao.getMachine(machines(0).id)
        machine must be (Some(machines(0)))
    }

    it must "not get nonexistent machine" in {
        val machine = dao.getMachine(Id[MachineState])
        machine must be (None)
    }

    it must "mark machine as assigned" in {
        val machineId = dao.getAvailableMachines(0).id
        dao.setMachineAvailability(machineId, available = false)
        dao.getAvailableMachines must not (containMachine(machineId))
    }

    it must "mark machine as available" in {
        val machineId = dao.getAvailableMachines(0).id
        dao.setMachineAvailability(machineId, available = false)
        dao.getAvailableMachines must not (containMachine(machineId))
        dao.setMachineAvailability(machineId, available = true)
        dao.getAvailableMachines must containMachine(machineId)
    }
}
