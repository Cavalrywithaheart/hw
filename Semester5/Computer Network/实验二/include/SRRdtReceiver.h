#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H
#include "RdtReceiver.h"
class SRRdtReceiver : public RdtReceiver {
  private:
    int expectSequenceNumberRcvd; // 期待收到的下一个报文序号
    int seqSize;
    int windowSize;
    int packetRcv[8];
    Packet lastAckPkt; //上次发送的确认报文
    Packet packetBuffer[8];

  public:
    void receive(const Packet &packet); //接收报文，将被NetworkService调用

  public:
    SRRdtReceiver();
    virtual ~SRRdtReceiver();
};

#endif