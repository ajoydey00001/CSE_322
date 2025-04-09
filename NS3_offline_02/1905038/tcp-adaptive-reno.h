
#ifndef TCP_ADAPTIVERENO_H
#define TCP_ADAPTIVERENO_H

#include "tcp-congestion-ops.h"
#include "tcp-westwood-plus.h"
#include "ns3/tcp-recovery-ops.h"
#include "ns3/sequence-number.h"
#include "ns3/traced-value.h"
#include "ns3/event-id.h"

namespace ns3
{

class Packet;
class TcpHeader;
class Time;
class EventId;

class TcpAdaptiveReno : public TcpWestwoodPlus
{
  public:
    static TypeId GetTypeId();

    TcpAdaptiveReno ();
    TcpAdaptiveReno (const TcpAdaptiveReno& sock);
  virtual ~TcpAdaptiveReno (void);

    enum FilterType
    {
        NONE,
        TUSTIN
    };

    uint32_t GetSsThresh(Ptr<const TcpSocketState> tcb_s_state, uint32_t bytesInFlight) override;

    void PktsAcked(Ptr<TcpSocketState> tcb_s_state, uint32_t Acked_packet, const Time& Rtt) override;

    virtual Ptr<TcpCongestionOps> Fork() override;

  private:
    
    void EstimateBW(const Time& rtt, Ptr<TcpSocketState> tcb);
    double EstimateCongestionLevel();

    void EstimateIncWnd(Ptr<TcpSocketState> tcb_s_state);



  protected:
    virtual void CongestionAvoidance (Ptr<TcpSocketState> tcb_s_state, uint32_t Acked_segment);

  Time                   Minimum_Rtt;              
  Time                   Current_Rtt;            
  Time                   j_packet_l_Rtt;            // RTT of j packet loss
  Time                   Congestionn_Rtt;                //Conjestion RTT (j th event)
  Time                   Pre_Congestionn_Rtt;            // Previous Conjestion RTT (j-1 th event)
  int32_t                Increment_window;                
  uint32_t               Base_window;               
  int32_t                Probe_window;              
};

} 

#endif 