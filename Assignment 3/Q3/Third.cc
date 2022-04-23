// Code adapted from examples/tutorial/sixth.cc

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

// Application
class MyApp : public Application
{
public:
  MyApp ();
  virtual ~MyApp ();

  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp ()
{
  m_socket = 0;
}

/* static */
TypeId MyApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("MyApp")
    .SetParent<Application> ()
    .SetGroupName ("Tutorial")
    .AddConstructor<MyApp> ()
    ;
  return tid;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
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
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}


// Function to store congestion window in file
static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << newCwnd << std::endl;
}

// Function to calculate packet drop
int c1 = 0;
static void
RxDrop (int pdrop, Ptr<const Packet> p)
{
  c1+=1;
}

//  N1 ----- 
//           N3
//  N2 ----- 


int main(int argc, char *argv[]){

    std::string TcpVersion;

    int configuration=0;

    CommandLine cmd;
    cmd.AddValue("Config", "Configuration", configuration); // Add Command Line option to set configuration
    cmd.Parse (argc, argv);

    // Create nodes
    NodeContainer nodesN1N3;
    nodesN1N3.Create (2); // N1 N3 created

    NodeContainer nodesN2N3;
    nodesN2N3.Create (1); // N2 created
    nodesN2N3.Add (nodesN1N3.Get (1)); // Add N3


    // Create Point to Point links
    PointToPointHelper pointToPointN1N3;
    pointToPointN1N3.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
    pointToPointN1N3.SetChannelAttribute ("Delay", StringValue ("3ms")); // P2P N1N3

    NetDeviceContainer devicesN1N3;
    devicesN1N3 = pointToPointN1N3.Install (nodesN1N3); // Link and nodes attached

    Ptr<RateErrorModel> em1 = CreateObject<RateErrorModel> ();
    em1->SetAttribute ("ErrorRate", DoubleValue (0.00001));
    devicesN1N3.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em1)); // At device 2 rate error model

    PointToPointHelper pointToPointN2N3;
    pointToPointN2N3.SetDeviceAttribute ("DataRate", StringValue ("9Mbps"));
    pointToPointN2N3.SetChannelAttribute ("Delay", StringValue ("3ms")); // P2P N2N3

    NetDeviceContainer devicesN2N3;
    devicesN2N3 = pointToPointN2N3.Install (nodesN2N3); // Link and nodes attached

    Ptr<RateErrorModel> em2 = CreateObject<RateErrorModel> ();
    em2->SetAttribute ("ErrorRate", DoubleValue (0.00001));
    devicesN2N3.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em2)); // At device 2 rate error model


    // Install internet stack on each node
    InternetStackHelper stack;
    stack.Install (nodesN1N3);
    stack.Install (nodesN2N3.Get(0));


    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.252");
    Ipv4InterfaceContainer interfacesN1N3 = address.Assign (devicesN1N3); // Assign IP Address to devices
    address.SetBase ("10.1.2.0", "255.255.255.252");
    Ipv4InterfaceContainer interfacesN2N3 = address.Assign (devicesN2N3); // Assign IP Address to devices

    
    // Sink application 1
    uint16_t sinkPort1 = 8801;
    Address sinkAddress1 (InetSocketAddress (interfacesN1N3.GetAddress (1), sinkPort1));
    PacketSinkHelper packetSinkHelper1 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort1));
    ApplicationContainer sinkApps1 = packetSinkHelper1.Install (nodesN1N3.Get(1));
    sinkApps1.Start (Seconds (0.));
    sinkApps1.Stop (Seconds (30.));

    // Sink application 2
    uint16_t sinkPort2 = 8802;
    Address sinkAddress2 (InetSocketAddress (interfacesN1N3.GetAddress (1), sinkPort2));
    PacketSinkHelper packetSinkHelper2 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort2));
    ApplicationContainer sinkApps2 = packetSinkHelper2.Install (nodesN1N3.Get (1));
    sinkApps2.Start (Seconds (0.));
    sinkApps2.Stop (Seconds (30.));

    // Sink application 3
    uint16_t sinkPort3 = 8803;
    Address sinkAddress3 (InetSocketAddress (interfacesN2N3.GetAddress (1), sinkPort3));
    PacketSinkHelper packetSinkHelper3 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort3));
    ApplicationContainer sinkApps = packetSinkHelper3.Install (nodesN2N3.Get (1));
    sinkApps.Start (Seconds (0.));
    sinkApps.Stop (Seconds (30.));


    // Create sockets at N1 and N2
    TypeId tid = TypeId::LookupByName ("ns3::TcpNewReno");
    TypeId tidCSE = TypeId::LookupByName ("ns3::TcpNewRenoCSE");
    
    Ptr<Socket> SocketN1, SocketN2, SocketN3;

    if(configuration == 1){
        Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid));
        SocketN1 = Socket::CreateSocket (nodesN1N3.Get (0), TcpSocketFactory::GetTypeId ());
        Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid));
        SocketN2 = Socket::CreateSocket (nodesN1N3.Get (0), TcpSocketFactory::GetTypeId ());
        Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid));
        SocketN3 = Socket::CreateSocket (nodesN2N3.Get (0), TcpSocketFactory::GetTypeId ());
    }
    else if(configuration == 2){
        Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid));
        SocketN1 = Socket::CreateSocket (nodesN1N3.Get (0), TcpSocketFactory::GetTypeId ());
        Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid));
        SocketN2 = Socket::CreateSocket (nodesN1N3.Get (0), TcpSocketFactory::GetTypeId ());
        Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tidCSE));
        SocketN3 = Socket::CreateSocket (nodesN2N3.Get (0), TcpSocketFactory::GetTypeId ());
    }
    else{
        Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tidCSE));
        SocketN1 = Socket::CreateSocket (nodesN1N3.Get (0), TcpSocketFactory::GetTypeId ());
        Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tidCSE));
        SocketN2 = Socket::CreateSocket (nodesN1N3.Get (0), TcpSocketFactory::GetTypeId ());
        Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tidCSE));
        SocketN3 = Socket::CreateSocket (nodesN2N3.Get (0), TcpSocketFactory::GetTypeId ());
    }


    // Application 1
    Ptr<MyApp> app1 = CreateObject<MyApp> ();
    app1->Setup (SocketN1, sinkAddress1, 3000, 100000, DataRate ("1.5Mbps"));
    nodesN1N3.Get (0)->AddApplication (app1);
    app1->SetStartTime (Seconds (1.));
    app1->SetStopTime (Seconds (20.));

    // Application 2
    Ptr<MyApp> app2 = CreateObject<MyApp> ();
    app2->Setup (SocketN2, sinkAddress2, 3000, 100000, DataRate ("1.5Mbps"));
    nodesN1N3.Get (0)->AddApplication (app2);
    app2->SetStartTime (Seconds (5.));
    app2->SetStopTime (Seconds (25.));

    // Application 3
    Ptr<MyApp> app3 = CreateObject<MyApp> ();
    app3->Setup (SocketN3, sinkAddress3, 3000, 100000, DataRate ("1.5Mbps"));
    nodesN2N3.Get (0)->AddApplication (app3);
    app3->SetStartTime (Seconds (15.));
    app3->SetStopTime (Seconds (30.));


    // Congestion Window Tracer
    AsciiTraceHelper asciiTraceHelper;
    Ptr<OutputStreamWrapper> cwndStream1 = asciiTraceHelper.CreateFileStream(std::to_string(configuration)+"_1"+".cwnd");
    Ptr<OutputStreamWrapper> cwndStream2 = asciiTraceHelper.CreateFileStream(std::to_string(configuration)+"_2"+".cwnd");
    Ptr<OutputStreamWrapper> cwndStream3 = asciiTraceHelper.CreateFileStream(std::to_string(configuration)+"_3"+".cwnd");
    SocketN1->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, cwndStream1));
    SocketN2->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, cwndStream2));
    SocketN3->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, cwndStream3));


    // Packet Drops
    int pdrop1 = 1, pdrop3 = 3;
    devicesN1N3.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, pdrop1));
    devicesN2N3.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, pdrop3));

    Simulator::Stop (Seconds (30));
    Simulator::Run ();
    Simulator::Destroy ();

    NS_LOG_UNCOND ("Packets dropped by connections " << c1);

}