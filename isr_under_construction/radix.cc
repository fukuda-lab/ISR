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

#define BIT_TEST(k, b)  (((uint8_t *)(k))[(b) >> 3] & (0x80 >> ((b) & 0x7)))

using namespace std;

/* do not use prefixlen as variable*/
static int prefixlen = 0;
unsigned int get_string_to_uint_ipv4_addr(string addr)
{
        int i=0;
        struct sockaddr_in sa;

		if(addr == "0.0.0.0/0"){
			prefixlen = 0;
			return 0;
		}

        while(i < addr.length()-1){
                if(addr[i] == '/'){
                        break;
                }
                i++;
        }

        if(i == (addr.length()-1)){
                prefixlen = 32;

                int ret = inet_pton(AF_INET, addr.c_str(), &(sa.sin_addr));
                if(ret == -1){
                        perror("inet_pton");
                }

                sa.sin_addr.s_addr = ntohl(sa.sin_addr.s_addr);
                return sa.sin_addr.s_addr;
        }
        else{
                string saddr = addr.substr(0,i);

                string prefix_len = addr.substr(i+1);
                int plen = stoi(prefix_len);
                if(plen == 0){
                        plen = 32;
                }

                prefixlen = plen;

                unsigned int nmask = 0;

                for(int j=0;j<plen;j++){
                        nmask = nmask << 1;
                        nmask = nmask | 1;
                }

                nmask = nmask << (32-plen);

                int ret = inet_pton(AF_INET, saddr.c_str(), &(sa.sin_addr));
                if(ret == -1){
                        perror("inet_pton");
                }

                sa.sin_addr.s_addr = ntohl(sa.sin_addr.s_addr);

                return (sa.sin_addr.s_addr & nmask);
        }
}

unsigned int get_prefixlen()
{
	return prefixlen;
}


unsigned int add_string_ipv4_addr(string addr, struct radix_tree *rtree, struct route_data data)
{
    int i=0;
    struct sockaddr_in sa;
	int prefix[4];
    int plen;
	uint8_t addr1[4];

    while(i < addr.length()-1){
        if(addr[i] == '/'){
            break;
        }
        i++;
    }

    if(i == (addr.length()-1)){

		int ret = sscanf(addr.c_str(),"%d.%d.%d.%d/%d", &prefix[0], &prefix[1],&prefix[2], &prefix[3], &plen);
		if(ret < 0){
			return -1;
		}

        plen = 32;

		addr1[0] = prefix[0];
        addr1[1] = prefix[1];
        addr1[2] = prefix[2];
        addr1[3] = prefix[3];
		
		radix_tree_add(rtree,data,plen,addr1);
    }
    else{
		int ret = sscanf(addr.c_str(),"%d.%d.%d.%d/%d", &prefix[0], &prefix[1],&prefix[2], &prefix[3], &plen);
        if(ret < 0){
            return -1;
        }

    	addr1[0] = prefix[0];
        addr1[1] = prefix[1];
        addr1[2] = prefix[2];
        addr1[3] = prefix[3];

        radix_tree_add(rtree,data,plen,addr1);

        }

	return 0;
}

pair<uint8_t[4], int> get_string_to_u8_ipv4_addr(string addr)
{
    int i=0;
    struct sockaddr_in sa;
	int prefix[4];
	uint8_t addr1[4];
	pair<uint8_t[4], int> ap;

    while(i < addr.length()-1){
        if(addr[i] == '/'){
            break;
        }
        i++;
    }

    if(i == (addr.length()-1)){

		int ret = sscanf(addr.c_str(),"%d.%d.%d.%d/%d", &prefix[0], &prefix[1],&prefix[2], &prefix[3], &ap.second);
		if(ret < 0){
			cout << "sscanf error at get_string_to_u8_ipv4_addr" << endl;
		}

        ap.second = 32;

		ap.first[0] = prefix[0];
        ap.first[1] = prefix[1];
        ap.first[2] = prefix[2];
        ap.first[3] = prefix[3];
		
		return ap;
    }
    else{
		int ret = sscanf(addr.c_str(),"%d.%d.%d.%d/%d", &prefix[0], &prefix[1],&prefix[2], &prefix[3], &ap.second);
        if(ret < 0){
            cout << "sscanf error at get_string_to_u8_ipv4_addr" << endl;
        }

    	ap.first[0] = prefix[0];
        ap.first[1] = prefix[1];
        ap.first[2] = prefix[2];
        ap.first[3] = prefix[3];

        return ap;

    }

	return ap;
}

uint16_t get_proto_number(string p)
{
	uint16_t pp;

	if(p == "tcp"){
		pp = 6;
		return pp;
	}
	else if(p == "udp"){
		pp = 17;
		return pp;
	}
	else if(p == "icmp"){
		pp = 1;
		return pp;
	}
	else if(p == "ip"){
		pp = 201;
	}
	else{
		return 200;
	}
}

uint16_t get_port_number(string p)
{
	uint16_t pp;

	if(p == "53"){
		pp = 53;
		return pp;
	}
	else if(p == "80"){
		pp= 80;
		return pp;
	}
	else if(p == "443"){
		pp = 443;
		return pp;
	}
	else if(p == "25"){
		pp = 25;
		return pp;
	}
	else if(p == "110"){
		pp = 110;
		return pp;
	}
	else if(p == "143"){
		pp = 143;
		return pp;
	}
	else if(p == "993"){
		pp = 993;
		return pp;
	}
	else if(p == "995"){
		pp = 995;
		return pp;
	}
	else if(p == "123"){
		pp = 123;
		return pp;
	}
	else{
		cout << p << endl;
		cout << "warninggggggggg" << endl;
		return 0;
	}
}

string dec_to_binary(unsigned int n)
{
    string r;
    while (n != 0){
        r += ( n % 2 == 0 ? "0" : "1" );
        n /= 2;
    }
    return r;
}


struct header_constraint create_hc()
{
	struct header_constraint hc;

	return hc;
}


int radd(struct radix_tree_node **node, struct route_data data, int prefix, uint8_t *addr, int depth)
{
	struct radix_tree_node *new_node;

	if(*node == NULL){
		new_node = new radix_tree_node;
		if(new_node == NULL){
			printf("failed to allocate radix_tree_node at radd().\n");
		}
		*node = new_node;
		(*node)->right = NULL;
		(*node)->left = NULL;
	}
	
	if(prefix == depth){
		(*node)->data.push_back(data);
		return 0;
	}
	else{
		if(BIT_TEST(addr,depth)){
			return radd(&(*node)->right, data, prefix, addr, depth+1);
		}
		else{
			return radd(&(*node)->left, data, prefix, addr, depth+1);
		}
	}
}

int radix_tree_add(struct radix_tree *rtree, struct route_data data, int prefix, uint8_t *addr)
{
	return radd(&rtree->root, data, prefix, addr, 0);
}


int bitset_radd(struct radix_tree_node **node, struct route_data data, int prefix, bitset<32> addr, int depth)
{
	struct radix_tree_node *new_node;

	if(*node == NULL){
		new_node = new radix_tree_node;
		if(new_node == NULL){
			printf("failed to allocate radix_tree_node at radd().\n");
		}
		*node = new_node;
		(*node)->right = NULL;
		(*node)->left = NULL;
	}
	
	if(prefix == depth){
		(*node)->data.push_back(data);
		return 0;
	}
	else{
		if(addr[31-depth]){
			return bitset_radd(&(*node)->right, data, prefix, addr, depth+1);
		}
		else{
			return bitset_radd(&(*node)->left, data, prefix, addr, depth+1);
		}
	}
}

int bitset_radix_tree_add(struct radix_tree *rtree, struct route_data data, int prefix, bitset<32> addr)
{
	return bitset_radd(&rtree->root, data, prefix, addr, 0);
}

bool is_preferred_route(struct route_data node_data, struct route_data inserted_data)
{
	if(node_data.weight != 0 || inserted_data.weight != 0){
		cout << "wait !! weight !!" << endl;
		cout << "node_data.weight: " << node_data.weight << "  inserted_data.weight: " << inserted_data.weight << endl;
		return false;
	}

	if(node_data.local_pref < inserted_data.local_pref){
		cout << "wait !! local_pref !!" << endl;
		return true;
	}

	if(node_data.as_number.size() > inserted_data.as_number.size() && inserted_data.as_number.size() != 0){
		return true;
	}

	if(node_data.as_number.size() == 0 && inserted_data.as_number.size() > 0){
		return true;
	}

	return false;
}

int bitset_radd_w_pref(struct radix_tree_node **node, struct route_data data, int prefix, bitset<32> addr, int depth)
{
	struct radix_tree_node *new_node;

	if(*node == NULL){
		new_node = new radix_tree_node;
		if(new_node == NULL){
			printf("failed to allocate radix_tree_node at radd().\n");
		}
		*node = new_node;
		(*node)->right = NULL;
		(*node)->left = NULL;
	}
	
	if(prefix == depth){
		if((*node)->data.size() != 0){ 
			//compare bgp parameter
			for(int i=0;i<(*node)->data.size();i++){
				bool t = is_preferred_route((*node)->data[i],data);
				if(t == true){
					//put (i-1)-th position
					(*node)->data.insert((*node)->data.begin()+i, data);
					return 0;
				}
			}
			(*node)->data.push_back(data);
		}
		else{
			(*node)->data.push_back(data);
		}
		return 0;
	}
	else{
		if(addr[31-depth]){
			return bitset_radd_w_pref(&(*node)->right, data, prefix, addr, depth+1);
		}
		else{
			return bitset_radd_w_pref(&(*node)->left, data, prefix, addr, depth+1);
		}
	}
}

int bitset_radix_tree_add_w_pref(struct radix_tree *rtree, struct route_data data, int prefix, bitset<32> addr)
{
	return bitset_radd_w_pref(&rtree->root, data, prefix, addr, 0);
}

int radix_tree_shrink(struct radix_tree_node **cur)
{
	int lret;
	int rret;

	if(cur == NULL){
		return 0;
	}

	lret = radix_tree_shrink(&(*cur)->left); 
	rret = radix_tree_shrink(&(*cur)->right);

	if(lret || rret){
		return 1;
	}
	else{
		if((*cur)->left == NULL && (*cur)->right == NULL){
			free(*cur);
			*cur = NULL;
			return 0;
		}
		else{
			return 1;
		}
	}
}

void rdelete(struct radix_tree_node **node, int prefix, uint8_t *addr, int depth)
{
	if(*node == NULL){
		printf("No item to delete\n");
		return;
	}

	if(prefix == depth){
		if(*node != NULL){
			radix_tree_shrink(node);
			return;
		}
		else{
			return;
		}
	}
	else{
		if(BIT_TEST(addr,depth)){
			return rdelete(&(*node)->right, prefix, addr, depth+1);
		}
		else{
			return rdelete(&(*node)->left, prefix, addr, depth+1);
		}

	}
}

void radix_tree_delete(struct radix_tree *rtree, int prefix, uint8_t *addr)
{
	return rdelete(&rtree->root, prefix, addr, 0);
}

vector<route_data> rlookup(struct radix_tree_node *node, struct radix_tree_node *prev_node, vector<struct route_data> vrd, uint8_t *addr, int depth)
{
	if(node == NULL){
		if(vrd.size() != 0){
			return vrd;
		}
		else{
			printf("Can not find a route at rlookup() \n");
			return vrd;
		}
	}

	if(!node->data.empty()){
		for(int i=0;i<node->data.size();i++){
			vrd.push_back(node->data[i]);
		}
	}

	if(BIT_TEST(addr,depth)){
		return rlookup(node->right, prev_node, vrd, addr, depth+1);
	}
	else{
		return rlookup(node->left, prev_node, vrd, addr, depth+1);
	}

}

vector<route_data> radix_tree_lookup(struct radix_tree *rtree, uint8_t *addr)
{
	vector<struct route_data> vrd;
	return rlookup(rtree->root, NULL, vrd, addr, 0);
}

int cnt = 0;
int pcnt = 0;
void rpc_get(struct radix_tree_node **node, struct radix_tree_node *parent, vector< pair< string, vector<route_data> > > *prefix_chunk_vec, int *idx)
{
	if(*node != NULL){
		if(parent == NULL && !(*node)->data.empty()){
			parent = *node;

			pair< string, vector<route_data> > prefix_chunk;
			
			
			//cout << "parent for chunk ";
			//bitset_to_readable_addr((*node)->data[0].hc.dst_addr,(*node)->data[0].hc.dst_prefixlen);
			//cout<< endl;
			//cout << "parent for chunk " << (*node)->data[0].addr << endl;

			prefix_chunk.first = (*node)->data[0].str_prefix;
			prefix_chunk_vec->push_back(prefix_chunk);

			//rpc_get(&(*node)->left, parent, prefix_chunk_vec, idx);
			//rpc_get(&(*node)->right, parent, prefix_chunk_vec,idx);
			
			//prefix_chunk.second = (*node)->data;
			for(int i=0;i<(*node)->data.size();i++){
				(*prefix_chunk_vec)[*idx].second.push_back((*node)->data[i]);
				cnt++;
			}
			pcnt++;

			//parent = NULL;
			//*idx += 1;
			//cout << "add parent chunk:  " << prefix_chunk.first << endl;
		}
		else if(parent != NULL && !(*node)->data.empty()){
			//(*prefix_chunk_vec)[idx].second.push_back((*node)->data);
			//rpc_get(&(*node)->left, parent, prefix_chunk_vec, idx);
			//rpc_get(&(*node)->right, parent, prefix_chunk_vec,idx);

			//cout << "child for chunk ";
			//bitset_to_readable_addr((*node)->data[0].hc.dst_addr,(*node)->data[0].hc.dst_prefixlen);
			//cout<< endl;
			//cout << "child for chunk " << (*node)->data[0].addr << endl;
			for(int i=0;i<(*node)->data.size();i++){
				(*prefix_chunk_vec)[*idx].second.push_back((*node)->data[i]);
				cnt++;
			}
			pcnt++;

			//rpc_get(&(*node)->left, parent, prefix_chunk_vec, idx);
			//rpc_get(&(*node)->right, parent, prefix_chunk_vec,idx);
		}
		//else{
			rpc_get(&(*node)->left, parent, prefix_chunk_vec, idx);
			rpc_get(&(*node)->right, parent, prefix_chunk_vec,idx);
		//}

		if(*node == parent){
			parent = NULL;
			*idx += 1;
		}
	}
}

void radix_tree_prefix_chunk_get(struct radix_tree *rtree, vector< pair< string, vector<route_data> > > *prefix_chunk_vec)
{
	int idx = 0;
	//bfs_rpc_get(&rtree->root, prefix_chunk_vec, &idx);
	rpc_get(&rtree->root, NULL, prefix_chunk_vec, &idx);

	cout << "total rules: " << cnt << endl;
	cout << "total prefix: " << pcnt << endl;
}

void r_rflush(struct radix_tree_node *node, uint8_t *addr, int prefix_len, int depth)
{
	if(prefix_len == depth){
		//node->nexthop = "nn";
		return;
	}

	if(BIT_TEST(addr,depth)){
		return r_rflush(node->right, addr, prefix_len, depth+1);
	}
	else{
		return r_rflush(node->left, addr, prefix_len, depth+1);
	}

}

void radix_tree_route_flush(struct radix_tree *rtree, uint8_t *addr, int prefix_len)
{
	return r_rflush(rtree->root, addr, prefix_len, 0);
}

void radix_tree_free_node(struct radix_tree_node *node)
{
	if(node != NULL){
		radix_tree_free_node(node->left);
		radix_tree_free_node(node->right);
		free(node);
	}
}

int radix_tree_free(struct radix_tree *tree)
{
	radix_tree_free_node(tree->root);
	if(tree){
		free(tree);
	}
	return 0;
}

void radix_tree_init(struct radix_tree *rtree)
{
	if(rtree == NULL){
		rtree = new radix_tree;
	}
	else{
		printf("the radix tree is already allocated.\n");

	}
}
