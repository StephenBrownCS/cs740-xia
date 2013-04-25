require(library xia_router_lib.click);
require(library xia_address.click);

// host & router instantiation
host0 :: XIAEndHost (RE AD0 HID0, HID0, 1500, 0, aa:aa:aa:aa:aa:aa);
host1 :: XIAEndHost (RE AD1 HID1, HID1, 1600, 1, aa:aa:aa:aa:aa:aa);
host2 :: XIAEndHost (RE AD2 HID2, HID2, 1700, 2, aa:aa:aa:aa:aa:aa);
host3 :: XIAEndHost (RE AD3 HID3, HID3, 1800, 3, aa:aa:aa:aa:aa:aa);

router0 :: XIARouter2Port(RE AD0 RHID0, AD0, RHID0, 0.0.0.0, 1900, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa);
router1 :: XIARouter2Port(RE AD1 RHID1, AD1, RHID1, 0.0.0.0, 2000, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa);
router2 :: XIARouter2Port(RE AD2 RHID2, AD2, RHID2, 0.0.0.0, 2100, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa);
router3 :: XIARouter2Port(RE AD3 RHID3, AD3, RHID3, 0.0.0.0, 2200, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa);

// interconnection -- host - ad
host0[0] -> LinkUnqueue(0.005, 1 GB/s) -> [0]router0;
router0[0] -> LinkUnqueue(0.005, 1 GB/s) -> [0]host0;

host1[0] -> LinkUnqueue(0.005, 1 GB/s) -> [0]router1;
router1[0] -> LinkUnqueue(0.005, 1 GB/s) ->[0]host1;

host2[0] -> LinkUnqueue(0.005, 1 GB/s) -> [0]router2;
router2[0] -> LinkUnqueue(0.005, 1 GB/s) ->[0]host2;

host3[0] -> LinkUnqueue(0.005, 1 GB/s) -> [0]router3;
router3[0] -> LinkUnqueue(0.005, 1 GB/s) ->[0]host3;

// interconnection -- ad - ad
router0[1] -> LinkUnqueue(0.005, 1 GB/s) ->[1]router1;
router0[1] -> LinkUnqueue(0.005, 1 GB/s) ->[1]router3;
router1[1] -> LinkUnqueue(0.005, 1 GB/s) ->[1]router0;
router1[1] -> LinkUnqueue(0.005, 1 GB/s) ->[1]router2;
router2[1] -> LinkUnqueue(0.005, 1 GB/s) ->[1]router1;
router3[1] -> LinkUnqueue(0.005, 1 GB/s) ->[1]router0;

ControlSocket(tcp, 7777);

//	H3
//	|
//	R3
//	|
//	AD3
//	|
//	AD0----AD1-----AD2
//	|	|	|
//	R0	R1	R2
//	|	|	|
//	H0	H1	H2

