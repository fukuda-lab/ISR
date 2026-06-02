#ifndef SPA_H
#define SPA_H

#include <iostream>
#include <vector>
#include <string>
#include <utility> // std::pairのため

using namespace std;

struct v4_route{
    bitset<32> prefix;
    int prefix_len;
};

struct specification{
    bool block_or_permit;
    struct v4_route route;
    vector<string> forwarding_path;

     bool operator==(const specification& other) const {
        return block_or_permit == other.block_or_permit && forwarding_path == other.forwarding_path;
    }
};




std::vector<std::pair<specification, specification>> find_conflicts(
    std::vector<specification> vec_path_spec);

#endif