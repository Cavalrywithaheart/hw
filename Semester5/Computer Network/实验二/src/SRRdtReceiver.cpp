#include "SRRdtReceiver.h"
#include "Global.h"

SRRdtReceiver::SRRdtReceiver() {
    expectSequenceNumberRcvd = 0;
    seqSize = 8;
    windowSize = 4;
    lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
    lastAckPkt.checksum = 0;
    lastAckPkt.seqnum = -1; //忽略该字段
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
        lastAckPkt.payload[i] = '.';
    lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
    for (int i = 0; i < 8; i++)
        packetRcv[i] = 0;
}

SRRdtReceiver::~SRRdtReceiver() {}

void SRRdtReceiver::receive(const Packet &packet) {
    //检查校验和是否正确
    int checkSum = pUtils->calculateCheckSum(packet);
    //如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
    if (checkSum == packet.checksum) {
        if (expectSequenceNumberRcvd == packet.seqnum) {
            pUtils->printPacket("接收方正确收到发送方的报文", packet);
            Message msg;
            memcpy(msg.data, packet.payload, sizeof(packet.payload));
            pns->delivertoAppLayer(RECEIVER, msg);
            packetRcv[expectSequenceNumberRcvd] = 0;
            expectSequenceNumberRcvd = (expectSequenceNumberRcvd + 1) % seqSize;
            int oldExpect = expectSequenceNumberRcvd;
            for (int i = oldExpect; i != (oldExpect + windowSize - 1) % seqSize; i = (i + 1) % seqSize) {
                if (packetRcv[i] == 1) {
                    Message msg;
                    memcpy(msg.data, packetBuffer[i].payload, sizeof(packetBuffer[i].payload));
                    pns->delivertoAppLayer(RECEIVER, msg);
                    pUtils->printPacket("数据递交到上层", packetBuffer[i]);
                    packetRcv[i] = 0;
                    expectSequenceNumberRcvd = (expectSequenceNumberRcvd + 1) % seqSize;
                } else
                    break;
            }
        } else if ((expectSequenceNumberRcvd + windowSize) % seqSize > expectSequenceNumberRcvd) {
            if (packet.seqnum >= expectSequenceNumberRcvd && packet.seqnum < (expectSequenceNumberRcvd + windowSize) % seqSize) {
                pUtils->printPacket("接收方收到发送方的错序报文", packet);
                packetBuffer[packet.seqnum] = packet;
                packetRcv[packet.seqnum] = 1;
            }
        } else if ((expectSequenceNumberRcvd + windowSize) % seqSize < expectSequenceNumberRcvd) {
            if (packet.seqnum >= expectSequenceNumberRcvd || packet.seqnum < (expectSequenceNumberRcvd + windowSize) % seqSize) {
                pUtils->printPacket("接收方收到发送方的错序报文", packet);
                packetBuffer[packet.seqnum] = packet;
                packetRcv[packet.seqnum] = 1;
            }
        }
        lastAckPkt.acknum = packet.seqnum; //确认序号等于收到的报文序号
        lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
        pUtils->printPacket("接收方发送确认报文", lastAckPkt);
        pns->sendToNetworkLayer(SENDER, lastAckPkt); //调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
    } else
        pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
}
