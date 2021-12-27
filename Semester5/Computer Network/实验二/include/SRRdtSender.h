#ifndef SR_RDT_SENDER_H
#define SR_RDT_SENDER_H
#include "RdtSender.h"
class SRRdtSender : public RdtSender {
  private:
    int expectSequenceNumberSend; // 下一个发送序号
    bool waitingState;            // 是否处于等待Ack的状态
    int base;                     //基序号
    int nextSeqNum;               //下一个序号
    int windowSize;                 //窗口长度
    int seqSize;                  //序列长度
    int rcvstatus[8];             //窗口内状态
    Packet packetWaitingAck[8];

  public:
    bool getWaitingState();
    bool send(const Message
                  &message); //发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
    void receive(const Packet &ackPkt); //接受确认Ack，将被NetworkServiceSimulator调用
    void timeoutHandler(int seqNum);    // Timeout handler，将被NetworkServiceSimulator调用
    void printSlideWindow();

  public:
    SRRdtSender();
    virtual ~SRRdtSender();
};

#endif