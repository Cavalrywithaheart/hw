#include "SRRdtSender.h"
#include "Global.h"

SRRdtSender::SRRdtSender() {
    expectSequenceNumberSend = 0;
    waitingState = false;
    base = 0;
    nextSeqNum = 0;
    windowSize = 4;
    seqSize = 8;
    for (int i = 0; i < 8; i++)
        rcvstatus[i] = 0;
}

SRRdtSender::~SRRdtSender() {}

bool SRRdtSender::getWaitingState() {
    return waitingState = (base + windowSize) % seqSize == nextSeqNum % seqSize; //判断窗口是否已经满了，是否进入等待状态
}

bool SRRdtSender::send(const Message &message) {
    if (getWaitingState()) //发送方处于等待确认状态
        return false;
    packetWaitingAck[nextSeqNum].acknum = -1; //忽略该字段
    packetWaitingAck[nextSeqNum].seqnum = nextSeqNum;
    packetWaitingAck[nextSeqNum].checksum = 0;
    memcpy(packetWaitingAck[nextSeqNum].payload, message.data, sizeof(message.data));
    packetWaitingAck[nextSeqNum].checksum = pUtils->calculateCheckSum(packetWaitingAck[nextSeqNum]);
    pUtils->printPacket("发送方发送报文", packetWaitingAck[nextSeqNum]);
    std::cout << std::endl << "发送前： ";
    printSlideWindow();
    pns->startTimer(SENDER, Configuration::TIME_OUT, nextSeqNum);
    pns->sendToNetworkLayer(RECEIVER, packetWaitingAck[nextSeqNum]); //调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
    nextSeqNum = (nextSeqNum + 1) % seqSize;
    std::cout << std::endl << "发送后： ";
    printSlideWindow();
    return true;
}

void SRRdtSender::receive(const Packet &ackPkt) {
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    //如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
    if (checkSum == ackPkt.checksum) {
        pns->stopTimer(SENDER, ackPkt.acknum);
        std::cout << std::endl << "移动窗口：";
        printSlideWindow();
        if (base == ackPkt.acknum) {
            rcvstatus[base] = 0;
            base = (base + 1) % seqSize;
            int oldBase = base;
            for (int i = oldBase; i != (oldBase + windowSize - 1) % seqSize; i = (i + 1) % seqSize) {
                if (rcvstatus[i] == 1) {
                    rcvstatus[i] = 0;
                    base = (base + 1) % seqSize;
                } else
                    break;
            }
        } else if ((base + windowSize) % seqSize > base) {
            if (ackPkt.acknum >= base && ackPkt.acknum < (base + windowSize) % seqSize)
                rcvstatus[ackPkt.acknum] = 1;
        } else if ((base + windowSize) % seqSize < base) {
            if (ackPkt.acknum >= base || ackPkt.acknum < (base + windowSize) % seqSize)
                rcvstatus[ackPkt.acknum] = 1;
        } else {
            pUtils->printPacket("接受方收到的包失序", ackPkt);
        }
    } else {
        pUtils->printPacket("接受方收到的包损坏", ackPkt);
    }
    return;
}

void SRRdtSender::timeoutHandler(int seqNum) {
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
    pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[seqNum]);
    pUtils->printPacket("重发超时分组", this->packetWaitingAck[seqNum]);
}

void SRRdtSender::printSlideWindow() {
    for (int i = 0; i < seqSize; i++) {
        if (i == nextSeqNum)
            std::cout << "\033[34m" << i << "\033[0m  ";
        else if (i >= base && i <= (base + windowSize - 1) % seqSize && base < (base + windowSize - 1) % seqSize)
            std::cout << "\033[33m" << i << "\033[0m  ";
        else if (base > (base + windowSize - 1) % seqSize && (i >= base || i <= (base + windowSize - 1) % seqSize))
            std::cout << "\033[33m" << i << "\033[0m  ";
        else
            std::cout << i << "  ";
    }
    std::cout << std::endl << std::endl;
}