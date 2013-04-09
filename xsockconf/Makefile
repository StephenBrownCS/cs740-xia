# This file will copy the xsockconf.ini files into the right directories
XIA-CORE = ../../xia-core

all: xhcp.o xrouted.o topology.o bin.o initiate.o

xhcp.o:
	cp xhcp_xsockconf.ini ../../xia-core/daemons/xhcp/xsockconf.ini

xrouted.o: 
	cp xrouted_xsockconf.ini ../../xia-core/daemons/xrouted/xsockconf.ini

topology.o:
	cp xia_video_topology.click ../../xia-core/etc/click/.

bin.o:
	cp bin_xsockconf.ini ../../xia-core/bin/xsockconf.ini

# start the network. This will not work if click is already started
initiate.o:
	$(XIA-CORE)/bin/xianet -s xia_video_topology.click start
	$(XIA-CORE)/bin/xrouted router0 xroute_r0 &
	$(XIA-CORE)/bin/xrouted router1 xroute_r1 &
	$(XIA-CORE)/bin/xrouted router2 xroute_r2 &
	$(XIA-CORE)/bin/xrouted router3 xroute_r3 &
	sleep 2
	$(XIA-CORE)/bin/xhcp_serverd xhcp_server_r0 &
	$(XIA-CORE)/bin/xhcp_serverd xhcp_server_r1 &
	$(XIA-CORE)/bin/xhcp_serverd xhcp_server_r2 &
	$(XIA-CORE)/bin/xhcp_serverd xhcp_server_r3 &
	sleep 2
	$(XIA-CORE)/bin/xhcp_clientd www_h.host0.com.xia host0 xhcp_client_h0 &
	$(XIA-CORE)/bin/xhcp_clientd www_h.host1.com.xia host1 xhcp_client_h1 &
	$(XIA-CORE)/bin/xhcp_clientd www_h.host2.com.xia host2 xhcp_client_h2 &
	$(XIA-CORE)/bin/xhcp_clientd www_h.host3.com.xia host3 xhcp_client_h3 &
	sleep 2
	$(XIA-CORE)/bin/xnameservice &
	sleep 2
	$(XIA-CORE)/bin/xianet check &
	sleep 2

# TODO: add the client and server links once we know where they are going to be
