#include <iostream>
#include <sstream>
#include <bitset>
#include <queue>
#include <stack>
#include <list>
#include <utility>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <arpa/inet.h>

#include "./include/radix.h"
#include "./include/util.h"

string bitset_to_readable_addr_str(bitset<32> addr, int plen)
{
    if(plen == 0){
        cout << "any ";
        string a = "any";
        return a;
    }

    bitset<32> res[4];
    bitset<32> tmp(0b11111111);

    res[0] =  (addr >> 24) & tmp;
    res[1] =  (addr >> 16) & tmp;
    res[2] =  (addr >> 8) & tmp;
    res[3] =   addr & tmp;
    //res[1] = (addr >> 16) & 0b11111111;
    //res[2] = (addr >> 8) & 0b11111111;
    //res[1] = addr & tmp;

    //cout << res[0].to_ulong() << "." << res[1].to_ulong() << "." << res[2].to_ulong() << "." << res[3].to_ulong() << "/" << plen << " : ";

    string a = to_string(res[0].to_ulong()) + "." + to_string(res[1].to_ulong()) + "." + to_string(res[2].to_ulong()) + "." + to_string(res[3].to_ulong()) + "/" + to_string(plen);

    return a;
}

void bitset_to_readable_addr(bitset<32> addr, int plen)
{
    if(plen == 0){
        cout << "any ";
    }

    bitset<32> res[4];
    bitset<32> tmp(0b11111111);

    res[0] =  (addr >> 24) & tmp;
    res[1] =  (addr >> 16) & tmp;
    res[2] =  (addr >> 8) & tmp;
    res[3] =   addr & tmp;
    //res[1] = (addr >> 16) & 0b11111111;
    //res[2] = (addr >> 8) & 0b11111111;
    //res[1] = addr & tmp;

    cout << res[0].to_ulong() << "." << res[1].to_ulong() << "." << res[2].to_ulong() << "." << res[3].to_ulong() << "/" << plen << " : ";
}

void print_header_constraint(struct header_constraint *hc)
{
    cout << "src addr: "; 
    bitset_to_readable_addr(hc->src_addr, hc->src_prefixlen);
     cout << "| dst addr: "; 
    bitset_to_readable_addr(hc->dst_addr, hc->dst_prefixlen);
    //port here !!!
    cout << "| src port: ";
    cout << hc->src_port.range.first << "-" << hc->src_port.range.second << " ";
    cout << "| dst port: ";
    cout << hc->dst_port.range.first << "-" << hc->dst_port.range.second << " ";
    cout << "| proto ";
    for(auto itr = hc->proto_number.line.begin(); itr != hc->proto_number.line.end(); ++itr) {
            cout << itr->first << " ";
    }
    cout << endl;
}

void print_header_constraint_no_endl(struct header_constraint *hc)
{
    cout << "src addr "; 
    bitset_to_readable_addr(hc->src_addr, hc->src_prefixlen);
     cout << "dst addr "; 
    bitset_to_readable_addr(hc->dst_addr, hc->dst_prefixlen);
    cout << "src port ";
    cout << hc->src_port.range.first << "-" << hc->src_port.range.second << " : ";
    cout << "dst port ";
    cout << hc->dst_port.range.first << "-" << hc->dst_port.range.second << " : ";
    cout << "proto ";
    cout << hc->proto_number.line.size();
    //port here !!!
}