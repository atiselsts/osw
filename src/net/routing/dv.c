/*
 * Copyright (c) 2013 OSW. All rights reserved.
 * Copyright (c) 2008-2012 the MansOS team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of  conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//
// Distance-vector routing, mote side algorithm
//

#include "../mac.h"
#include "../routing.h"
#include "../socket.h"
#include <alarms.h>
#include <timing.h>
#include <print.h>
#include <random.h>
#include <net/net_stats.h>

static Socket_t roSocket;
static Alarm_t roForwardTimer;
static Alarm_t roRequestTimer;

#if MULTIHOP_FORWARDER
static void roForwardTimerCb(void *);
#endif
static void roRequestTimerCb(void *);
static void routingReceive(Socket_t *s, uint8_t *data, uint16_t len);

static Seqnum_t lastSeenSeqnum;
static uint16_t hopCountToRoot = MAX_HOP_COUNT;
static uint32_t lastRootMessageTime = (uint32_t) -ROUTING_INFO_VALID_TIME;
static OswAddress_t nexthopToRoot;

// -----------------------------------------------

static void markForwardTimerActive(uint16_t times)
{
    roForwardTimer.data = (void *) times;
}

#if MULTIHOP_FORWARDER
static bool isForwardTimerActive(void)
{
    return (bool) roForwardTimer.data;
}
#endif

static inline bool isRoutingInfoValid(void)
{
    return timeAfter32(lastRootMessageTime + ROUTING_INFO_VALID_TIME, (uint32_t)getJiffies());
}

void routingInit(void)
{
    socketOpen(&roSocket, routingReceive);
    socketBind(&roSocket, ROUTING_PROTOCOL_PORT);
    socketSetDstAddress(&roSocket, OSW_ADDR_BROADCAST);

#if MULTIHOP_FORWARDER
    alarmInit(&roForwardTimer, roForwardTimerCb, NULL);
#endif
    alarmInit(&roRequestTimer, roRequestTimerCb, NULL);
    alarmSchedule(&roRequestTimer, randomInRange(2000, 3000));
}

#if MULTIHOP_FORWARDER
static void roForwardTimerCb(void *x)
{
    if (hopCountToRoot >= MAX_HOP_COUNT) return;

    PRINTF("forward routing packet\n");

    RoutingInfoPacket_t routingInfo;
    routingInfo.packetType = ROUTING_INFORMATION;
    routingInfo.senderType = 0;
    routingInfo.rootAddress = rootAddress;
    routingInfo.hopCount = hopCountToRoot + 1;
    routingInfo.seqnum = lastSeenSeqnum;
    routingInfo.rootClockMs = getSyncTimeMs64();
    routingInfo.moteNumber = 0;

    // XXX: INC_NETSTAT(NETSTAT_PACKETS_SENT, EMPTY_ADDR);
    socketSend(&roSocket, &routingInfo, sizeof(routingInfo));

    uint16_t timesLeft = (uint16_t)roForwardTimer.data;
    --timesLeft;
    markForwardTimerActive(timesLeft);
    if (timesLeft) {
        // reschedule alarm
        alarmSchedule(&roForwardTimer, randomInRange(100, 300));
    }
}
#endif

static void roRequestTimerCb(void *x)
{
    if (isRoutingInfoValid()) return;

    // PRINTF("send routing request\n");

    RoutingRequestPacket_t req;
    req.packetType = ROUTING_REQUEST;
    req.senderType = 0;
    socketSend(&roSocket, &req, sizeof(req));
    markForwardTimerActive(0);
}

static void routingReceive(Socket_t *s, uint8_t *data, uint16_t len)
{
    // PRINTF("routingReceive %d bytes\n", len);

    if (len == 0) {
        PRINTF("routingReceive: no data!\n");
        return;
    }

#if PRECONFIGURED_NH_TO_ROOT
    if (s->recvMacInfo->originalSrc.shortAddr != PRECONFIGURED_NH_TO_ROOT) {
        PRINTF("Dropping routing info: not from the nexthop, but from %#04x\n",
                s->recvMacInfo->originalSrc.shortAddr);
        return;
    }
    PRINTF("Got routing info from the nexthop\n");
#endif

    uint8_t type = *data;
    if (type == ROUTING_REQUEST) {
#if MULTIHOP_FORWARDER
        if (!isForwardTimerActive()) {
            markForwardTimerActive(1);
            alarmSchedule(&roForwardTimer, randomInRange(1000, 3000));
        }
#endif
        return;
    }

    if (type != ROUTING_INFORMATION) {
        PRINTF("routingReceive: unknown type!\n");
        return;
    }

    if (len < sizeof(RoutingInfoPacket_t)) {
        PRINTF("routingReceive: too short for info packet!\n");
        return;
    }

    RoutingInfoPacket_t ri;
    memcpy(&ri, data, sizeof(RoutingInfoPacket_t));

    bool update = false;
    if (!isRoutingInfoValid() || timeAfter16(ri.seqnum, lastSeenSeqnum)) {
        // XXX: theoretically should add some time to avoid switching to
        // worse path only because packets from it travel faster
        update = true;
        //PRINTF("update routing info - more recent seqnum\n");
    } else if (ri.seqnum == lastSeenSeqnum) {
        if (ri.hopCount < hopCountToRoot) {
            update = true;
            //PRINTF("update routing info - better metric\n");
        }
    }
    if (ri.hopCount > MAX_HOP_COUNT) update = false;

    if (update) {
        rootAddress = ri.rootAddress;
        nexthopToRoot = s->recvMacInfo->originalSrc.shortAddr;
        lastSeenSeqnum = ri.seqnum;
        hopCountToRoot = ri.hopCount;
#if MULTIHOP_FORWARDER
        if (!isForwardTimerActive()) {
            markForwardTimerActive(1);
            alarmSchedule(&roForwardTimer, randomInRange(1000, 3000));
        }
#endif
        lastRootMessageTime = getJiffies();
    }
}

static bool checkHoplimit(MacInfo_t *info)
{
    if (IS_LOCAL(info)) return true; // only for forwarding
    if (!info->hoplimit) return true; // hoplimit is optional
    if (--info->hoplimit) return true; // shold be larger than zero after decrement
    return false;
}

RoutingDecision_e routePacket(MacInfo_t *info)
{
    OswAddrVariant_t *dst = &info->originalDst;

    // PRINTF("dst address=0x%04x, nexthop=0x%04x\n", dst->shortAddr, info->immedDst.shortAddr);
    // PRINTF("  localAddress=0x%04x\n", localAddress);

    // fix root address if we are sending it to the root
    if (IS_LOCAL(info) && dst->shortAddr == OSW_ADDR_ROOT) {
        intToAddr(info->originalDst, rootAddress);
        // info->hoplimit = hopCountToRoot;
        info->hoplimit = MAX_HOP_COUNT;
    }

    if (isLocalAddress(dst)) {
        INC_NETSTAT(NETSTAT_PACKETS_RECV, info->originalSrc.shortAddr);
        return RD_LOCAL;
    }
    if (isBroadcast(dst)) {
        if (!IS_LOCAL(info)){
            INC_NETSTAT(NETSTAT_PACKETS_RECV, info->originalSrc.shortAddr);
        }
        // don't forward broadcast packets
        return IS_LOCAL(info) ? RD_BROADCAST : RD_LOCAL;
    }

    // check if hop limit allows the packet to be forwarded
    if (!checkHoplimit(info)) {
        PRINTF("hoplimit reached!\n");
        return RD_DROP;
    }

#if MULTIHOP_FORWARDER
    if (dst->shortAddr == rootAddress) {
        if (isRoutingInfoValid()) {
            //PRINTF("using 0x%04x as nexthop to root\n", nexthopToRoot);
            if (!IS_LOCAL(info)) {
#if PRECONFIGURED_NH_TO_ROOT
                if (info->immedDst.shortAddr != localAddress) {
                    PRINTF("Dropping, I'm not a nexthop for sender %#04x\n",
                            info->originalSrc.shortAddr);
                    INC_NETSTAT(NETSTAT_PACKETS_DROPPED_RX, EMPTY_ADDR);
                    return RD_DROP;
                }
#endif
                // if (info->hoplimit < hopCountToRoot){
                //     PRINTF("Dropping, can't reach host with left hopCounts\n");
                //     return RD_DROP;
                // } 
                PRINTF("****************** Forwarding a packet to root for %#04x!\n",
                        info->originalSrc.shortAddr);
                INC_NETSTAT(NETSTAT_PACKETS_FWD, nexthopToRoot);
            } else{
                //INC_NETSTAT(NETSTAT_PACKETS_SENT, nexthopToRoot);     // Done @ comm.c
            }
            info->immedDst.shortAddr = nexthopToRoot;
            return RD_UNICAST;
        } else {
            PRINTF("root routing info not present or expired!\n");
            return RD_DROP;
        }
    }
#endif

    if (IS_LOCAL(info)) {
        //INC_NETSTAT(NETSTAT_PACKETS_SENT, dst->shortAddr);        // Done @ comm.c
        // send out even with an unknown nexthop, makes sense?
        return RD_UNICAST;
    }
    return RD_DROP;
}
