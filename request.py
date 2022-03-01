#!/usr/bin/python3
from scapy.all import *

Qdsec = DNSQR(qname='aaaaa.example.com')
dns = DNS(id=0xAAAA, qr=0, qdcount=1, ancount=0, nscount=0,
arcount=0, qd=Qdsec)
ip = IP(dst='10.10.30.7', src='10.10.30.6')
udp = UDP(dport=53, sport=9999, chksum=0)
request = ip/udp/dns
with open ('ip_req.bin', 'wb') as f:
    f.write(bytes(request))