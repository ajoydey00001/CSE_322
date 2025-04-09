#include "ns3/aodv-module.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/olsr-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-error-rate-model.h"
#include "ns3/yans-wifi-helper.h"

//    10.1.1.0                         10.1.3.0
//                    10.1.2.0
//  n0 - ----------               ------------------n0*
//  n1  ------------              ------------------n1*
//  n2   -----------X -----------Y------------------n2*
//  n3  ------------              ------------------n3*
//  n4  ------------              ------------------n4*

using namespace ns3;

int number_flow = 10;
int number_node = 20;
int num_packets_per_second = 500;
int cov_area = 5;

int time_of_simulation = 25;

uint32_t rx_packet = 0;
uint32_t rx_byte = 0;
uint32_t tx_packet = 0;
float throughput_in_average = 0;
float packet_delivery_ratio = 0;

void
counting_at_sink_end(Ptr<const Packet> p, const Address& a)
{
    rx_byte = rx_byte + p->GetSize();
    rx_packet = rx_packet + 1;
}

void
counting_at_source_end(Ptr<const Packet> p)
{
    tx_packet = tx_packet + 1;
}

void
simulation_output()
{
    packet_delivery_ratio = (rx_packet * 100.00) / tx_packet;
    throughput_in_average = (rx_byte * 8.0) / ((Simulator::Now().GetSeconds()) * 1000);
    //NS_LOG_UNCOND(std::endl);
    NS_LOG_UNCOND("total overview of simulation");
    // NS_LOG_UNCOND(std::endl);
    NS_LOG_UNCOND("Total packets are send by sources :  " << tx_packet);
    NS_LOG_UNCOND("Total packets are received by sinks :  " << rx_packet);
    NS_LOG_UNCOND("Ratio of packet delivery :  " << packet_delivery_ratio);
    NS_LOG_UNCOND("Average throughput :  " << throughput_in_average);
}

void
store_in_file(std::string file)
{
    std::ofstream mfile(file, std::ios_base::app);
    mfile << number_node * 2 << " " << number_flow << " " << num_packets_per_second << " "
          << cov_area << " "<< throughput_in_average << " " << packet_delivery_ratio << "\n";

    mfile.close();
}

int
main(int argc, char* argv[])
{
    // variant type of Tcp
    std::string tcp_variant = "TcpNewReno";
    std::string file = "./scratch/1905038/part1/plot/data.txt";
    std::string bn_delay = "2ms";

    uint32_t size_of_payload = 1024;
    int tx_range = 5;

    CommandLine cmd(__FILE__);
    cmd.AddValue("cov_area", "Coverage area", cov_area);
    cmd.AddValue("num_packets_per_second", "number of packets per second", num_packets_per_second);
    cmd.AddValue("number_flow", "number of total flows", number_flow);
    cmd.AddValue("number_node", "number of total nodes", number_node);
    cmd.AddValue("file", "File to store data", file);
    cmd.Parse(argc, argv);

    cov_area = cov_area * tx_range;
    int rate_of_data = (8 * num_packets_per_second * size_of_payload) / 1000;
    number_node /= 2;

    std::string bn_data_rate = std::to_string(rate_of_data / 10) + "Kbps";
    std::string dr = std::to_string(rate_of_data) + "Kbps";
    NS_LOG_UNCOND(std::endl);
    NS_LOG_UNCOND("Number of nodes : " << number_node*2 << " ,  number of flows : " << number_flow
                                       << " , packets per sec : " << num_packets_per_second
                                       << " , coverage area : " << cov_area
                                       << " , data rate : " << rate_of_data << " kbps"
                                       << " , bottleneck data rate : " << (rate_of_data / 10)
                                       << " kbps");

   
    tcp_variant = std::string("ns3::") + tcp_variant;
   
    TypeId tcp_tid;
    NS_ABORT_MSG_UNLESS(TypeId::LookupByNameFailSafe(tcp_variant, &tcp_tid),
                        "TypeId " << tcp_variant << " not found");
    Config::SetDefault("ns3::TcpL4Protocol::SocketType",
                       TypeIdValue(TypeId::LookupByName(tcp_variant)));
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(size_of_payload));
    Config::SetDefault("ns3::RangePropagationLossModel::MaxRange", DoubleValue(cov_area));

    NodeContainer bottle_neck_nodes;
    bottle_neck_nodes.Create(2);

    PointToPointHelper bottle_neck_link;
    bottle_neck_link.SetDeviceAttribute("DataRate", StringValue(bn_data_rate));
    bottle_neck_link.SetChannelAttribute("Delay", StringValue(bn_delay));

    NetDeviceContainer bottle_neck_devices;
    bottle_neck_devices = bottle_neck_link.Install(bottle_neck_nodes);

    // wifi connection in both left and right

    NodeContainer left_nodes_wifi, right_nodes_wifi;
    left_nodes_wifi.Create(number_node);
    right_nodes_wifi.Create(number_node);

    NodeContainer left_AP_node = bottle_neck_nodes.Get(0);
    NodeContainer right_AP_node = bottle_neck_nodes.Get(1);

    YansWifiChannelHelper left_channel = YansWifiChannelHelper::Default();
    YansWifiChannelHelper right_channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper left_phy, right_phy;
    left_channel.AddPropagationLoss("ns3::RangePropagationLossModel");
    right_channel.AddPropagationLoss("ns3::RangePropagationLossModel");
    
    left_phy.SetChannel(left_channel.Create());
    right_phy.SetChannel(right_channel.Create());

    

    // Data Link layer
    WifiHelper left_wifi, right_wifi;

    WifiMacHelper left_mac, right_mac;
    Ssid left_ssid =
        Ssid("ns-3-ssid-left"); //  creates an 802.11 service set identifier (SSID) object
    Ssid right_ssid = Ssid("ns-3-ssid-right");

    left_mac.SetType("ns3::StaWifiMac",
                     "Ssid",
                     SsidValue(left_ssid),
                     "ActiveProbing",
                     BooleanValue(false));
    right_mac.SetType("ns3::StaWifiMac",
                      "Ssid",
                      SsidValue(right_ssid),
                      "ActiveProbing",
                      BooleanValue(false));

    // ActiveProbing false -  probe requests will not be sent by MACs created by this
    // helper, and stations will listen for AP beacons.
    NetDeviceContainer left_sta_Devices, right_sta_Devices;
    left_sta_Devices = left_wifi.Install(left_phy, left_mac, left_nodes_wifi);
    right_sta_Devices = right_wifi.Install(right_phy, right_mac, right_nodes_wifi);

    //  configure the AP (access point) node
    left_mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(left_ssid));
    right_mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(right_ssid));

    NetDeviceContainer left_ap_device, right_ap_device;
    left_ap_device = left_wifi.Install(left_phy, left_mac, left_AP_node);
    right_ap_device = right_wifi.Install(right_phy, right_mac, right_AP_node);

    MobilityHelper mobility;

    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(0.0),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(1.0),
                                  "DeltaY",
                                  DoubleValue(1.0),
                                  "GridWidth",
                                  UintegerValue(3),
                                  "LayoutType",
                                  StringValue("RowFirst"));

    // Construction of wireless high rate static network
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    // install on STA nodes
    mobility.Install(left_nodes_wifi);
    mobility.Install(right_nodes_wifi);
    mobility.Install(left_AP_node);
    mobility.Install(right_AP_node);

    InternetStackHelper stack;
    stack.Install(left_nodes_wifi);
    stack.Install(right_nodes_wifi);
    stack.Install(left_AP_node);
    stack.Install(right_AP_node);

    Ipv4AddressHelper address;
    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer bottle_neck_interface;
    bottle_neck_interface = address.Assign(bottle_neck_devices);

    Ipv4InterfaceContainer left_node_interface, right_node_interface;
    Ipv4InterfaceContainer left_ap_node_interface, right_ap_node_interface;

    address.SetBase("10.1.3.0", "255.255.255.0");
    right_node_interface = address.Assign(right_sta_Devices);
    right_ap_node_interface = address.Assign(right_ap_device);

    address.SetBase("10.1.1.0", "255.255.255.0");
    left_node_interface = address.Assign(left_sta_Devices);
    left_ap_node_interface = address.Assign(left_ap_device);

    
    int mapping[number_node];
    for (int i = 0; i < number_node; i++)
    {
        mapping[i] = number_node - i - 1;
    }
    uint16_t port_number = 8010;
    int i = 0;

    for (int j = 0; j < number_flow; j++)
    {
        
        PacketSinkHelper h1(
            "ns3::TcpSocketFactory",
            InetSocketAddress(Ipv4Address::GetAny(), port_number)); // 0.0.0.0 address
        ApplicationContainer s1 = h1.Install(right_nodes_wifi.Get(mapping[i]));
        s1.Start(Seconds(0.0));
        s1.Stop(Seconds(time_of_simulation));
        Ptr<PacketSink> s2 = StaticCast<PacketSink>(s1.Get(0));
        s2->TraceConnectWithoutContext("Rx", MakeCallback(&counting_at_sink_end));

        OnOffHelper h2(
            "ns3::TcpSocketFactory",
            (InetSocketAddress(right_node_interface.GetAddress(mapping[i]), port_number)));
        h2.SetAttribute("PacketSize", UintegerValue(size_of_payload));
        h2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        h2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        h2.SetAttribute("DataRate", DataRateValue(DataRate(dr)));
        ApplicationContainer s3 = h2.Install(left_AP_node.Get(0));
        s3.Start(Seconds(1.0));
        s3.Stop(Seconds(time_of_simulation));
        Ptr<OnOffApplication> s4 = StaticCast<OnOffApplication>(s3.Get(0));
        s4->TraceConnectWithoutContext("Tx", MakeCallback(&counting_at_source_end));

        i = (i + 1) % number_node;
        port_number += 1;
    }

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(time_of_simulation));
    Simulator::Run();

    simulation_output();
    store_in_file(file);

    Simulator::Destroy();
    return 0;
}