#ifndef RAD_H
#define RAD_H

using namespace std;

struct port_number{
    bool is_any; //true == range
    //vector<uint8_t> vec_pnum;
    unordered_map<uint16_t, bool> line;
    pair<uint16_t, uint16_t> range;
};

struct proto_number{
    bool is_any; //true == range
    //vector<uint8_t> vec_pnum;
    unordered_map<uint16_t, bool> line;
    //pair<uint8_t, uint8_t> range;
};


struct header_constraint{
    bitset<32> src_addr;
    int src_prefixlen;
    bitset<32> dst_addr;
    int dst_prefixlen;
    struct port_number src_port;
    struct port_number dst_port;
    struct proto_number proto_number;

    bitset<32> dst_maximal;
};

struct route_data{
    string str_prefix;
    bitset<32> prefix;
    int prefixlen;
    unsigned int weight;
    unsigned int local_pref;
    vector<unsigned int> as_number;
    /*
	struct header_constraint hc;
	string addr;
	string dev;
	string type;
	vector< string > outports;
    */
};

struct radix_tree_node{
	struct radix_tree_node *left;
	struct radix_tree_node *right;

	vector<route_data> data;
};

struct radix_tree{
	struct radix_tree_node *root;
};

struct msl_radix_tree{
	struct msl_radix_tree_node *root;
};

int radix_tree_add(struct radix_tree *rtree, struct route_data data, int prefix, uint8_t *addr);
pair<uint8_t[4], int> get_string_to_u8_ipv4_addr(string addr);
int radix_tree_shrink(struct radix_tree_node **cur);
void radix_tree_delete(struct radix_tree *rtree, int prefix, uint8_t *addr);
vector<route_data> radix_tree_lookup(struct radix_tree *rtree, uint8_t *addr);
void radix_tree_route_flush(struct radix_tree *rtree, uint8_t *addr, int prefix_len);
int radix_tree_free(struct radix_tree *tree);
void radix_tree_init(struct radix_tree *rtree);
void radix_tree_prefix_chunk_get(struct radix_tree *rtree, vector< pair< string, vector<route_data> > > *prefix_chunk_vec);
int msl_radix_tree_free(struct msl_radix_tree *tree);
void read_linux_fib_to_radix_tree(string device, struct radix_tree *rtree, string path);
void read_arista_fib_to_radix_tree(string device, struct radix_tree *rtree, string path);
unsigned int get_string_to_uint_ipv4_addr(string addr);
void acl_parse(string line, struct radix_tree *radix);
void i2_style_route_parse(string dev, string addr, string plen, string outport, struct radix_tree *radix);
int bitset_radix_tree_add(struct radix_tree *rtree, struct route_data data, int prefix, bitset<32> addr);
unsigned int get_prefixlen();
int bitset_radix_tree_add_w_pref(struct radix_tree *rtree, struct route_data data, int prefix, bitset<32> addr);

#endif