/*
 *  Point-to-Point Tunneling Protocol for Linux
 *
 *    Authors: Dmitry Kozlov <xeb@mail.ru>
 *
 *    This program is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU General Public License
 *    as published by the Free Software Foundation; either version
 *    2 of the License, or (at your option) any later version.
 *
 */

#include <linux/string.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/netdevice.h>
#include <linux/net.h>
#include <linux/skbuff.h>
#include <linux/vmalloc.h>
#include <linux/init.h>
#include <linux/ppp_channel.h>
#include <linux/ppp_defs.h>
#include <linux/if_pppox.h>
#include <linux/ppp-ioctl.h>
#include <linux/notifier.h>
#include <linux/file.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/rcupdate.h>
#include <linux/spinlock.h>

#include <net/sock.h>
#include <net/protocol.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/gre.h>
#include <net/pptp.h>

#include <linux/uaccess.h>
#ifdef CONFIG_TENDA_PRIVATE_KM
#include <net/km_common.h>
#endif

#define PPTP_DRIVER_VERSION "0.8.5"

#define MAX_CALLID 65535

#if defined(CONFIG_BCM_KF_BLOG) && defined(CONFIG_BLOG)
extern int bcm_ppp_rcv_decomp_run(struct ppp_channel *);

DECLARE_BITMAP(callid_bitmap, MAX_CALLID + 1);
struct pppox_sock __rcu **callid_sock;

DEFINE_SPINLOCK(chan_lock);
#else
static DECLARE_BITMAP(callid_bitmap, MAX_CALLID + 1);
static struct pppox_sock __rcu **callid_sock;

static DEFINE_SPINLOCK(chan_lock);
#endif

static struct proto pptp_sk_proto __read_mostly;
static const struct ppp_channel_ops pptp_chan_ops;
static const struct proto_ops pptp_ops;

static struct pppox_sock *lookup_chan(u16 call_id, __be32 s_addr)
{
    struct pppox_sock *sock;
    struct pptp_opt *opt;

    rcu_read_lock();
    sock = rcu_dereference(callid_sock[call_id]);
    if (sock) {
        opt = &sock->proto.pptp;
        if (opt->dst_addr.sin_addr.s_addr != s_addr)
            sock = NULL;
        else
            sock_hold(sk_pppox(sock));
    }
    rcu_read_unlock();

    return sock;
}

static int lookup_chan_dst(u16 call_id, __be32 d_addr)
{
    struct pppox_sock *sock;
    struct pptp_opt *opt;
    int i;

    rcu_read_lock();
    i = 1;
    for_each_set_bit_from(i, callid_bitmap, MAX_CALLID) {
        sock = rcu_dereference(callid_sock[i]);
        if (!sock)
            continue;
        opt = &sock->proto.pptp;
        if (opt->dst_addr.call_id == call_id &&
              opt->dst_addr.sin_addr.s_addr == d_addr)
            break;
    }
    rcu_read_unlock();

    return i < MAX_CALLID;
}

static int add_chan(struct pppox_sock *sock,
            struct pptp_addr *sa)
{
    static int call_id;

    spin_lock(&chan_lock);
    if (!sa->call_id)    {
        call_id = find_next_zero_bit(callid_bitmap, MAX_CALLID, call_id + 1);
        if (call_id == MAX_CALLID) {
            call_id = find_next_zero_bit(callid_bitmap, MAX_CALLID, 1);
            if (call_id == MAX_CALLID)
                goto out_err;
        }
        sa->call_id = call_id;
    } else if (test_bit(sa->call_id, callid_bitmap)) {
        goto out_err;
    }

    sock->proto.pptp.src_addr = *sa;
    set_bit(sa->call_id, callid_bitmap);
    rcu_assign_pointer(callid_sock[sa->call_id], sock);
    spin_unlock(&chan_lock);

    return 0;

out_err:
    spin_unlock(&chan_lock);
    return -1;
}

static void del_chan(struct pppox_sock *sock)
{
    spin_lock(&chan_lock);
    clear_bit(sock->proto.pptp.src_addr.call_id, callid_bitmap);
    RCU_INIT_POINTER(callid_sock[sock->proto.pptp.src_addr.call_id], NULL);
    spin_unlock(&chan_lock);
}

static int pptp_xmit(struct ppp_channel *chan, struct sk_buff *skb)
{
    struct sock *sk = (struct sock *) chan->private;
    struct pppox_sock *po = pppox_sk(sk);
    struct net *net = sock_net(sk);
    struct pptp_opt *opt = &po->proto.pptp;
    struct pptp_gre_header *hdr;
    unsigned int header_len = sizeof(*hdr);
    struct flowi4 fl4;
    int islcp;
    int len;
    unsigned char *data;
    __u32 seq_recv;


    struct rtable *rt;
    struct net_device *tdev;
    struct iphdr  *iph;
    int    max_headroom;

    if (sk_pppox(po)->sk_state & PPPOX_DEAD)
        goto tx_error;

    rt = ip_route_output_ports(net, &fl4, NULL,
                   opt->dst_addr.sin_addr.s_addr,
                   opt->src_addr.sin_addr.s_addr,
                   0, 0, IPPROTO_GRE,
                   RT_TOS(0), 0);
    if (IS_ERR(rt))
        goto tx_error;

    tdev = rt->dst.dev;

    max_headroom = LL_RESERVED_SPACE(tdev) + sizeof(*iph) + sizeof(*hdr) + 2;

    if (skb_headroom(skb) < max_headroom || skb_cloned(skb) || skb_shared(skb)) {
        struct sk_buff *new_skb = skb_realloc_headroom(skb, max_headroom);
        if (!new_skb) {
            ip_rt_put(rt);
            goto tx_error;
        }
        if (skb->sk)
            skb_set_owner_w(new_skb, skb->sk);
        consume_skb(skb);
        skb = new_skb;
    }

    data = skb->data;
    islcp = ((data[0] << 8) + data[1]) == PPP_LCP && 1 <= data[2] && data[2] <= 7;

    /* compress protocol field */
    if ((opt->ppp_flags & SC_COMP_PROT) && data[0] == 0 && !islcp)
        skb_pull(skb, 1);

    /* Put in the address/control bytes if necessary */
    if ((opt->ppp_flags & SC_COMP_AC) == 0 || islcp) {
        data = skb_push(skb, 2);
        data[0] = PPP_ALLSTATIONS;
        data[1] = PPP_UI;
    }

    len = skb->len;

    seq_recv = opt->seq_recv;

#if defined(CONFIG_BCM_KF_BLOG) && defined(CONFIG_BLOG)
    /*always add ack, makes it simple to adjust ack in blog_emit */
    if ((opt->ack_sent == seq_recv) && !blog_gre_tunnel_accelerated())
        header_len -= sizeof(hdr->ack);
#else
    if (opt->ack_sent == seq_recv)
        header_len -= sizeof(hdr->ack);
#endif

    /* Push down and install GRE header */
    skb_push(skb, header_len);
    hdr = (struct pptp_gre_header *)(skb->data);

    hdr->gre_hd.flags = GRE_KEY | GRE_VERSION_1 | GRE_SEQ;
    hdr->gre_hd.protocol = GRE_PROTO_PPP;
    hdr->call_id = htons(opt->dst_addr.call_id);

#if defined(CONFIG_BCM_KF_BLOG) && defined(CONFIG_BLOG)
    /* for accelerated case update the sequencenumber later */
        if (blog_gre_tunnel_accelerated())
        {
            hdr->seq         = 0;
            hdr->gre_hd.flags |= GRE_ACK;
            hdr->ack = 0;
        }
        else{
#endif


    hdr->seq = htonl(++opt->seq_sent);
    if (opt->ack_sent != seq_recv)    {
        /* send ack with this message */
        hdr->gre_hd.flags |= GRE_ACK;
        hdr->ack  = htonl(seq_recv);
        opt->ack_sent = seq_recv;
    }
#if defined(CONFIG_BCM_KF_BLOG) && defined(CONFIG_BLOG)
    }
#endif

    hdr->payload_len = htons(len);

    /*    Push down and install the IP header. */

    skb_reset_transport_header(skb);
    skb_push(skb, sizeof(*iph));
    skb_reset_network_header(skb);
    memset(&(IPCB(skb)->opt), 0, sizeof(IPCB(skb)->opt));
    IPCB(skb)->flags &= ~(IPSKB_XFRM_TUNNEL_SIZE | IPSKB_XFRM_TRANSFORMED | IPSKB_REROUTED);

    iph =    ip_hdr(skb);
    iph->version =    4;
    iph->ihl =    sizeof(struct iphdr) >> 2;
    if (ip_dont_fragment(sk, &rt->dst))
        iph->frag_off    =    htons(IP_DF);
    else
        iph->frag_off    =    0;
    iph->protocol = IPPROTO_GRE;
    iph->tos      = 0;
    iph->daddr    = fl4.daddr;
    iph->saddr    = fl4.saddr;
    iph->ttl      = ip4_dst_hoplimit(&rt->dst);
    iph->tot_len  = htons(skb->len);

    skb_dst_drop(skb);
    skb_dst_set(skb, &rt->dst);

    nf_reset(skb);

    skb->ip_summed = CHECKSUM_NONE;
    ip_select_ident(net, skb, NULL);
    ip_send_check(iph);

    ip_local_out(net, skb->sk, skb);
    return 1;

tx_error:
    kfree_skb(skb);
    return 1;
}

static int pptp_rcv_core(struct sock *sk, struct sk_buff *skb)
{
    struct pppox_sock *po = pppox_sk(sk);
    struct pptp_opt *opt = &po->proto.pptp;
    int headersize, payload_len, seq;
    __u8 *payload;
    struct pptp_gre_header *header;

    if (!(sk->sk_state & PPPOX_CONNECTED)) {
        if (sock_queue_rcv_skb(sk, skb))
            goto drop;
        return NET_RX_SUCCESS;
    }

    header = (struct pptp_gre_header *)(skb->data);
    headersize  = sizeof(*header);

    /* test if acknowledgement present */
    if (GRE_IS_ACK(header->gre_hd.flags)) {
        __u32 ack;

        if (!pskb_may_pull(skb, headersize))
            goto drop;
        header = (struct pptp_gre_header *)(skb->data);

        /* ack in different place if S = 0 */
        ack = GRE_IS_SEQ(header->gre_hd.flags) ? header->ack : header->seq;

        ack = ntohl(ack);

        if (ack > opt->ack_recv)
            opt->ack_recv = ack;
        /* also handle sequence number wrap-around  */
        if (WRAPPED(ack, opt->ack_recv))
            opt->ack_recv = ack;
    } else {
        headersize -= sizeof(header->ack);
    }
    /* test if payload present */
    if (!GRE_IS_SEQ(header->gre_hd.flags))
        goto drop;

    payload_len = ntohs(header->payload_len);
    seq         = ntohl(header->seq);

    /* check for incomplete packet (length smaller than expected) */
    if (!pskb_may_pull(skb, headersize + payload_len))
        goto drop;

    payload = skb->data + headersize;
#if defined(CONFIG_BCM_KF_BLOG) && defined(CONFIG_BLOG) 
#define    PPTP_RCV_SEQ_CHECK(seq_rx, label1, label2, label3) \
    { \
        if (seq < (seq_rx) || WRAPPED(opt->seq_recv, seq)) { \
            if ((payload[0] == PPP_ALLSTATIONS) && (payload[1] == PPP_UI) && \
                    (PPP_PROTOCOL(payload) == PPP_LCP) && \
                    ((payload[4] == PPP_LCP_ECHOREQ) || (payload[4] == PPP_LCP_ECHOREP))) \
                goto label1; \
            else \
                goto label2; \
        } else { \
            goto label3; \
        } \
    }

    if (!blog_gre_tunnel_accelerated() || bcm_ppp_rcv_decomp_run(&po->chan))
    {   
        /* check for expected sequence number */
        PPTP_RCV_SEQ_CHECK(opt->seq_recv + 1, allow_packet, drop, allow_packet2);
    }
    else /* blog_gre_tunnel_accelerated is true, so opt->seq_recv and opt->ack_recv have been ++ by  pptp_rcv_check() */
    {
        /* check for expected sequence number */
#ifdef CONFIG_TENDA_PRIVATE_KM
        /* 第一个参数为0的目地是让seq < (seq_rx)一定不成立，由于在bcm gre加速开启时，博通会在收包时就将
           opt->seq_recv 赋值，当系统开启RPS功能之后，会经历软中断过程，数据被其他CPU处理，其处理时间会
           比只用博通bcm_enet内核线程时间长,如果服务器pptpd在协商过程中快速发来两个包，seq 1先经过了bcm 
           gre加速处理，opt->seq_recv被赋值1，在还没有经过pptp_recv_core处理前，又一个包seq 2经过bcm 
           gre处理，opt->seq_recv更新为2，此时seq 1包经过pptp_recv_core时，pptp seq check失败，导致丢
           包，pptpc连接失败 */
        PPTP_RCV_SEQ_CHECK(0, allow_packet, drop, allow_packet);
#else
        PPTP_RCV_SEQ_CHECK(opt->seq_recv, allow_packet, drop, allow_packet);
#endif
allow_packet2:
#else
    /* check for expected sequence number */
#ifdef CONFIG_SA_FASTPATH_NAT_PPTP
        int flags = 0;
        if (ctf_pptp_compress_packet(payload)) {
            opt->pptp_over_mppe = true;
        }
        /* pppoe+pptp客户端加速跑流掉线问题(不加密)，是由于pptp数据包走了fas-
          tnat序列号被更改，seq<opt->seq_resv条件不满足，故fastnat开启时去除该
          判断条件;但在pptp(加密)传输时，全部数据包走协议栈，数据报文需满足
          seq<opt->seq_recv，才能保证数据包的序列号正确地进行传输 */
        if ((CTFE_OK == ctf_enable()) && !opt->pptp_over_mppe) {
            flags = WRAPPED(opt->seq_recv, seq);
        } else {
            flags = seq < opt->seq_recv + 1 || WRAPPED(opt->seq_recv, seq);
        }
        if (flags) {
#else
        /* check for expected sequence number */
        if (seq < opt->seq_recv + 1 || WRAPPED(opt->seq_recv, seq)) {
#endif
            if ((payload[0] == PPP_ALLSTATIONS) && (payload[1] == PPP_UI) &&
                (PPP_PROTOCOL(payload) == PPP_LCP) &&
                ((payload[4] == PPP_LCP_ECHOREQ) || (payload[4] == PPP_LCP_ECHOREP)))
                goto allow_packet;
    } else {
#endif
        opt->seq_recv = seq;
allow_packet:
        skb_pull(skb, headersize);

        if (payload[0] == PPP_ALLSTATIONS && payload[1] == PPP_UI) {
            /* chop off address/control */
            if (skb->len < 3)
                goto drop;
            skb_pull(skb, 2);
        }

        if ((*skb->data) & 1) {
            /* protocol is compressed */
            *(u8 *)skb_push(skb, 1) = 0;
        }

        skb->ip_summed = CHECKSUM_NONE;
        skb_set_network_header(skb, skb->head-skb->data);
        ppp_input(&po->chan, skb);

        return NET_RX_SUCCESS;
    }
drop:
    kfree_skb(skb);
    return NET_RX_DROP;
}

static int pptp_rcv(struct sk_buff *skb)
{
    struct pppox_sock *po;
    struct pptp_gre_header *header;
    struct iphdr *iph;

    if (skb->pkt_type != PACKET_HOST)
        goto drop;

    if (!pskb_may_pull(skb, 12))
        goto drop;

    iph = ip_hdr(skb);

    header = (struct pptp_gre_header *)skb->data;

    if (header->gre_hd.protocol != GRE_PROTO_PPP || /* PPTP-GRE protocol for PPTP */
        GRE_IS_CSUM(header->gre_hd.flags) ||    /* flag CSUM should be clear */
        GRE_IS_ROUTING(header->gre_hd.flags) || /* flag ROUTING should be clear */
        !GRE_IS_KEY(header->gre_hd.flags) ||    /* flag KEY should be set */
        (header->gre_hd.flags & GRE_FLAGS))     /* flag Recursion Ctrl should be clear */
        /* if invalid, discard this packet */
        goto drop;

    po = lookup_chan(htons(header->call_id), iph->saddr);
    if (po) {
        skb_dst_drop(skb);
        nf_reset(skb);
        return sk_receive_skb(sk_pppox(po), skb, 0);
    }
drop:
    kfree_skb(skb);
    return NET_RX_DROP;
}

static int pptp_bind(struct socket *sock, struct sockaddr *uservaddr,
    int sockaddr_len)
{
    struct sock *sk = sock->sk;
    struct sockaddr_pppox *sp = (struct sockaddr_pppox *) uservaddr;
    struct pppox_sock *po = pppox_sk(sk);
    int error = 0;

    if (sockaddr_len < sizeof(struct sockaddr_pppox))
        return -EINVAL;

    lock_sock(sk);

    if (sk->sk_state & PPPOX_DEAD) {
        error = -EALREADY;
        goto out;
    }

    if (sk->sk_state & PPPOX_BOUND) {
        error = -EBUSY;
        goto out;
    }

    if (add_chan(po, &sp->sa_addr.pptp))
        error = -EBUSY;
    else
        sk->sk_state |= PPPOX_BOUND;

out:
    release_sock(sk);
    return error;
}

static int pptp_connect(struct socket *sock, struct sockaddr *uservaddr,
    int sockaddr_len, int flags)
{
    struct sock *sk = sock->sk;
    struct sockaddr_pppox *sp = (struct sockaddr_pppox *) uservaddr;
    struct pppox_sock *po = pppox_sk(sk);
    struct pptp_opt *opt = &po->proto.pptp;
    struct rtable *rt;
    struct flowi4 fl4;
    int error = 0;

    if (sockaddr_len < sizeof(struct sockaddr_pppox))
        return -EINVAL;

    if (sp->sa_protocol != PX_PROTO_PPTP)
        return -EINVAL;

    if (lookup_chan_dst(sp->sa_addr.pptp.call_id, sp->sa_addr.pptp.sin_addr.s_addr))
        return -EALREADY;

    lock_sock(sk);
    /* Check for already bound sockets */
    if (sk->sk_state & PPPOX_CONNECTED) {
        error = -EBUSY;
        goto end;
    }

    /* Check for already disconnected sockets, on attempts to disconnect */
    if (sk->sk_state & PPPOX_DEAD) {
        error = -EALREADY;
        goto end;
    }

    if (!opt->src_addr.sin_addr.s_addr || !sp->sa_addr.pptp.sin_addr.s_addr) {
        error = -EINVAL;
        goto end;
    }

    po->chan.private = sk;
    po->chan.ops = &pptp_chan_ops;

    rt = ip_route_output_ports(sock_net(sk), &fl4, sk,
                   opt->dst_addr.sin_addr.s_addr,
                   opt->src_addr.sin_addr.s_addr,
                   0, 0,
                   IPPROTO_GRE, RT_CONN_FLAGS(sk), 0);
    if (IS_ERR(rt)) {
        error = -EHOSTUNREACH;
        goto end;
    }
    sk_setup_caps(sk, &rt->dst);

    po->chan.mtu = dst_mtu(&rt->dst);
    if (!po->chan.mtu)
        po->chan.mtu = PPP_MRU;
    po->chan.mtu -= PPTP_HEADER_OVERHEAD;

    po->chan.hdrlen = 2 + sizeof(struct pptp_gre_header);
    error = ppp_register_channel(&po->chan);
    if (error) {
        pr_err("PPTP: failed to register PPP channel (%d)\n", error);
        goto end;
    }

    opt->dst_addr = sp->sa_addr.pptp;
    sk->sk_state |= PPPOX_CONNECTED;

 end:
    release_sock(sk);
    return error;
}

static int pptp_getname(struct socket *sock, struct sockaddr *uaddr,
    int peer)
{
    int len = sizeof(struct sockaddr_pppox);
    struct sockaddr_pppox sp;

    memset(&sp.sa_addr, 0, sizeof(sp.sa_addr));

    sp.sa_family    = AF_PPPOX;
    sp.sa_protocol  = PX_PROTO_PPTP;
    sp.sa_addr.pptp = pppox_sk(sock->sk)->proto.pptp.src_addr;

    memcpy(uaddr, &sp, len);

    return len;
}

static int pptp_release(struct socket *sock)
{
    struct sock *sk = sock->sk;
    struct pppox_sock *po;
    int error = 0;

    if (!sk)
        return 0;

    lock_sock(sk);

    if (sock_flag(sk, SOCK_DEAD)) {
        release_sock(sk);
        return -EBADF;
    }

    po = pppox_sk(sk);
    del_chan(po);
    synchronize_rcu();

    pppox_unbind_sock(sk);
    sk->sk_state = PPPOX_DEAD;

    sock_orphan(sk);
    sock->sk = NULL;

    release_sock(sk);
    sock_put(sk);

    return error;
}

static void pptp_sock_destruct(struct sock *sk)
{
    if (!(sk->sk_state & PPPOX_DEAD)) {
        del_chan(pppox_sk(sk));
        pppox_unbind_sock(sk);
    }
    skb_queue_purge(&sk->sk_receive_queue);
    dst_release(rcu_dereference_protected(sk->sk_dst_cache, 1));
}

static int pptp_create(struct net *net, struct socket *sock, int kern)
{
    int error = -ENOMEM;
    struct sock *sk;
    struct pppox_sock *po;
    struct pptp_opt *opt;

    sk = sk_alloc(net, PF_PPPOX, GFP_KERNEL, &pptp_sk_proto, kern);
    if (!sk)
        goto out;

    sock_init_data(sock, sk);

    sock->state = SS_UNCONNECTED;
    sock->ops   = &pptp_ops;

    sk->sk_backlog_rcv = pptp_rcv_core;
    sk->sk_state       = PPPOX_NONE;
    sk->sk_type        = SOCK_STREAM;
    sk->sk_family      = PF_PPPOX;
    sk->sk_protocol    = PX_PROTO_PPTP;
    sk->sk_destruct    = pptp_sock_destruct;

    po = pppox_sk(sk);
    opt = &po->proto.pptp;

    opt->seq_sent = 0; opt->seq_recv = 0xffffffff;
    opt->ack_recv = 0; opt->ack_sent = 0xffffffff;

    error = 0;
out:
    return error;
}

static int pptp_ppp_ioctl(struct ppp_channel *chan, unsigned int cmd,
    unsigned long arg)
{
    struct sock *sk = (struct sock *) chan->private;
    struct pppox_sock *po = pppox_sk(sk);
    struct pptp_opt *opt = &po->proto.pptp;
    void __user *argp = (void __user *)arg;
    int __user *p = argp;
    int err, val;

    err = -EFAULT;
    switch (cmd) {
    case PPPIOCGFLAGS:
        val = opt->ppp_flags;
        if (put_user(val, p))
            break;
        err = 0;
        break;
    case PPPIOCSFLAGS:
        if (get_user(val, p))
            break;
        opt->ppp_flags = val & ~SC_RCV_BITS;
#if defined CONFIG_TENDA_PRIVATE_KM
        /* 去掉协议域压缩和地址域压缩，解决博通方案中pptp服务器开启fc跑流不通的问题 */
        opt->ppp_flags &= (~(SC_COMP_AC | SC_COMP_PROT));
#endif
        err = 0;    
        break;
    default:
        err = -ENOTTY;
    }

    return err;
}

static const struct ppp_channel_ops pptp_chan_ops = {
    .start_xmit = pptp_xmit,
    .ioctl      = pptp_ppp_ioctl,
};

static struct proto pptp_sk_proto __read_mostly = {
    .name     = "PPTP",
    .owner    = THIS_MODULE,
    .obj_size = sizeof(struct pppox_sock),
};

static const struct proto_ops pptp_ops = {
    .family     = AF_PPPOX,
    .owner      = THIS_MODULE,
    .release    = pptp_release,
    .bind       = pptp_bind,
    .connect    = pptp_connect,
    .socketpair = sock_no_socketpair,
    .accept     = sock_no_accept,
    .getname    = pptp_getname,
    .listen     = sock_no_listen,
    .shutdown   = sock_no_shutdown,
    .setsockopt = sock_no_setsockopt,
    .getsockopt = sock_no_getsockopt,
    .sendmsg    = sock_no_sendmsg,
    .recvmsg    = sock_no_recvmsg,
    .mmap       = sock_no_mmap,
    .ioctl      = pppox_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = pppox_compat_ioctl,
#endif
};

static const struct pppox_proto pppox_pptp_proto = {
    .create = pptp_create,
    .owner  = THIS_MODULE,
};

static const struct gre_protocol gre_pptp_protocol = {
    .handler = pptp_rcv,
};

static int __init pptp_init_module(void)
{
    int err = 0;
    pr_info("PPTP driver version " PPTP_DRIVER_VERSION "\n");

    callid_sock = vzalloc(array_size(sizeof(void *), (MAX_CALLID + 1)));
    if (!callid_sock)
        return -ENOMEM;

    err = gre_add_protocol(&gre_pptp_protocol, GREPROTO_PPTP);
    if (err) {
        pr_err("PPTP: can't add gre protocol\n");
        goto out_mem_free;
    }

    err = proto_register(&pptp_sk_proto, 0);
    if (err) {
        pr_err("PPTP: can't register sk_proto\n");
        goto out_gre_del_protocol;
    }

    err = register_pppox_proto(PX_PROTO_PPTP, &pppox_pptp_proto);
    if (err) {
        pr_err("PPTP: can't register pppox_proto\n");
        goto out_unregister_sk_proto;
    }

    return 0;

out_unregister_sk_proto:
    proto_unregister(&pptp_sk_proto);
out_gre_del_protocol:
    gre_del_protocol(&gre_pptp_protocol, GREPROTO_PPTP);
out_mem_free:
    vfree(callid_sock);

    return err;
}

static void __exit pptp_exit_module(void)
{
    unregister_pppox_proto(PX_PROTO_PPTP);
    proto_unregister(&pptp_sk_proto);
    gre_del_protocol(&gre_pptp_protocol, GREPROTO_PPTP);
    vfree(callid_sock);
}

module_init(pptp_init_module);
module_exit(pptp_exit_module);

MODULE_DESCRIPTION("Point-to-Point Tunneling Protocol");
MODULE_AUTHOR("D. Kozlov (xeb@mail.ru)");
MODULE_LICENSE("GPL");
MODULE_ALIAS_NET_PF_PROTO(PF_PPPOX, PX_PROTO_PPTP);
