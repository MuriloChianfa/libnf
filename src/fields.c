/* 

 Copyright (c) 2013-2015, Tomas Podermanski
    
 This file is part of libnf.net project.

 Libnf is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Libnf is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with libnf.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "config.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <netinet/in.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include "nffile.h"
#include "nfnet.h"
#include "bookkeeper.h"
//#include "nfxstat.h"
//#include "nf_common.h"
#include "rbtree.h"
#include "nftree.h"
#include "nfprof.h"
#include "nfdump.h"
#include "nfx.h"
#include "nflowcache.h"
#include "nfstat.h"
//#include "nfexport.h"
#include "ipconv.h"
#include "flist.h"
#include "util.h"

#include "libnf_internal.h"
#include "libnf.h"
#include "fields.h"

/* short names for master record and extensions_arr */
#define MR rec->master_record
#define EA rec->extensions_arr


/* function definicion for jump table */
/* TAG for check_items_map.pl: lnf_rec_fset */
/* TAG for check_items_map.pl: lnf_rec_fget */
/* ----------------------- */
static int inline lnf_field_fget_FIRST(lnf_rec_t *rec, void *p) { 
	*((uint64_t *)p) = MR->msecFirst;
	return __bit_array_get(EA, EXgenericFlowID) ? LNF_OK : LNF_ERR_NOTSET;
}
static int inline lnf_field_fset_FIRST(lnf_rec_t *rec, void *p) { 
	MR->msecFirst = *((uint64_t *)p);
	__bit_array_set(EA, EXgenericFlowID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_LAST(lnf_rec_t *rec, void *p) { 
	*((uint64_t *)p) = MR->msecLast;
	return __bit_array_get(EA, EXgenericFlowID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_LAST(lnf_rec_t *rec, void *p) { 
	MR->msecLast = *((uint64_t *)p);
	__bit_array_set(EA, EXgenericFlowID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_RECEIVED(lnf_rec_t *rec, void *p) { 
	*((uint64_t *)p) = MR->msecReceived;
	return __bit_array_get(EA, EXgenericFlowID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_RECEIVED(lnf_rec_t *rec, void *p) { 
	MR->msecReceived = *((uint64_t *)p);
	__bit_array_set(EA, EXgenericFlowID, 1);
	return LNF_OK;	
}

/* ----------------------- */
static int inline lnf_field_fget_DPKTS(lnf_rec_t *rec, void *p) { 
	*((uint64_t *)p) = MR->inPackets;
	return __bit_array_get(EA, EXgenericFlowID) ? LNF_OK : LNF_ERR_NOTSET;
	return LNF_OK;
}
static int inline lnf_field_fset_DPKTS(lnf_rec_t *rec, void *p) { 
	MR->inPackets = *((uint64_t *)p);
	__bit_array_set(EA, EXgenericFlowID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_DOCTETS(lnf_rec_t *rec, void *p) { 
	*((uint64_t *)p) = MR->inBytes;
	return __bit_array_get(EA, EXgenericFlowID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_DOCTETS(lnf_rec_t *rec, void *p) { 
	MR->inBytes = *((uint64_t *)p);
	__bit_array_set(EA, EXgenericFlowID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_OUT_PKTS(lnf_rec_t *rec, void *p) { 
	*((uint64_t *)p) = MR->out_pkts;
	return __bit_array_get(EA, EXcntFlowID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_OUT_PKTS(lnf_rec_t *rec, void *p) { 
	MR->out_pkts = *((uint64_t *)p);
	__bit_array_set(EA, EXcntFlowID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_OUT_BYTES(lnf_rec_t *rec, void *p) { 
	*((uint64_t *)p) = MR->out_bytes;
	return __bit_array_get(EA, EXcntFlowID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_OUT_BYTES(lnf_rec_t *rec, void *p) { 
	MR->out_bytes = *((uint64_t *)p);
	__bit_array_set(EA, EXcntFlowID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_AGGR_FLOWS(lnf_rec_t *rec, void *p) { 
	*((uint64_t *)p) = MR->aggr_flows;
	return __bit_array_get(EA, EXcntFlowID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_AGGR_FLOWS(lnf_rec_t *rec, void *p) { 
	MR->aggr_flows = *((uint64_t *)p);
	__bit_array_set(EA, EXcntFlowID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_SRCPORT(lnf_rec_t *rec, void *p) { 
	*((uint16_t *)p) = MR->srcPort;
	return __bit_array_get(EA, EXgenericFlowID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_SRCPORT(lnf_rec_t *rec, void *p) { 
	MR->srcPort = *((uint16_t *)p);
	__bit_array_set(EA, EXgenericFlowID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_DSTPORT(lnf_rec_t *rec, void *p) { 
	*((uint16_t *)p) = MR->dstPort;
	return __bit_array_get(EA, EXgenericFlowID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_DSTPORT(lnf_rec_t *rec, void *p) { 
	MR->dstPort = *((uint16_t *)p);
	__bit_array_set(EA, EXgenericFlowID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_TCP_FLAGS(lnf_rec_t *rec, void *p) { 
	*((uint8_t *)p) = MR->tcp_flags;
	return __bit_array_get(EA, EXgenericFlowID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_TCP_FLAGS(lnf_rec_t *rec, void *p) { 
	MR->tcp_flags = *((uint8_t *)p);
	__bit_array_set(EA, EXgenericFlowID, 1);
	return LNF_OK;
}

/* ----------------------- */
// Required extension 1 - IP addresses 
// NOTE: srcaddr and dst addr do not uses ip_addr_t union/structure 
// however the structures are compatible so we will pretend 
// that v6.srcaddr and v6.dst addr points to same structure 
static int inline lnf_field_fget_SRCADDR(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = (ip_addr_t *)&MR->V6.srcaddr;
	
	((ip_addr_t *)p)->V6[0] = htonll(d->V6[0]);
	((ip_addr_t *)p)->V6[1] = htonll(d->V6[1]);

	return __bit_array_get(EA, EXipv4FlowID) || __bit_array_get(EA, EXipv6FlowID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_SRCADDR(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = (ip_addr_t *)&MR->V6.srcaddr;

	d->V6[0] = ntohll( ((ip_addr_t *)p)->V6[0] );
	d->V6[1] = ntohll( ((ip_addr_t *)p)->V6[1] );

	if (IN6_IS_ADDR_V4COMPAT((struct in6_addr *)p)) {
		ClearFlag(MR->mflags, V3_FLAG_IPV6_ADDR);
		__bit_array_set(EA, EXipv4FlowID, 1);
	} else {
		SetFlag(MR->mflags, V3_FLAG_IPV6_ADDR);
		__bit_array_set(EA, EXipv6FlowID, 1);
	}
	
	return LNF_OK;
}


/* ----------------------- */
static int inline lnf_field_fget_DSTADDR(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = (ip_addr_t *)&MR->V6.dstaddr;
	
	((ip_addr_t *)p)->V6[0] = htonll(d->V6[0]);
	((ip_addr_t *)p)->V6[1] = htonll(d->V6[1]);

	return __bit_array_get(EA, EXipv4FlowID) || __bit_array_get(EA, EXipv6FlowID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_DSTADDR(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = (ip_addr_t *)&MR->V6.dstaddr;

	d->V6[0] = ntohll( ((ip_addr_t *)p)->V6[0] );
	d->V6[1] = ntohll( ((ip_addr_t *)p)->V6[1] );

	if (IN6_IS_ADDR_V4COMPAT((struct in6_addr *)p)) {
		ClearFlag(MR->mflags, V3_FLAG_IPV6_ADDR);
		__bit_array_set(EA, EXipv4FlowID, 1);
	} else {
		SetFlag(MR->mflags, V3_FLAG_IPV6_ADDR);
		__bit_array_set(EA, EXipv6FlowID, 1);
	}
	return LNF_OK;
}


/* ----------------------- */
static int inline lnf_field_fget_IP_NEXTHOP(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = (ip_addr_t *)&MR->ip_nexthop;
	
	((ip_addr_t *)p)->V6[0] = htonll(d->V6[0]);
	((ip_addr_t *)p)->V6[1] = htonll(d->V6[1]);

	return __bit_array_get(EA, EXipNextHopV4ID) || __bit_array_get(EA, EXipNextHopV6ID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_IP_NEXTHOP(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = &MR->ip_nexthop;

	d->V6[0] = ntohll( ((ip_addr_t *)p)->V6[0] );
	d->V6[1] = ntohll( ((ip_addr_t *)p)->V6[1] );

	if (IN6_IS_ADDR_V4COMPAT((struct in6_addr *)p)) {
		ClearFlag(MR->mflags, V3_FLAG_IPV6_NH);
		__bit_array_set(EA, EXipNextHopV4ID, 1);
	} else {
		SetFlag(MR->mflags, V3_FLAG_IPV6_NH);
		__bit_array_set(EA, EXipNextHopV6ID, 1);
	}
	return LNF_OK;
}


/* ----------------------- */
static int inline lnf_field_fget_SRC_MASK(lnf_rec_t *rec, void *p) { 
	*((uint8_t *)p) = MR->src_mask;
	return __bit_array_get(EA, EXflowMiscID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_SRC_MASK(lnf_rec_t *rec, void *p) { 
	MR->src_mask = *((uint8_t *)p);
	__bit_array_set(EA, EXflowMiscID, 1);
	return LNF_OK;
}


/* ----------------------- */
static int inline lnf_field_fget_DST_MASK(lnf_rec_t *rec, void *p) { 
	 *((uint8_t *)p) = MR->dst_mask;
	return __bit_array_get(EA, EXflowMiscID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_DST_MASK(lnf_rec_t *rec, void *p) { 
	MR->dst_mask = *((uint8_t *)p);
	__bit_array_set(EA, EXflowMiscID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_TOS(lnf_rec_t *rec, void *p) { 
	*((uint8_t *)p) = MR->tos;
	return __bit_array_get(EA, EXgenericFlowID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_TOS(lnf_rec_t *rec, void *p) { 
	MR->tos = *((uint8_t *)p);
	__bit_array_set(EA, EXgenericFlowID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_DST_TOS(lnf_rec_t *rec, void *p) { 
	*((uint8_t *)p) = MR->dst_tos;
	return __bit_array_get(EA, EXflowMiscID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_DST_TOS(lnf_rec_t *rec, void *p) { 
	MR->dst_tos = *((uint8_t *)p);
	__bit_array_set(EA, EXflowMiscID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_SRCAS(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) = MR->srcas;
	return __bit_array_get(EA, EXasRoutingID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_SRCAS(lnf_rec_t *rec, void *p) { 
	MR->srcas = *((uint32_t *)p);
	__bit_array_set(EA, EXasRoutingID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_DSTAS(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) = MR->dstas;
	return __bit_array_get(EA, EXasRoutingID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_DSTAS(lnf_rec_t *rec, void *p) { 
	MR->dstas = *((uint32_t *)p);
	__bit_array_set(EA, EXasRoutingID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_BGPNEXTADJACENTAS(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) = MR->bgpNextAdjacentAS;
	return __bit_array_get(EA, EXasAdjacentID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_BGPNEXTADJACENTAS(lnf_rec_t *rec, void *p) { 
	MR->bgpNextAdjacentAS = *((uint32_t *)p);
	__bit_array_set(EA, EXasAdjacentID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_BGPPREVADJACENTAS(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) = MR->bgpPrevAdjacentAS;
	return __bit_array_get(EA, EXasAdjacentID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_BGPPREVADJACENTAS(lnf_rec_t *rec, void *p) { 
	MR->bgpPrevAdjacentAS = *((uint32_t *)p);
	__bit_array_set(EA, EXasAdjacentID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_BGP_NEXTHOP(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = (ip_addr_t *)&MR->bgp_nexthop;
	
	((ip_addr_t *)p)->V6[0] = htonll(d->V6[0]);
	((ip_addr_t *)p)->V6[1] = htonll(d->V6[1]);

	return __bit_array_get(EA, EXbgpNextHopV4ID) || __bit_array_get(EA, EXbgpNextHopV6ID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_BGP_NEXTHOP(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = &MR->bgp_nexthop;

	d->V6[0] = ntohll( ((ip_addr_t *)p)->V6[0] );
	d->V6[1] = ntohll( ((ip_addr_t *)p)->V6[1] );

	if (IN6_IS_ADDR_V4COMPAT((struct in6_addr *)p)) {
		ClearFlag(MR->mflags, V3_FLAG_IPV6_NHB);
		__bit_array_set(EA, EXbgpNextHopV4ID, 1);
	} else {
		SetFlag(MR->mflags, V3_FLAG_IPV6_NHB);
		__bit_array_set(EA, EXbgpNextHopV6ID, 1);
	}
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_PROT(lnf_rec_t *rec, void *p) { 
	*((uint8_t *)p) = MR->proto;
	return __bit_array_get(EA, EXgenericFlowID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_PROT(lnf_rec_t *rec, void *p) { 
	MR->proto = *((uint8_t *)p);
	__bit_array_set(EA, EXgenericFlowID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_SRC_VLAN(lnf_rec_t *rec, void *p) { 
	*((uint16_t *)p) = MR->src_vlan;
	return __bit_array_get(EA, EXvLanID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_SRC_VLAN(lnf_rec_t *rec, void *p) { 
	MR->src_vlan = *((uint16_t *)p);
	__bit_array_set(EA, EXvLanID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_DST_VLAN(lnf_rec_t *rec, void *p) { 
	*((uint16_t *)p) = MR->dst_vlan;
	return __bit_array_get(EA, EXvLanID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_DST_VLAN(lnf_rec_t *rec, void *p) { 
	MR->dst_vlan = *((uint16_t *)p);
	__bit_array_set(EA, EXvLanID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_IN_SRC_MAC(lnf_rec_t *rec, void *p) { 
	int i;
	for (i = 0; i < 6; i++) {
		((uint8_t *)p)[5 - i] = ((uint8_t *)(&MR->in_src_mac))[i];
    } 
	return __bit_array_get(EA, EXmacAddrID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_IN_SRC_MAC(lnf_rec_t *rec, void *p) { 
	int i;
	MR->in_src_mac = 0x0;
	for (i = 0; i < 6; i++) {
		((uint8_t *)(&MR->in_src_mac))[5 - i] = ((uint8_t *)p)[i];
	}
	__bit_array_set(EA, EXmacAddrID, 1);
	return LNF_OK;
}


/* ----------------------- */
static int inline lnf_field_fget_OUT_DST_MAC(lnf_rec_t *rec, void *p) { 
	int i;
	for (i = 0; i < 6; i++) {
		((uint8_t *)p)[5 - i] = ((uint8_t *)(&MR->out_dst_mac))[i];
    } 
	return __bit_array_get(EA, EXmacAddrID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_OUT_DST_MAC(lnf_rec_t *rec, void *p) { 
	int i;
	MR->out_dst_mac = 0x0;
	for (i = 0; i < 6; i++) {
		((uint8_t *)(&MR->out_dst_mac))[5 - i] = ((uint8_t *)p)[i];
	}
	__bit_array_set(EA, EXmacAddrID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_OUT_SRC_MAC(lnf_rec_t *rec, void *p) { 
	int i;
	for (i = 0; i < 6; i++) {
		((uint8_t *)p)[5 - i] = ((uint8_t *)(&MR->out_src_mac))[i];
    } 
	return __bit_array_get(EA, EXmacAddrID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_OUT_SRC_MAC(lnf_rec_t *rec, void *p) { 
	int i;
	MR->out_src_mac = 0x0;
	for (i = 0; i < 6; i++) {
		((uint8_t *)(&MR->out_src_mac))[5 - i] = ((uint8_t *)p)[i];
	}
	__bit_array_set(EA, EXmacAddrID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_IN_DST_MAC(lnf_rec_t *rec, void *p) { 
	int i;
	for (i = 0; i < 6; i++) {
		((uint8_t *)p)[5 - i] = ((uint8_t *)(&MR->in_dst_mac))[i];
    } 
	return __bit_array_get(EA, EXmacAddrID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_IN_DST_MAC(lnf_rec_t *rec, void *p) { 
	int i;
	MR->in_dst_mac = 0x0;
	for (i = 0; i < 6; i++) {
		((uint8_t *)(&MR->in_dst_mac))[5 - i] = ((uint8_t *)p)[i];
	}
	__bit_array_set(EA, EXmacAddrID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_MPLS_LABEL(lnf_rec_t *rec, void *p) { 
	memcpy(p, MR->mpls_label, sizeof(lnf_mpls_t));
	return __bit_array_get(EA, EXmplsLabelID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_MPLS_LABEL(lnf_rec_t *rec, void *p) { 
	memcpy(MR->mpls_label, p, sizeof(lnf_mpls_t));
	__bit_array_set(EA, EXmplsLabelID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_INPUT(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) = MR->input;
	return __bit_array_get(EA, EXflowMiscID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_INPUT(lnf_rec_t *rec, void *p) { 
	MR->input = *((uint32_t *)p);
	__bit_array_set(EA, EXflowMiscID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_OUTPUT(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) =  MR->output;
	return __bit_array_get(EA, EXflowMiscID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_OUTPUT(lnf_rec_t *rec, void *p) { 
	MR->output = *((uint32_t *)p);
	__bit_array_set(EA, EXflowMiscID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_DIR(lnf_rec_t *rec, void *p) { 
	*((uint8_t *)p) =  MR->dir;
	return __bit_array_get(EA, EXflowMiscID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_DIR(lnf_rec_t *rec, void *p) { 
	MR->dir = *((uint8_t *)p);
	__bit_array_set(EA, EXflowMiscID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_FWD_STATUS(lnf_rec_t *rec, void *p) { 
	*((uint8_t *)p) = MR->fwd_status;
	return __bit_array_get(EA, EXgenericFlowID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_FWD_STATUS(lnf_rec_t *rec, void *p) { 
	MR->fwd_status = *((uint8_t *)p);
	__bit_array_set(EA, EXgenericFlowID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_IP_ROUTER(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = (ip_addr_t *)&MR->ip_router;
	
	((ip_addr_t *)p)->V6[0] = htonll(d->V6[0]);
	((ip_addr_t *)p)->V6[1] = htonll(d->V6[1]);

	return __bit_array_get(EA, EXipReceivedV4ID) || __bit_array_get(EA, EXipReceivedV6ID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_IP_ROUTER(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = &MR->ip_router;

	d->V6[0] = ntohll( ((ip_addr_t *)p)->V6[0] );
	d->V6[1] = ntohll( ((ip_addr_t *)p)->V6[1] );

	if (IN6_IS_ADDR_V4COMPAT((struct in6_addr *)p)) {
		ClearFlag(MR->mflags, V3_FLAG_IPV6_EXP);
		__bit_array_set(EA,  EXipReceivedV4ID, 1);
	} else {
		SetFlag(MR->mflags, V3_FLAG_IPV6_EXP);
		__bit_array_set(EA,  EXipReceivedV6ID, 1);
	}
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_ENGINE_TYPE(lnf_rec_t *rec, void *p) { 
	*((uint8_t *)p) = MR->engine_type;
	return LNF_OK;
}

static int inline lnf_field_fset_ENGINE_TYPE(lnf_rec_t *rec, void *p) { 
	MR->engine_type = *((uint8_t *)p);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_ENGINE_ID(lnf_rec_t *rec, void *p) { 
	*((uint8_t *)p) = MR->engine_id;
	return LNF_OK;
}

static int inline lnf_field_fset_ENGINE_ID(lnf_rec_t *rec, void *p) { 
	MR->engine_id = *((uint8_t *)p);
	return LNF_OK;
}

/* ----------------------- */
#ifdef NSEL
static int inline lnf_field_fget_EVENT_TIME(lnf_rec_t *rec, void *p) { 
		*((uint64_t *)p) = MR->msecEvent;
		return __bit_array_get(EA, EXnselCommonID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_EVENT_TIME(lnf_rec_t *rec, void *p) { 
	MR->msecEvent = *((uint64_t *)p);
	__bit_array_set(EA, EXnselCommonID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_CONN_ID(lnf_rec_t *rec, void *p) { 
		*((uint32_t *)p) = MR->connID;
		return __bit_array_get(EA, EXnselCommonID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_CONN_ID(lnf_rec_t *rec, void *p) { 
	MR->connID = *((uint32_t *)p);
	__bit_array_set(EA, EXnselCommonID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_ICMP_CODE(lnf_rec_t *rec, void *p) { 
	*((uint8_t *)p) = MR->icmp_code;
	return __bit_array_get(EA, EXnselCommonID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_ICMP_CODE(lnf_rec_t *rec, void *p) { 
	MR->icmp_code = *((uint8_t *)p);
	__bit_array_set(EA, EXnselCommonID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_ICMP_TYPE(lnf_rec_t *rec, void *p) { 
	*((uint8_t *)p) = MR->icmp_type;
	return __bit_array_get(EA, EXnselCommonID ) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_ICMP_TYPE(lnf_rec_t *rec, void *p) { 
	MR->icmp_type = *((uint8_t *)p);
	__bit_array_set(EA, EXnselCommonID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_FW_XEVENT(lnf_rec_t *rec, void *p) { 
	*((uint16_t *)p) = MR->fwXevent;
	return __bit_array_get(EA, EXnselCommonID) ? LNF_OK : LNF_ERR_NOTSET;
	/* dummy record for check_items_map.pl MR->xlate_flags */
}

static int inline lnf_field_fset_FW_XEVENT(lnf_rec_t *rec, void *p) { 
	MR->fwXevent = *((uint16_t *)p);
	__bit_array_set(EA, EXnselCommonID, 1);
	return LNF_OK;
	/* dummy record for check_items_map.pl MR->xlate_flags */
}

/* ----------------------- */
static int inline lnf_field_fget_FW_EVENT(lnf_rec_t *rec, void *p) { 
	*((uint8_t *)p) = MR->event;
	return __bit_array_get(EA, EXnselCommonID) ? LNF_OK : LNF_ERR_NOTSET;
	/* dummy record for check_items_map.pl MR->xlate_flags */
}

static int inline lnf_field_fset_FW_EVENT(lnf_rec_t *rec, void *p) { 
	MR->event = *((uint8_t *)p);
	__bit_array_set(EA, EXnselCommonID, 1);
	return LNF_OK;
	/* dummy record for check_items_map.pl MR->xlate_flags */
}

/* ----------------------- */
static int inline lnf_field_fget_XLATE_SRC_IP(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = (ip_addr_t *)&MR->xlate_src_ip;
	
	((ip_addr_t *)p)->V6[0] = htonll(d->V6[0]);
	((ip_addr_t *)p)->V6[1] = htonll(d->V6[1]);

	return __bit_array_get(EA, EXnselXlateIPv4ID) || __bit_array_get(EA, EXnselXlateIPv6ID) ? LNF_OK : LNF_ERR_NOTSET;
}
		
static int inline lnf_field_fset_XLATE_SRC_IP(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = &MR->xlate_src_ip;

	d->V6[0] = ntohll( ((ip_addr_t *)p)->V6[0] );
	d->V6[1] = ntohll( ((ip_addr_t *)p)->V6[1] );

	if (IN6_IS_ADDR_V4COMPAT((struct in6_addr *)p)) {
		__bit_array_set(EA,  EXnselXlateIPv4ID, 1);
		MR->xlate_flags = 0;
	} else {
		__bit_array_set(EA,  EXnselXlateIPv6ID, 1);
		MR->xlate_flags = 1;
	}
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_XLATE_DST_IP(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = (ip_addr_t *)&MR->xlate_dst_ip;
	
	((ip_addr_t *)p)->V6[0] = htonll(d->V6[0]);
	((ip_addr_t *)p)->V6[1] = htonll(d->V6[1]);

	return __bit_array_get(EA, EXnselXlateIPv4ID) || __bit_array_get(EA, EXnselXlateIPv6ID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_XLATE_DST_IP(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = &MR->xlate_dst_ip;

	d->V6[0] = ntohll( ((ip_addr_t *)p)->V6[0] );
	d->V6[1] = ntohll( ((ip_addr_t *)p)->V6[1] );

	if (IN6_IS_ADDR_V4COMPAT((struct in6_addr *)p)) {
		__bit_array_set(EA,  EXnselXlateIPv4ID, 1);
		MR->xlate_flags = 0;
	} else {
		__bit_array_set(EA,  EXnselXlateIPv6ID, 1);
		MR->xlate_flags = 1;
	}
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_XLATE_SRC_PORT(lnf_rec_t *rec, void *p) { 
	*((uint16_t *)p) = MR->xlate_src_port;
	return __bit_array_get(EA,  EXnselXlatePortID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_XLATE_SRC_PORT(lnf_rec_t *rec, void *p) { 
	MR->xlate_src_port = *((uint16_t *)p);
	__bit_array_set(EA,  EXnselXlatePortID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_XLATE_DST_PORT(lnf_rec_t *rec, void *p) { 
	*((uint16_t *)p) = MR->xlate_dst_port;
	return __bit_array_get(EA,  EXnselXlatePortID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_XLATE_DST_PORT(lnf_rec_t *rec, void *p) { 
	MR->xlate_dst_port = *((uint16_t *)p);
	__bit_array_set(EA,  EXnselXlatePortID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_INGRESS_ACL_ID(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) = MR->ingressAcl[0];
	return __bit_array_get(EA, EXnselAclID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_INGRESS_ACL_ID(lnf_rec_t *rec, void *p) { 
	MR->ingressAcl[0] = *((uint32_t *)p);
	__bit_array_set(EA, EXnselAclID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_INGRESS_ACE_ID(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) = MR->ingressAcl[1];
	return __bit_array_get(EA, EXnselAclID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_INGRESS_ACE_ID(lnf_rec_t *rec, void *p) { 
	MR->ingressAcl[1] = *((uint32_t *)p);
	__bit_array_set(EA, EXnselAclID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_INGRESS_XACE_ID(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) = MR->ingressAcl[2];
	return __bit_array_get(EA, EXnselAclID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_INGRESS_XACE_ID(lnf_rec_t *rec, void *p) { 
	MR->ingressAcl[2] = *((uint32_t *)p);
	__bit_array_set(EA, EXnselAclID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_INGRESS_ACL(lnf_rec_t *rec, void *p) { 
	((lnf_acl_t *)p)->acl_id = MR->ingressAcl[0];
	((lnf_acl_t *)p)->ace_id = MR->ingressAcl[1];
	((lnf_acl_t *)p)->xace_id = MR->ingressAcl[2];
	return __bit_array_get(EA, EXnselAclID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_INGRESS_ACL(lnf_rec_t *rec, void *p) { 
	MR->ingressAcl[0] = ((lnf_acl_t *)p)->acl_id;
	MR->ingressAcl[1] = ((lnf_acl_t *)p)->ace_id;
	MR->ingressAcl[2] = ((lnf_acl_t *)p)->xace_id;
	__bit_array_set(EA, EXnselAclID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_EGRESS_ACL_ID(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) = MR->egressAcl[0];
	return __bit_array_get(EA, EXnselAclID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_EGRESS_ACL_ID(lnf_rec_t *rec, void *p) { 
	MR->egressAcl[0] = *((uint32_t *)p);
	__bit_array_set(EA, EXnselAclID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_EGRESS_ACE_ID(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) = MR->egressAcl[1];
	return __bit_array_get(EA, EXnselAclID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_EGRESS_ACE_ID(lnf_rec_t *rec, void *p) { 
	MR->egressAcl[1] = *((uint32_t *)p);
	__bit_array_set(EA, EXnselAclID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_EGRESS_XACE_ID(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) = MR->egressAcl[2];
	return __bit_array_get(EA, EXnselAclID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_EGRESS_XACE_ID(lnf_rec_t *rec, void *p) { 
	MR->egressAcl[2] = *((uint32_t *)p);
	__bit_array_set(EA, EXnselAclID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_EGRESS_ACL(lnf_rec_t *rec, void *p) { 
	((lnf_acl_t *)p)->acl_id = MR->egressAcl[0];
	((lnf_acl_t *)p)->ace_id = MR->egressAcl[1];
	((lnf_acl_t *)p)->xace_id = MR->egressAcl[2];
	return __bit_array_get(EA, EXnselAclID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_EGRESS_ACL(lnf_rec_t *rec, void *p) { 
	MR->egressAcl[0] = ((lnf_acl_t *)p)->acl_id;
	MR->egressAcl[1] = ((lnf_acl_t *)p)->ace_id;
	MR->egressAcl[2] = ((lnf_acl_t *)p)->xace_id;
	__bit_array_set(EA, EXnselAclID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_USERNAME(lnf_rec_t *rec, void *p) { 
	memcpy(p, MR->username, strlen(MR->username) + 1);
	return __bit_array_get(EA, EXnselUserID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_USERNAME(lnf_rec_t *rec, void *p) { 
	int len;

	len = strlen((char *)p);
	if ( len > sizeof(MR->username) -  1 ) {
		len = sizeof(MR->username) - 1;
	}

	memcpy(MR->username, p, len );
	MR->username[len] = '\0';

	__bit_array_set(EA, EXnselUserID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_INGRESS_VRFID(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) = MR->ingressVrf;
	return __bit_array_get(EA, EXvrfID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_INGRESS_VRFID(lnf_rec_t *rec, void *p) { 
	MR->ingressVrf = *((uint32_t *)p);
	__bit_array_set(EA, EXvrfID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_EVENT_FLAG(lnf_rec_t *rec, void *p) { 
	*((uint8_t *)p) = MR->event_flag;
	return __bit_array_get(EA, EXnselCommonID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_EVENT_FLAG(lnf_rec_t *rec, void *p) { 
	MR->event_flag = *((uint8_t *)p);
	__bit_array_set(EA, EXnselCommonID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_EGRESS_VRFID(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) = MR->egressVrf;
	return __bit_array_get(EA, EXvrfID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_EGRESS_VRFID(lnf_rec_t *rec, void *p) { 
	MR->egressVrf = *((uint32_t *)p);
	__bit_array_set(EA, EXvrfID, 1);
	return LNF_OK;
}

/* ----------------------- */
// EX_PORT_BLOCK_ALLOC added 2014-04-19
static int inline lnf_field_fget_BLOCK_START(lnf_rec_t *rec, void *p) { 
	*((uint16_t *)p) = MR->block_start;
	return __bit_array_get(EA, EXnelXlatePortID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_BLOCK_START(lnf_rec_t *rec, void *p) { 
	MR->block_start = *((uint16_t *)p);
	__bit_array_set(EA, EXnelXlatePortID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_BLOCK_END(lnf_rec_t *rec, void *p) { 
	*((uint16_t *)p) = MR->block_end;
	return __bit_array_get(EA, EXnelXlatePortID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_BLOCK_END(lnf_rec_t *rec, void *p) { 
	MR->block_end = *((uint16_t *)p);
	__bit_array_set(EA, EXnelXlatePortID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_BLOCK_STEP(lnf_rec_t *rec, void *p) { 
	*((uint16_t *)p) = MR->block_step;
	return __bit_array_get(EA, EXnelXlatePortID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_BLOCK_STEP(lnf_rec_t *rec, void *p) { 
	MR->block_step = *((uint16_t *)p);
	__bit_array_set(EA, EXnelXlatePortID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_BLOCK_SIZE(lnf_rec_t *rec, void *p) { 
	*((uint16_t *)p) = MR->block_size;
	return __bit_array_get(EA, EXnelXlatePortID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_BLOCK_SIZE(lnf_rec_t *rec, void *p) { 
	MR->block_size = *((uint16_t *)p);
	__bit_array_set(EA, EXnelXlatePortID, 1);
	return LNF_OK;
}

#endif
/* ----------------------- */
		// extra fields
static int inline lnf_field_fget_CLIENT_NW_DELAY_USEC(lnf_rec_t *rec, void *p) { 
	*((uint64_t *)p) = MR->client_nw_delay_usec;
	return __bit_array_get(EA, EXlatencyID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_CLIENT_NW_DELAY_USEC(lnf_rec_t *rec, void *p) { 
	MR->client_nw_delay_usec = *((uint64_t *)p);
	__bit_array_set(EA, EXlatencyID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_SERVER_NW_DELAY_USEC(lnf_rec_t *rec, void *p) { 
	*((uint64_t *)p) = MR->server_nw_delay_usec;
	return __bit_array_get(EA, EXlatencyID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_SERVER_NW_DELAY_USEC(lnf_rec_t *rec, void *p) { 
	MR->server_nw_delay_usec = *((uint64_t *)p);
	__bit_array_set(EA, EXlatencyID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_APPL_LATENCY_USEC(lnf_rec_t *rec, void *p) { 
	*((uint64_t *)p) = MR->appl_latency_usec;
	return __bit_array_get(EA, EXlatencyID) ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_APPL_LATENCY_USEC(lnf_rec_t *rec, void *p) { 
	MR->appl_latency_usec = *((uint64_t *)p);
	__bit_array_set(EA, EXlatencyID, 1);
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_INET_FAMILY(lnf_rec_t *rec, void *p) { 
	if ( TestFlag(MR->flags, FLAG_IPV6_ADDR) ) {
		*((uint32_t *)p) = AF_INET6;
	} else { 
		*((uint32_t *)p) = AF_INET;
	}
	return LNF_OK;
}

static int inline lnf_field_fset_INET_FAMILY(lnf_rec_t *rec, void *p) { 
	if ( *((uint32_t *)p) == AF_INET ) {
		ClearFlag(MR->flags, FLAG_IPV6_ADDR);
		return LNF_OK;
	} else if ( *((uint32_t *)p) == AF_INET6 ) {
		SetFlag(MR->flags, FLAG_IPV6_ADDR);
		return LNF_OK;
	} else {
		return LNF_ERR_NOTSET;
	}
}

/* ----------------------- */
static int inline lnf_field_fget_EXPORTER_IP(lnf_rec_t *rec, void *p) { 
	if ( MR->exp_ref != NULL || rec->flags & LNF_REC_EXPORTER) {
		ip_addr_t *d = (ip_addr_t *)&rec->exporter->info.ip;
	
		((ip_addr_t *)p)->V6[0] = htonll(d->V6[0]);
		((ip_addr_t *)p)->V6[1] = htonll(d->V6[1]);

		return LNF_OK;
	} else {
		return LNF_ERR_NOTSET;
	}
}

static int inline lnf_field_fset_EXPORTER_IP(lnf_rec_t *rec, void *p) { 
	ip_addr_t *d = &rec->exporter->info.ip;

	d->V6[0] = ntohll( ((ip_addr_t *)p)->V6[0] );
	d->V6[1] = ntohll( ((ip_addr_t *)p)->V6[1] );

	rec->flags |= LNF_REC_EXPORTER;
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_EXPORTER_ID(lnf_rec_t *rec, void *p) { 
	if ( MR->exp_ref != NULL  || rec->flags & LNF_REC_EXPORTER ) {
		*((uint32_t *)p) = rec->exporter->info.id;
		return LNF_OK;
	} else {
		return LNF_ERR_NOTSET;
	}
}

static int inline lnf_field_fset_EXPORTER_ID(lnf_rec_t *rec, void *p) { 
	rec->exporter->info.id = *((uint32_t *)p);
	rec->flags |= LNF_REC_EXPORTER;
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_EXPORTER_VERSION(lnf_rec_t *rec, void *p) { 
	if ( MR->exp_ref != NULL  || rec->flags & LNF_REC_EXPORTER ) {
		*((uint32_t *)p) = rec->exporter->info.version;
		return LNF_OK;
	} else {
		return LNF_ERR_NOTSET;
	}
}

static int inline lnf_field_fset_EXPORTER_VERSION(lnf_rec_t *rec, void *p) { 
	rec->exporter->info.version = *((uint32_t *)p);
	rec->flags |= LNF_REC_EXPORTER;
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_SEQUENCE_FAILURES(lnf_rec_t *rec, void *p) { 
	*((uint32_t *)p) = rec->sequence_failures;
	return rec->flags & LNF_REC_EXPORTER ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_SEQUENCE_FAILURES(lnf_rec_t *rec, void *p) { 
	rec->sequence_failures = *((uint32_t *)p);
	rec->flags |= LNF_REC_EXPORTER;
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_SAMPLER_MODE(lnf_rec_t *rec, void *p) { 
//	*((uint16_t *)p) = rec->sampler->info.mode;
	return rec->flags & LNF_REC_SAMPLER ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_SAMPLER_MODE(lnf_rec_t *rec, void *p) { 
//	rec->sampler->info.mode = *((uint16_t *)p);
	rec->flags |= LNF_REC_SAMPLER;
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_SAMPLER_INTERVAL(lnf_rec_t *rec, void *p) { 
//	*((uint32_t *)p) = rec->sampler->info.interval;
	return rec->flags & LNF_REC_SAMPLER ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_SAMPLER_INTERVAL(lnf_rec_t *rec, void *p) { 
//	rec->sampler->info.interval = *((uint32_t *)p);
	rec->flags |= LNF_REC_SAMPLER;
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_SAMPLER_ID(lnf_rec_t *rec, void *p) { 
//	*((uint32_t *)p) = rec->sampler->info.id;
	return rec->flags & LNF_REC_SAMPLER ? LNF_OK : LNF_ERR_NOTSET;
}

static int inline lnf_field_fset_SAMPLER_ID(lnf_rec_t *rec, void *p) { 
//	rec->sampler->info.id = *((uint32_t *)p);
	rec->flags |= LNF_REC_SAMPLER;
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fset_EMPTY_(lnf_rec_t *rec, void *p) { 
	return LNF_OK;
}

#define LNF_DURATION ((MR->msecLast) - (MR->msecFirst))

/* ----------------------- */
static int inline lnf_field_fget_CALC_DURATION(lnf_rec_t *rec, void *p) { 
	*((uint64_t *)p) = LNF_DURATION;
	return LNF_OK;
}

/* ----------------------- */
static int inline lnf_field_fget_CALC_BPS(lnf_rec_t *rec, void *p) { 
	if (LNF_DURATION > 0) {
		*((double *)p) = (MR->inBytes * 8) / (LNF_DURATION / 1000.0);
		return LNF_OK;
	} else {
		*((double *)p) = 0;
		return LNF_ERR_NAN;
	}
}

/* ----------------------- */
static int inline lnf_field_fget_CALC_PPS(lnf_rec_t *rec, void *p) { 
	if (LNF_DURATION > 0) {
		*((double *)p) = MR->inPackets / (LNF_DURATION / 1000.0);
		return LNF_OK;
	} else {
		*((double *)p) = 0;
		return LNF_ERR_NAN;
	}
}

/* ----------------------- */
static int inline lnf_field_fget_CALC_BPP(lnf_rec_t *rec, void *p) { 
	if (MR->inPackets > 0) {
		*((double *)p) = MR->inBytes / MR->inPackets;
		return LNF_OK;
	} else {
		return LNF_ERR_NAN;
	}
}

/* ----------------------- */
static int inline lnf_field_fget_BREC1(lnf_rec_t *rec, void *p) { 
	lnf_brec1_t *brec1 = p;
	lnf_field_fget_FIRST(rec, &brec1->first);
	lnf_field_fget_LAST(rec, &brec1->last);
	lnf_field_fget_SRCADDR(rec, &brec1->srcaddr);
	lnf_field_fget_DSTADDR(rec, &brec1->dstaddr);
	lnf_field_fget_PROT(rec, &brec1->prot);
	lnf_field_fget_SRCPORT(rec, &brec1->srcport);
	lnf_field_fget_DSTPORT(rec, &brec1->dstport);
	lnf_field_fget_DOCTETS(rec, &brec1->bytes);
	lnf_field_fget_DPKTS(rec, &brec1->pkts);
	lnf_field_fget_AGGR_FLOWS(rec, &brec1->flows);
	return LNF_OK;
}


static int inline lnf_field_fset_BREC1(lnf_rec_t *rec, void *p) { 
	lnf_brec1_t *brec1 = p;

	lnf_field_fset_FIRST(rec, &brec1->first);
	lnf_field_fset_LAST(rec, &brec1->last);
	lnf_field_fset_SRCADDR(rec, &brec1->srcaddr);
	lnf_field_fset_DSTADDR(rec, &brec1->dstaddr);
	lnf_field_fset_PROT(rec, &brec1->prot);
	lnf_field_fset_SRCPORT(rec, &brec1->srcport);
	lnf_field_fset_DSTPORT(rec, &brec1->dstport);
	lnf_field_fset_DOCTETS(rec, &brec1->bytes);
	lnf_field_fset_DPKTS(rec, &brec1->pkts);
	lnf_field_fset_AGGR_FLOWS(rec, &brec1->flows);
	return LNF_OK;

}
/* ----------------------- */

/* text description of the fields */
/* WARNING if you change something in here, make sure to also change LNF_FLD_xxx_ALIAS
   if relevant, otherwise expect some inconsistent behaviour */
lnf_field_def_t lnf_fields_def[] = {
// pod:  =====================
	[LNF_FLD_FIRST] = {
		LNF_UINT64,	sizeof(LNF_UINT64_T), LNF_AGGR_MIN,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"first",	"Timestamp of the first packet seen (in miliseconds)",
		"flowStartSysUpTime", 0, 22,
		NULL, 0, 0, 
		lnf_field_fget_FIRST,
		lnf_field_fset_FIRST },

	[LNF_FLD_LAST] = {
		LNF_UINT64,	sizeof(LNF_UINT64_T), LNF_AGGR_MAX,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"last",		"Timestamp of the last packet seen (in miliseconds)",
		"flowEndSysUpTime", 0, 21,
		NULL, 0, 0, 
		lnf_field_fget_LAST,
		lnf_field_fset_LAST },

	[LNF_FLD_RECEIVED] = {
		LNF_UINT64, sizeof(LNF_UINT64_T),	LNF_AGGR_MAX,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"received",	"Timestamp regarding when the packet was received by collector",
		"collectionTimeMillisecondse", 0, 258,
		NULL, 0, 0, 
		lnf_field_fget_RECEIVED,
		lnf_field_fset_RECEIVED },
		
// pod:
// pod:  Statistical items
// pod:  =====================
	[LNF_FLD_DOCTETS] = {
		LNF_UINT64, sizeof(LNF_UINT64_T),		LNF_AGGR_SUM,	LNF_SORT_DESC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"bytes",	"The number of bytes",
		"octetDeltaCount", 0, 1,
		NULL, 0, 0, 
		lnf_field_fget_DOCTETS,
		lnf_field_fset_DOCTETS},

	[LNF_FLD_DPKTS] = {
		LNF_UINT64, sizeof(LNF_UINT64_T),			LNF_AGGR_SUM,	LNF_SORT_DESC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"packets",		"The number of packets",
		"packetDeltaCount", 0, 2,
		NULL, 0, 0, 
		lnf_field_fget_DPKTS,
		lnf_field_fset_DPKTS},

    [LNF_FLD_DPKTS_ALIAS] = {
        LNF_UINT64, sizeof(LNF_UINT64_T),			LNF_AGGR_SUM,	LNF_SORT_DESC,
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        "pkts",		"The number of packets",
        "packetDeltaCount", 0, 2,
        NULL, 0, 0,
        lnf_field_fget_DPKTS,
        lnf_field_fset_DPKTS},

	[LNF_FLD_OUT_BYTES] = {
		LNF_UINT64, sizeof(LNF_UINT64_T),		LNF_AGGR_SUM,	LNF_SORT_DESC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"outbytes",	"The number of output bytes",
		"postOctetDeltaCount", 0, 23,
		NULL, 0, 0, 
		lnf_field_fget_OUT_BYTES,
		lnf_field_fset_OUT_BYTES},

	[LNF_FLD_OUT_PKTS] = { 
		LNF_UINT64, sizeof(LNF_UINT64_T),		LNF_AGGR_SUM,	LNF_SORT_DESC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"outpackets",	"The number of output packets",
		"postPacketDeltaCount", 0, 24,
		NULL, 0, 0, 
		lnf_field_fget_OUT_PKTS,
		lnf_field_fset_OUT_PKTS},

    [LNF_FLD_OUT_PKTS_ALIAS] = {
        LNF_UINT64, sizeof(LNF_UINT64_T),		LNF_AGGR_SUM,	LNF_SORT_DESC,
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        "outpkts",	"The number of output packets",
        "postPacketDeltaCount", 0, 24,
        NULL, 0, 0,
        lnf_field_fget_OUT_PKTS,
        lnf_field_fset_OUT_PKTS},

	[LNF_FLD_AGGR_FLOWS] = {
		LNF_UINT64, sizeof(LNF_UINT64_T),	LNF_AGGR_SUM,	LNF_SORT_DESC,
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"flows",	"The number of flows (aggregated)",
		"deltaFlowCount", 0, 3,
		NULL, 0, 0, 
// probably other elemt woul be more suitable
		lnf_field_fget_AGGR_FLOWS,
		lnf_field_fset_AGGR_FLOWS},

// pod:
// pod:  Layer 4 information
// pod:  =====================
	[LNF_FLD_SRCPORT] = {
		LNF_UINT16, sizeof(LNF_UINT16_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"srcport",		"Source port",
		"sourceTransportPort", 0, 7,
		NULL, 0, 0, 
		lnf_field_fget_SRCPORT,
		lnf_field_fset_SRCPORT},

	[LNF_FLD_DSTPORT] = {
		LNF_UINT16, sizeof(LNF_UINT16_T), 		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"dstport",		"Destination port",
		"destinationTransportPort", 0, 11,
		NULL, 0, 0, 
		lnf_field_fget_DSTPORT,
		lnf_field_fset_DSTPORT},

	[LNF_FLD_TCP_FLAGS] = {
		LNF_UINT8, sizeof(LNF_UINT8_T),		LNF_AGGR_OR,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"flags",		"TCP flags",
		"tcpControlBits", 0, 6,
		NULL, 0, 0, 
		lnf_field_fget_TCP_FLAGS,
		lnf_field_fset_TCP_FLAGS},

    [LNF_FLD_TCP_FLAGS_ALIAS] = {
        LNF_UINT8, sizeof(LNF_UINT8_T),		LNF_AGGR_OR,	LNF_SORT_ASC,
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        "tcpflags",		"TCP flags",
        "tcpControlBits", 0, 6,
        NULL, 0, 0,
        lnf_field_fget_TCP_FLAGS,
        lnf_field_fset_TCP_FLAGS},

// pod}:
// pod:  Layer 3 information
// pod:  =====================
	[LNF_FLD_SRCADDR ] = {
		LNF_ADDR, sizeof(LNF_ADDR_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"srcip",	"Source IP address",
		"sourceIPv4Address", 0, 8, 
		"sourceIPv6Address", 0, 27, 
		lnf_field_fget_SRCADDR,
		lnf_field_fset_SRCADDR},

	[LNF_FLD_DSTADDR] = {
		LNF_ADDR, sizeof(LNF_ADDR_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"dstip",	"Destination IP address",
		"destinationIPv4Address", 0, 12, 
		"destinationIPv6Address", 0, 28, 
		lnf_field_fget_DSTADDR,
		lnf_field_fset_DSTADDR},

    [LNF_FLD_SRCADDR_ALIAS] = {
        LNF_ADDR, sizeof(LNF_ADDR_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        "srcnet",	"Source IP address",
        "sourceIPv4Address", 0, 8,
        "sourceIPv6Address", 0, 27,
        lnf_field_fget_SRCADDR,
        lnf_field_fset_SRCADDR},

    [LNF_FLD_DSTADDR_ALIAS] = {
        LNF_ADDR, sizeof(LNF_ADDR_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        "dstnet",	"Destination IP address",
        "destinationIPv4Address", 0, 12,
        "destinationIPv6Address", 0, 28,
        lnf_field_fget_DSTADDR,
        lnf_field_fset_DSTADDR},

	[LNF_FLD_IP_NEXTHOP] = {
		LNF_ADDR, sizeof(LNF_ADDR_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"nextip",		"IP next hop",
		"ipNextHopIPv4Address", 0, 15, 
		"ipNextHopIPv6Address", 0, 62, 
		lnf_field_fget_IP_NEXTHOP,
		lnf_field_fset_IP_NEXTHOP},

    [LNF_FLD_IP_NEXTHOP_ALIAS] = {
        LNF_ADDR, sizeof(LNF_ADDR_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        "nexthop",		"IP next hop",
        "ipNextHopIPv4Address", 0, 15,
        "ipNextHopIPv6Address", 0, 62,
        lnf_field_fget_IP_NEXTHOP,
        lnf_field_fset_IP_NEXTHOP},

	[LNF_FLD_SRC_MASK] = {
		LNF_UINT8, sizeof(LNF_UINT8_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"srcmask",		"Source mask",
		"sourceIPv4PrefixLength", 0, 9, 
		"sourceIPv6PrefixLength", 0, 29, 
		lnf_field_fget_SRC_MASK, 
		lnf_field_fset_SRC_MASK}, 

	[LNF_FLD_DST_MASK] = {
		LNF_UINT8, sizeof(LNF_UINT8_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"dstmask",		"Destination mask",
		"destinationIPv4PrefixLength", 0, 13, 
		"destinationIPv6PrefixLength", 0, 30, 
		lnf_field_fget_DST_MASK, 
		lnf_field_fset_DST_MASK}, 

	[LNF_FLD_TOS] = {
		LNF_UINT8, sizeof(LNF_UINT8_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"tos",		"Source type of service",
		"ipClassOfService", 0, 5, 
		NULL, 0, 0, 
		lnf_field_fget_TOS, 
		lnf_field_fset_TOS}, 

	[LNF_FLD_DST_TOS] = {
		LNF_UINT8, sizeof(LNF_UINT8_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"dsttos",	"Destination type of service",
		"postIpClassOfService", 0, 55, 
		NULL, 0, 0, 
		lnf_field_fget_DST_TOS,
		lnf_field_fset_DST_TOS},

	[LNF_FLD_SRCAS] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"srcas",	"Source AS number",
		"bgpSourceAsNumber", 0, 16, 
		NULL, 0, 0, 
		lnf_field_fget_SRCAS,
		lnf_field_fset_SRCAS},

	[LNF_FLD_DSTAS] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"dstas",	"Destination AS number",
		"bgpDestinationAsNumber", 0, 17, 
		NULL, 0, 0, 
		lnf_field_fget_DSTAS,
		lnf_field_fset_DSTAS},

	[LNF_FLD_BGPNEXTADJACENTAS] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"nextas",	"BGP Next AS",
		"bgpNextAdjacentAsNumber", 0, 128, 
		NULL, 0, 0, 
		lnf_field_fget_BGPNEXTADJACENTAS,
		lnf_field_fset_BGPNEXTADJACENTAS},

	[LNF_FLD_BGPPREVADJACENTAS] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"prevas",	"BGP Previous AS",
		"bgpPrevAdjacentAsNumber", 0, 129, 
		NULL, 0, 0, 
		lnf_field_fget_BGPPREVADJACENTAS,
		lnf_field_fset_BGPPREVADJACENTAS},

	[LNF_FLD_BGP_NEXTHOP] = {
		LNF_ADDR, sizeof(LNF_ADDR_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"bgpnexthop",	"BGP next hop",
		"bgpNextHopIPv4Address", 0, 18, 
		"bgpNextHopIPv6Address", 0, 63, 
		lnf_field_fget_BGP_NEXTHOP,
		lnf_field_fset_BGP_NEXTHOP},

	[LNF_FLD_PROT] = {
		LNF_UINT8, sizeof(LNF_UINT8_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"proto",	"IP protocol",
		"protocolIdentifier", 0, 4, 
		NULL, 0, 0, 
		lnf_field_fget_PROT, 
		lnf_field_fset_PROT}, 

// pod:
// pod:  Layer 2 information
// pod:  =====================
	[LNF_FLD_SRC_VLAN] = {
		LNF_UINT16, sizeof(LNF_UINT16_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"srcvlan",		"Source vlan label",
		"vlanId", 0, 58, 
		NULL, 0, 0, 
		lnf_field_fget_SRC_VLAN,
		lnf_field_fset_SRC_VLAN},

	[LNF_FLD_DST_VLAN] = {
		LNF_UINT16, sizeof(LNF_UINT16_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"dstvlan",		"Destination vlan label",
		"postVlanId", 0, 59, 
		NULL, 0, 0, 
		lnf_field_fget_DST_VLAN, 
		lnf_field_fset_DST_VLAN}, 

	[LNF_FLD_IN_SRC_MAC] = {
		LNF_MAC, sizeof(LNF_MAC_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_}, 
		"insrcmac",		"In source MAC address",
		"sourceMacAddress", 0, 56, 
		NULL, 0, 0, 
		lnf_field_fget_IN_SRC_MAC,
		lnf_field_fset_IN_SRC_MAC},

	[LNF_FLD_OUT_SRC_MAC] = {
		LNF_MAC, sizeof(LNF_MAC_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"outsrcmac",	"Out destination MAC address",
		"destinationMacAddress", 0, 80, 
		NULL, 0, 0, 
		lnf_field_fget_OUT_SRC_MAC,
		lnf_field_fset_OUT_SRC_MAC},

	[LNF_FLD_IN_DST_MAC] = {
		LNF_MAC, sizeof(LNF_MAC_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"indstmac",	"In destination MAC address",
		"postSourceMacAddress", 0, 81, 
		NULL, 0, 0, 
		lnf_field_fget_IN_DST_MAC, 
		lnf_field_fset_IN_DST_MAC}, 

	[LNF_FLD_OUT_DST_MAC] = {
		LNF_MAC, sizeof(LNF_MAC_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"outdstmac",	"Out source MAC address",
		"postDestinationMacAddress", 0, 57, 
		NULL, 0, 0, 
		lnf_field_fget_OUT_DST_MAC, 
		lnf_field_fset_OUT_DST_MAC}, 

// pod:
// pod:  MPLS information
// pod:  =====================
	[LNF_FLD_MPLS_LABEL] = {
		LNF_MPLS, sizeof(LNF_MPLS_T),	LNF_AGGR_KEY,	LNF_SORT_NONE,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"mpls",		"MPLS labels",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_MPLS_LABEL,
		lnf_field_fset_MPLS_LABEL},

// pod:
// pod:  Layer 1 information
// pod:  =====================
	[LNF_FLD_INPUT] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"inif",		"SNMP input interface number",
		"ingressInterface", 0, 10, 
		NULL, 0, 0, 
		lnf_field_fget_INPUT,
		lnf_field_fset_INPUT},

	[LNF_FLD_OUTPUT] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"outif",	"SNMP output interface number",
		"egressInterface", 0, 14, 
		NULL, 0, 0, 
		lnf_field_fget_OUTPUT,
		lnf_field_fset_OUTPUT},

	[LNF_FLD_DIR] = {
		LNF_UINT8, sizeof(LNF_UINT8_T),			LNF_AGGR_KEY,	LNF_SORT_ASC,
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"dir",		"Flow directions ingress/egress",
		"flowDirection", 0, 61, 
		NULL, 0, 0, 
		lnf_field_fget_DIR, 
		lnf_field_fset_DIR}, 

	[LNF_FLD_FWD_STATUS] = {
		LNF_UINT8, sizeof(LNF_UINT8_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"fwd",		"Forwarding status",
		"forwardingStatus", 0, 89, 
		NULL, 0, 0, 
		lnf_field_fget_FWD_STATUS,
		lnf_field_fset_FWD_STATUS},
// pod:
// pod:  Exporter information
// pod:  =====================
	[LNF_FLD_IP_ROUTER] = {
		LNF_ADDR, sizeof(LNF_ADDR_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"routerip",	"Exporting router IP",
		"exporterIPv4Address", 0, 130, 
		"exporterIPv6Address", 0, 131, 
		lnf_field_fget_IP_ROUTER, 
		lnf_field_fset_IP_ROUTER},

    [LNF_FLD_IP_ROUTER_ALIAS] = {
        LNF_ADDR, sizeof(LNF_ADDR_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        "router",	"Exporting router IP",
        "exporterIPv4Address", 0, 130,
        "exporterIPv6Address", 0, 131,
        lnf_field_fget_IP_ROUTER,
        lnf_field_fset_IP_ROUTER},

	[LNF_FLD_ENGINE_TYPE] = {
		LNF_UINT8, sizeof(LNF_UINT8_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"engine-type",	"Type of exporter",
		"engineType", 0, 38, 
		NULL, 0, 0, 
		lnf_field_fget_ENGINE_TYPE,
		lnf_field_fset_ENGINE_TYPE},

	[LNF_FLD_ENGINE_ID] = {
		LNF_UINT8, sizeof(LNF_UINT8_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"engine-id",	"Internal SysID of exporter",
		"engineId", 0, 39, 
		NULL, 0, 0, 
		lnf_field_fget_ENGINE_ID,
		lnf_field_fset_ENGINE_ID},

    [LNF_FLD_ENGINE_TYPE_ALIAS] = {
        LNF_UINT8, sizeof(LNF_UINT8_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        "systype",	"Type of exporter",
        "engineType", 0, 38,
        NULL, 0, 0,
        lnf_field_fget_ENGINE_TYPE,
        lnf_field_fset_ENGINE_TYPE},

    [LNF_FLD_ENGINE_ID_ALIAS] = {
        LNF_UINT8, sizeof(LNF_UINT8_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        "sysid",	"Internal SysID of exporter",
        "engineId", 0, 39,
        NULL, 0, 0,
        lnf_field_fget_ENGINE_ID,
        lnf_field_fset_ENGINE_ID},

// pod:
// pod:  NSEL fields, see: http://www.cisco.com/en/US/docs/security/asa/asa81/netflow/netflow.html
// pod:  =====================
	[LNF_FLD_EVENT_TIME] = {
		LNF_UINT64, sizeof(LNF_UINT64_T),		LNF_AGGR_MIN,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"eventtime",	"NSEL The time that the flow was created",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_EVENT_TIME,
		lnf_field_fset_EVENT_TIME},

	[LNF_FLD_CONN_ID] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"connid",		"NSEL An identifier of a unique flow for the device",
		"flowId", 0, 148, 
		NULL, 0, 0, 
		lnf_field_fget_CONN_ID,
		lnf_field_fset_CONN_ID},

	[LNF_FLD_ICMP_CODE] = {
		LNF_UINT8, sizeof(LNF_UINT8_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"icmp-code",		"NSEL ICMP code value",
		"icmpCodeIPv4", 0, 177, 
		"icmpCodeIPv6", 0, 179, 
		lnf_field_fget_ICMP_CODE,
		lnf_field_fset_ICMP_CODE},

	[LNF_FLD_ICMP_TYPE] = {
		LNF_UINT8, sizeof(LNF_UINT8_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"icmp-type",		"NSEL ICMP type value",
		"icmpTypeIPv4", 0, 176, 
		"icmpTypeIPv6", 0, 178, 
		lnf_field_fget_ICMP_TYPE,
		lnf_field_fset_ICMP_TYPE},

    [LNF_FLD_ICMP_CODE_ALIAS] = {
        LNF_UINT8, sizeof(LNF_UINT8_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        "icmpcode",		"NSEL ICMP code value",
        "icmpCodeIPv4", 0, 177,
        "icmpCodeIPv6", 0, 179,
        lnf_field_fget_ICMP_CODE,
        lnf_field_fset_ICMP_CODE},

    [LNF_FLD_ICMP_TYPE_ALIAS] = {
        LNF_UINT8, sizeof(LNF_UINT8_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        "icmptype",		"NSEL ICMP type value",
        "icmpTypeIPv4", 0, 176,
        "icmpTypeIPv6", 0, 178,
        lnf_field_fget_ICMP_TYPE,
        lnf_field_fset_ICMP_TYPE},

	[LNF_FLD_FW_XEVENT] = {
		LNF_UINT16, sizeof(LNF_UINT16_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"xevent",		"NSEL Extended event code",
		"firewallEvent", 0, 233, 
		NULL, 0, 0, 
		lnf_field_fget_FW_XEVENT,
		lnf_field_fset_FW_XEVENT},

	[LNF_FLD_FW_EVENT] = {
		LNF_UINT8, sizeof(LNF_UINT8_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"event",		"NSEL Extended event code",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_FW_EVENT,
		lnf_field_fset_FW_EVENT},

	[LNF_FLD_XLATE_SRC_IP] = {
		LNF_ADDR, sizeof(LNF_ADDR_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"xsrcip",	"NSEL Mapped source IPv4 address",
		"postNATSourceIPv4Address", 0, 225, 
		"postNATSourceIPv6Address", 0, 281, 
		lnf_field_fget_XLATE_SRC_IP,
		lnf_field_fset_XLATE_SRC_IP},

	[LNF_FLD_XLATE_DST_IP] = {
		LNF_ADDR, sizeof(LNF_ADDR_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"xdstip",	"NSEL Mapped destination IPv4 address",
		"postNATDestinationIPv4Address", 0, 226, 
		"postNATDestinationIPv6Address", 0, 282, 
		lnf_field_fget_XLATE_DST_IP,
		lnf_field_fset_XLATE_DST_IP},

	[LNF_FLD_XLATE_SRC_PORT] = {
		LNF_UINT16, sizeof(LNF_UINT16_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"xsrcport",	"NSEL Mapped source port",
		"postNAPTSourceTransportPort", 0, 227, 
		NULL, 0, 0, 
		lnf_field_fget_XLATE_SRC_PORT,
		lnf_field_fset_XLATE_SRC_PORT},

	[LNF_FLD_XLATE_DST_PORT] = {
		LNF_UINT16, sizeof(LNF_UINT16_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"xdstport",	"NSEL Mapped destination port",
		"postNAPTDestinationTransportPort", 0, 228, 
		NULL, 0, 0, 
		lnf_field_fget_XLATE_DST_PORT,
		lnf_field_fset_XLATE_DST_PORT},

// pod: NSEL The input ACL that permitted or denied the flow
	[LNF_FLD_INGRESS_ACL_ID] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"iacl",		"Hash value or ID of the ACL name",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_INGRESS_ACL_ID,
		lnf_field_fset_INGRESS_ACL_ID},

	[LNF_FLD_INGRESS_ACE_ID] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"iace", 	"Hash value or ID of the ACL name",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_INGRESS_ACE_ID,
		lnf_field_fset_INGRESS_ACE_ID},

	[LNF_FLD_INGRESS_XACE_ID] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"ixace",	"Hash value or ID of an extended ACE configuration",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_INGRESS_XACE_ID,
		lnf_field_fset_INGRESS_XACE_ID},

	[LNF_FLD_INGRESS_ACL] = {
		LNF_ACL, sizeof(LNF_ACL_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"ingressacl",		"96 bit value including all items in ACL (iacl, iace, ixace)",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_INGRESS_ACL,
		lnf_field_fset_INGRESS_ACL},

// pod: NSEL The output ACL that permitted or denied a flow  
	[LNF_FLD_EGRESS_ACL_ID] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"eacl",		"Hash value or ID of the ACL name",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_EGRESS_ACL_ID,
		lnf_field_fset_EGRESS_ACL_ID},

	[LNF_FLD_EGRESS_ACE_ID] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"eace",		"Hash value or ID of the ACL name",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_EGRESS_ACE_ID,
		lnf_field_fset_EGRESS_ACE_ID},

	[LNF_FLD_EGRESS_XACE_ID] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"exace",	"Hash value or ID of an extended ACE configuration",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_EGRESS_XACE_ID,
		lnf_field_fset_EGRESS_XACE_ID},

	[LNF_FLD_EGRESS_ACL] = {
		LNF_ACL, sizeof(LNF_ACL_T),	LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"egressacl",		"96 bit value including all items in ACL (eacl, eace, exace)",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_EGRESS_ACL,
		lnf_field_fset_EGRESS_ACL},

	[LNF_FLD_USERNAME] = {
		LNF_STRING, sizeof(LNF_STRING_T),			LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"username",	"NSEL username",
		"userName", 0, 371, 
		NULL, 0, 0, 
		lnf_field_fget_USERNAME,
		lnf_field_fset_USERNAME},

// pod:
// pod:  NEL (NetFlow Event Logging) fields
// pod:  =====================
	[LNF_FLD_INGRESS_VRFID] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"ingressvrfid",		"NEL NAT ingress vrf id",
		"ingressVRFID", 0, 234, 
		NULL, 0, 0, 
		lnf_field_fget_INGRESS_VRFID,
		lnf_field_fset_INGRESS_VRFID},

	[LNF_FLD_EVENT_FLAG] = {
		LNF_UINT8, sizeof(LNF_UINT8_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"eventflag",		"NAT event flag (always set to 1 by nfdump)",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_EVENT_FLAG,
		lnf_field_fset_EVENT_FLAG},

	[LNF_FLD_EGRESS_VRFID] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"egressvrfid",		"NAT egress VRF ID",
		"egressVRFID", 0, 235, 
		NULL, 0, 0, 
		lnf_field_fget_EGRESS_VRFID,
		lnf_field_fset_EGRESS_VRFID},

	[LNF_FLD_BLOCK_START] = {
		LNF_UINT16, sizeof(LNF_UINT16_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"blockstart",		"NAT pool block start",
		"portRangeStart", 0, 361, 
		NULL, 0, 0, 
		lnf_field_fget_BLOCK_START,
		lnf_field_fset_BLOCK_START},

// pod:
// pod:  NEL Port Block Allocation (added 2014-04-19)
// pod:  =====================
	[LNF_FLD_BLOCK_END] = {
		LNF_UINT16, sizeof(LNF_UINT16_T),			LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"blockend",			"NAT pool block end",
		"portRangeEnd", 0, 362, 
		NULL, 0, 0, 
		lnf_field_fget_BLOCK_END,
		lnf_field_fset_BLOCK_END},

	[LNF_FLD_BLOCK_STEP] = {
		LNF_UINT16, sizeof(LNF_UINT16_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"blockstep",		"NAT pool block step",
		"portRangeStepSize", 0, 363, 
		NULL, 0, 0, 
		lnf_field_fget_BLOCK_STEP,
		lnf_field_fset_BLOCK_STEP},

	[LNF_FLD_BLOCK_SIZE] = {
		LNF_UINT16, sizeof(LNF_UINT16_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"blocksize",		"NAT pool block size",
		"portRangeNumPorts", 0, 364, 
		NULL, 0, 0, 
		lnf_field_fget_BLOCK_SIZE,
		lnf_field_fset_BLOCK_SIZE},
// pod:
// pod:  Extra/special fields
// pod:  =====================
	[LNF_FLD_CLIENT_NW_DELAY_USEC] = {
		LNF_UINT64, sizeof(LNF_UINT64_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"cl",	"nprobe latency client_nw_delay_usec",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_CLIENT_NW_DELAY_USEC,
		lnf_field_fset_CLIENT_NW_DELAY_USEC},

	[LNF_FLD_SERVER_NW_DELAY_USEC] = {
		LNF_UINT64, sizeof(LNF_UINT64_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"sl",	"nprobe latency server_nw_delay_usec",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_SERVER_NW_DELAY_USEC,
		lnf_field_fset_SERVER_NW_DELAY_USEC},

	[LNF_FLD_APPL_LATENCY_USEC] = {
		LNF_UINT64, sizeof(LNF_UINT64_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"al",	"nprobe latency appl_latency_usec",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_APPL_LATENCY_USEC,
		lnf_field_fset_APPL_LATENCY_USEC},

	[LNF_FLD_INET_FAMILY] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"inetfamily",	"IENT family for src/dst IP address (ipv4 or ipv6); platform dependant",
		"ipVersion", 0, 60,
		NULL, 0, 0, 
		lnf_field_fget_INET_FAMILY,
		lnf_field_fset_INET_FAMILY},
	[LNF_FLD_EXPORTER_IP] = {
		LNF_ADDR, sizeof(LNF_ADDR_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"exporterid",	"Exporter IP address",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_EXPORTER_IP,
		lnf_field_fset_EXPORTER_IP},
	[LNF_FLD_EXPORTER_ID] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"exporterid",	"Exporter Observation Domain ID",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_EXPORTER_ID,
		lnf_field_fset_EXPORTER_ID},
	[LNF_FLD_EXPORTER_VERSION] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"exporterversion",	"Version of exporter",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_EXPORTER_VERSION,
		lnf_field_fset_EXPORTER_VERSION},
	[LNF_FLD_SEQUENCE_FAILURES] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_SUM,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"exporterversion",	"Naumber of sequence failures of data received from exporter",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_SEQUENCE_FAILURES,
		lnf_field_fset_SEQUENCE_FAILURES},
	[LNF_FLD_SAMPLER_MODE] = {
		LNF_UINT16, sizeof(LNF_UINT16_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"samplermode",	"Sampling mode",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_SAMPLER_MODE,
		lnf_field_fset_SAMPLER_MODE},
	[LNF_FLD_SAMPLER_INTERVAL] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"sampleinterval",	"Ssampling interval",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_SAMPLER_INTERVAL,
		lnf_field_fset_SAMPLER_INTERVAL},
	[LNF_FLD_SAMPLER_ID] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_KEY,	LNF_SORT_ASC,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"samplerid",	"Sampler ID assigned by exporting device",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_SAMPLER_ID,
		lnf_field_fset_SAMPLER_ID},
// pod:
// pod:  Calculated items
// pod:  =====================
	[LNF_FLD_CALC_DURATION] = {
		LNF_UINT64, sizeof(LNF_UINT64_T),		LNF_AGGR_SUM,	LNF_SORT_ASC,	
		{LNF_FLD_FIRST, LNF_FLD_LAST, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"duration",	"Flow duration (in milliseconds)",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_CALC_DURATION,
		lnf_field_fset_EMPTY_},

	[LNF_FLD_CALC_BPS] = {
		LNF_DOUBLE, sizeof(LNF_DOUBLE_T),		LNF_AGGR_SUM,	LNF_SORT_DESC,	
		{LNF_FLD_DOCTETS, LNF_FLD_CALC_DURATION, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"bps",	"Bytes per second",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_CALC_BPS,
		lnf_field_fset_EMPTY_},

	[LNF_FLD_CALC_PPS] = {
		LNF_DOUBLE, sizeof(LNF_DOUBLE_T),		LNF_AGGR_SUM,	LNF_SORT_DESC,	
		{LNF_FLD_DPKTS, LNF_FLD_CALC_DURATION, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"pps",	"Packets per second",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_CALC_PPS,
		lnf_field_fset_EMPTY_},

	[LNF_FLD_CALC_BPP] = {
		LNF_DOUBLE, sizeof(LNF_DOUBLE_T),		LNF_AGGR_SUM,	LNF_SORT_ASC,	
		{LNF_FLD_DPKTS, LNF_FLD_DOCTETS, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"bpp",	"Bytes per packet",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_CALC_BPP,
		lnf_field_fset_EMPTY_},

// pod:
// pod:  Structure lnf_brec1_t - basic record 
// pod:  =====================
	[LNF_FLD_BREC1] = {
		LNF_BASIC_RECORD1, sizeof(LNF_BASIC_RECORD1_T),		LNF_AGGR_KEY,	LNF_SORT_NONE,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		"brec1",	"basic record 1",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_BREC1,
		lnf_field_fset_BREC1},

// pod:
// pod:  Pair items 
// pod:  =====================
	[LNF_FLD_PAIR_PORT] = {
		LNF_UINT16, sizeof(LNF_UINT16_T),		LNF_AGGR_KEY,	LNF_SORT_NONE,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_SRCPORT, LNF_FLD_DSTPORT},
		"port",		"Source or destination port (pair field)",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_SRCPORT,
		lnf_field_fset_SRCPORT},

	[LNF_FLD_PAIR_ADDR] = {
		LNF_ADDR, sizeof(LNF_ADDR_T),		LNF_AGGR_KEY,	LNF_SORT_NONE,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_SRCADDR, LNF_FLD_DSTADDR},
		"ip",		"Source or destination ip address (pair field)",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_SRCADDR,
		lnf_field_fset_SRCADDR},

    [LNF_FLD_PAIR_ADDR_ALIAS] = {
        LNF_ADDR, sizeof(LNF_ADDR_T),		LNF_AGGR_KEY,	LNF_SORT_NONE,
        {LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
        {LNF_FLD_SRCADDR, LNF_FLD_DSTADDR},
        "net",		"Source or destination ip address (pair field)",
        NULL, 0, 0,
        NULL, 0, 0,
        lnf_field_fget_SRCADDR,
        lnf_field_fset_SRCADDR},

	[LNF_FLD_PAIR_AS] = {
		LNF_UINT32, sizeof(LNF_UINT32_T),		LNF_AGGR_KEY,	LNF_SORT_NONE,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_SRCAS, LNF_FLD_DSTAS},
		"as",		"Source or destination ASn (pair field)",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_SRCAS,
		lnf_field_fset_SRCAS},

	[LNF_FLD_PAIR_IF] = {
		LNF_UINT16, sizeof(LNF_UINT16_T),		LNF_AGGR_KEY,	LNF_SORT_NONE,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_SRCAS, LNF_FLD_DSTAS},
		"if",		"Input or output interface (pair field)",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_INPUT,
		lnf_field_fset_INPUT},

	[LNF_FLD_PAIR_VLAN] = {
		LNF_UINT16, sizeof(LNF_UINT16_T),		LNF_AGGR_KEY,	LNF_SORT_NONE,	
		{LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_, LNF_FLD_ZERO_},
		{LNF_FLD_SRC_VLAN, LNF_FLD_DST_VLAN},
		"vlan",		"Source or destination vlan (pair field)",
		NULL, 0, 0, 
		NULL, 0, 0, 
		lnf_field_fget_SRC_VLAN,
		lnf_field_fset_SRC_VLAN},


	[LNF_FLD_TERM_] = {
		0, 0,				0,				0,				
		{0, 0, 0, 0},
		{0, 0},
		NULL,	NULL,
		NULL, 0, 0, 
		NULL, 0, 0, 
		NULL, 
		NULL}
};

/* return info information for fiels 
*/
int lnf_fld_info(int field, int info, void *data, size_t size) {

	lnf_field_def_t *fe;
	int i;
	size_t reqsize; /* requested minimum size */
	char buf[LNF_INFO_BUFSIZE];

	if (info == LNF_FLD_INFO_FIELDS) {
		int *fld  = (int *)buf;
		int x = 0;
		/* find ID for field */
		for (i = LNF_FLD_ZERO_; i < LNF_FLD_TERM_; i++) {
			if (lnf_fields_def[i].name != NULL) {
				*fld = i;
				fld++;
				x++;
			}
		}
		*fld = LNF_FLD_TERM_;
		x++;

		if (size < sizeof(int) * x) {
			return LNF_ERR_NOMEM;
		} else {
			memcpy(data, buf, sizeof(int) * x);
			return LNF_OK;
		}
	}

	if (field < LNF_FLD_ZERO_ || field > LNF_FLD_TERM_) {
		return LNF_ERR_UNKFLD;
	}

	fe = &lnf_fields_def[field];

	switch (info) {
		case LNF_FLD_INFO_TYPE:
			*((int *)buf) = fe->type;
			reqsize = sizeof(fe->type);
			break;
		case LNF_FLD_INFO_SIZE:
			*((int *)buf) = fe->size;
			reqsize = sizeof(fe->size);
			break;
		case LNF_FLD_INFO_NAME:
			strcpy(buf, fe->name);
			reqsize = strlen(fe->name) + 1;
			break;
		case LNF_FLD_INFO_DESCR:
			strcpy(buf, fe->fld_descr);
			reqsize = strlen(fe->fld_descr) + 1;
			break;
		case LNF_FLD_INFO_AGGR:
			*((int *)buf) = fe->default_aggr;
			reqsize = sizeof(fe->default_aggr);
			break;
		case LNF_FLD_INFO_SORT:
			*((int *)buf) = fe->default_sort;
			reqsize = sizeof(fe->default_sort);
			break;
		case LNF_FLD_INFO_IPFIX_NAME:
			if (fe->ipfix_name == NULL) {
				return  LNF_ERR_NOTSET;
			}
			strcpy(buf, fe->ipfix_name);
			reqsize = strlen(fe->ipfix_name) + 1;
			break;
		case LNF_FLD_INFO_IPFIX_EID:
			*((int *)buf) = fe->ipfix_enterprise;
			reqsize = sizeof(fe->ipfix_enterprise);
			break;
		case LNF_FLD_INFO_IPFIX_ID:
			*((int *)buf) = fe->ipfix_id;
			reqsize = sizeof(fe->ipfix_id);
			break;
		case LNF_FLD_INFO_IPFIX_NAME6:
			if (fe->ipfix_name6 == NULL) {
				return  LNF_ERR_NOTSET;
			}
			strcpy(buf, fe->ipfix_name6);
			reqsize = strlen(fe->ipfix_name6) + 1;
			break;
		case LNF_FLD_INFO_IPFIX_EID6:
			*((int *)buf) = fe->ipfix_enterprise6;
			reqsize = sizeof(fe->ipfix_enterprise6);
			break;
		case LNF_FLD_INFO_IPFIX_ID6:
			*((int *)buf) = fe->ipfix_id6;
			reqsize = sizeof(fe->ipfix_id6);
			break;
		default:
			return LNF_ERR_OTHER;
	}

	if (reqsize <= size) {
		memcpy(data, buf, reqsize);
		return LNF_OK;
	} else {
		return LNF_ERR_NOMEM;
	}
}

/* parse fields from string 
* accepted format like srcip/24/64 
*/
int lnf_fld_parse(const char *str, int *numbits, int *numbits6) {

	char *name, *strbits;
	int field = LNF_FLD_ZERO_;
	char lastch;
	int i;


	/* find first token */
	name = strsep((char **)&str, "/");

	if (name == NULL) {	
		return LNF_ERR_OTHER;
	}

	lastch = name[strlen(name) - 1];

	/* symbol 4 or 6 on last position */
	if (lastch == '4' || lastch == '6') {
		name[strlen(name) - 1] = '\0';
	}

	/* find ID for field */
	for (i = LNF_FLD_ZERO_; i < LNF_FLD_TERM_; i++) {
		if (lnf_fields_def[i].name != NULL) {
			if (strcmp(name, lnf_fields_def[i].name) == 0) {
				field = i;
				break;
			}
			/* try alternative IPFIX names */
			if (lnf_fields_def[i].ipfix_name != NULL && strcmp(name, lnf_fields_def[i].ipfix_name) == 0) {
				field = i;
				break;
			}
			if (lnf_fields_def[i].ipfix_name6 != NULL && strcmp(name, lnf_fields_def[i].ipfix_name6) == 0) {
				field = i;
				break;
			}
		}
	}

	if (field == LNF_FLD_ZERO_) {
		return LNF_FLD_ZERO_;
	}

	if (lnf_fld_type(field) != LNF_ADDR && lnf_fld_type(field) != LNF_UINT64) {
		if (numbits != NULL) { *numbits = 0; }
		if (numbits6 != NULL) { *numbits6 = 0; }
		return field;
	}
	
	if (lnf_fld_type(field) == LNF_ADDR) {
		if (numbits != NULL) { *numbits = 32; }
		if (numbits6 != NULL) { *numbits6 = 128; }
	} else {
		if (numbits != NULL) { *numbits = 0; }
		if (numbits6 != NULL) { *numbits6 = 0; }
	}

	/* numbits */
	if (str != NULL) {
		strbits = strsep((char **)&str, "/");
		if (strbits != NULL && numbits != NULL) {
			if (lastch == '6') {
				*numbits6 = strtol(strbits, NULL, 10);
			} else {
				*numbits = strtol(strbits, NULL, 10);
			}
		}
	}
	
	/* numbits6 */
	if (str != NULL && numbits6 != NULL) {
		if (lastch == '6') {
			*numbits = strtol(strbits, NULL, 10);
		} else {
			*numbits6 = strtol(str, NULL, 10);
		}
	}

	return field;
}

