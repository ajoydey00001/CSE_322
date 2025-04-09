/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//

using namespace ns3;

/*Define a Log component with a specific name.

This macro should be used at the top of every file in which you want to use
the NS_LOG macro. This macro defines a new "log component" which can be later
selectively enabled or disabled with the ns3::LogComponentEnable and
ns3::LogComponentDisable functions or with the NS_LOG environment variable.*/

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);

    // Hook your own values in the parser
    uint32_t nPackets = 1;
    cmd.AddValue("nPackets", "Number of packets to echo", nPackets);

    cmd.Parse(argc, argv);

    //  sets the time resolution to one nanosecond
    Time::SetResolution(Time::NS);

    // enable two logging components that are built into
    // the Echo Client and Echo Server applications
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // Add logging to code
    NS_LOG_INFO("Creating Topology");

    // create the ns-3 Node objects that will represent the computers in the simulation
    //  the container calls down into the ns-3 system proper to create
    // two Node objects and stores pointers to those objects internally.
    NodeContainer node_1, node_2, node_3;
    // nodes.Create(4);
    node_1.Create(2);
    node_2.Create(2);
    node_3.Add(node_1.Get(1));
    node_3.Add(node_2.Get(0));

    // PointToPointHelper to configure and connect
    // ns-3 PointToPointNetDevice and PointToPointChannel objects
    PointToPointHelper pointToPoint1, pointToPoint2;
    // Attribute of the PointToPointNetDevice - Also assigns MAC address
    pointToPoint1.SetDeviceAttribute("DataRate", StringValue("10Mbps")); // Data Link Layer
    // Propagation Delay - Attribute associated with the PointToPointChannel.
    // change delay to see what happens!!
    pointToPoint1.SetChannelAttribute("Delay", StringValue("2ms")); // Physical Layer

    // Hold the PointToPointNetDevices created
    NetDeviceContainer device_1, device_2, device_3;
    // devices = pointToPoint.Install(nodes);
    device_1 = pointToPoint1.Install(node_1);
    device_2 = pointToPoint1.Install(node_2);
    pointToPoint2.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    pointToPoint2.SetChannelAttribute("Delay", StringValue("5ms"));

    device_3 = pointToPoint2.Install(node_3);
    

    // install an Internet Stack (TCP, UDP, IP, etc.) on each of the
    // nodes in the node container.
    InternetStackHelper stack1,stack2,stack3;
    stack1.Install(node_1);
    stack2.Install(node_2);
    
    // Assign IP Addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0"); // network address + subnet mask

    // performs the actual8 IP address assignments
    // Ipv4Interface - mapping between nodes and their IP Address
    Ipv4InterfaceContainer interfaces1 = address.Assign(device_1); // network layer
    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces2 = address.Assign(device_3);
    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces3 = address.Assign(device_2);

    // std::cout << "Node 1 IP Address " << interfaces.GetAddress(1) << std::endl;

    // Application Layer + Transport Layer
    // Echo Application - Client sends echo message, Server replies and client accepts the response
    //  Helper - an object used to help us create the actual applications
    ON_OFF_HELPER_H echoServer(9); // "required attribute" - port number

    // creates the actual application - UDPEchoServer
    ApplicationContainer serverApps = echoServer.Install(node_1.Get(0));
    // Applications require a time to “start” generating traffic and
    // may take an optional time to “stop”.
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // required attributes - RemoteAddress, RemotePort
    PacketSinkHelper echoClient(interfaces1.GetAddress(0), 9);
    echoClient.SetAttribute("MaxPackets",
                            UintegerValue(nPackets)); // maximum #packets during simulation
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    // echoClient.SetAttribute("MaxPackets", UintegerValue(nPackets));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0)); // later than server
    clientApps.Stop(Seconds(10.0));

    AsciiTraceHelper ascii;
    pointToPoint.EnableAsciiAll(ascii.CreateFileStream("scratch/first/first.tr"));
    pointToPoint.EnablePcapAll("scratch/first/first");

    // Simulator::Stop(Seconds(3.0));
    //  looking through the list of scheduled events and executing them
    Simulator::Run();
    Simulator::Destroy(); // destroy all objects created
    return 0;
}
