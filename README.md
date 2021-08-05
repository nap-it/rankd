<img src="docs/logo/long.png"  width="500" height="102">
<a href="https://code.nap.av.it.pt/ruieduardo.fa.lopes/rankd/-/wikis/home" rel="some text"><img src="https://img.shields.io/badge/wiki-repo%20wiki-blue"></a>
<img src="https://img.shields.io/badge/build-not%20configured-inactive">
<img src="https://img.shields.io/badge/docs-not%20published-inactive">
<img src="https://img.shields.io/badge/version-0.1.0%20(alpha)-green">

# Rankd

Rank deamon (Rankd) is an implementation of a service to retrieve information on a machine where a server instance is being ran. Such information ranges from kernel data to userspace applications being used and network capabilities.

This service has the goal to rank a machine against others, in order to allow the creation and allocation of proper paths (path reservation) after a capability negotiation, as required for a certain task to be performed under a given latency- and jitter-strict usage.

## Getting Started with Rankd

Rankd (to be pronounced _rank-ed_) while it is being developed, has a first goal to allow clients to retrieve status data from a server instance. Almost like a network management agent (such as an SNMP agent), a Rankd machine can give information about their kernel (such as version, operative system, processor precision and family, among others), and about their userspace items (such as applications being ran, by genre, or even some network capabilities).

As this is still under development, currently there is no release available, but it is expected to launch a version soon enough to be used in a set of equipments of Aveiro City Living Lab's infrastructure, in order to be able to coordinate maintenance actions with no further ado.

### Running Rankd

In order to run this project you simply have to run the compiled executable called `rankd` and, if everything runs well, you should receive a feedback on a server waiting for requests in the port 7265 (T9-code for `r-a-n-k`).
