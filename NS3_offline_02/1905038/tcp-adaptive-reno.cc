
#include "tcp-adaptive-reno.h"

#include "rtt-estimator.h"
#include "tcp-socket-base.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE("TcpAdaptiveReno");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(TcpAdaptiveReno);

TypeId
TcpAdaptiveReno::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::TcpAdaptiveReno")
            .SetParent<TcpNewReno>()
            .SetGroupName("Internet")
            .AddConstructor<TcpAdaptiveReno>()
            .AddAttribute(
                "FilterType",
                "Use this to choose no filter or Tustin's approximation filter",
                EnumValue(TcpAdaptiveReno::TUSTIN),
                MakeEnumAccessor(&TcpAdaptiveReno::m_fType),
                MakeEnumChecker(TcpAdaptiveReno::NONE, "None", TcpAdaptiveReno::TUSTIN, "Tustin"))
            .AddTraceSource("EstimatedBW",
                            "The estimated bandwidth",
                            MakeTraceSourceAccessor(&TcpAdaptiveReno::m_currentBW),
                            "ns3::TracedValueCallback::DataRate");
    return tid;
}

TcpAdaptiveReno::TcpAdaptiveReno()
    : TcpWestwoodPlus(),
      Minimum_Rtt(Time(0)),
      Current_Rtt(Time(0)),
      j_packet_l_Rtt(Time(0)),
      Congestionn_Rtt(Time(0)),
      Pre_Congestionn_Rtt(Time(0)),
      Increment_window(0),
      Base_window(0),
      Probe_window(0)
{
    NS_LOG_FUNCTION(this);
}

TcpAdaptiveReno::TcpAdaptiveReno(const TcpAdaptiveReno& sock)
    : TcpWestwoodPlus(sock),
      Minimum_Rtt(sock.Minimum_Rtt),
      Current_Rtt(sock.Current_Rtt),
      j_packet_l_Rtt(sock.j_packet_l_Rtt),
      Congestionn_Rtt(sock.Congestionn_Rtt),
      Pre_Congestionn_Rtt(sock.Pre_Congestionn_Rtt),
      Increment_window(sock.Increment_window),
      Base_window(sock.Base_window),
      Probe_window(sock.Probe_window)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_LOGIC("Invoked the copy constructor");
}

TcpAdaptiveReno::~TcpAdaptiveReno()
{
}

void
TcpAdaptiveReno::PktsAcked(Ptr<TcpSocketState> tcb_s_state, uint32_t Acked_packet, const Time& Rtt)
{
    NS_LOG_FUNCTION(this << tcb_s_state << Acked_packet << Rtt);

    if (Rtt.IsZero() == true)
    {
        NS_LOG_WARN("RTT measured is zero!");
        return;
    }
    m_ackedSegments = m_ackedSegments + Acked_packet;

    if (Minimum_Rtt.IsZero() == true)
    {
        Minimum_Rtt = Rtt;
    }
    else if (Rtt <= Minimum_Rtt)
    {
        Minimum_Rtt = Rtt;
    }
    Current_Rtt = Rtt;

    TcpWestwoodPlus::EstimateBW(Rtt, tcb_s_state);
}

double
TcpAdaptiveReno::EstimateCongestionLevel()
{
    float a = 0.85;
    if (Pre_Congestionn_Rtt < Minimum_Rtt)
        a = 0;

    double j_Congestion_Rtt = a * Pre_Congestionn_Rtt.GetSeconds();

    j_Congestion_Rtt = j_Congestion_Rtt + (1 - a) * j_packet_l_Rtt.GetSeconds();

    Congestionn_Rtt = Seconds(j_Congestion_Rtt);

    double temp1 = Current_Rtt.GetSeconds() - Minimum_Rtt.GetSeconds();
    double temp2 = j_Congestion_Rtt - Minimum_Rtt.GetSeconds();
    temp1 = temp1 / temp2;
    temp1 = std::min(temp1, 1.0);
    return temp1;
}

void
TcpAdaptiveReno::EstimateIncWnd(Ptr<TcpSocketState> tcb_s_state)
{
    int M = 1000;
    double MSS = static_cast<double>(tcb_s_state->m_segmentSize * tcb_s_state->m_segmentSize);
    double B = m_currentBW.Get().GetBitRate();
    double m_maxIncWnd = (B / M) * MSS;

    double ALPHA = 10;
    double temp = ((1 / ALPHA) - ((1 / ALPHA + 1) / (std::exp(ALPHA))));
    double BETA = 2 * m_maxIncWnd * temp;
    temp = ((1 / ALPHA) - ((1 / ALPHA + 0.5) / (std::exp(ALPHA))));
    double GAMMA = 1 - (2 * m_maxIncWnd * temp);

    double C = EstimateCongestionLevel();
    temp = (m_maxIncWnd / std::exp(ALPHA * C));
    Increment_window = (int)(temp + (BETA * C) + GAMMA);
}

void
TcpAdaptiveReno::CongestionAvoidance(Ptr<TcpSocketState> tcb_s_state, uint32_t Acked_segment)
{
    NS_LOG_FUNCTION(this << tcb_s_state << Acked_segment);

    if (Acked_segment)
    {
        EstimateIncWnd(tcb_s_state);
        double MSS = static_cast<double>(tcb_s_state->m_segmentSize * tcb_s_state->m_segmentSize);
        int W = tcb_s_state->m_cWnd.Get();
        double temp = MSS / W;
        temp = std::max(1.0, temp);
        Base_window = Base_window + static_cast<uint32_t>(temp);
        temp = (double)(Increment_window / W);
        temp = (double)(temp + Probe_window);
        Probe_window = std::max(temp, (double)0);
        tcb_s_state->m_cWnd = Base_window + Probe_window;
    }
}

uint32_t
TcpAdaptiveReno::GetSsThresh(Ptr<const TcpSocketState> tcb_s_state,
                             uint32_t bytesInFlight [[maybe_unused]])
{
    Pre_Congestionn_Rtt = Congestionn_Rtt;

    j_packet_l_Rtt = Current_Rtt;
    double C = EstimateCongestionLevel();

    int temp1 = (uint32_t)(tcb_s_state->m_cWnd / (1.0 + C));
    int temp2 = 2 * tcb_s_state->m_segmentSize;

    uint32_t ssthresh = std::max(temp1, temp2);

    Base_window = ssthresh;
    Probe_window = 0;

    return ssthresh;
}

Ptr<TcpCongestionOps>
TcpAdaptiveReno::Fork()
{
    return CreateObject<TcpAdaptiveReno>(*this);
}

} // namespace ns3