
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/applications-module.h"
#include "ns3/stats-module.h"
#include "ns3/callback.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/csma-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Our task 01");


class OwnApplication : public Application
{
public:
  OwnApplication ();
  virtual ~OwnApplication ();

  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, DataRate dataRate, uint32_t simultime);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
  uint32_t        m_simultime;
};

OwnApplication::OwnApplication ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0),
    m_simultime (0)
{
}

OwnApplication::~OwnApplication ()
{
  m_socket = 0;
}

/* static */
TypeId OwnApplication::GetTypeId (void)
{
  static TypeId tid = TypeId ("OwnApplication")
    .SetParent<Application> ()
    .SetGroupName ("Tutorial")
    .AddConstructor<OwnApplication> ()
    ;
  return tid;
}

void
OwnApplication::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, DataRate dataRate, uint32_t simultime)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_dataRate = dataRate;
  m_simultime = simultime;
  // NS_LOG_UNCOND("Setting up :"<<socket<<"; address : "<<address<<" ; packetSize : "<<packetSize<<" ; nPackets : "<<nPackets<<" ; dataRate : "<<dataRate);
}

void
OwnApplication::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
    if (InetSocketAddress::IsMatchingType (m_peer))
    {
      m_socket->Bind ();
    }
  else
    {
      m_socket->Bind6 ();
    }
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
OwnApplication::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
OwnApplication::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if(Simulator::Now().GetSeconds() < m_simultime) ScheduleTx();
}

void
OwnApplication::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &OwnApplication::SendPacket, this);
    }
}

static void
CongestionWndChange (Ptr<OutputStreamWrapper> s, uint32_t oldCwnd, uint32_t newCwnd)
{
  
  *s->GetStream () << Simulator::Now ().GetSeconds () << " " << newCwnd << std::endl;
}

//    10.3.1.0                         10.1.1.0
//                    10.2.1.0
//  n0 - ----------               ------------------n0*
//                                  
//                  X -----------Y
//  
//  n1  ------------              ------------------n1*




int main(int argc, char *argv[]){
    uint32_t size_payload = 1024;
    std::string congestion_algo_1 = "ns3::TcpNewReno"; // TcpNewReno
    std::string congestion_algo_2 = "ns3::TcpAdaptiveReno"; //TcpAdaptiveReno TcpWestwood, TcpHighSpeed
    int leaf_number = 2;
    int flow_number = 2;
    std::string data_rate_sender = "1Gbps";
    std::string delay_sender = "1ms";
    int simtime = 25; // 60
    int clrtime = 2;
    int rate_bottleneck = 50;
    int delay_bottleneck = 100;
    int expression = 6;
    int choice = 1;

    
    CommandLine cmd (__FILE__);
   
    cmd.AddValue ("rate_bottleneck","Max Packets allowed in the device queue", rate_bottleneck);
    cmd.AddValue ("expression", "Packet loss rate", expression);
    cmd.AddValue("choice","1 for TcpAdaptiveReno ,  2 for TcpHighSpeed",choice);
   
    cmd.Parse (argc,argv);

    if(choice == 1){
      congestion_algo_2 = "ns3::TcpAdaptiveReno";
    }
    else{
      congestion_algo_2 = "ns3::TcpHighSpeed";
    }

    std::string file = "scratch/Offline_02/data_1.txt";
    flow_number = leaf_number;
    double rate_packet_loss = (1.0 / std::pow(10, expression));
    std::string str_rate_bottleneck = std::to_string(rate_bottleneck) + "Mbps";
    std::string str_delay_bottleneck = std::to_string(delay_bottleneck) + "ms";
   
    NS_LOG_UNCOND("Congestion Algo 1 = "<<congestion_algo_1<<" , Congestion Algo 2 = "<<congestion_algo_2<<" , Leaf_nuber = "<<leaf_number<<
                  " , Bottle Neck Data rate = "<<str_rate_bottleneck<<
                  " , Packet Loss rate = "<<rate_packet_loss<<
                  " ; Sending Data rate = "<<data_rate_sender << std::endl);

    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (size_payload));

    PointToPointHelper helper_bottle_neck;
 
    helper_bottle_neck.SetDeviceAttribute  ("DataRate", StringValue (str_rate_bottleneck));
    helper_bottle_neck.SetChannelAttribute ("Delay", StringValue (str_delay_bottleneck));


    PointToPointHelper helper_leaf_nodes;
 
    helper_leaf_nodes.SetDeviceAttribute  ("DataRate", StringValue (data_rate_sender));
    helper_leaf_nodes.SetChannelAttribute ("Delay", StringValue (delay_sender));
   
    helper_leaf_nodes.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue (std::to_string (delay_bottleneck * rate_bottleneck) + "p"));

    PointToPointDumbbellHelper helper_dumbbell (leaf_number, helper_leaf_nodes,
                                  leaf_number, helper_leaf_nodes,
                                  helper_bottle_neck);

    Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
    em->SetAttribute ("ErrorRate", DoubleValue (rate_packet_loss));
    helper_dumbbell.m_routerDevices.Get(1)->SetAttribute ("ReceiveErrorModel", PointerValue (em)); 

    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (congestion_algo_1));
    InternetStackHelper stack1;
    stack1.Install(helper_dumbbell.GetLeft(0));
    stack1.Install(helper_dumbbell.GetRight(0));
    stack1.Install (helper_dumbbell.GetLeft ());
    stack1.Install (helper_dumbbell.GetRight ());
    
    // tcp variant 2
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (congestion_algo_2));
    InternetStackHelper stack2;
    stack2.Install(helper_dumbbell.GetLeft(1));
    stack2.Install(helper_dumbbell.GetRight(1));

    // ASSIGN IP Addresses
    helper_dumbbell.AssignIpv4Addresses (Ipv4AddressHelper ("10.3.1.0", "255.255.255.0"), // left nodes
                          Ipv4AddressHelper ("10.2.1.0", "255.255.255.0"),  // right nodes
                          Ipv4AddressHelper ("10.1.1.0", "255.255.255.0")); // routers 
    Ipv4GlobalRoutingHelper::PopulateRoutingTables (); // populate routing table
    

    // install flow monitor
    FlowMonitorHelper monitor_flow;
    monitor_flow.SetMonitorAttribute("MaxPerHopDelay", TimeValue(Seconds(clrtime)));
    Ptr<FlowMonitor> monitor = monitor_flow.InstallAll ();

    uint16_t port_number = 8000;
    int iter = 0;
    while(iter != 2){

      Address Address_sink (InetSocketAddress (helper_dumbbell.GetRightIpv4Address (iter), port_number+iter));
      PacketSinkHelper HelperPacketSink ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny(), port_number+iter));
      ApplicationContainer AppSink = HelperPacketSink.Install (helper_dumbbell.GetRight (iter));
      AppSink.Start (Seconds (0.0));
      AppSink.Stop (Seconds (simtime+clrtime));

      Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (helper_dumbbell.GetLeft (iter), TcpSocketFactory::GetTypeId ());
      Ptr<OwnApplication> myapp = CreateObject<OwnApplication> ();
      myapp->Setup (ns3TcpSocket, Address_sink, size_payload, DataRate (data_rate_sender), simtime);
      helper_dumbbell.GetLeft (iter)->AddApplication (myapp);
      myapp->SetStartTime (Seconds (1));
      myapp->SetStopTime (Seconds (simtime));

      AsciiTraceHelper asciiTraceHelper;
      Ptr<OutputStreamWrapper>StreamWrapper;
      
      if(iter == 0){
          StreamWrapper = asciiTraceHelper.CreateFileStream ("scratch/Offline_02/flow1.txt");
      }
      else{
         StreamWrapper = asciiTraceHelper.CreateFileStream ("scratch/Offline_02/flow2.txt");
      }

      ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CongestionWndChange, StreamWrapper));

      iter++;
    }
    
  
    Simulator::Stop (Seconds (simtime+clrtime));
    Simulator::Run ();

    int i = 0;
    float Throughput_Cur = 0;
    float Throughput_CurArr1,Throughput_CurArr2;
    Throughput_CurArr1=Throughput_CurArr2=0;
    double numerator = 0;
    double denominator = 0;

    std::ofstream printfile(file, std::ios_base::app);

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (monitor_flow.GetClassifier ());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

    for (auto iteration = stats.begin (); iteration != stats.end (); ++iteration) {
      
      Throughput_Cur = iteration->second.rxBytes * 8.0;
      Throughput_Cur = Throughput_Cur / ((simtime+clrtime)*1000) ;
      if(i%2 == 0) { Throughput_CurArr1 = Throughput_CurArr1 + iteration->second.rxBytes; }
      if(i%2 == 1) { Throughput_CurArr2 = Throughput_CurArr2 + iteration->second.rxBytes; }
      i = i + 1;
      numerator = numerator + Throughput_Cur;
      denominator = denominator + (Throughput_Cur * Throughput_Cur);
      if(i == flow_number) break;
    }

    double JainIndex = (numerator * numerator) ;
    JainIndex = JainIndex / ( i * denominator) ;
    Throughput_CurArr1 = Throughput_CurArr1 / ((simtime + clrtime)*1000);
    Throughput_CurArr2 = Throughput_CurArr2 / ((simtime + clrtime)*1000);
  
    // columns are bttlneck rate , random packet loss , throughput1 , throughput2 , jain_index
    printfile << str_rate_bottleneck.substr(0, str_rate_bottleneck.length()-4) << " " << (-1)*expression << " " << Throughput_CurArr1 << " " << Throughput_CurArr2 <<" " <<JainIndex<<std::endl;
   
    Simulator::Destroy ();

    return 0;
}