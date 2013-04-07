# This file will copy the xsockconf.ini files into the right directories
XIA-CORE = ../../xia-core

all: xhcp.o xrouted.o topology.o initiate.o

xhcp.o:
	cp xhcp_xsockconf.ini ../../xia-core/daemons/xhcp/xsockconf.ini

xrouted.o: 
	cp xrouted_xsockconf.ini ../../xia-core/daemons/xrouted/xsockconf.ini

topology.o:
	cp xia_video_topology.click ../../xia-core/etc/click/.

# start the network. This will not work if click is already started
initiate.o:
	$(XIA-CORE)/bin/xianet -s xia_video_topology.click start
	

# TODO: add the client and server links once we know where they are going to be
