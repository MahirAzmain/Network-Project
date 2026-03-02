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
    gates:
        input radioIn;
        output radioOut;
}

simple IdealMac
{
    parameters:
        double transmissionDelay @unit(s) = default(0.001s);
    gates:
        input upperLayerIn;
        output upperLayerOut;
        input radioIn;
        output radioOut;
}

simple RPLNetworkLayer
{
    parameters:
        int mopType = default(0);
    gates:
        input upperLayerIn;
        output upperLayerOut;
        input lowerLayerIn;
        output lowerLayerOut;
        input rplIn;
        output rplOut;
}

simple RPLUpwardRouting
{
    parameters:
        int mopType = default(0);
    gates:
        input netIn;
        output netOut;
}

simple AppLayer
{
    parameters:
        double sendInterval @unit(s) = default(10s);
        int dataPacketSize @unit(B) = default(100B);
    gates:
        input networkIn;
        output networkOut;
}

module RPLNode
{
    parameters:
        int nodeId;
        string topology = default("grid");

    submodules:
        phy: IdealPhy;
        mac: IdealMac;
        net: RPLNetworkLayer;
        rpl: RPLUpwardRouting;
        app: AppLayer;

    connections:
        app.networkOut --> net.upperLayerIn;
        net.upperLayerOut --> app.networkIn;

        net.lowerLayerOut --> mac.upperLayerIn;
        mac.upperLayerOut --> net.lowerLayerIn;

        net.rplOut --> rpl.netIn;
        rpl.netOut --> net.rplIn;

        mac.radioOut --> phy.radioIn;
        phy.radioOut --> mac.radioIn;
}

network RPLNetwork
{
    parameters:
        int numNodes = default(7);

    submodules:
        node[numNodes]: RPLNode {
            nodeId = index;
        }

    connections allowunconnected:
}
