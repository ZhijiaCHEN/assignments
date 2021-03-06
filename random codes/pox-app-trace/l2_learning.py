# Copyright 2011-2012 James McCauley
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at:
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
An L2 learning switch.

It is derived from one written live for an SDN crash course.
It is somwhat similar to NOX's pyswitch in that it installs
exact-match rules for each flow.
"""

from pox.core import core
import pox.openflow.libopenflow_01 as of
from pox.lib.util import dpid_to_str, str_to_dpid
from pox.lib.util import str_to_bool
import time
from threading import Lock

log = core.getLogger()

# We don't want to flood immediately when a switch connects.
# Can be overriden on commandline.
_flood_delay = 0

relPacket = []  # (switch, in port, source mac, destination mac)
relLearnt = []  # (switch, mac, port)
relFwd = []  # (swith, mac, port)
relFlush = []  # (switch, mac)
relDrop = []  # (switch, mac)

mutex = Lock()
fmutex = Lock()

tabS = '    '

EDBS = ''
multicastS = ''
currentStateS = ''
packetInS = ''

IDBS = ''
descisionS = ''
nextStateS = ''

macToPort = {} # let's have a global mac to port table

def format_dpid(dpid):
    return 's'+str(int(dpid_to_str(dpid).replace('-', ''), 16))

def format_address(adr):
    return 'm'+str(int(adr.toStr().replace(':', ''), 16))

def format_port(port):
    return 'p'+str(port)
    
def write_trace():
    flowF = open('mac-learning-flow.txt', 'a')
    flowF.write('EDB{{\n{0}{1}{2}{3}}}\n'.format(multicastS, currentStateS, packetInS, tabS))
    flowF.write('IDB{{\n{0}{1}{2}}}\n\n'.format(descisionS, nextStateS, tabS))
    flowF.close()

class LearningSwitch (object):
    """
    The learning switch "brain" associated with a single OpenFlow switch.

    When we see a packet, we'd like to output it on a port which will
    eventually lead to the destination.  To accomplish this, we build a
    table that maps addresses to ports.

    We populate the table by observing traffic.  When we see a packet
    from some source coming from some port, we know that source is out
    that port.

    When we want to forward traffic, we look up the desintation in our
    table.  If we don't know the port, we simply send the message out
    all ports except the one it came in on.  (In the presence of loops,
    this is bad!).

    In short, our algorithm looks like this:

    For each packet from the switch:
    1) Use source address and switch port to update address/port table
    2) Is transparent = False and either Ethertype is LLDP or the packet's
       destination address is a Bridge Filtered address?
       Yes:
          2a) Drop packet -- don't forward link-local traffic (LLDP, 802.1x)
              DONE
    3) Is destination multicast?
       Yes:
          3a) Flood the packet
              DONE
    4) Port for destination address in our address/port table?
       No:
          4a) Flood the packet
              DONE
    5) Is output port the same as input port?
       Yes:
          5a) Drop packet and similar ones for a while
    6) Install flow table entry in the switch so that this
       flow goes out the appopriate port
       6a) Send the packet out appropriate port
    """

    def __init__(self, connection, transparent):
        # Switch we'll be adding L2 learning switch capabilities to
        self.connection = connection
        self.transparent = transparent

        # Our table
        # self.macToPort = {} # why make each switch has a mac to port table?
        self.macToPort = macToPort


        # We want to hear PacketIn messages, so we listen
        # to the connection
        connection.addListeners(self)

        # We just use this to know when to log a helpful message
        self.hold_down_expired = _flood_delay == 0

        # log.debug("Initializing LearningSwitch, transparent=%s",
        #          str(self.transparent))

    def _handle_PacketIn(self, event):
        """
        Handle packet in messages from the switch to implement above algorithm.
        """
        global EDBS, multicastS, currentStateS, packetInS, IDBS, descisionS, nextStateS
        mutex.acquire()
        packet = event.parsed
        dpid = format_dpid(event.dpid)
        inPort = format_port(event.port)
        src = format_address(packet.src)
        dst = format_address(packet.dst)
        #flowF.write("(\n")
        #flowF.write(event.ofp.show())
        #flowF.write("matching fields: \n")
        #flowF.write(of.ofp_match.from_packet(packet).show())
        #flowF.write(",\n")

        packetTuple = (dpid_to_str(event.dpid), event.port, packet.src.toStr(
        ), packet.dst.toStr())  # (switch, in port, source mac, destination mac)
        # if packetTuple not in self.packet: self.packet.append(packetTuple)
        relPacket.append(packetTuple)
        packetInS = '{}packet_in({}, {}, {}, {})\n'.format(tabS, dpid, inPort, src, dst)

        def flood(message=None):
            """ Floods the packet """
            msg = of.ofp_packet_out()
            if time.time() - self.connection.connect_time >= _flood_delay:
                # Only flood if we've been connected for a little while...

                if self.hold_down_expired is False:
                    # Oh yes it is!
                    self.hold_down_expired = True
                    log.info("%s: Flood hold-down expired -- flooding",
                             dpid_to_str(event.dpid))

                if message is not None:
                    log.debug(message)
                #log.debug("%i: flood %s -> %s", event.dpid,packet.src,packet.dst)
                # OFPP_FLOOD is optional; on some switches you may need to change
                # this to OFPP_ALL.
                msg.actions.append(of.ofp_action_output(port=of.OFPP_FLOOD))
            else:
                pass
                #log.info("Holding down flood for %s", dpid_to_str(event.dpid))
            msg.data = event.ofp
            msg.in_port = event.port
            self.connection.send(msg)
            flushTuple = (dpid_to_str(event.dpid),
                          packet.dst.toStr())  # (switch, mac)
            relFlush.append(flushTuple)

        def drop(duration=None):
            """
            Drops this packet and optionally installs a flow to continue
            dropping similar ones for a while
            """
            if duration is not None:
                if not isinstance(duration, tuple):
                    duration = (duration, duration)
                msg = of.ofp_flow_mod()
                msg.match = of.ofp_match.from_packet(packet)
                msg.idle_timeout = duration[0]
                msg.hard_timeout = duration[1]
                msg.buffer_id = event.ofp.buffer_id
                self.connection.send(msg)
            elif event.ofp.buffer_id is not None:
                msg = of.ofp_packet_out()
                msg.buffer_id = event.ofp.buffer_id
                msg.in_port = event.port
                self.connection.send(msg)
            dropTuple = (dpid_to_str(event.dpid),
                         packet.dst.toStr())  # (switch, mac)
            relDrop.append(dropTuple)

        currentStateS = ''
        nextStateS = ''
        for k in self.macToPort.keys():
            currentStateS += '{}state({}, {}, {}),\n'.format(tabS, k[0], k[1], self.macToPort[k])
        if (dpid, src) not in self.macToPort.keys():
            self.macToPort[(dpid, src)] = inPort  # 1
            nextStateS = '{}next_state({}, {}, {})\n'.format(tabS, dpid, src, inPort)

        learntTuple = (dpid_to_str(event.dpid), packet.src.toStr(),
                       event.port)  # (switch, mac, port)
        if learntTuple not in relLearnt:
            # print 'learnt tuple: {0}'.format(learntTuple)
            relLearnt.append(learntTuple)
        if not self.transparent:  # 2
            if packet.type == packet.LLDP_TYPE or packet.dst.isBridgeFiltered():
                descisionS = '{}drop({}, {})\n'.format(tabS, dpid, dst)
                drop()  # 2a
                write_trace()
                mutex.release()
                return

        multicastS = ''
        if packet.dst.is_multicast:
            multicastS = '{}multicast({}),\n'.format(tabS, dst)
            descisionS = '{}flood({}, {}),\n'.format(tabS, dpid, dst)
            flood()  # 3a
        else:
            if (dpid, dst) not in self.macToPort:  # 4
                # print "4a flood from unknow dst, packet src: {0}, packet dst: {1}".format(packet.src, packet.dst)
                descisionS = '{}flood({}, {}),\n'.format(tabS, dpid, dst)
                flood("Port for %s unknown -- flooding" % (packet.dst,))  # 4a
            else:
                port = int(self.macToPort[(dpid, dst)][1:])
                if port == event.port:  # 5
                    # 5a
                    descisionS = '{}drop({}, {}),\n'.format(tabS, dpid, dst)
                    drop(10)
                    write_trace()
                    mutex.release()
                    return
                # 6
                log.debug("installing flow for %s.%i -> %s.%i" %
                          (packet.src, event.port, packet.dst, port))
                msg = of.ofp_flow_mod()
                msg.match = of.ofp_match.from_packet(packet, event.port)
                msg.idle_timeout = 0
                msg.hard_timeout = 0
                msg.actions.append(of.ofp_action_output(port=port))
                msg.data = event.ofp  # 6a
                self.connection.send(msg)
                descisionS = '{}fwd({}, {}, {}),\n'.format(tabS, dpid, dst, format_port(port))
                # (swith, mac, port)
                fwdTuple = (dpid_to_str(event.dpid), packet.dst.toStr(), port)
                relFwd.append(fwdTuple)
                # print "6a installing flow for {0}.{1} -> {2}.{3}".format(packet.src, event.port, packet.dst, port)
        write_trace()
        mutex.release()


class l2_learning (object):
    """
    Waits for OpenFlow switches to connect and makes them learning switches.
    """

    def __init__(self, transparent, ignore=None):
        """
        Initialize

        See LearningSwitch for meaning of 'transparent'
        'ignore' is an optional list/set of DPIDs to ignore
        """
        core.openflow.addListeners(self)
        self.transparent = transparent
        self.ignore = set(ignore) if ignore else ()
        self.sws = []

    def _handle_ConnectionUp(self, event):
        if event.dpid in self.ignore:
            log.debug("Ignoring connection %s" % (event.connection,))
            return
        log.debug("Connection %s" % (event.connection,))
        LearningSwitch(event.connection, self.transparent)
        self.sws.append(dpid_to_str(event.dpid))

    def _handle_ConnectionDown(self, event):
        self.sws.remove(dpid_to_str(event.dpid))
        if len(self.sws) > 0:
            return
        else:
            return
        #global relLearnt
        #global relDrop
        #global relFlush
        #global relPacket
        #global relFwd

        # if relLearnt is None: return

        inputStr = 'Input{\n'
        for t in relPacket:
            inputStr += '\tpacket{0},\n'.format(str(t))
        for t in relLearnt:
            inputStr += '\tlearnt{0},\n'.format(str(t))
        if len(relPacket)+len(relLearnt) > 0:
            inputStr = inputStr[0:-2]
        inputStr += '\n}\n'

        outputStr = 'Output{\n'
        for t in relFwd:
            outputStr += '\tfwd{0},\n'.format(str(t))
        for t in relFlush:
            outputStr += '\tflush{0},\n'.format(str(t))
        if len(relFwd)+len(relFlush) > 0:
            outputStr = outputStr[0:-2]
        outputStr += '\n}\n'

        with open('mac-learning-with-fw-trace.txt', 'a') as f:
            f.write(inputStr)
            f.write(outputStr)
            f.write('\n')

        #relLearnt = None
        #relPacket = None
        #relFwd = None
        #relFlush = None


def launch(transparent=False, hold_down=_flood_delay, ignore=None):
    """
    Starts an L2 learning switch.
    """
    try:
        global _flood_delay
        _flood_delay = int(str(hold_down), 10)
        assert _flood_delay >= 0
    except:
        raise RuntimeError("Expected hold-down to be a number")

    if ignore:
        ignore = ignore.replace(',', ' ').split()
        ignore = set(str_to_dpid(dpid) for dpid in ignore)

    core.registerNew(l2_learning, str_to_bool(transparent), ignore)
