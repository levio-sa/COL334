// Adapted from tcp-congestion-ops.cc

#include "TcpNewRenoCSE.h"
#include "ns3/log.h"
#include "math.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TcpNewRenoCSE");
NS_OBJECT_ENSURE_REGISTERED (TcpNewRenoCSE);

TypeId
TcpNewRenoCSE::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::TcpNewRenoCSE")
        .SetParent<TcpNewReno> ()
        .AddConstructor<TcpNewRenoCSE> ()
        .SetGroupName ("Internet")
    ;
    return tid;
}

TcpNewRenoCSE::TcpNewRenoCSE (void)
    : TcpNewReno ()
{
  NS_LOG_FUNCTION (this);
}


TcpNewRenoCSE::TcpNewRenoCSE (const TcpNewRenoCSE& sock)
  : TcpNewReno (sock) 
{
  NS_LOG_FUNCTION (this);
}

TcpNewRenoCSE::~TcpNewRenoCSE (void)
{
  NS_LOG_FUNCTION (this);
}

Ptr<TcpCongestionOps>
TcpNewRenoCSE::Fork (void)
{
  return CopyObject<TcpNewRenoCSE> (this);
}

uint32_t
TcpNewRenoCSE::SlowStart (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION (this << tcb << segmentsAcked);

    if (segmentsAcked >= 1)
    {
        double adder = pow(tcb->m_segmentSize,1.9)/(tcb->m_cWnd);
        tcb->m_cWnd += static_cast<uint32_t> (adder);
        NS_LOG_INFO ("In SlowStart, updated to cwnd " << tcb->m_cWnd << " ssthresh " << tcb->m_ssThresh);
        return segmentsAcked - 1;
    }

    return 0;
}

void
TcpNewRenoCSE::CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
  NS_LOG_FUNCTION (this << tcb << segmentsAcked);

  if (segmentsAcked > 0)
    {
        double adder = 0.5*tcb->m_segmentSize;
        tcb->m_cWnd += static_cast<uint32_t> (adder);
        NS_LOG_INFO ("In CongAvoid, updated to cwnd " << tcb->m_cWnd <<
                    " ssthresh " << tcb->m_ssThresh);
    }
}


void
TcpNewRenoCSE::IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION (this << tcb << segmentsAcked);

    if (tcb->m_cWnd < tcb->m_ssThresh) // slow start
    {
        segmentsAcked = SlowStart (tcb, segmentsAcked);
    }

    if (tcb->m_cWnd >= tcb->m_ssThresh) // congestion avoidance
    {
        CongestionAvoidance (tcb, segmentsAcked);
    }
}

std::string
TcpNewRenoCSE::GetName () const
{
  return "TcpNewRenoCSE";
}

uint32_t
TcpNewRenoCSE::GetSsThresh (Ptr<const TcpSocketState> tcb,
                      uint32_t bytesInFlight)
{
  NS_LOG_FUNCTION (this << tcb << bytesInFlight);

  return std::max (2 * tcb->m_segmentSize, bytesInFlight / 2);

}

} // namespace ns3
