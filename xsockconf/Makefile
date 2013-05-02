# This file will copy the xsockconf.ini files into the right directories
XIA-CORE = ../../xia-core/bin

all: hosts.o topology.o bin.o initiate.o

hosts.o:
	cp hosts.xia ../../xia-core/etc/.

topology.o:
	cp xia_video_topology.click ../../xia-core/etc/click/.

bin.o:
	cp bin_xsockconf.ini ../../xia-core/bin/xsockconf.ini
	cp bin_xsockconf.ini ../../xia-core/xsockconf.ini
	cp bin_xsockconf.ini ./xsockconf.ini
	cp app_xsockconf.ini ../server/xsockconf.ini
	cp app_xsockconf.ini ../client/xsockconf.ini
# start the network. This will not work if click is already started
initiate.o:
	$(XIA-CORE)/xianet -s xia_video_topology.click start
	sleep 2
	$(XIA-CORE)/xrouted router0 router0 &
	$(XIA-CORE)/xrouted router1 router1 &
	$(XIA-CORE)/xrouted router2 router2 &
	$(XIA-CORE)/xrouted router3 router3 &
	sleep 2
	$(XIA-CORE)/xhcp_serverd router0 &
	$(XIA-CORE)/xhcp_serverd router1 &
	$(XIA-CORE)/xhcp_serverd router2 &
	$(XIA-CORE)/xhcp_serverd router3 &
	sleep 2
	$(XIA-CORE)/xhcp_clientd www_h.host0.com.xia host0 host0 &
	$(XIA-CORE)/xhcp_clientd www_h.host1.com.xia host1 host1 &
	$(XIA-CORE)/xhcp_clientd www_h.host2.com.xia host2 host2 &
	$(XIA-CORE)/xhcp_clientd www_h.host3.com.xia host3 host3 &
	$(XIA-CORE)/xhcp_clientd www_h.host4.com.xia host4 host4 &
	$(XIA-CORE)/xhcp_clientd www_h.host5.com.xia host5 host5 &
	$(XIA-CORE)/xhcp_clientd www_h.host6.com.xia host6 host6 &
	$(XIA-CORE)/xhcp_clientd www_h.host7.com.xia host7 host7 &
	sleep 2
	$(XIA-CORE)/xnameservice &
	sleep 2
	$(XIA-CORE)/xianet check &
	sleep 2

#	$(XIA-CORE)/click/userlevel/click -R $(XIA-CORE)/etc/click/xia_video_topology.click &
# TODO: add the client and server links once we know where they are going to be
