require(library xia_router_lib.click);
require(library xia_address.click);

// host & router instantiation
host0 :: XIAEndHost (RE AD0 HID0, HID0, 1500, 0, aa:aa:aa:aa:aa:aa);
host1 :: XIAEndHost (RE AD1 HID1, HID1, 1600, 1, aa:aa:aa:aa:aa:aa);
host2 :: XIAEndHost (RE AD2 HID2, HID2, 1700, 2, aa:aa:aa:aa:aa:aa);
host3 :: XIAEndHost (RE AD3 HID3, HID3, 1800, 3, aa:aa:aa:aa:aa:aa);
host4 :: XIAEndHost (RE AD0 HID4, HID4, 1900, 4, aa:aa:aa:aa:aa:aa);
host5 :: XIAEndHost (RE AD1 HID5, HID5, 2000, 5, aa:aa:aa:aa:aa:aa);
host6 :: XIAEndHost (RE AD2 HID6, HID6, 2100, 6, aa:aa:aa:aa:aa:aa);
host7 :: XIAEndHost (RE AD3 HID7, HID7, 2200, 7, aa:aa:aa:aa:aa:aa);

router0 :: XIARouter4Port(RE AD0 RHID0, AD0, RHID0, 0.0.0.0, 2300, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa);
router1 :: XIARouter4Port(RE AD1 RHID1, AD1, RHID1, 0.0.0.0, 2400, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa);
router2 :: XIARouter4Port(RE AD2 RHID2, AD2, RHID2, 0.0.0.0, 2500, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa);
router3 :: XIARouter4Port(RE AD3 RHID3, AD3, RHID3, 0.0.0.0, 2600, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa, aa:aa:aa:aa:aa:aa);

// interconnection -- host - ad
host0[0] -> LinkUnqueue(0.005, 1 GB/s) -> [0]router0;
router0[0] -> LinkUnqueue(0.005, 1 GB/s) -> [0]host0;

host1[0] -> LinkUnqueue(0.005, 1 GB/s) -> [0]router1;
router1[0] -> LinkUnqueue(0.005, 1 GB/s) ->[0]host1;

host2[0] -> LinkUnqueue(0.005, 1 GB/s) -> [0]router2;
router2[0] -> LinkUnqueue(0.005, 1 GB/s) ->[0]host2;

host3[0] -> LinkUnqueue(0.005, 1 GB/s) -> [0]router3;
router3[0] -> LinkUnqueue(0.005, 1 GB/s) ->[0]host3;

host4[0] -> LinkUnqueue(0.005, 1 GB/s) -> [3]router0;
router0[3] -> LinkUnqueue(0.005, 1 GB/s) ->[0]host4;

host5[0] -> LinkUnqueue(0.005, 1 GB/s) -> [3]router1;
router1[3] -> LinkUnqueue(0.005, 1 GB/s) ->[0]host5;

host6[0] -> LinkUnqueue(0.005, 1 GB/s) -> [3]router2;
router2[3] -> LinkUnqueue(0.005, 1 GB/s) ->[0]host6;

host7[0] -> LinkUnqueue(0.005, 1 GB/s) -> [3]router3;
router3[3] -> LinkUnqueue(0.005, 1 GB/s) ->[0]host7;
// interconnection -- ad - ad
router0[1] -> LinkUnqueue(0.005, 1 GB/s) ->[1]router1;
router0[2] -> LinkUnqueue(0.005, 1 GB/s) ->[1]router3;
router1[1] -> LinkUnqueue(0.005, 1 GB/s) ->[1]router0;
router1[2] -> LinkUnqueue(0.005, 1 GB/s) ->[1]router2;
router2[1] -> LinkUnqueue(0.005, 1 GB/s) ->[2]router1;
router2[2] -> LinkUnqueue(0.005, 1 GB/s) ->[2]router3;
router3[1] -> LinkUnqueue(0.005, 1 GB/s) ->[2]router0;
router3[2] -> LinkUnqueue(0.005, 1 GB/s) ->[2]router2;

ControlSocket(tcp, 7777);

//	H3 H7
//	|  |
//	R3 | 
//	| -----\
//	AD3     ---\---
//	|              \
//	AD0----AD1-----AD2
//	|	|	|   
//	R0	R1 	R2  
//	|  |	|  |	|  |
//	H0 H4	H1 H5	H2 H6

