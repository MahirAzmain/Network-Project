//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
package rpl;

simple IdealPhy
{
    parameters:
        double transmissionRange @unit(m) = default(100m);
        @display("i=device/antennatower;is=vl;b=120,80,rect,gold,gold,4");
    gates:
        input radioIn @labels(Radio);
        output radioOut @labels(Radio);
}

simple IdealMac
{
    parameters:
        double transmissionDelay @unit(s) = default(0.001s);
        @display("i=block/layer;is=vl;b=120,80,rect,cyan,cyan,4");
    gates:
        input upperLayerIn @labels(Packet);
        output upperLayerOut @labels(Packet);
        input radioIn @labels(Frame);
        output radioOut @labels(Frame);
}

simple RPLNetworkLayer
{
    parameters:
        int mopType = default(0);
        @display("i=block/network2;is=vl;b=140,80,rect,green,green,4");
    gates:
        input upperLayerIn @labels(Data);
        output upperLayerOut @labels(Data);
        input lowerLayerIn @labels(Packet);
        output lowerLayerOut @labels(Packet);
        input rplIn @labels(Control);
        output rplOut @labels(Control);
        input ndIn @labels(Discovery);
        output ndOut @labels(Discovery);
}

simple IPv6NeighbourDiscovery
{
    parameters:
        double sendInterval @unit(s) = default(5s);
        @display("i=block/network;is=vl;b=140,80,rect,purple,purple,4");
    gates:
        input networkIn @labels(IPv6Packet);
        output networkOut @labels(IPv6Packet);
}

simple RPLUpwardRouting
{
    parameters:
        int mopType = default(0);
        @display("i=block/routing;is=vl;b=120,80,rect,red,red,4");
    gates:
        input netIn @labels(DIO/DIS/DAO);
        output netOut @labels(DIO/DIS/DAO);
}

simple AppLayer
{
    parameters:
        double sendInterval @unit(s) = default(10s);
        int dataPacketSize @unit(B) = default(100B);
        @display("i=block/app;is=vl;b=140,80,rect,blue,blue,4");
    gates:
        input networkIn @labels(Data);
        output networkOut @labels(Data);
}

module RPLNode
{
    parameters:
        int nodeId;
        bool isRoot = (nodeId == 0);

        @display("bgb=550,650;bgs=15,s");

    submodules:
        app: AppLayer {
            @display("p=275,100;is=vl;t=APPLICATION\nDATA,t,blue,22");
        }
        net: RPLNetworkLayer {
            @display("p=275,210;is=vl;t=NETWORK\nIPv6,t,green,22");
        }
        nd: IPv6NeighbourDiscovery {
            @display("p=410,210;is=vl;t=NEIGHBOUR\nDISCOVERY,t,purple,20");
        }
        rpl: RPLUpwardRouting {
            @display("p=540,210;is=vl;t=RPL\nCONTROL,t,red,22");
        }
        mac: IdealMac {
            @display("p=275,400;is=vl;t=MAC\nLINK,t,cyan,22");
        }
        phy: IdealPhy {
            @display("p=275,540;is=vl;t=PHYSICAL\nRADIO,t,gold,22");
        }

    connections:
        app.networkOut --> { @display("ls=blue,6,s"); } --> net.upperLayerIn;
        net.upperLayerOut --> { @display("ls=blue,6,s"); } --> app.networkIn;
        net.ndOut --> { @display("ls=purple,5,s"); } --> nd.networkIn;
        nd.networkOut --> { @display("ls=purple,5,s"); } --> net.ndIn;
        net.lowerLayerOut --> { @display("ls=green,5,s"); } --> mac.upperLayerIn;
        mac.upperLayerOut --> { @display("ls=green,5,s"); } --> net.lowerLayerIn;
        net.rplOut --> { @display("ls=red,5,da"); } --> rpl.netIn;
        rpl.netOut --> { @display("ls=red,5,da"); } --> net.rplIn;
        mac.radioOut --> { @display("ls=orange,5,s"); } --> phy.radioIn;
        phy.radioOut --> { @display("ls=orange,5,s"); } --> mac.radioIn;
}

network RPLNetwork
{
    parameters:
        int numNodes = default(7);
        @display("bgb=1800,1200;bgi=background/white;bgs=25,s");

    submodules:
        root: RPLNode {
            nodeId = 0;
            @display("p=900,600;i=block/broadcast;b=280,200,rect,black,blue,10;t=GATEWAY\nROOT,t,black,45");
        }
        laptop1: RPLNode { nodeId = 1; @display("p=1000,250;i=device/wifilaptop,blue,90;t=LAPTOP-1,t,blue,32"); }
        desktop1: RPLNode { nodeId = 2; @display("p=1203,775;i=device/pc,green,90;t=DESKTOP-1,t,green,32"); }
        phone1: RPLNode { nodeId = 3; @display("p=597,775;i=device/cellphone,orange,90;t=PHONE-1,t,orange,32"); }
        laptop2: RPLNode { nodeId = 4; @display("p=1333,383;i=device/wifilaptop,purple,85;t=LAPTOP-2,t,purple,30"); }
        desktop2: RPLNode { nodeId = 5; @display("p=900,1000;i=device/pc,cyan,85;t=Desktop-2,t,cyan,30"); }
        phone2: RPLNode { nodeId = 6; @display("p=467,383;i=device/cellphone,red,85;t=PHONE-2,t,red,30"); }

    connections allowunconnected:
}
