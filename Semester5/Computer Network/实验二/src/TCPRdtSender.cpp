#include "TCPRdtSender.h"
#include "Global.h"

TCPRdtSender::TCPRdtSender() {
    expectSequenceNumberSend = 0;
    waitingState = false;
    base = 0;
    nextSeqNum = 0;
    windowSize = 4;
    seqSize = 8;
    dupAck = 0;
}

TCPRdtSender::~TCPRdtSender() {}

bool TCPRdtSender::getWaitingState() { return waitingState = (base + windowSize) % seqSize == nextSeqNum % seqSize; } //判断窗口是否已经满了，是否进入等待状态

bool TCPRdtSender::send(const Message &message) {
    if (getWaitingState()) //窗口已满
        return false;
    packetWaitingAck[nextSeqNum].acknum = -1; //忽略该字段
    packetWaitingAck[nextSeqNum].seqnum = nextSeqNum;
    packetWaitingAck[nextSeqNum].checksum = 0;
    memcpy(packetWaitingAck[nextSeqNum].payload, message.data, sizeof(message.data));
    packetWaitingAck[nextSeqNum].checksum = pUtils->calculateCheckSum(packetWaitingAck[nextSeqNum]);
    pUtils->printPacket("发送方发送报文", packetWaitingAck[nextSeqNum]);
    std::cout << std::endl << "发送前： ";
    printSlideWindow();
    if (base == nextSeqNum)
        pns->startTimer(SENDER, Configuration::TIME_OUT, base);      //启动发送方定时器
    pns->sendToNetworkLayer(RECEIVER, packetWaitingAck[nextSeqNum]); //调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
    nextSeqNum = (nextSeqNum + 1) % seqSize;
    std::cout << std::endl << "发送后： ";
    printSlideWindow();
    return true;
}

void TCPRdtSender::receive(const Packet &ackPkt) {
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    //如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
    if (checkSum == ackPkt.checksum) {
        if (isinWindow(ackPkt.acknum)) {
            base = (ackPkt.acknum + 1) % seqSize;
            pUtils->printPacket("发送方正确收到确认", ackPkt);
            std::cout << std::endl << "移动窗口：";
            printSlideWindow();
            pns->stopTimer(SENDER, base);
            if (base != nextSeqNum) {
                pns->startTimer(SENDER, Configuration::TIME_OUT, base);
                dupAck = 0;
                return;
            }
        } else {
            dupAck += 1;
            if (dupAck == 3) {
                pns->stopTimer(SENDER, base);
                pns->sendToNetworkLayer(RECEIVER, packetWaitingAck[base]);
                pns->startTimer(SENDER, Configuration::TIME_OUT, base);
                pUtils->printPacket("已收到3个冗余ack，触发快速重传", packetWaitingAck[base]);
                dupAck = 0;
            }
        }
    } else
        pUtils->printPacket("接收方收到的包损坏", ackPkt);
}

void TCPRdtSender::timeoutHandler(int seqNum) {
    if (base == nextSeqNum) //窗口为空的特殊情况
        return;
    else {
        pns->stopTimer(SENDER, base);
        pns->sendToNetworkLayer(RECEIVER, packetWaitingAck[base]);
        pns->startTimer(SENDER, Configuration::TIME_OUT, base);
        pUtils->printPacket("重发超时分组", packetWaitingAck[base]);
    }
}

bool TCPRdtSender::isinWindow(int seqNum) {
    if ((base + windowSize) % seqSize < base)
        return (seqNum >= base || seqNum < (base + windowSize) % seqSize);
    if ((base + windowSize) % seqSize > base)
        return (seqNum >= base && seqNum < (base + windowSize) % seqSize);
    return false;
}

void TCPRdtSender::printSlideWindow() {
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