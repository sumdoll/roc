/*
 * Copyright (c) 2015 Roc authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

//! @file roc_netio/target_uv/roc_netio/udp_receiver.h
//! @brief UDP receiver.

#ifndef ROC_NETIO_UDP_RECEIVER_H_
#define ROC_NETIO_UDP_RECEIVER_H_

#include <uv.h>

#include "roc_core/buffer_pool.h"
#include "roc_core/iallocator.h"
#include "roc_core/list.h"
#include "roc_core/list_node.h"
#include "roc_core/refcnt.h"
#include "roc_packet/address.h"
#include "roc_packet/iwriter.h"
#include "roc_packet/packet_pool.h"

namespace roc {
namespace netio {

//! UDP receiver.
class UDPReceiver : public core::RefCnt<UDPReceiver>, public core::ListNode {
public:
    //! Initialize.
    UDPReceiver(uv_loop_t& event_loop,
                packet::IWriter& writer,
                packet::PacketPool& packet_pool,
                core::BufferPool<uint8_t>& buffer_pool,
                core::IAllocator& allocator);

    //! Destroy.
    ~UDPReceiver();

    //! Start receiver.
    //! @remarks
    //!  Should be called from the event loop thread.
    bool start(packet::Address& bind_address);

    //! Asynchronous stop.
    //! @remarks
    //!  Should be called from the event loop thread.
    void stop();

    //! Asynchronous remove.
    //! @remarks
    //!  Should be called from the event loop thread.
    void remove(core::List<UDPReceiver>& container);

    //! Get bind address.
    const packet::Address& address() const;

private:
    static void close_cb_(uv_handle_t* handle);
    static void alloc_cb_(uv_handle_t* handle, size_t size, uv_buf_t* buf);
    static void recv_cb_(uv_udp_t* handle,
                         ssize_t nread,
                         const uv_buf_t* buf,
                         const sockaddr* addr,
                         unsigned flags);

    friend class core::RefCnt<UDPReceiver>;

    void destroy();

    bool init_();
    bool bind_(packet::Address&);
    bool getsockname_(packet::Address&);
    bool start_(const packet::Address&);
    void close_();

    core::IAllocator& allocator_;

    uv_loop_t& loop_;

    uv_udp_t handle_;
    bool handle_initialized_;

    packet::Address address_;
    packet::IWriter& writer_;

    packet::PacketPool& packet_pool_;
    core::BufferPool<uint8_t>& buffer_pool_;

    core::List<UDPReceiver>* container_;

    unsigned packet_counter_;
};

} // namespace netio
} // namespace roc

#endif // ROC_NETIO_UDP_RECEIVER_H_
