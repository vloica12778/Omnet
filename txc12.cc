//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003 Ahmet Sekercioglu
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <stdio.h>
#include <string.h>
#include <vector>
#include <omnetpp.h>
#include <iostream>
#include <iterator>
#include <map>
#include <algorithm>
#include <math.h>       /* ceil */
using namespace omnetpp;

/**
 * Let's make it more interesting by using several (n) `tic' modules,
 * and connecting every module to every other. For now, let's keep it
 * simple what they do: module 0 generates a message, and the others
 * keep tossing it around in random directions until it arrives at
 * module 2.
 */
// Include a generated file: the header file created from tictoc13.msg.
// It contains the definition of the TictocMsg10 class, derived from
// cMessage.
#include "tictoc13_m.h"
#include "VirtualNode.h"

class Txc12 : public cSimpleModule
{
private:
    std::vector< int > list;
    std::vector< int > c1;
    std::vector< int > c2;
    std::vector< int > c3;
    std::vector< int > c;
    std::map<int,std::vector< int >> li;
    std::map<int,std::vector< int >> mapC1;
    std::map<int,std::vector< int >> mapC2;
    std::map<int,std::vector< int >> mapC3;
    std::map<int,VirtualNode*> lv;
    int m = 10;
    int mv = 0;
    int t = 1;
    int totalNeed=0;
    simsignal_t arrivalSignal;
    simsignal_t arrivalSignal1;
    simsignal_t arrivalSignal2;
    simsignal_t arrivalSignal3;
  protected:
    virtual TicTocMsg13 *generateMessage(int dest);
    virtual TicTocMsg13 *generateMessageList(int dest, std::string type,std::vector<int> list);
    virtual void forwardMessage(TicTocMsg13 *msg);
    virtual void sendListNeigbor(std::string type,std::vector<int> list);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int intersection(std::vector<int > &v1,std::vector<int> &v2);
    virtual std::vector<int >  intersectionL(std::vector<int > &v1,std::vector<int> &v2);
    virtual std::vector<int >  compareTwoList(std::vector<int > &v1,std::vector<int> &v2);
    virtual std::vector< int > calculIntersection();
    virtual void createvirtualNode();
    virtual void calculC1();
    virtual void calculC2();
    virtual void calculC3();
    virtual void verification();
    virtual void ended();
    virtual void finish() override;
};

Define_Module(Txc12);
template<typename A, typename B>
std::pair<B,A> flip_pair(const std::pair<A,B> &p)
{
    return std::pair<B,A>(p.second, p.first);
}

template<typename A, typename B>
std::multimap<B,A> flip_map(const std::map<A,B> &src)
{
    std::multimap<B,A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()),
                   flip_pair<A,B>);
    return dst;
}

void Txc12::initialize()
{
    arrivalSignal = registerSignal("arrival");
    arrivalSignal1 = registerSignal("arrival1");
    arrivalSignal2 = registerSignal("arrival2");
    arrivalSignal3 = registerSignal("arrival3");
    // small teste just want to display information about parameter.
    int b = getNumParams();
    for (int i=0; i<b; i++)
    {
        cPar& p = par(i);
        if( strcmp(p.getName(), "m") == 0)
        {
          this->m =  stoi(p.str());
          emit(arrivalSignal3, m);
        }
        EV << "parameter: " << p.getName() << "\n";
        EV << " type:" << cPar::getTypeName(p.getType()) << "\n";
        EV << " contains:" <<  p.str()<< "\n";

    }
        int n = gateSize("gate");
        for(int i = 0; i < (n-1); i++){
                TicTocMsg13 *msg = generateMessage(i);
                msg->setM(this->m);
                send(msg, "gate$o", i);
           }

}

void Txc12::handleMessage(cMessage *msg)
{
        if(!msg->isSelfMessage()){
            TicTocMsg13 *ttmsg = check_and_cast<TicTocMsg13 *>(msg);
          //bubble("ARRIVED, starting new one!");
            EV << "message type \t" << ttmsg->getType() << '\n';
          if(ttmsg->getType() == "no"){
              if(!std::count(list.begin(), list.end(), msg->getSenderModule()->getIndex())){
                  list.push_back(msg->getSenderModule()->getIndex());
                  EV << "memory get by user " << ttmsg->getM() << '\n';
                  this->mv = this->mv + ttmsg->getM();// count the memory send by neighbor.
              }
              if(gateSize("gate")/2 == list.size()){
                   totalNeed = list.size() * t;

                  if(totalNeed > m){
                      emit(arrivalSignal2, totalNeed);
                      EV << "Liste de voisins noeud " << getIndex();
                      createvirtualNode();
                  }

                   if(totalNeed <m){
                      emit(arrivalSignal1, m-totalNeed);
                   }

                   for(int i = 0; i < list.size(); i++)
                            {
                                EV  << '\t' << list[i];
                            }
                   EV << "size " << list.size();
                   sendListNeigbor("li", list);
               }
          }
          else if(ttmsg->getType() == "li"){
                  auto j = this->li.find(msg->getSenderModule()->getIndex());
                  if(j== li.end())
                  li.insert({msg->getSenderModule()->getIndex(), ttmsg->getList()});


              if(gateSize("gate")/2 == li.size()){
                  if(this->c1.empty()){
                      EV << "List  " << li.size();
                      calculC1();
                      sendListNeigbor("c1",this->c1);
                  }

             }
          }
          else if(ttmsg->getType() == "c1")
          {  auto j = this->mapC1.find(msg->getSenderModule()->getIndex());
          if(j== mapC1.end())
              mapC1.insert({msg->getSenderModule()->getIndex(), ttmsg->getList()});
                if(gateSize("gate")/2 == mapC1.size()){
                    if(this->c2.empty()){
                        EV << "Map size " << mapC1.size();
                                           calculC2();
                                           sendListNeigbor("c2",this->c2);

                    }

               }
          }
          else if(ttmsg->getType() == "c2")
            {auto j = this->mapC2.find(msg->getSenderModule()->getIndex());
            if(j== mapC2.end())
                mapC2.insert({msg->getSenderModule()->getIndex(), ttmsg->getList()});
                  if(gateSize("gate")/2 == mapC2.size()){
                      if(this->c3.empty()){
                      EV << "Map size " << mapC2.size();
                      calculC3();
                      sendListNeigbor("c3",this->c3);
                      }
                 }
            }
          else if(ttmsg->getType() == "c3")
                      {
              auto j = this->mapC3.find(msg->getSenderModule()->getIndex());
                        if(j== mapC3.end())
                          mapC3.insert({msg->getSenderModule()->getIndex(), ttmsg->getList()});
                            if(gateSize("gate")/2 == mapC3.size()){

                                EV << "Verification";
                                verification();

                           }
                      }
        }
}

void Txc12::forwardMessage(TicTocMsg13 *msg)
{
    // In this example, we just pick a random gate to send it on.
    // We draw a random number between 0 and the size of gate `gate[]'.
    int n = gateSize("gate");
    int k = intuniform(0, n-1);

    EV << "Forwarding message " << msg << " on gate[" << k << "]\n";
    // $o and $i suffix is used to identify the input/output part of a two way gate
    for(int i = 0; i < (n-1); i++){
        send(msg, "gate$o", i);
    }
}

TicTocMsg13 *Txc12::generateMessage(int dest)
{
    // Produce source and destination addresses.
    int src = getIndex();  // our module index
    int n = getVectorSize();  // module vector size


    char msgname[20];
    //sprintf(msgname, "tic-%d-to-%d", src, dest);

    // Create message object and set source and destination field.
    TicTocMsg13 *msg = new TicTocMsg13(msgname);
    msg->setSource(src);
    msg->setDestination(dest);
    return msg;
}
TicTocMsg13 *Txc12::generateMessageList(int dest,std::string type,std::vector<int> list)
{
    // Produce source and destination addresses.
    int src = getIndex();  // our module index
    int n = getVectorSize();  // module vector size


    char msgname[20];
 //   sprintf(msgname, "tic-%d-to-%d", src, dest);

    // Create message object and set source and destination field.
    TicTocMsg13 *msg = new TicTocMsg13(msgname);
    msg->setSource(src);
    msg->setDestination(dest);
    msg->setType(type);
    msg->setList(list);
    return msg;
}

void Txc12::sendListNeigbor(std::string type,std::vector<int> list){
    int n = gateSize("gate");
        for(int i = 0; i < n; i++){
            TicTocMsg13 *msg = generateMessageList(i, type, list);
            send(msg, "gate$o", i);
        }
}

std::vector< int > Txc12::calculIntersection()
 {
     std::vector< int > currentList;
     std::vector< int > result;
     std::map<int,int> si;
     for (auto itr = li.begin(); itr != li.end(); ++itr) {
        EV << itr->first << '\t' << itr->second.size() << '\n';
        currentList = itr->second;
        si.insert({itr->first,intersection(currentList, this->list)});
     }
     std::multimap<int, int> dst = flip_map(si);
     for (auto itr = li.begin(); itr != li.end(); ++itr ) {
             EV << itr->first << '\t' << itr->second.size() << '\n';
             result.push_back(itr->first);
     }
     std::reverse(result.begin(), result.end());
     return result;
 }
 void Txc12::calculC1()
 {
     this->c1.push_back(getIndex());
     std::vector< int > si = calculIntersection();

     EV << "Intsection  Node " << getIndex() << " :" ;
          for(int i = 0; i < si.size(); i++)
          {
              EV << '\t' << si[i] ;
          }




     std::vector< int > sci = this->list;
     EV << "START CALCUL C1" << '\t' << si.size() << '\n';
     while(si.size() > 0)
     {
         int k = si[0];
         EV << "value of k" << k;

         this->c1.push_back(k);
         si.erase(si.begin());
         auto j = this->li.find(k);
         EV << "Node v " << j->first << '\t' << j->second.size() << '\n';
         sci = intersectionL(sci, j->second);
         for(int i = 0; i < si.size(); i++){
             if(!std::count(j->second.begin(), j->second.end(), si[i])){
                 auto it = find(si.begin(), si.end(), si[i]);
                         si.erase(it);
             }
         }
     }
     EV << "c1 value Node " << getIndex() << " :" ;
     for(int i = 0; i < c1.size(); i++)
     {
         EV << '\t' << c1[i] ;
     }

 }
 void Txc12::calculC2(){
     std::map<int,std::vector<int >> si;
     this->c2 = this->c1;
     for (auto itr = mapC1.begin(); itr != mapC1.end(); ++itr) {
         int id = getIndex();
         if(std::count(itr->second.begin(), itr->second.end(), id)){
             si.insert({itr->first,itr->second});
         }
      }
     EV <<"si size "  << si.size();
         for (auto itr = si.begin(); itr != si.end(); ++itr) {
                this->c2 = compareTwoList(c2, itr->second);
             }

     EV << " C2 value Noeud " <<getIndex() << " :";
     for(int i = 0; i < c2.size(); i++)
         {
             EV << '\t' << c2[i];
         }
 }
 void Txc12::calculC3(){
     std::map<int,std::vector<int >> si;
     this->c3 = this->c2;
     int id = getIndex();
     for (auto itr = mapC2.begin(); itr != mapC2.end(); ++itr) {
         if(std::count(this->c3.begin(), this->c3.end(), itr->first)){
             si.insert({itr->first,itr->second});
         }
      }

     for (auto itr = si.begin(); itr != si.end(); ++itr) {
         if(!std::count(itr->second.begin(), itr->second.end(), id)){
              auto it = find(this->c3.begin(), this->c3.end(), itr->first);
                    if (it != this->c3.end())
                                  {
                                      this->c3.erase(it);
                                  }
          }
     }
     EV << " C3 value Node "  <<getIndex() << " :";
     for(int i = 0; i < c3.size(); i++)
         {
             EV << '\t' << c3[i] ;
         }
 }

 void Txc12::verification()
{
     std::string me ="error";
     if(this->c3.size() == 1){
         me = "success";
     }
     for (auto itr = this->mapC3.begin(); itr != this->mapC3.end(); ++itr) {
             if(std::count(itr->second.begin(), itr->second.end(), getIndex())){
                if (std::count(this->c3.begin(), this->c3.end(), itr->first))
                 {
                    me = "success";
                }
              }
         }

     EV << "Verification C3 value  "  << me  << '\n';

     if(me == "success"){
            ended();
        }
}

 int Txc12::intersection(std::vector<int > &v1,
                                       std::vector<int> &v2){
     std::vector<int>::iterator v3;

     std::sort(v1.begin(), v1.end());
     std::sort(v2.begin(), v2.end());

     v3 = std::set_intersection(v1.begin(),v1.end(),
                           v2.begin(),v2.end(),
                           v1.begin());

             //EV<< "The intersection has " << (v3 - v1.begin()) << " elements:";

     return (v3 - v1.begin());
 }

 std::vector<int> Txc12::intersectionL(std::vector<int > &v1,
                                       std::vector<int> &v2){
     std::vector<int> v4;
     std::vector<int>::iterator v3;
     std::vector<int>::iterator it;
     std::sort(v1.begin(), v1.end());
     std::sort(v2.begin(), v2.end());

     v3= std::set_intersection(v1.begin(),v1.end(),
                           v2.begin(),v2.end(),
                           v1.begin());
     for (it = v1.begin(); it != v3; ++it)
           v4.push_back(*it);
     return v4;
 }

 std::vector<int >  Txc12::compareTwoList(std::vector<int > &v1,std::vector<int> &v2){
         if(v1.size() > v2.size())
         {
             return v1;
         }
         else if(v1.size() < v2.size())
         {
             return v2;
         }
         else if(v1.size() == v2.size())
         {
             /*for (const auto &i: v1)
                   sort(v1.begin(), v1.end());
             for (const auto &i: v2)
                 sort(v2.begin(), v2.end());*/
             for(int i = 0; i < v1.size(); i++)
             {
                 if(v1[i] > v2[i])
                 {
                     return v1;
                 }
                 else if(v1[i] < v2[i]){
                     return v2;
                 }
             }
         }else{
             return v1;
         }
         return v1;
 }
 // this function is use to create virtual Node in the netWord;
 void Txc12::createvirtualNode()
 {
     int numberofvirtualNode = ceil(totalNeed/m);
     int MemoryOfNodeVirtual = totalNeed;
            // send a signal
          emit(arrivalSignal, numberofvirtualNode);
          //emit(arrivalSignal,MemoryOfNodeVirtual);
     EV << "\n TOTAL NUMBER OF VIRTUAL NODE %.1f "<< numberofvirtualNode << '\n';
     //this->addGate("gater", cGate::INOUT, true);
     this->setGateSize("gater", gateSize("gater")+numberofvirtualNode);
     for(int i = 0; i < numberofvirtualNode; i++)
                 {
         cModuleType *moduleType = cModuleType::get("VirtualNode");
         std::string nodename = "v_p_"+ std::to_string(getId())+"_"+std::to_string(i);
              cModule *a = moduleType->create(nodename.data(), this);
              a->par("m") = 10;
              a->finalizeParameters();
              //a->setGateSize("in", 3);
              //a->setGateSize("out", 3);
              // create internals, and schedule it
              a->buildInside();
              a->scheduleStart(simTime());

              cDatarateChannel *channel = cDatarateChannel::create("channel");
              channel->setDelay(0.001);
              //getSystemModule()->addGate("gate++", cGate::INOUT);
              //std::string gaterNameString = "gater"
              a->addGate("gater", cGate::INOUT, true);
              a->setGateSize("gater", 1);

              cDisplayString& displayString = a->getDisplayString();
              displayString.setTagArg("i", 0, "device/pc3");
              displayString.setTagArg("p", 0, 40+i+intuniform(10, 100));
              displayString.setTagArg("p", 1, 20+i);
              //this->setGateSize("gate", gateSize("gate")+1);
              a->gate("gater$o",a->gateSize("gater")-1)->connectTo(this->gate("gater$i",gateSize("gater$i")-i-1));
              this->gate("gater$o",this->gateSize("gater$o")-i-1)->connectTo(a->gate("gater$i",a->gateSize("gater$i")-1));
              a->callInitialize();
                 }

 }


 void Txc12::ended(){
     EV << "Final Noeud " << getIndex() << " :";
         for(int i = 0; i < c3.size(); i++)
             {
                 EV << '\t' << c2[i];
             }
 }

 void Txc12::finish()
 {
     EV << "timer \t" << simTime();
 }
