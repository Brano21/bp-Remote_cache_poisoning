# bp-Remote_cache_poisoning

Here you can download VMs and the resources you need to run the attack.

## Introduction
The objective of this lab is for students to gain first-hand experience on the remote DNS cache poisoning attack, also called the Kaminsky DNS attack. In the topology.png file, you can see that this lab contains 4 virtual machines. First is a router with a 10.10.30.1 IP address, second is a local DNS server (the victim) with a 10.10.30.7 IP address, third is an attacker (you) with 10.10.30.6 and last is a server-wan with 10.10.40.40 IP address. Your job is to poison the cache of the local DNS server with a false reply.

## How remote cache poisoning works
In the real world, the attacker and the local DNS server are not in the same network. The attacker, by not listening to the communication, does not know the source IP address, destination port number, and transaction ID. These three pieces of information are critical to an attacker if he wants the attack to be successful. However, two real issues need to be addressed. 
##### The first issue is timing.
In a local attack, the attacker knew how to intercept packets, so he knew exactly when the packet was sent. You can't see this attacker in a remote attack. However, this problem can be easily solved. For the attacker to know when to send fake responses, he sends a request to the local DNS server and launches an attack, and floods the local DNS server with fake responses. 
##### The second issue is the local DNS server cache. 
If the attack is not successful and the legitimate NS manages to respond, the response is cached. If the attacker continued, the attack would no longer make sense because, in the next attempt, no request will be sent to the NS of the website, but the local DNS server will take the response from its cache. The attacker would therefore have to wait for the TTL to expire for the record to become invalid. In this case, the local DNS server would have to resend the request and the attacker would have the opportunity to perform the attack but again only one attempt. If he failed, he would have to wait for the TTL to expire again, he had one try, and so on. Days may pass until the TTL expires. Therefore, if an attacker always had only one opportunity in a few days, for example, two or three, then the attack itself would have to take decades or hundreds to be successful. For this reason, a remote attack was impractical in practice.

#### Dan Kaminsky attack 
Dan came up with a very clever idea. Instead of the attacker still asking one question (stuba.sk), he asks another, for example, a.stuba.sk. Most likely, the attacker loses and the local DNS server receives a legitimate response from the real name server. If the name does not exist on the name server, the local DNS server receives a response that the name does not exist and caches this information. So a.stuba.sk will be cached, either with a real IP address or with a record that says that this name does not exist. This is fine because the attacker does not ask the same question again but now sends b.stuba.sk. If the record is also cached, the attacker can continue, c.stuba.sk, d, e, f, etc. It always asks another question, so the answer to it will not be cached. The local DNS server will have to send requests, so the attacker does not have to wait for the cached TTL entry to expire. In this attack, the attacker does not intentionally focus on the answer section. What is important is the authority section. If the attack is successful, the attacker's name server will be cached on the local DNS server as the authority for the domain. At this point, the cache is infected and the domain is hijacked by the attackers

## Tasks


## Respurces
https://seedsecuritylabs.org/Labs_16.04/PDF/DNS_Remote_new.pdf
