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
	sleep 4
	$(XIA-CORE)/bin/xrouted router0 router0 &
	$(XIA-CORE)/bin/xrouted router1 router1 &
	$(XIA-CORE)/bin/xrouted router2 router2 &
	$(XIA-CORE)/bin/xrouted router3 router3 &
	sleep 2
	$(XIA-CORE)/bin/xhcp_serverd router0 &
	$(XIA-CORE)/bin/xhcp_serverd router1 &
	$(XIA-CORE)/bin/xhcp_serverd router2 &
	$(XIA-CORE)/bin/xhcp_serverd router3 &
	sleep 2
	$(XIA-CORE)/bin/xhcp_clientd www_h.host0.com.xia host0 host0 &
	$(XIA-CORE)/bin/xhcp_clientd www_h.host1.com.xia host1 host1 &
	$(XIA-CORE)/bin/xhcp_clientd www_h.host2.com.xia host2 host2 &
	$(XIA-CORE)/bin/xhcp_clientd www_h.host3.com.xia host3 host3 &
	$(XIA-CORE)/bin/xhcp_clientd www_h.host4.com.xia host4 host4 &
	$(XIA-CORE)/bin/xhcp_clientd www_h.host5.com.xia host5 host5 &
	sleep 2
	$(XIA-CORE)/bin/xnameservice &
	sleep 2
	$(XIA-CORE)/bin/xianet check &
	sleep 2

#	$(XIA-CORE)/click/userlevel/click -R $(XIA-CORE)/etc/click/xia_video_topology.click &
# TODO: add the client and server links once we know where they are going to be
