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

#include "VirtualNode.h"
Define_Module(VirtualNode)
void VirtualNode::setLi(std::map<int,std::vector< int >> li)
{
    this->li = li;
}
void VirtualNode::setC1(std::map<int,std::vector< int >> li)
{
    this->mapC1 = li;
}
void VirtualNode::setC2(std::map<int,std::vector< int >> li)
{
    this->mapC2 = li;
}
void VirtualNode::setC3(std::map<int,std::vector< int >> li)
{
    this->mapC3 = li;
}

std::map<int,std::vector< int >>  VirtualNode::getLi() const
{
    return this->li;
}

std::map<int,std::vector< int >>  VirtualNode::getC1() const
{
    return this->mapC1;
}

std::map<int,std::vector< int >>  VirtualNode::getC2() const
{
    return this->mapC2;
}

std::map<int,std::vector< int >>  VirtualNode::getC3() const
{
    return this->mapC3;
}
void VirtualNode::initialize()
{
    // small teste just want to display information about parameter.
    int b = getNumParams();
    for (int i=0; i<b; i++)
    {
        cPar& p = par(i);
        if( strcmp(p.getName(), "m") == 0)
        {
          this->m =  stoi(p.str());
        }
        EV << "parameter: " << p.getName() << "\n";
        EV << " type:" << cPar::getTypeName(p.getType()) << "\n";
        EV << " contains:" <<  p.str()<< "\n";

    }
        int n = gateSize("gate");
        for(int i = 0; i < (n-1); i++){
               // TicTocMsg13 *msg = generateMessage(i);
                //msg->setM(this->m);
                //send(msg, "gate$o", i);
           }

}
void VirtualNode::handleMessage(cMessage *msg)
{

}

