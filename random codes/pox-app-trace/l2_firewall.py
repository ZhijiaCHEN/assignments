from pox.core import core
from pox.lib.revent import EventHalt
import pox.openflow.libopenflow_01 as of
from pox.lib.revent import *
from pox.lib.util import dpid_to_str, str_to_dpid
from pox.lib.addresses import EthAddr
from collections import namedtuple
import os
from threading import Lock

import csv

log = core.getLogger()
policyFile = "%s/assignments/random codes/pox-app-trace/firewall-policies.csv" % os.environ['HOME']
fmutex = Lock()


class Firewall (EventMixin):
    def __init__(self):
        # Listen to dependencies
        def startup():
            core.openflow.addListeners(self, priority=1)
        core.call_when_ready(startup, ('openflow'))
        log.debug("Enabling Firewall Module")

        self.sws = []
        self.relBlock = []  # (switch, source mac, destination mac)
        self.relPacket = []  # (switch, in port, source mac, destination mac)
        self.relDrop = []  # (switch, source mac, destination mac)
        with open(policyFile, 'rb') as f:
            reader = csv.DictReader(f)
            for row in reader:
                self.relBlock.append(
                    (EthAddr(row['mac_0']).toStr(), EthAddr(row['mac_1']).toStr()))
                self.relBlock.append(
                    (EthAddr(row['mac_1']).toStr(), EthAddr(row['mac_0']).toStr()))

    def _handle_PacketIn(self, event):
        """
        Handle packet in messages
        """
        fmutex.acquire()
        packet = event.parsed
        flowF = open('firewall-flow.txt', 'a')
        flowF.write("(\n")
        flowF.write(event.ofp.show())
        flowF.write("matching fields: \n")
        flowF.write(of.ofp_match.from_packet(packet).show())
        flowF.write(",\n")
        packetTuple = (dpid_to_str(event.dpid), event.port, packet.src.toStr(
        ), packet.dst.toStr())  # (switch, in port, source mac, destination mac)
        self.relPacket.append(packetTuple)

        src = packet.src
        dst = packet.dst
        if (src.toStr(), dst.toStr()) in self.relBlock:
            print "receive packet should be blocked"
            # (switch, source mac, destination mac)
            dropTuple = (dpid_to_str(event.dpid), src.toStr(), dst.toStr())
            self.relDrop.append(dropTuple)
            match = of.ofp_match()
            match.dl_src = src
            match.dl_dst = dst
            msg = of.ofp_flow_mod()
            msg.match = match
            msg.data = event.ofp
            msg.idle_timeout = 0
            msg.hard_timeout = 0
            flowF.write(msg.show())
            flowF.write("\n)\n\n")
            flowF.close()
            event.connection.send(msg)

            msg.priority = 255

            fmutex.release()
            log.debug("Firewall rules installed on %s",
                      dpid_to_str(event.dpid))
            return EventHalt  # Halt the event, no other rules should be applied to the packet
        else:
            print "receive packet should be allowed"
            flowF.write("None\n)\n\n")
            flowF.close()
            fmutex.release()

    def _handle_ConnectionUp(self, event):
        if dpid_to_str(event.dpid) not in self.sws:
            self.sws.append(dpid_to_str(event.dpid))

    def _handle_ConnectionDown(self, event):
        self.sws.remove(dpid_to_str(event.dpid))
        if len(self.sws) > 0:
            return

        inputStr = 'Input{\n'
        for t in self.relBlock:
            inputStr += '\tblock{0},\n'.format(str(t))
        for t in self.relPacket:
            inputStr += '\tpacket{0},\n'.format(str(t))
        if len(self.relBlock)+len(self.relPacket) > 0:
            inputStr = inputStr[0:-2]
        inputStr += '\n}\n'

        outputStr = 'Output{\n'
        for t in self.relDrop:
            outputStr += '\tdrop{0},\n'.format(str(t))
        if len(self.relDrop) > 0:
            outputStr = outputStr[0:-2]
        outputStr += '\n}\n'

        with open('firewall-trace.txt', 'a') as f:
            f.write(inputStr)
            f.write(outputStr)
            f.write('\n')


def launch():
    '''
    Starting the Firewall module
    '''
    core.registerNew(Firewall)
