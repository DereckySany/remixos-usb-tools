/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtNetwork module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QNETWORKINTERFACE_WIN_P_H
#define QNETWORKINTERFACE_WIN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <winsock2.h>
#include <windows.h>
#include <time.h>

QT_BEGIN_NAMESPACE

#ifndef GAA_FLAG_INCLUDE_ALL_INTERFACES
# define GAA_FLAG_INCLUDE_ALL_INTERFACES 0x0100
#endif
#ifndef MAX_ADAPTER_ADDRESS_LENGTH
// definitions from iptypes.h
# define MAX_ADAPTER_DESCRIPTION_LENGTH  128 // arb.
# define MAX_ADAPTER_NAME_LENGTH         256 // arb.
# define MAX_ADAPTER_ADDRESS_LENGTH      8   // arb.
# define DEFAULT_MINIMUM_ENTITIES        32  // arb.
# define MAX_HOSTNAME_LEN                128 // arb.
# define MAX_DOMAIN_NAME_LEN             128 // arb.
# define MAX_SCOPE_ID_LEN                256 // arb.

# define GAA_FLAG_SKIP_UNICAST       0x0001
# define GAA_FLAG_SKIP_ANYCAST       0x0002
# define GAA_FLAG_SKIP_MULTICAST     0x0004
# define GAA_FLAG_SKIP_DNS_SERVER    0x0008
# define GAA_FLAG_INCLUDE_PREFIX     0x0010
# define GAA_FLAG_SKIP_FRIENDLY_NAME 0x0020

# define IP_ADAPTER_DDNS_ENABLED               0x01
# define IP_ADAPTER_REGISTER_ADAPTER_SUFFIX    0x02
# define IP_ADAPTER_DHCP_ENABLED               0x04
# define IP_ADAPTER_RECEIVE_ONLY               0x08
# define IP_ADAPTER_NO_MULTICAST               0x10
# define IP_ADAPTER_IPV6_OTHER_STATEFUL_CONFIG 0x20

# define MIB_IF_TYPE_OTHER               1
# define MIB_IF_TYPE_ETHERNET            6
# define MIB_IF_TYPE_TOKENRING           9
# define MIB_IF_TYPE_FDDI                15
# define MIB_IF_TYPE_PPP                 23
# define MIB_IF_TYPE_LOOPBACK            24
# define MIB_IF_TYPE_SLIP                28

#endif
// copied from qnativesocketengine_win.cpp
struct qt_in6_addr {
    u_char qt_s6_addr[16];
};
typedef struct {
    short   sin6_family;            /* AF_INET6 */
    u_short sin6_port;              /* Transport level port number */
    u_long  sin6_flowinfo;          /* IPv6 flow information */
    struct  qt_in6_addr sin6_addr;  /* IPv6 address */
    u_long  sin6_scope_id;          /* set of interfaces for a scope */
} qt_sockaddr_in6;

// copied from MSDN online help
typedef enum {
  IpPrefixOriginOther = 0, 
  IpPrefixOriginManual, 
  IpPrefixOriginWellKnown, 
  IpPrefixOriginDhcp, 
  IpPrefixOriginRouterAdvertisement
} IP_PREFIX_ORIGIN;

typedef enum {
  IpSuffixOriginOther = 0, 
  IpSuffixOriginManual, 
  IpSuffixOriginWellKnown, 
  IpSuffixOriginDhcp, 
  IpSuffixOriginLinkLayerAddress, 
  IpSuffixOriginRandom
} IP_SUFFIX_ORIGIN;

typedef enum {
    IpDadStateInvalid    = 0,
    IpDadStateTentative,
    IpDadStateDuplicate,
    IpDadStateDeprecated,
    IpDadStatePreferred,
} IP_DAD_STATE;

typedef enum {
    IfOperStatusUp = 1,
    IfOperStatusDown,
    IfOperStatusTesting,
    IfOperStatusUnknown,
    IfOperStatusDormant,
    IfOperStatusNotPresent,
    IfOperStatusLowerLayerDown
} IF_OPER_STATUS;

typedef struct _IP_ADAPTER_UNICAST_ADDRESS {
  union {
    ULONGLONG Alignment;
    struct {
      ULONG Length;
      DWORD Flags;
    };
  };
  struct _IP_ADAPTER_UNICAST_ADDRESS* Next;
  SOCKET_ADDRESS Address;
  IP_PREFIX_ORIGIN PrefixOrigin;
  IP_SUFFIX_ORIGIN SuffixOrigin;
  IP_DAD_STATE DadState;
  ULONG ValidLifetime;
  ULONG PreferredLifetime;
  ULONG LeaseLifetime;
} IP_ADAPTER_UNICAST_ADDRESS, *PIP_ADAPTER_UNICAST_ADDRESS;

typedef struct _IP_ADAPTER_ANYCAST_ADDRESS 
 IP_ADAPTER_ANYCAST_ADDRESS, *PIP_ADAPTER_ANYCAST_ADDRESS;

typedef struct _IP_ADAPTER_MULTICAST_ADDRESS 
 IP_ADAPTER_MULTICAST_ADDRESS, 
 *PIP_ADAPTER_MULTICAST_ADDRESS;

typedef struct _IP_ADAPTER_DNS_SERVER_ADDRESS
 IP_ADAPTER_DNS_SERVER_ADDRESS,
 *PIP_ADAPTER_DNS_SERVER_ADDRESS;

typedef struct _IP_ADAPTER_PREFIX {
  union {
    ULONGLONG  Alignment;
    struct {
      ULONG Length;
      DWORD Flags;
    };
  };
  struct _IP_ADAPTER_PREFIX* Next;
  SOCKET_ADDRESS Address;
  ULONG PrefixLength;
} IP_ADAPTER_PREFIX, 
 *PIP_ADAPTER_PREFIX;

typedef struct _IP_ADAPTER_ADDRESSES {
  union {
    ULONGLONG Alignment;
    struct {
      ULONG Length;
      DWORD IfIndex;
    };
  };
  struct _IP_ADAPTER_ADDRESSES* Next;
  PCHAR AdapterName;
  PIP_ADAPTER_UNICAST_ADDRESS FirstUnicastAddress;
  PIP_ADAPTER_ANYCAST_ADDRESS FirstAnycastAddress;
  PIP_ADAPTER_MULTICAST_ADDRESS FirstMulticastAddress;
  PIP_ADAPTER_DNS_SERVER_ADDRESS FirstDnsServerAddress;
  PWCHAR DnsSuffix;
  PWCHAR Description;
  PWCHAR FriendlyName;
  BYTE PhysicalAddress[MAX_ADAPTER_ADDRESS_LENGTH];
  DWORD PhysicalAddressLength;
  DWORD Flags;
  DWORD Mtu;
  DWORD IfType;
  IF_OPER_STATUS OperStatus;
  DWORD Ipv6IfIndex;
  DWORD ZoneIndices[16];
  PIP_ADAPTER_PREFIX FirstPrefix;
} IP_ADAPTER_ADDRESSES, 
 *PIP_ADAPTER_ADDRESSES;

typedef struct {
    char String[4 * 4];
} IP_ADDRESS_STRING, *PIP_ADDRESS_STRING, IP_MASK_STRING, *PIP_MASK_STRING;

typedef struct _IP_ADDR_STRING {
  struct _IP_ADDR_STRING* Next;
  IP_ADDRESS_STRING IpAddress;
  IP_MASK_STRING IpMask;
  DWORD Context;
} IP_ADDR_STRING, 
 *PIP_ADDR_STRING;

typedef struct _IP_ADAPTER_INFO {
  struct _IP_ADAPTER_INFO* Next;
  DWORD ComboIndex;
  char AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
  char Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
  UINT AddressLength;
  BYTE Address[MAX_ADAPTER_ADDRESS_LENGTH];
  DWORD Index;
  UINT Type;
  UINT DhcpEnabled;
  PIP_ADDR_STRING CurrentIpAddress;
  IP_ADDR_STRING IpAddressList;
  IP_ADDR_STRING GatewayList;
  IP_ADDR_STRING DhcpServer;
  BOOL HaveWins;
  IP_ADDR_STRING PrimaryWinsServer;
  IP_ADDR_STRING SecondaryWinsServer;
  time_t LeaseObtained;
  time_t LeaseExpires;
} IP_ADAPTER_INFO, 
 *PIP_ADAPTER_INFO;

QT_END_NAMESPACE

#endif
