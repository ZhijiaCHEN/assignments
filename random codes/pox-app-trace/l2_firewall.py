from pox.core import core
from pox.lib.revent import EventHalt
import pox.openflow.libopenflow_01 as of
from pox.lib.revent import *
from pox.lib.util import dpidToStr
from pox.lib.addresses import EthAddr
from collections import namedtuple
import os

import csv

log = core.getLogger()
policyFile = "%s/assignments/random codes/pox-app-trace/firewall-policies.csv" % os.environ['HOME']


class Firewall (EventMixin):
    def __init__(self):
        # Listen to dependencies
        def startup():
            core.openflow.addListeners(self, priority=1)
        core.call_when_ready(startup, ('openflow'))
        log.debug("Enabling Firewall Module")

        self.sw = []
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
        flowF = open('mac-learning-flow.txt', 'r+')
        flowF.write("(\n")
        flowF.write(event.ofp.show())
        flowF.write(",\n")
        packet = event.parsed
        packetTuple = (dpid_to_str(event.dpid), event.port, packet.src.toStr(
        ), packet.dst.toStr())  # (switch, in port, source mac, destination mac)
        self.relPacket.append(packetTuple)

        src = packet.src
        dst = packet.dst
        if (src.toStr(), dst.toStr()) in self.relBlock:
            # (switch, source mac, destination mac)
            dropTuple = (dpid_to_str(event.dpid), src.toStr(), dst.toStr())
            self.relDrop.append(dropTuple)
            match = of.ofp_match()
            match.dl_src = src
            match.dl_dst = dst
            msg = of.ofp_flow_mod()
            msg.match = match
            msg.priority = 255
            event.connection.send(msg)
            flowF.write(msg.show())
            flowF.write("\n)\n\n")
            log.debug("Firewall rules installed on %s", dpidToStr(event.dpid))
        else:
            flowF.write("None\n)\n\n")
        flowF.close()
        return EventHalt  # Halt the event, no other rules should be applied to the packet

    def _handle_ConnectionUp(self, event):
        if dpid_to_str(event.dpid) not in self.sw:
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
        inputStr = inputStr[0:-2]
        inputStr += '\n}\n'

        outputStr = 'Output{\n'
        for t in self.relDrop:
            outputStr += '\tdrop{0},\n'.format(str(t))
        outputStr = outputStr[0:-2]
        outputStr += '\n}\n'

        with open('mac-learning-trace.txt', 'w') as f:
            f.write(inputStr)
            f.write(outputStr)
            f.write('\n')


def launch():
    '''
    Starting the Firewall module
    '''
    core.registerNew(Firewall)
