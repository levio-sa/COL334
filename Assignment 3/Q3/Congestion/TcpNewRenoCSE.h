// Adapted from tcp-congestion-ops.h

#ifndef TCPNEWRENOCSE_H
#define TCPNEWRENOCSE_H

#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-recovery-ops.h"

namespace ns3 {

class TcpNewRenoCSE : public TcpNewReno
{
public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId (void);

    /**
     * Create an unbound tcp socket.
     */
    TcpNewRenoCSE (void);

    /**
     * \brief Copy constructor
     * \param sock the object to copy
     */
    TcpNewRenoCSE (const TcpNewRenoCSE& sock);
    virtual ~TcpNewRenoCSE (void);

    virtual std::string GetName () const;


  /**
   * \brief Adjust cwnd
   *
   * \param tcb internal congestion state
   * \param segmentsAcked count of segments ACKed
   */
    virtual void IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);

  /**
   * \brief Get slow start threshold
   *
   * \param tcb internal congestion state
   * \param bytesInFlight bytes in flight
   *
   * \return the slow start threshold value
   */
    virtual uint32_t GetSsThresh (Ptr<const TcpSocketState> tcb,
                                    uint32_t bytesInFlight);

    virtual Ptr<TcpCongestionOps> Fork ();

protected:
    virtual uint32_t SlowStart (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
    virtual void CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
private:
};

} // namespace ns3

#endif // TCPNEWRENOCSE_H
