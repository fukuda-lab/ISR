#ifndef UTIL_H
#define UTIL_H


void bitset_to_readable_addr(bitset<32> addr, int plen);
void print_header_constraint(struct header_constraint *hc);
void print_header_constraint_no_endl(struct header_constraint *hc);
string bitset_to_readable_addr_str(bitset<32> addr, int plen);

#endif