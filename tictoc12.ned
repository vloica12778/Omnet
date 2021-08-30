//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

simple Txc12
{
    parameters:
        @display("i=block/routing");
        @signal[arrival](type="long");
        @statistic[virtualNodeLength](title="Number of virtual Node"; source="arrival"; record=vector,stats; interpolationmode=none);
        @signal[arrival1](type="long");
        @statistic[RestOfMemory](title="Rest Of Memory"; source="arrival1"; record=vector,stats; interpolationmode=none);
        @signal[arrival2](type="long");
        @statistic[UseMemory](title="MemoryUse"; source="arrival2"; record=vector,stats; interpolationmode=none);
         @signal[arrival3](type="long");
        @statistic[NodeMemory](title="NodeMemory"; source="arrival3"; record=vector,stats; interpolationmode=none);
      
        int m;
    gates:
        inout gate[];  // declare two way connections
        inout gater[]; // virtual connections
}

simple VirtualNode
{
 	parameters:
 	    @display("i=device/clock");
 	    int m;
 	    int vr;
 	gates:
 	    inout gate[];
        
}
// using two way connections to further simplify the network definition
network Tictoc12
{
    parameters:
        int numberofMode;
        double connectedness;
    types:
        channel Channel extends ned.DelayChannel {
           delay = 1ns;
        }
    submodules:
        tic[numberofMode]: Txc12;
    connections:
    for i=0..numberofMode-1, for j=i..numberofMode-1,if i!=j && uniform(0,1)<connectedness {
    		tic[i].gate++ <--> Channel <--> tic[j].gate++;
              
        }
       
        
}