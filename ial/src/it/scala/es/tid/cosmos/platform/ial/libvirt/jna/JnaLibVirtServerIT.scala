package es.tid.cosmos.platform.ial.libvirt.jna

import scala.concurrent.Await
import scala.concurrent.duration.Duration
import scala.concurrent.ExecutionContext.Implicits.global

import org.scalatest.{BeforeAndAfter, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.ial.libvirt.TransientLibVirtServerProperties
import es.tid.cosmos.platform.ial.MachineProfile

class JnaLibVirtServerIT extends FlatSpec with MustMatchers with BeforeAndAfter {

  var server = new JnaLibVirtServer(new TransientLibVirtServerProperties(
    name = "Test",
    description = "Test libvirt Server",
    MachineProfile.M,
    connectionChain = "openvz+ssh://cosmos@192.168.63.12/system?socket=/var/run/libvirt/libvirt-sock",
    numberOfCpus = 2,
    totalMemory = 32768,
    domainTemplate = "centos-6-x86_64",
    bridgeName = "vzbr0",
    domainHostname = "andromeda52",
    domainIpAddress = "192.168.63.62"))

  before { Await.ready(server.destroyDomain(), Duration.Inf) }

  "The JNA libvirt server" must "create a new domain" in {
    val dom_> = server.createDomain()
    val dom = Await.result(dom_>, Duration.Inf)
    dom.name must be (server.domainName)
    dom must be ('active)
  }

  it must "indicate the domain is created after creation" in {
    val dom_> = server.createDomain()
    Await.ready(dom_>, Duration.Inf)
    Await.result(server.isCreated(), Duration.Inf) must be (true)
  }

  it must "indicate the domain is not created before creation" in {
    Await.result(server.isCreated(), Duration.Inf) must be (false)
  }

  it must "destroy existing domain" in {
    val dest_> = server.destroyDomain()
    Await.result(dest_>, Duration.Inf)
  }

  it must "create the domain even if it already exists" in {
    val dom_> = server.createDomain().flatMap(_ => server.createDomain())
    val dom = Await.result(dom_>, Duration.Inf)
    dom.name must be (server.domainName)
    dom must be ('active)
  }

  after { Await.ready(server.destroyDomain(), Duration.Inf) }
}
