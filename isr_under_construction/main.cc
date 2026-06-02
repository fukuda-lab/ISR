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
#include <random>
#include <regex>
#include <unistd.h>
#include <optional> 
#include <filesystem>
#include <cmath>
#include <stdexcept>
#include "./include/radix.h"
#include "./include/util.h"
#include "./include/spec_path_analy.h"

struct interface{
    string name;
    vector<string> v4_addr;
    vector<string> v6_addr;
};

/*
struct v4_route{
    bitset<32> prefix;
    int prefix_len;
};
*/

/*
struct specification{
    bool block_or_permit;
    struct v4_route route;
    vector<string> forwarding_path;
};
*/

/*
struct str_specification{
    struct v4_route route;
    bool block_or_permit;
    vector<string> forwarding_path;

    bool operator==(const str_specification& other) const {
        return block_or_permit == other.block_or_permit && path == other.path;
    }
};
*/

struct bgp_diff_message{
    bool diff = false;

    string type;

    struct v4_route adv_route;

    vector<unsigned int> as_path;
    unsigned int community;
    unsigned int lp;
};

struct bgp_message{
    //update, withdrow
    string type;

    struct v4_route adv_route;

    vector<unsigned int> as_path;
    unsigned int community;
    unsigned int lp;

    unsigned int total_cost = 0;

    unsigned int unique_id = 0;
    bool sonzai = false;
    bool terminated = false;

    string terminated_point;
   // struct bgp_diff_message diff_msg;
};

//test_03_17
using dPair = pair<struct bgp_message, struct router *>;

struct compare_bgp_message{
    bool operator()(dPair const& bm1, dPair const& bm2){
        return bm1.first.as_path.size() > bm2.first.as_path.size();
    }
};


struct compare_ospf_message{
    bool operator()(dPair const& bm1, dPair const& bm2){
        return bm1.first.total_cost > bm2.first.total_cost;
    }
};


using dTuple = tuple<struct bgp_message, struct bgp_message, struct router *>;

struct compare_bgp_message_tuple{
    bool operator()(dTuple const& bm1, dTuple const& bm2){
        // return get<0>(bm1).as_wipath.size() > get<0>(bm2).as_path.size();
        if(get<0>(bm1).terminated == false && get<0>(bm2).terminated == false){
            return get<0>(bm1).as_path.size() > get<0>(bm2).as_path.size();
        }
        /*
        else if(get<0>(bm1).sonzai == true && get<0>(bm2).sonzai == true){
            return get<0>(bm1).as_path.size() > get<0>(bm2).as_path.size();
        }*/
        else{
            return get<1>(bm1).as_path.size() > get<1>(bm2).as_path.size();
        }
    }
};

using prisePair = pair<int, struct router*>;

/*
struct compare_bgp_message_tuple{
    bool operator()(dTuple const& bm1, dTuple const& bm2){
        if(get<0>(bm1).terminated == false && get<0>(bm2).terminated == false){
            return get<0>(bm1).as_path.size() > get<0>(bm2).as_path.size();
        }
        else if(get<0>(bm1).terminated == false && get<0>(bm2).terminated == true){
            return true;
        }
        else{
            return false;
        }
    }
};
*/

struct filter{
    string prefix;
    string action;
    string action_value;
    int action_value_numeric;
};

struct adj_rib_elm{
    string peer;
    vector<string> routes;
};


struct bgp_table_elm{
    struct v4_route adv_route;

    vector<unsigned int> as_path;
    unsigned int community = 0;
    unsigned int lp = 0;

    bool sonzai = false;
    bool terminated = false;

    unsigned long long id = 0;

    struct bgp_message prev_bgp_message;
    struct bgp_message prev_diff_bgp_message;

    string terminated_point;

    unsigned int total_cost = 0;

    //add=1,delete=2,changed=3
    unsigned int in_out = 0;

    unsigned int readv_id = 0;

    bool empty = false;
};

struct bgp_table_elms
{
    vector<vector<unsigned int>> vec_path;

    struct v4_route adv_route;

    vector<unsigned int> as_path;
    unsigned int community = 0;
    unsigned int lp = 0;

    bool sonzai = false;
    bool terminated = false;

    unsigned long long id = 0;

    struct bgp_message prev_bgp_message;
    struct bgp_message prev_diff_bgp_message;

    string terminated_point;

    unsigned int total_cost = 0;

    //add=1,delete=2,changed=3
    unsigned int in_out = 0;

    unsigned int readv_id = 0;

    bool empty = false;
};

struct bgp{
    /*
    struct radix_tree adj_rib_in;
    struct radix_tree loc_rib;
    struct radix_tree adj_rib_out;
    */

    unsigned int as_number;

    unordered_map<unsigned int, vector<v4_route>> adj_rib_in;
    //unordered_map<unsigned int, vector<pair<v4_route,bool>>> adj_rib_in_diff;

    unordered_map<unsigned int, vector<filter>> import_filter;
    unordered_map<unsigned int, vector<filter>> default_import_filter;
    //unordered_map<unsigned int, vector<pair<filter,bool>>> import_filter_diff;

    struct radix_tree *loc_rib;
    struct radix_tree *loc_rib_diff;

    bool new_advertised_prefix = false;
    struct v4_route new_route;

    vector<unsigned int> advertised_peers;
    unordered_map<unsigned int, bool> received_peers;

    //test_03_22
    //prefix, path, bgp_attributes+add/del 
    unordered_map<bitset<32>, list<struct bgp_table_elms>> loc_rib_table;
    //unordered_map<bitset<32>, list<struct bgp_table_elms>> loc_diff_rib_table;
    //vector<struct bgp_table_elms> loc_rib_table;
    //vector<struct bgp_table_elms> loc_diff_rib_table;


    unordered_map<unsigned int, vector<v4_route>> adj_rib_out;
    //unordered_map<unsigned int, vector<pair<v4_route,bool>>> adj_rib_out_diff;

    unordered_map<unsigned int, vector<filter>> export_filter;
    //unordered_map<unsigned int, vector<pair<filter,bool>>> export_filter_diff;

    vector<pair<unsigned int, vector<v4_route>>> advertised_v4_route;
    //unordered_map<unsigned int, unordered_map<string, pair<v4_route,bool>>> advertised_v4_route_diff;

    vector<struct router *> i_peer_relation;
    //unordered_map<unsigned int, bool> i_peer_relation_diff;

    unordered_map<unsigned int, struct router *> e_peer_relation;
    //unordered_map<unsigned int, bool> e_peer_relation_diff;

    //only for test
    unordered_map<string, unsigned int> link_cost;
};

/*
struct bgp_diff{
    unsigned int as_number;

    unordered_map<unsigned int, vector<pair<v4_route, bool>>> adj_rib_in;
    unordered_map<unsigned int, vector<pair<filter,bool>>> import_filter;
    struct radix_tree *loc_rib;
    unordered_map<unsigned int, vector<pair<v4_route,bool>>> adj_rib_out;
    unordered_map<unsigned int, vector<pair<filter,bool>>> export_filter;

    vector<pair<unsigned int, vector<pair<v4_route,bool>>>> advertised_v4_route;

    unordered_map<unsigned int, bool> peer_relation;
};
*/

struct RouteMapInfo {
    string src_node;
    string dst_node;
    string action; // "permit" または "deny"
};

struct ospf_peer{
    string peer_name;
    unsigned int cost;
};

struct ospf{
    vector<ospf_peer> peer_info;
    unordered_map<string, unsigned int> peer_cost_map;

    //diff costはここに入れる
    unordered_map<string, unsigned int> diff_peer_cost_map;
};

struct router{
    string name;
    vector<struct interface> infs;
    struct radix_tree rib;

    struct bgp bgp;
    struct bgp bgp_diff;
    //test_05_22
    struct bgp virtual_bgp_diff;

    struct ospf ospf;
    struct ospf ospf_diff;
};

using namespace std;
namespace fs = std::filesystem;


static unordered_map<unsigned int, vector<router *>> as_number_to_router;
static unordered_map<string, struct router *> name_to_router;
static unordered_map<string, string> topology;
static unordered_map<string, vector<string>> se_topo;
static unordered_map<string, vector<string>> zoo_topo;
 //test_03_17
static unordered_map<string, struct bgp_message> traverse_history;
static unordered_map<string, struct bgp_message> diff_traverse_history;
 //test_03_17
static unordered_map<unsigned int, unordered_map<unsigned int, bool>> all_pair_reachability;
static vector<struct specification> vec_specs;
static vector<bool> is_spec_satisfied;
static string cur_prefix;

//routername.type.value
struct config_change{
    struct router *router;
    string type;
    string value;
};

struct side_effect_info{
    struct router *router;
    struct router *se_affected_router;
    string type;

    int action_value_numeric;

    struct v4_route route;
    vector<unsigned int> before_repair_path;
    vector<unsigned int> after_repair_path;


    struct specification *src_spec;
};

struct path_group{
    struct v4_route prefix;

    unordered_map<string, bool> nodes;
    list<string> boundary_nodes;
    unordered_map<string, bool> boundary_node;
    unordered_map<string, bool> connected_node;
    vector<string> shortest_path;
    vector<vector<string>> path_list;
};


//spec to config change
//spec:IP_Prefix_path
static unordered_map<string, vector<struct config_change>> spec_to_config_changes;

//config change to side effect
static unordered_map<string, vector<struct side_effect_info>> cchange_to_se_info;

//se_path_group
static vector<path_group> vec_path_group;

//test_05_21
static priority_queue<dPair, vector<dPair>, compare_bgp_message> dif_pq;
//test_08_27
static priority_queue<dPair, vector<dPair>, compare_ospf_message> dif_ospf_pq;
static vector<int> sec_vec;

static vector<struct specification> spec_list;

static unsigned int side_effect_cnt = 0;

static vector<unsigned int> total_number_of_uncompressed_path;
static vector<unsigned int> total_number_of_compressed_path;
static vector<double> compressed_rate;

static unsigned long long msg_id = 0;



struct BoxPlotData {
    string type;
    double min_val;
    double q1;
    double median;
    double q3;
    double max_val;
};

struct ospf_config {
    unsigned int cost;
    string nexthop;
 };
 
static unordered_map<string, vector<ospf_config>> dev_to_ospf_config;

static unordered_map<string, unordered_map<string, string>> devdev_to_inf2;


struct bgp_table_elm convert_bgp_elms_to_elm(struct bgp_table_elms bte)
{
    struct bgp_table_elm bte_elm;
    bte_elm.adv_route = bte.adv_route;
    bte_elm.as_path = bte.as_path;
    bte_elm.community = bte.community;
    bte_elm.lp = bte.lp;
    bte_elm.id = bte.id;
    bte_elm.sonzai = bte.sonzai;
    bte_elm.terminated = bte.terminated;
    bte_elm.terminated_point = bte.terminated_point;
    bte_elm.total_cost = bte.total_cost;
    bte_elm.in_out = bte.in_out;
    bte_elm.readv_id = bte.readv_id;
    bte_elm.empty = bte.empty;
    return bte_elm;
}

//parse_connectionsを先に読む
//
void parse_connections(string file_path) {
    std::ifstream input_file(file_path);

    // ファイルが開けない場合はエラーメッセージを表示して空のマップを返す
    if (!input_file.is_open()) {
        std::cerr << "Error: Could not open file " << file_path << std::endl;
        return;
    }

    std::string line;
    // ファイルを1行ずつ読み込む
    while (std::getline(input_file, line)) {
        std::stringstream ss(line);
        std::string device1, interface1, device2, interface2;

        // 1行から4つの要素を読み込む
        if (ss >> device1 >> interface1 >> device2 >> interface2) {
            // device1 -> device2 の接続情報を格納
            //connections[device1][device2] = {interface1, interface2};
            devdev_to_inf2[device1][interface1] = device2;
            // device2 -> device1 の接続情報も格納（双方向）
            devdev_to_inf2[device2][interface2] = device1;
            //connections[device2][device1] = {interface2, interface1};

            // cout << "dev: " << device1 << " is linked with " << device2 << endl;
        }
    }

    input_file.close();
    return;
}

std::vector<std::pair<std::string, unsigned int>> parse_interface_costs(string file_path) {
    std::vector<std::pair<std::string, unsigned int>> if_cost;
    std::ifstream input_file(file_path);

    if (!input_file.is_open()) {
        std::cerr << "Warning: Could not open file " << file_path << std::endl;
        return if_cost;
    }

    std::string line;
    while (std::getline(input_file, line)) {
        std::stringstream ss(line);
        std::string interface_name;
        std::string cost_keyword;
        unsigned int cost_value;

        if (ss >> interface_name >> cost_keyword >> cost_value) {
            if (cost_keyword == "cost") {
                if_cost.push_back({interface_name, cost_value});
            }
        }
    }
    input_file.close();
    return if_cost;
}

void parse_interface_costs_from_directory(string directory_path) {
    std::vector<std::pair<std::string, unsigned int>> all_costs;

    // ディレクトリ内の各エントリをループで処理
    for (const auto& entry : std::filesystem::directory_iterator(directory_path)) {
        // エントリが通常のファイルであるかを確認（サブディレクトリなどを除外）
        if (entry.is_regular_file()) {
            // ファイルのパスを文字列として取得
            std::string file_path = entry.path().string();
            std::string device_name = entry.path().stem().string();
            
            // 単一ファイルをパースするヘルパー関数を呼び出す
            std::vector<std::pair<std::string, unsigned int>> if_cost = parse_interface_costs(file_path);

            for(int i=0;i<if_cost.size();i++){
                string next_dev = devdev_to_inf2[device_name][if_cost[i].first];
                // cout << "router: " << device_name << " interface: " << if_cost[i].first  << ", peer: " << next_dev << ", cost: " << if_cost[i].second << endl;
                if(next_dev.empty()){
                    // cout << "peer is edgePorts, let's continue" << endl;
                    continue;
                }

                struct router *r = name_to_router[device_name];
                
                struct ospf_peer op;
                op.peer_name = next_dev;
                op.cost = if_cost[i].second;

                r->ospf.peer_info.push_back(op);
                r->ospf.peer_cost_map[next_dev] = if_cost[i].second;
    
                r->ospf_diff.peer_info.push_back(op);
                r->ospf_diff.peer_cost_map[next_dev] = if_cost[i].second;
            }
            
            // 取得した結果を総合結果のvectorに結合する
            //all_costs.insert(all_costs.end(), costs_from_file.begin(), costs_from_file.end());
        }
    }

    // cout << "done" << endl;

    return;
}

 std::vector<ospf_config> parse_ospf_file(const std::string& filepath) {
     std::vector<ospf_config> ospf_config_vec;
     std::ifstream file(filepath); 
 
     if (!file.is_open()) {
         std::cerr << "Error: Could not open file: " << filepath << std::endl;
         return ospf_config_vec;
     }
 
     std::string cost_line;
     std::string desc_line;
     std::string separator;
 
     // 1ブロック（3行）ずつ読み込みを試みる
     while (std::getline(file, cost_line) && 
            std::getline(file, desc_line) &&
            std::getline(file, separator)) {
         
         try {
             ospf_config current_config;
 
             size_t cost_token_pos = cost_line.find("cost ");
             if (cost_token_pos != std::string::npos) {
                 current_config.cost = std::stoul(cost_line.substr(cost_token_pos + 5));
             } else {
                 continue; 
             }
             
             size_t nexthop_start_pos = desc_line.find("\"To ");
             size_t nexthop_end_pos = desc_line.rfind('\"');
             if (nexthop_start_pos != std::string::npos && nexthop_end_pos != std::string::npos) {
                 nexthop_start_pos += 4;
                 current_config.nexthop = desc_line.substr(nexthop_start_pos, nexthop_end_pos - nexthop_start_pos);
             } else {
                 continue;
             }
 
             ospf_config_vec.push_back(current_config);
 
         } catch (const std::exception& e) {
             std::cerr << "Warning: Failed to parse a block. Error: " << e.what() << std::endl;
             exit(-1);
         }
     }
 
     file.close();
     return ospf_config_vec;
 }

void read_ospf_file(string dir)
{
    vector<string> file_vec;

    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            file_vec.push_back(entry.path().filename().string());
        }
    }

    for(int i=0;i<file_vec.size();i++){
        // cout << "----------------" << endl;
        string f_path = dir+file_vec[i];
        
        // cout << "read " << f_path << endl;

        vector<ospf_config> oc_vec = parse_ospf_file(f_path);

        fs::path file_path(file_vec[i]);
        string dev_name = file_path.stem().string();
        //struct router *r = name_to_router[dev_name];

        //nexthopの追加(topology)
        for(int i=0;i<oc_vec.size();i++){
            string next = oc_vec[i].nexthop;

            zoo_topo[dev_name].push_back(next);
            // cout << "src: " << dev_name << " dst: " << next << endl;
        }

        //costの追加
        dev_to_ospf_config[dev_name] = oc_vec;
    }
}

//read_configuration
void read_ospf_configuration()
{
    for(auto itr=dev_to_ospf_config.begin();itr!=dev_to_ospf_config.end();itr++){
        string dev_name = itr->first;
        vector<ospf_config> oc_vec = itr->second;

        struct router *r = name_to_router[dev_name];

        for(int i=0;i<oc_vec.size();i++){
            string next = oc_vec[i].nexthop;
            unsigned int cost = oc_vec[i].cost;

            //costの設定⇨bgpに載せる
            struct ospf_peer op;
            op.peer_name = next;
            op.cost = cost;

            // cout << "router: " << dev_name << ", peer: " << next << ", cost: " << cost << endl;

            r->ospf.peer_info.push_back(op);
            r->ospf.peer_cost_map[next] = cost;

            r->ospf_diff.peer_info.push_back(op);
            r->ospf_diff.peer_cost_map[next] = cost;
        }
    }
}

double calculate_median(const std::vector<double>& sorted_data, size_t start, size_t end) {
    size_t count = end - start;
    if (count == 0) {
        return 0.0;
    }
    size_t mid = start + count / 2;
    if (count % 2 == 0) {
        return (sorted_data[mid - 1] + sorted_data[mid]) / 2.0;
    } else {
        return sorted_data[mid];
    }
}

BoxPlotData calculate_boxplot_data(std::vector<double> data) {
    if (data.empty()) {
        throw std::invalid_argument("Error: Input vector cannot be empty.");
    }
    std::sort(data.begin(), data.end());
    BoxPlotData result;
    const size_t n = data.size();
    result.min_val = data.front();
    result.max_val = data.back();
    result.median = calculate_median(data, 0, n);
    size_t mid_point = n / 2;
    if (n % 2 == 0) {
        result.q1 = calculate_median(data, 0, mid_point);
        result.q3 = calculate_median(data, mid_point, n);
    } else {
        result.q1 = calculate_median(data, 0, mid_point);
        result.q3 = calculate_median(data, mid_point + 1, n);
    }
    return result;
}



std::vector<RouteMapInfo> get_route_map_info(string file_path) {
    std::vector<RouteMapInfo> results;
    std::ifstream config_file(file_path);
    std::string line;

    std::regex pattern("^route-map ([^ ]+) (permit|deny)");
    std::smatch matches;

    string prev_src  = "empty";
    string prev_dst = "empty";

    while (std::getline(config_file, line)) {
        if (std::regex_search(line, matches, pattern)) {
            std::string map_name = matches[1].str();
            std::string action = matches[2].str();

            const std::string prefix = "RMap_";
            const std::string infix = "_from_";
            
            size_t prefix_pos = map_name.find(prefix);
            size_t infix_pos = map_name.find(infix);


            if (prefix_pos != std::string::npos && infix_pos != std::string::npos) {
                
                size_t src_start = prefix.length();
                std::string src = map_name.substr(src_start, infix_pos - src_start);

                size_t dst_start = infix_pos + infix.length();
                std::string dst = map_name.substr(dst_start);

                RouteMapInfo info;
                info.src_node = src;
                info.dst_node = dst;
                info.action   = action;
                
                if(prev_src == info.src_node && prev_dst == info.dst_node){
                    continue;
                }

                prev_src = info.src_node;
                prev_dst = info.dst_node;


                results.push_back(info);
            }
        }
    }
    return results;
}


void print_uint_path(vector<unsigned int> path)
{
    // cout << "AS_Path: ";
    
    for(int i=0;i<path.size();i++){
        // cout << path[i] << " ";
    }

    // cout << endl;

    // cout << "Dev_Path: ";

    for(int i=0;i<path.size();i++){
        // cout << as_number_to_router[path[i]][0]->name  << " ";
    }

    // cout << endl;
}

void print_str_path(vector<string> path)
{
    return;
    cout << "AS_Path: ";
    
    for(int i=0;i<path.size();i++){
        cout << path[i] << " ";
    }

    cout << endl;
    return;
    cout << "Dev_Path: ";

    for(int i=0;i<path.size();i++){
        cout << as_number_to_router[stoi(path[i])][0]->name  << " ";
    }

    cout << endl;
}

void print_devpath(vector<string> path)
{
    return;
    cout << "Dev_Path: ";
    
    for(int i=0;i<path.size();i++){
        cout << path[i] << " ";
    }
    cout << endl;
}

int get_bgp_as_number(const std::string& file_name) {
    std::ifstream config_file(file_name);
    if (!config_file.is_open()) {
        std::cerr << "エラー: ファイル '" << file_name << "' を開けません。" << std::endl;
        return -1; // ファイルが開けない場合は空を返す
    }

    // 2. 正規表現を準備
    // ^ は行頭、(\\d+) は1桁以上の数字をキャプチャするグループ
    std::regex bgp_regex("^router bgp (\\d+)");
    
    std::string line;
    std::smatch match;

    // 3. ファイルを1行ずつ読み込んでチェック
    while (std::getline(config_file, line)) {
        // 現在の行が正規表現にマッチするか調べる
        if (std::regex_search(line, match, bgp_regex)) {
            // マッチした場合、最初のキャプチャグループ（AS番号）を返す
            if (match.size() > 1) {
                // match[1] はAS番号の文字列なので、stoiで整数に変換
                return std::stoi(match[1].str());
            }
        }
    }

    // ファイルの最後までAS番号が見つからなかった場合
    return -1;
}

void spec_analy_and_show()
{
    // cout << "spec analy starts" << endl; 

    //for scalability test
    for(int i=0;i<spec_list.size();i++){
        spec_list[i].block_or_permit = true;
    }

    ///spec analy
    find_conflicts(spec_list);

    //show
    for(int i=0;i<spec_list.size();i++){
        //prefixごとにprint
        // cout << "spec " << i << "th" << endl;
        if(spec_list[i].forwarding_path.size() == 0){
            // cout << "No forwarding path !" << endl;
            exit(-1);
            continue;
        }
        //print_str_path(spec_list[i].forwarding_path);
        print_devpath(spec_list[i].forwarding_path); 
        // cout << endl;
    }
}

vector<string> convert_upath_to_svpath(vector<unsigned int> upath)
{
    vector<string> svpath;

    //// cout << "upath: ";
    for(int i=0;i<upath.size();i++){
        //// cout << upath[i] << " ";
        svpath.push_back(to_string(upath[i]));
    }
    //// cout << endl;

    return svpath;
}

bool is_path_included(vector<unsigned int> as_path, vector<unsigned int> included_path)
{
        
    if(as_path.size() < included_path.size()){
        return false;
    }

    for(int i=0;i<included_path.size();i++){
        if(as_path[i] != included_path[i]){
            return false;
        }
    }
    return true;
}

vector<string> convert_upath_to_spath(vector<unsigned int> upath)
{
    vector<string> spath;
    for(int i=0;i<upath.size();i++){
        string s = to_string(upath[i]);
        spath.push_back(s);
    }

    return spath;
}

bool is_path_included_str(vector<string> as_path, vector<string> included_path)
{       
    if(as_path.size() < included_path.size()){
        return false;
    }

    if(as_path.size() == 0 || included_path.size() == 0){
        return false;
    }

    //included_pathと同じidxを見つけて、そこから全部一緒か

    int idx;
    for(idx=0;idx<as_path.size();idx++){
        if(as_path[idx] == included_path[0]){
            break;
        }
    }
    idx = 0;


    if(idx == as_path.size()){
        return false;
    }

    for(int i=0;i<included_path.size();i++){
        if(i == as_path.size()){
            return false;
        }
        // cout << "asp: " << as_path[idx] << endl;
        // cout << "included: " << included_path[i] << endl;
        if(idx >= as_path.size() ||  as_path[idx] != included_path[i]){
            return false;
        }
        idx++;
    }
    return true;
}

bool is_subpath_included_str(vector<string> as_path, vector<string> included_path)
{       
    if(as_path.size() < included_path.size()){
        return false;
    }

    if(as_path.size() == 0 || included_path.size() == 0){
        return false;
    }

    //included_pathと同じidxを見つけて、そこから全部一緒か

    int idx;
    for(idx=0;idx<as_path.size();idx++){
        if(as_path[idx] == included_path[0]){
            break;
        }
    }

    if(idx == as_path.size()){
        return false;
    }

    for(int i=0;i<included_path.size();i++){
        //// cout << "longer path: " << as_path[idx] << endl;
        //// cout << "subpath canditate: " << included_path[i] << endl;
        if(idx >= as_path.size() ||  as_path[idx] != included_path[i]){
            return false;
        }
        idx++;
    }
    return true;
}

vector<vector<string>> delete_subpaths(vector<vector<string>> path_vec)
{
    vector<vector<string>> new_path_vec;
    cout << "delete_subpath" << endl;

    for(int i=0;i<path_vec.size();i++){
        int j=0;
        bool f = false;

        for(j=0;j<path_vec.size();j++){
            if(i==j){
                continue;
            }

            if(is_subpath_included_str(path_vec[j], path_vec[i])){
                //cout << "the longer path includes the subpath canditate !" << endl;
                //cout << "subpaths: ";
                //print_str_path(path_vec[i]);
                //cout << "longer path: ";
                //print_str_path(path_vec[j]);
                f = true;
                break;           
            }
        }
        
        if(f == false){
            //// cout << "new_path_vec added" << endl;
            new_path_vec.push_back(path_vec[i]);
        }
    }

    return new_path_vec;
}


void print_as_numbers(struct bgp_message bm)
{
    return;
    // cout << "AS_PATH (size " << bm.as_path.size() << "): ";
    for(int i=0;i<bm.as_path.size();i++){
        // cout << bm.as_path[i] << " ";
    }
    // cout << endl;
}     

void print_as_number(struct bgp_table_elms bm)
{
    // cout << "AS_PATH (size " << bm.as_path.size() << "): ";
    for(int i=0;i<bm.as_path.size();i++){
        // cout << bm.as_path[i] << " ";
    }
    // cout << endl;
}

void print_as_number_to_router(struct bgp_table_elms bm)
{
    // cout << "AS_PATH (size " << bm.as_path.size() << "): ";
    for(int i=0;i<bm.as_path.size();i++){
        // cout << as_number_to_router[bm.as_path[i]][0]->name << " ";
    }
    // cout << endl;
}

void print_as_path(vector<unsigned int> as_path)
{
    return;
    cout << "AS_PATH (size " << as_path.size() << "): ";
    for(int i=0;i<as_path.size();i++){
        cout << as_number_to_router[as_path[i]][0]->name << " ";
    }


    cout << endl;
}    

void create_bm(struct bgp_message *bm, v4_route vr, vector<unsigned int> asp, unsigned int lp)
{
    bm->adv_route = vr;
    bm->as_path = asp;
    bm->lp = lp;
    bm->sonzai = true;
    bm->terminated = false;
}

bool is_path_equal(vector<unsigned int> as_path, vector<unsigned int> included_path)
{    
    if(as_path.size() != included_path.size()){
        return false;
    }

    for(int i=0;i<included_path.size();i++){
        if(as_path[i] != included_path[i]){
            return false;
        }
    }
    return true;
}

bool is_multiple_path_equal(vector<vector<unsigned int>> path1, vector<vector<unsigned int>> path2)
{
    if(path1.empty() && path2.empty()){
        return true;
    }

    if(path1.empty() || path2.empty()){
        return false;
    }

    return path1 == path2;
}

bool is_str_path_equal(vector<string> as_path, vector<string> included_path)
{    
    if(as_path.size() != included_path.size()){
        return false;
    }

    for(int i=0;i<included_path.size();i++){
        if(as_path[i] != included_path[i]){
            return false;
        }
    }
    return true;
}

int calc_ospf_preference_diff(struct bgp_table_elms past_bestr, struct bgp_table_elms new_bestr)
{
    if(past_bestr.total_cost != 0 || new_bestr.total_cost != 0){
        if(past_bestr.total_cost > new_bestr.total_cost){
            return past_bestr.lp - new_bestr.lp;
        }
        else{
            // cout << "Fatal error at calc_the_preference_diff" << endl;
            exit(1);
        }
    }

    return 0;
}

int ospf_find_the_best_route(struct bgp_table_elms bestr, list<struct bgp_table_elms> *diff_table)
{
    int idx = 0;
    for(auto itr=diff_table->begin();itr!=diff_table->end();itr++){
        if(bestr.as_path == itr->as_path){
            idx = calc_ospf_preference_diff(bestr, diff_table->front()); 
            return idx;
        }
    }

    return -1;
}

int calc_the_preference_diff(struct bgp_table_elms past_bestr, struct bgp_table_elms new_bestr)
{
    if(past_bestr.lp != 0 || new_bestr.lp != 0){
        if(past_bestr.lp < new_bestr.lp){
            return new_bestr.lp - past_bestr.lp;
        }
        else{
            // cout << "Fatal error at calc_the_preference_diff" << endl;
            exit(1);
        }
    }

    return 0;
}


int new_find_the_best_route_2(struct bgp_table_elms bestr, list<struct bgp_table_elms> *diff_table)
{
    int idx = 0;
    for(auto itr=diff_table->begin();itr!=diff_table->end();itr++){
        if(bestr.as_path == itr->as_path){
            idx = calc_the_preference_diff(bestr, diff_table->front()); 
            return idx;
        }
    }

    return -1;
}


//if there is diff, return diff values, if not, return -1
int new_find_the_best_route(struct bgp_table_elms bestr, list<struct bgp_table_elms> *diff_table)
{
    int idx = 0;
    for(auto itr=diff_table->begin();itr!=diff_table->end();itr++){
        if(bestr.as_path == itr->as_path){
            /*
            // cout << "bestr: " << bestr.lp << endl;
            // cout << "itr->lp: " << itr->lp << endl;
            // cout << "diff->front->lp: " << diff_table->front().lp << endl;
            // cout << "itr->as_path: " << itr->as_path.size() << endl;
            // cout << "diff_aspath: " << diff_table->front().as_path.size() << endl;
            */
            idx = calc_the_preference_diff(bestr, diff_table->front()); 
            /*
            if(idx == 0){
                // cout << "Fatal error at new_find_the_best_route" << endl;
                exit(1);
            }*/
            return idx;
        }
    }

    return -1;
}

bool check_best_path_changed(vector<string> best_path_before_repair, vector<string> best_path_after_repair)
{
    //spec a->b->c
    //path a->bのような部分パスのこうりょ
    if(best_path_before_repair.size() == best_path_after_repair.size()){
        for(int i=0;i<best_path_before_repair.size();i++){
            if(best_path_before_repair[i] != best_path_after_repair[i]){
                return false;
            }
        }        
        return true;
    }

    return false;
}

bool is_path_spec_compliance(vector<string> path, struct specification spec)
{
    if(path.size() > spec.forwarding_path.size()){
        // cout << "Fatal error at is_path_spec_compliance()" << endl;
        exit(1);
    }

    for(int i=0;i<path.size();i++){
        if(path[i] != spec.forwarding_path[i]){
            return false;
        }
    }

    return true;
}

bool check_spec_compliance(vector<string> best_path_before_repair, vector<string> best_path_after_repair, struct specification spec)
{
    //check if best path is changed && satisfy the spec

    if(check_best_path_changed(best_path_before_repair, best_path_after_repair)){
        if(is_path_spec_compliance(best_path_after_repair,spec)){
            return true;
        }
    }

    return false;
}

bool check_spec_compliance_only_before(vector<string> best_path_after_repair, struct specification spec)
{

    return false;
}

bool check_spec_compliance_only_after(vector<string> best_path_after_repair, struct specification spec)
{
    if(is_path_spec_compliance(best_path_after_repair, spec)){
        return true;
    }
    return false;
}

vector<struct router*> get_router_for_given_spec(struct specification spec)
{
    vector<struct router *> vec_router;

    //始点と終点だけにしても良いかも
    for(int i=0;i<spec.forwarding_path.size();i++){
        string rtr = spec.forwarding_path[i];
        struct router *cur_rtr = name_to_router[rtr];

        //check if best path is changed && satisfy the spec

        //beforeだけnull → afterがspecに準拠しているか
        //afterだけnull→到達性の排除ならあり得る、beforeのパスでafterのconfigを使ってdrop configを探す
        //before && afterもnull → スルー

        vector<string> diff_first_path = convert_upath_to_spath(cur_rtr->bgp_diff.loc_rib_table[spec.route.prefix].front().as_path);
        vector<string> cur_first_path = convert_upath_to_spath(cur_rtr->bgp.loc_rib_table[spec.route.prefix].front().as_path);

        if(cur_rtr->bgp.loc_rib_table[spec.route.prefix].empty()){
            if(!cur_rtr->bgp_diff.loc_rib_table[spec.route.prefix].empty()){
                //bool res = check_spec_compliance_only_after(cur_rtr->bgp_diff.loc_rib_table[spec.route.prefix].front().as_path,spec);
                bool res = check_spec_compliance_only_after(diff_first_path, spec);
                if(res == true){
                    vec_router.push_back(cur_rtr);
                }
            }
            continue;//?
        }
        else{
            if(cur_rtr->bgp_diff.loc_rib_table[spec.route.prefix].empty()){
                //bool res = check_spec_compliance_only_before(cur_rtr->bgp.loc_rib_table[spec.route.prefix].front().as_path,spec);]
                bool res = check_spec_compliance_only_before(cur_first_path, spec);
                //do diff check
            }
            continue;
        }

        list<struct bgp_table_elms> tmp_table_before_repair = cur_rtr->bgp.loc_rib_table[spec.route.prefix];
        list<struct bgp_table_elms> tmp_table_after_repair = cur_rtr->bgp_diff.loc_rib_table[spec.route.prefix];

        if(check_spec_compliance(convert_upath_to_spath(tmp_table_before_repair.front().as_path), convert_upath_to_spath(tmp_table_after_repair.front().as_path), spec)){
            //if so store the vec
            vec_router.push_back(cur_rtr);
        }
    }

    return vec_router;
}

string get_string_path(vector<unsigned int> path)
{
    string sp;

    for(int i=0;i<path.size();i++){
        if(i+1==path.size()){
            sp += to_string(path[i]);
            break;
        }
        sp += to_string(path[i]) + "→";
    }

    return sp;
}

string get_string_dev_path(vector<string> path)
{
    string sp;

    for(int i=0;i<path.size();i++){
        if(i+1==path.size()){
            sp += (as_number_to_router[stoi(path[i])][0])->name;
            break;
        }
        sp += as_number_to_router[stoi(path[i])][0]->name + "→";
    }

    return sp;
}

string get_string_path_str(vector<string> path)
{
    string sp;

    for(int i=0;i<path.size();i++){
        if(i+1==path.size()){
            sp += path[i];
            break;
        }
        sp += path[i] + "→";
    }

    return sp;
}

void how_to_identify_side_effect_group()
{
    //differential simulationをやる時
    //とりあえず、prefixごとに
    //diff-traceを作った後に、経路広報順にafter repairのグラフを探索する
    //副作用の関係グラフを作る
    //どれとどれが消えれば、どの副作用は追加しなくて良いか
}

//論文では、identicalなrouting messageで経路が知られて、かつパスが重複関係にある
void identify_root_config_source()
{
    //config change to side effectの各グループに対して
    //パスが重複関係にあるグループを作る
    //そのグループに対して、最小の部分パスを見つけて、それは次のrepairの仕様にする

    for(auto itr=cchange_to_se_info.begin();itr!=cchange_to_se_info.end();itr++){
        string chage_source = itr->first;
        // cout << "Start to process the change source: " << chage_source << endl;
        
        vector<side_effect_info> sei = itr->second;

        for(int i=0;i<sei.size();i++){
            vector<unsigned int> b_path = sei[i].after_repair_path;
            //b_pathが部分パスかチェック、そう
        }
        
    }
}

void how_to_refine_spec()
{
    //1. 副作用のパスをグループ化する
    //2. 部分グラフを作る
    //3. 境界面を計算する
    //4. それに含まれるパスを作る
}

void show_se_subgraph()
{
    // cout << "show_se_subgraph() " << endl;
    for(auto itr=se_topo.begin();itr!=se_topo.end();itr++){
        string src = itr->first;

        // cout << "src: " << src << " : dst ";
        for(int i=0;i<itr->second.size();i++){
            if(i==itr->second.size()){
                // cout << itr->second[i] << endl;
                break;
            }
            // cout << itr->second[i] << ", ";
        }
    }
    // cout << endl;
}

//best path before/after repairに、ラストノードを追加する
bool check_same_node(vector<string> stp, string n)
{
    //// cout << "checked node: " << n << endl;
    for(int i=0;i<stp.size();i++){
        //// cout << "cur node: " << stp[i] << endl;
        if(stp[i] == n){
            return false;
        }
    }

    return true;
}

void create_se_graph()
{
    unordered_map<string, bool> node_map;

    unordered_map<string, vector<string>> g;
    se_topo = g;

    // cout << "create_se_graph()" << endl;

    for(int i=0;i<vec_path_group.size();i++){
        for(int j=0;j<vec_path_group[i].path_list.size();j++){
            for(int k=0;k<vec_path_group[i].path_list[j].size();k++){
                node_map[vec_path_group[i].path_list[j][k]] = true;
            }
        }
    }

    //unordered_map<string, bool> visited;

    for(auto itr=node_map.begin();itr!=node_map.end();itr++){
        if(itr->second == false){
            continue;
        }

        string start_node = itr->first;
        // cout << "start node: " << start_node << endl;

        unordered_map<string, bool> visited;
        visited[start_node] = true;

        queue<string> q;

        vector<string> next = zoo_topo[start_node];
        for(int i=0;i<next.size();i++){
            q.push(next[i]);
        }

        while(!q.empty()){
            string n = q.front();
            q.pop();

            if(visited[n] == true){
                continue;
            }
            
            visited[n] = true;

            if(node_map[n] == true){
                //se_topo[start_node].push_back(n);
                if(check_same_node(se_topo[start_node], n)){
                    se_topo[start_node].push_back(n);
                }
            }
            else{
                vector<string> next_nodes = zoo_topo[n];
                for(int j=0;j<next_nodes.size();j++){
                    q.push(next_nodes[j]);
                }
            }
        }
    }

    show_se_subgraph();
}


void create_se_subgraph()
{
    unordered_map<string, bool> node_map;
    // cout << "create_se_subgraph()" << endl;

    for(int i=0;i<vec_path_group.size();i++){
        for(int j=0;j<vec_path_group[i].path_list.size();j++){
            for(int k=0;k<vec_path_group[i].path_list[j].size();k++){
                if(node_map[vec_path_group[i].path_list[j][k]] == true){
                    continue;
                }
                // cout << "add node: " << vec_path_group[i].path_list[j][k] << endl;
                node_map[vec_path_group[i].path_list[j][k]] = true;
            }
        }
    }

    //unordered_map<string, bool> visited;
    unordered_map<string, bool> start_checked;

    unordered_map<string, bool> node_map_chk = node_map;

    for(auto itr=node_map.begin();itr!=node_map.end();itr++){;
        if(itr->second == false){
            // cout << "wiw: false" << endl; 
            continue;
        }

        string start_node = itr->first;
        // cout << "start node: "<< start_node << endl;

        if(start_checked[start_node] == true){
            // cout << "start node was checked" << endl; 
            continue;
        }

        start_checked[start_node] = true;

        unordered_map<string, bool> visited;
        visited[start_node] = true;

        queue<string> q;

        //// cout << "start node: " << start_node << endl;

        vector<string> next = zoo_topo[start_node];

        // cout << "next node size: " << next.size() << endl;

        for(int i=0;i<next.size();i++){
            // cout << "next: " << next[i] << endl;
            q.push(next[i]);
        }

        while(!q.empty()){
            string n = q.front();
            q.pop();

            if(visited[n] == true){
                continue;
            }
            
            visited[n] = true;

            if(node_map_chk[n] == true){
                //se_topo[start_node].push_back(n);
                if(check_same_node(se_topo[start_node], n)){
                    se_topo[start_node].push_back(n);
                }
            }
            else{
                vector<string> next_nodes = zoo_topo[n];
                for(int j=0;j<next_nodes.size();j++){
                    q.push(next_nodes[j]);
                }
            }
        }
    }

    show_se_subgraph();   
}

void record_nodes_per_group()
{
    // cout << "start record_nodes_per_group()" << endl;

    for(int i=0;i<vec_path_group.size();i++){
        struct path_group *pg = &vec_path_group[i];

        for(int j=pg->path_list.size()-1;j>=0;j--){
            vector<string> pl = pg->path_list[j];
            bool f = false;

            for(int k=0;k<pl.size();k++){
                //string cur_node = as_number_to_router[stol(pl[k])][0]->name;
                string cur_node = pl[k];
                //// cout << "cur_node: " << cur_node << endl;

                if(pg->nodes[cur_node] == false){
                    //// cout << "record node: " << cur_node << endl;
                    pg->nodes[cur_node] = true;
                    f = true;
                }
            }

            /*
            if(f == false){
                break;
            }
            */
        }
    }

    // cout << "done recor_nodes_per_group" << endl;
}

vector<string> convert_upath_to_devpath(vector<unsigned int> spath)
{
    vector<string> devpath;

    for(int i=0;i<spath.size();i++){
        devpath.push_back(as_number_to_router[spath[i]][0]->name);
    }

    return devpath;
}

void create_path_group(vector<vector<unsigned int>> pv)
{
    // cout << "create_path_group()" << endl;
    // cout << "pv.size(): " << pv.size() << endl;
    for(int i=0;i<pv.size();i++){
        // cout << "-----------" << endl;
        // cout << "ith: " << i << endl;

        vector<string> cur_path = convert_upath_to_devpath(pv[i]);
        //vector<string> cur_path = convert_upath_to_spath(pv[i]);
        // cout << "cur_path: ";
        for(int j=0;j<cur_path.size();j++){
            // cout << cur_path[j] << " ";
        }
        // cout << endl;

        int b_size = vec_path_group.size();

        if(vec_path_group.size() != 0){
            for(int j=0;j<b_size;j++){
                // cout << "jth shortest path: ";
                for(int k=0;k<vec_path_group[j].shortest_path.size();k++){
                    // cout << vec_path_group[j].shortest_path[k] << " ";
                }
                // cout << endl;
                if(cur_path == vec_path_group[j].shortest_path){
                    // cout << "This is same as the jth shortest path" << endl;
                    break;
                }

                if(is_path_included_str(cur_path, vec_path_group[j].shortest_path)){
                    vec_path_group[j].path_list.push_back(cur_path);

                    // cout << "add path to path group" << endl;
                    // cout << "shortest path: ";
                    for(int k=0;k<vec_path_group[j].shortest_path.size();k++){
                        // cout << vec_path_group[j].shortest_path[k] << " ";
                    }
                    // cout << endl;

                    break;
                }

                if(j==b_size-1){
                    // cout << "j==size" << endl;
                    struct path_group pg;
                    pg.shortest_path = cur_path;
                    pg.path_list.push_back(cur_path);

                    vec_path_group.push_back(pg);

                    // cout << "create new path group" << endl;
                }
            }
        }
        else{
            // cout << "create new path group" << endl;
            struct path_group pg;
            pg.shortest_path = cur_path;
            pg.path_list.push_back(cur_path);

            vec_path_group.push_back(pg);
        }
    }
    // cout << "done create_path group" << endl;
    // cout << "------------------" << endl;
}

void compute_connected_nodes()
{
    // cout << "start to compute_connected_nodes()" << endl;

    for(int i=0;i<vec_path_group.size();i++){
        struct path_group *pg = &vec_path_group[i];
        // cout << "path group: "  << i << endl;
        // cout << "shortest path: ";
        for(int j=0;j<pg->shortest_path.size();j++){
            // cout << pg->shortest_path[j] << " ";
        }
        // cout << endl;

        // cout << "pg->nodes.size: " << pg->nodes.size() << endl;

        unordered_map<string, bool> visited;

        for(int j=pg->path_list.size()-1;j>=0;j--){
            for(int k=0;k<pg->path_list[j].size();k++){
                string cur_node = pg->path_list[j][k];

                if(visited[cur_node] == true){
                    continue;
                }

                visited[cur_node] = true;

                if(pg->nodes[cur_node] == false){
                    // cout << "pg->nodes[cur_node] == false" << endl;
                    continue;
                }

                vector<string> adj = zoo_topo[cur_node];

                for(int r=0;r<adj.size();r++){
                    // cout << "adj: " << adj[r] << endl;
                    //もし自分のpath_group内のノード以外と隣接していたらmark
                    if(k>0 && k<pg->path_list[j].size()-1){
                        string nei_first = pg->path_list[j][k-1];
                        string nei_second = pg->path_list[j][k+1];

                        if(adj[r] == nei_first || adj[r] == nei_second){
                            continue;
                        }

                        pg->connected_node[cur_node] = true;
                        break;
                    }
                    else if(k == 0){
                        string nei_second = pg->path_list[j][k+1];

                         if(adj[r] == nei_second){
                            continue;
                        }

                        pg->connected_node[cur_node] = true;
                        break;
                    }
                    else if(k==pg->path_list[j].size()-1){
                        //k != 0
                        string nei_first = pg->path_list[j][k-1];

                        if(adj[r] == nei_first){
                            continue;
                        }

                        pg->connected_node[cur_node] = true;
                        break;
                    }
                }
            }
        }
    }
}

void compute_boundary_nodes()
{
    //se_subgraph上で、境界面の計算をする

    //各グループの各ノードの隣接関係を見る
    //自グループ以外と隣接していたら、それは境界面なので含む必要がある

    // cout << "start to compute_boundary_nodes()" << endl;

    for(int i=0;i<vec_path_group.size();i++){
        struct path_group *pg = &vec_path_group[i];
        // cout << "path group: "  << i << endl;
        // cout << "shortest path: ";
        for(int j=0;j<pg->shortest_path.size();j++){
            // cout << pg->shortest_path[j] << " ";
        }
        // cout << endl;

        // cout << "pg->nodes.size: " << pg->nodes.size() << endl;

        unordered_map<string, bool> visited;

        for(auto itr=pg->nodes.begin();itr!=pg->nodes.end();itr++){
            string cur_node = itr->first;

            // cout << "cur node: " << cur_node << endl;

            if(visited[cur_node] == true){
                continue;
            }
            visited[cur_node] = true;

            if(pg->nodes[cur_node] == false){
                // cout << "pg->nodes[cur_node] == false" << endl;
                continue;
            }

            if(pg->boundary_node[cur_node] == true){
                // cout << "pg->boundary_node[cur_node] == true" << endl;
                continue;
            }

            vector<string> adj = se_topo[cur_node];

            // cout << "adj based on se_topo: " << adj.size() << endl;
            
            for(int j=0;j<adj.size();j++){
                // cout << "adj: " << adj[j] << endl;
                if(pg->nodes[adj[j]] == false){
                    // cout << "add boundary node: " << cur_node << endl;
                    pg->boundary_nodes.push_back(cur_node);
                    pg->boundary_node[cur_node] = true;
                    break;
                }
            }
        }
    }

    // cout << "done compute_boundary_nodes" << endl;
}



void add_spec(vector<string> path, struct v4_route v4r)
{
    struct specification spec;
    spec.forwarding_path = path;
    spec.route = v4r;

    //add path to spec
    //print_str_path(path);
    //print_devpath(path);

    //specに追加
    spec_list.push_back(spec);
}

bool is_path_in_list(struct path_group *pg, vector<string> path)
{
    for(int i=0;i<pg->path_list.size();i++){
        if(pg->path_list[i].size() == path.size()){
            bool f = true;
            for(int j=0;j<path.size();j++){
                if(pg->path_list[i][j] != path[j]){
                    f = false;
                    break;
                }
            }

            if(f == true){
                return true;
            }
        }
    }
}

void check_coverage(struct path_group *pg, unordered_map<string, bool> visited)
{
    for(auto itr=pg->boundary_node.begin();itr!=pg->boundary_node.end();itr++){
        if(itr->second == false){
            continue;
        }

        if(visited[itr->first] == false){
            // cout << "Fatal error at checK_coveraeg()" << endl;
            // cout << "Non coverage node is " << itr->first << endl;
            exit(-1);   
        }
    }
}

struct boundary_tuple{
    string cur_node;
    vector<string> path;
    unordered_map<string, bool> visited;
};

bool is_boundary_node_only_path(unordered_map<string, bool> checked_boundary_node, vector<string> cur_path)
{
    for(int i=0;i<cur_path.size();i++){
        string cur_node = cur_path[i];

        if(checked_boundary_node[cur_node] == false){
            return false;
        }
    }

    return true;
}

bool is_new_boundary_nodes(unordered_map<string, bool> boundary_node_checked, vector<string> path)
{
    for(int i=0;i<path.size();i++){
        //// cout << "i th: " << path[i] << endl;
        if(boundary_node_checked[path[i]] == false){
            //// cout << "new !!" << endl;
            return true;
        }
    }

    return false;
}

vector<string> get_connected_nodes(unordered_map<string, bool> cnode_map, vector<string> path)
{
    vector<string> cur_cnodes;

    // cout << "get_connected_nodes()" << endl;

    unordered_map<string, bool> visited;

    for(int i=0;i<path.size();i++){
        //// cout << "i th: " << path[i] << endl;
        if(cnode_map[path[i]] == true && visited[path[i]] == false){
            //// cout << "new !!" << endl;
            //boundary_node_checked[path[i]] = true;
            visited[path[i]] = true;
            cur_cnodes.push_back(path[i]);
        }
    }

    return cur_cnodes;
}

vector<string> get_boundary_nodes(unordered_map<string, bool> boundary_node_map, vector<string> path)
{
    vector<string> cur_boundary_nodes;

    // cout << "get_boudnary_nodes()" << endl;

    unordered_map<string, bool> visited;

    for(int i=0;i<path.size();i++){
        //// cout << "i th: " << path[i] << endl;
        if(boundary_node_map[path[i]] == true && visited[path[i]] == false){
            //// cout << "new !!" << endl;
            //boundary_node_checked[path[i]] = true;
            visited[path[i]] = true;
            cur_boundary_nodes.push_back(path[i]);
        }
    }

    return cur_boundary_nodes;
}

vector<string> get_new_connected_nodes(unordered_map<string, bool> connected_node_checked_map, vector<string> path)
{
    vector<string> new_connected_nodes;
    // cout << "get_new_connected_nodes()" << endl;

    unordered_map<string, bool> visited;

    for(int i=0;i<path.size();i++){
        if(connected_node_checked_map[path[i]] == false && visited[path[i]] == false){
            visited[path[i]] = true;
            new_connected_nodes.push_back(path[i]);
        }
    }

    return new_connected_nodes;
}

vector<string> get_new_boundary_nodes(unordered_map<string, bool> boundary_node_checked_map, vector<string> path)
{
    vector<string> new_boundary_nodes;
    // cout << "get_new_boudnary_nodes()" << endl;

    unordered_map<string, bool> visited;

    for(int i=0;i<path.size();i++){
        if(boundary_node_checked_map[path[i]] == false && visited[path[i]] == false){
            visited[path[i]] = true;
            new_boundary_nodes.push_back(path[i]);
        }
    }

    return new_boundary_nodes;
}

bool path_contain_all_boundary_nodes(vector<string> path, vector<string> new_boundary_nodes)
{
    for(int i=0;i<new_boundary_nodes.size();i++){
        string cur_node = new_boundary_nodes[i];

        bool flag = false;

        for(int j=0;j<path.size();j++){
            if(path[j] == cur_node){
                flag = true;
                break;
            }
        }

        if(flag == false){
            return false;
        }
    }

    return true;
}

bool path_contain_all_connected_nodes(vector<string> path, vector<string> new_connected_nodes)
{
    for(int i=0;i<new_connected_nodes.size();i++){
        string cur_node = new_connected_nodes[i];

        bool flag = false;

        for(int j=0;j<path.size();j++){
            if(path[j] == cur_node){
                flag = true;
                break;
            }
        }

        if(flag == false){
            return false;
        }
    }

    return true;
}

bool is_minimal_path_for_connected_nodes(vector<vector<string>> path_list, int idx, vector<string> new_connected_nodes)
{
    for(int i=idx;i>=0;i--){
        if(i < 0){
            continue;
        }

        vector<string> cur_path = path_list[i];

        if(path_contain_all_connected_nodes(cur_path, new_connected_nodes)){
            return false;
        }
    }

    return true;
}

bool is_minimal_path_for_boundary_nodes(vector<vector<string>> path_list, int idx, vector<string> new_boundary_nodes)
{
    for(int i=idx;i>=0;i--){
        if(i < 0){
            continue;
        }

        vector<string> cur_path = path_list[i];

        if(path_contain_all_boundary_nodes(cur_path, new_boundary_nodes)){
            return false;
        }
    }

    return true;
}

vector<vector<string>> create_cnode_minimal_path_set(struct path_group *pg)
{
    int connected_node_num = 0;
    unordered_map<string, bool> cnode = pg->connected_node;
    vector<vector<string>> path_list = pg->path_list;
    vector<vector<string>> path;

    // cout << "start to create_cnode_minimal_path_set(struct path_group *pg)" << endl;

    for(auto itr=cnode.begin();itr!=cnode.end();itr++){
        if(itr->second == true){
            connected_node_num++;
        }
    }

    unordered_map<string, bool> cnode_checked;

    // cout << "boundary node num:" << connected_node_num << endl;

    // cout << "path_list size: " << path_list.size() << endl;

    for(int i=path_list.size()-1;i>=0;i--){
        if(cnode_checked.size() == connected_node_num){
            break;
        }

        // cout << "cur_path" << endl;
        print_devpath(path_list[i]);

        unordered_map<string, bool> tmp_cnode_checked = cnode_checked;

        vector<string> cur_path = path_list[i];

        vector<string> cur_cnodes = get_connected_nodes(cnode, cur_path);
        vector<string> new_cnodes = get_new_connected_nodes(tmp_cnode_checked, cur_cnodes);
        
        if(new_cnodes.size() > 0){
            if(is_minimal_path_for_connected_nodes(path_list, i-1, new_cnodes)){
                //record path
                // cout << "record this path" << endl;
                path.push_back(cur_path);

                //delete node
                for(int t=0;t<new_cnodes.size();t++){
                    cnode_checked[new_cnodes[t]] = true;
                }
            }
        }
    }

    if(cnode_checked.size() != connected_node_num){
        // cout << "boundary_node_checked != boundary_node_num" << endl;

        if(cnode_checked.size() > connected_node_num){
            // cout << "boundary_node_checked.size() > boundary_node_num" << endl;
        }
        else{
            // cout << "Fatal error: some boundary node missing !" << endl;
        }

        for(auto itr=cnode.begin();itr!=cnode.end();itr++){
            if(itr->second == false){
                continue;
            }

            if(cnode_checked[itr->first] == false){
                // cout << "unchecked node: " << itr->first << endl;
            }
        }
        
        exit(-1);
    }

    return path;
}

vector<vector<string>> create_boundary_path_sets(struct path_group *pg)
{
    vector<vector<string>> path_list = pg->path_list;
    list<string> boundary_nodes = pg->boundary_nodes;
    unordered_map<string, bool> boundary_node = pg->boundary_node;
    vector<vector<string>> path;

    //path_listを逆からるーぷ
    //boundary_nodeからのみ構成されるパスの中で最長を見つける
    //そこのパスの中に存在するboundary_nodeは除外
    //boundary_nodeがなくなるまで続ける
    int boundary_node_num = boundary_nodes.size();

    if(boundary_node_num == 0){
        // cout << "no boundary node" << endl;
        vector<vector<string>> tmp_list;
        tmp_list.push_back(pg->shortest_path);
        return tmp_list;
    }

    unordered_map<string, bool> boundary_node_checked;
    /*
    vector<string> checked_boundary_nodes;

    for(auto itr=pg->boundary_node.begin();itr!=pg->boundary_node.end();itr++){
        is_boundary_node_checked[itr->first] = false;
    }
    */

   // cout << "boundary node num:" << boundary_node_num << endl;

    // cout << "path_list size: " << path_list.size() << endl;
    
    for(int i=path_list.size()-1;i>=0;i--){
        if(boundary_node_checked.size() == boundary_node_num){
            break;
        }

        // cout << "cur_boundary_path" << endl;
        print_devpath(path_list[i]);

        unordered_map<string, bool> tmp_bnode_checked = boundary_node_checked;

        vector<string> cur_path = path_list[i];

        //boundary_nodeからのみ構成されるパスか、かつすでに内包されているboundary_node以外を含むか
        //↑ 条件変更: 新しいboundary_nodeを含む && そのパスが内包するboundary_nodesを含む最小のパスであるか
        //if(is_boundary_node_only_path(pg->boundary_node, cur_path) && is_new_boundary_nodes(tmp_bnode_checked, cur_path)){
        vector<string> cur_boundary_nodes = get_boundary_nodes(boundary_node, cur_path);
        vector<string> new_boundary_nodes = get_new_boundary_nodes(tmp_bnode_checked, cur_boundary_nodes);
        if(new_boundary_nodes.size() > 0){
            if(is_minimal_path_for_boundary_nodes(path_list, i-1, new_boundary_nodes)){
                //record path
                // cout << "record this path" << endl;
                path.push_back(cur_path);

                //delete node
                for(int t=0;t<new_boundary_nodes.size();t++){
                    boundary_node_checked[new_boundary_nodes[t]] = true;
                }
            }
        }
    }

    if(boundary_node_checked.size() != boundary_node_num){
        /*
        if(boundary_node_num > 0 && path_list.size() > 0){
            vector<vector<string>> tmp_list;
            tmp_list.push_back(pg->shortest_path);
            return tmp_list;
        }
        else if(path_list.size() == 0){
            // cout << "Path list is 0 !!" << endl;
        }*/


        // cout << "boundary_node_checked != boundary_node_num" << endl;

        if(boundary_node_checked.size() > boundary_node_num){
            // cout << "boundary_node_checked.size() > boundary_node_num" << endl;
        }
        else{
            // cout << "Fatal error: some boundary node missing !" << endl;
        }

        for(auto itr=boundary_nodes.begin();itr!=boundary_nodes.end();itr++){
            if(boundary_node_checked[*itr] == false){
                // cout << "unchecked node: " << *itr << endl;
            }
        }
        
        exit(-1);
    }

    return path;

    //boundary_nodeを被覆するpathを作る
    //path_listからその最小pathを作る

    unordered_map<string, bool> visited;

    vector<string> stp = pg->shortest_path;

    string stp_last_node = stp[stp.size()-1];

    /*
    pair<string, vector<string>> qp;
    qp.first = stp_last_node;
    qp.second = stp;
    */

    struct boundary_tuple bt;
    bt.cur_node = stp_last_node;
    bt.path = stp;
    
    for(int i=0;i<stp.size();i++){
        visited[stp[i]] = true;
    }

    bt.visited = visited;


    //queue<pair<string, vector<string>>> q;
    //queue<tuple<string, vector<string>, unordered_map<string, bool>>> qq;

    queue<struct boundary_tuple> q;

    q.push(bt);

    //連続性、shortest pathがすべてのpathに内包されている、がかぎ
    //これのおかげで被覆アルゴリズムを考えずに、単純なグラフ探索 in se_topoですむ
    while(!q.empty()){
        struct boundary_tuple cur_bt = q.front();
        q.pop();

        vector<string> adj = se_topo[cur_bt.cur_node];

        for(int i=0;i<adj.size();i++){
            string next = adj[i];

            if(cur_bt.visited[next] == false && pg->boundary_node[next] == true){
                pair<string, vector<string>> tmp_qp;
                cur_bt.cur_node = next;
                cur_bt.path.push_back(next);
                cur_bt.visited[next] = true;

                //tmp_pq.secondのパスが、path_listに存在すればpusu
                //そうでなければスルー
                q.push(cur_bt);
            }
            else{

            }
        }
    }
    
    //check_coverage(pg, );
    
    // cout << "------------------" << endl;
    // cout << "boundary path set" << endl;
    for(int i=0;i<path.size();i++){
        add_spec(path[i], pg->prefix);
    }
    // cout << "--------------------" << endl;
}

int path_match_i(vector<string> path, vector<vector<string>> path_list)
{
    for(int i=0;i<path_list.size();i++){
        if(path.size() != path_list[i].size()){
            continue;
        }

        int j;
        for(j=0;j<path_list[i].size();j++){
            if(path[i] != path_list[i][j]){
                break;
            }
        }

        if(j != path_list[i].size()){
            continue;
        }

        return i;
    }

    return -1;
}


vector<vector<string>> delete_path(vector<vector<string>> path_list, vector<string> delete_path)
{
    int idx = path_match_i(delete_path,path_list);

    if(idx != -1){
        path_list.erase(path_list.begin() + idx);
    }

    return path_list;
}


bool boundary_path_match(vector<string> path, vector<vector<string>> boundary)
{
    for(int i=0;i<boundary.size();i++){
        if(path.size() > boundary[i].size()){
            continue;
        }

        int j=0;
        for(j=0;j<path.size();j++){
            if(path[j] != boundary[i][j]){
                break;
            }
        }

        if(j == path.size()){
            return true;
        }
    }

    return false;
}


bool path_unmatch(vector<string> path, vector<vector<string>> boundary)
{
    for(int i=0;i<boundary.size();i++){
        if(path.size() != boundary[i].size()){
            continue;
        }

        int j=0;
        for(j=0;j<boundary[i].size();j++){
            if(path[j] != boundary[i][j]){
                break;
            }
        }

        if(j == boundary[i].size()){
            return false;
        }
    }

    return true;
}

vector<vector<string>> delete_boundary_path(vector<vector<string>> path_list, vector<vector<string>> bounadry_path_list)
{
    vector<vector<string>> result;

    vector<int> idx;

    for(int i=0;i<path_list.size();i++){
        vector<string> path = path_list[i];

        if(path_unmatch(path, bounadry_path_list)){
            idx.push_back(i);
        }
    }

    for(int i=0;i<idx.size();i++){
        //// cout << "add: " << path_list[idx[i]] << endl;
        // cout << "added path: ";
        print_devpath(path_list[idx[i]]);
        result.push_back(path_list[idx[i]]);
    }

    return result;
}

vector<vector<string>> create_neighbor_path_sets(struct path_group *pg, vector<vector<string>> boundary_path_sets)
{
    vector<vector<string>> comp_path;
    unordered_map<string, bool> nodes = pg->nodes;
    unordered_map<string, bool> boundary_node = pg->boundary_node;
    vector<vector<string>> neighbor_path_list;

    //path_setのパスから直接到達可能なノードを含むパスを探す
    //boundary_pathを延伸してpathになりうる、かつ、boundary_pathに内包されていない@
    //その場合はbefore_repairのpathを追加する

    //path_listからboundary_pathを取り除く
    //not boundary nodeで、boundary_nodeと隣接して、尚且つ、隣接するノードまでの部分パスに内包されないを見つけて、comp_pathに追加 (見つかったパスをreplicateしたpath_listから取り除いていけば、次のステップで使える)
    //comp_path内のパスがなくなるまで、上と同じことをやり続ける

    //pg->path_listから、(i) boundary path setsではない && (ii) boundary nodesと隣接するノードを含む、2つを満たすパスを見つけて追加 

    // cout << "goto delete_boundary_path" << endl;
    neighbor_path_list = delete_boundary_path(pg->path_list, boundary_path_sets);

    if(neighbor_path_list.size() == 0){
        // cout << "neighbor path list size == 0" << endl;
        return comp_path;
    }

    // cout << "neighbor_path_list" << endl;
    for(int i=0;i<neighbor_path_list.size();i++){
        // cout << "i th: " << i << endl;
        print_devpath(neighbor_path_list[i]);
    }

    unordered_map<string, bool> visited;
    vector<vector<string>> next_list = neighbor_path_list;

    //もっと早くできる, loopをboundary_pathに

    //↓ boundary path以外で、境界ノードと隣接していて、なおかつ、これまでのstepで生成されたパス仕様にない方がされない

    unordered_map<string, bool> comp_nodes;
    //vector<int> delete_idx;

    for(int i=0;i<neighbor_path_list.size();i++){
        // cout << "cur_check_path: " << endl;
        print_devpath(neighbor_path_list[i]);

        for(int j=neighbor_path_list[i].size()-1;j>=0;j--){
            bool done = false;

            // cout << "check: " << neighbor_path_list[i][j] << endl;

            if(visited[neighbor_path_list[i][j]] == true){
                break;
            }

            visited[neighbor_path_list[i][j]] = true;

            if(boundary_node[neighbor_path_list[i][j]] == true){
                continue;
            }

            vector<string> adj = se_topo[neighbor_path_list[i][j]];

            for(int k=0;k<adj.size();k++){
                // cout << "adj: " << adj[k] << endl;
                if( j > 0 && adj[k] == neighbor_path_list[i][j-1]){
                    // cout << "j: " << j << endl;
                    // cout << "adj[k] == neighbor_path_list: " << endl;
                    continue;
                }

                if(boundary_node[adj[k]] == true){
                    vector<string> cut_path;
                    for(int f=0;f<=j;f++){
                        comp_nodes[neighbor_path_list[i][f]] = true;
                        cut_path.push_back(neighbor_path_list[i][f]);
                    }

                    if(boundary_path_match(cut_path, boundary_path_sets)){
                        // cout << "cut_path is included by boudnary_path" << endl;
                        done = true;
                        break;
                    }

                    // cout << "comp_path is added" << endl;
                    print_devpath(cut_path);

                    if(cut_path.size() == 0){
                        // cout << "cut_path size == 0" << endl;
                        exit(-1);
                    }
                    comp_path.push_back(cut_path);

                    delete_path(next_list, neighbor_path_list[i]);
                    done = true;
                    break;
                    //visited[neighbor_path_list[i][j]] = true;
                }
            }

            if(done){
                break;
            }
        }
    }

    if(next_list.size() == 0 || comp_path.size() == 0){
        return comp_path;
    }

    queue<vector<string>> q;

    
    for(int i=0;i<next_list.size();i++){
        q.push(next_list[i]);
    }

    /*
    for(int i=0;i<comp_path.size();i++){
        q.push(comp_path[i]);
    }
    */

   // cout << "second trial !" << endl;

    unordered_map<string, bool> n_visited;

    while(!q.empty()){
        vector<string> cur_path = q.front();
        q.pop();

        for(int i=cur_path.size()-1;i>=0;i--){
            bool done = false;

            if(n_visited[cur_path[i]] == true){
                break;
            }

            n_visited[cur_path[i]] = true;

            if(boundary_node[cur_path[i]] == true){
                continue;
            }

            vector<string> adj = se_topo[cur_path[i]];

            for(int k=0;k<adj.size();k++){
                if( i > 0 && adj[k] == cur_path[i-1]){
                    continue;
                }

                if(comp_nodes[adj[k]] == true){
                    vector<string> cut_path;
                    for(int f=0;f==i;f++){
                        cut_path.push_back(cur_path[f]);
                        comp_nodes[cur_path[f]] = true;
                    }

                    if(boundary_path_match(cut_path, boundary_path_sets)){
                        // cout << "cut_path is included by boudnary_path" << endl;
                        done = true;
                        break;
                    }

                    if(boundary_node[adj[k]] == true){
                        // cout << "tabun strange thing happen !" << endl;
                        continue;
                    }

                    // cout << "comp_path is added";
                    print_devpath(cut_path);

                    if(cut_path.size() == 0){
                        // cout << "cut_path size == 0" << endl;
                        exit(-1);
                    }

                    comp_path.push_back(cut_path);
                    q.push(cut_path);
                    done = true;
                }
            }

            if(done){
                break;
            }
        }
    }

    /*
    // cout << "------------------" << endl;
    // cout << "neighbor path set" << endl;
    for(int i=0;i<comp_path.size();i++){
        add_spec(comp_path[i], pg->prefix);
    }
    */


    return comp_path;

    for(int i=0;i<boundary_path_sets.size();i++){
        vector<string> cur_boundary_path = boundary_path_sets[i];



        //boundary_path内のノードで、path_listに含まれているノード
        //boundary_pathに内包されていない@

        //boundary_nodeじゃない、かつ、path_listに含まれているノード
        //その場合は、そのノードを含む最長のパス(before rpeair)を追加
        
        string last_node = cur_boundary_path[cur_boundary_path.size()-1];

        vector<string> adj = se_topo[last_node];

        for(int j=0;j<adj.size();j++){
            unordered_map<string, bool> visited;
            //adj[j]がboundary_nodeじゃない && 既存specにないパスに含まれる
            //その場合は、(複数ある場合もある)そのpathを後ろから探索して、後ろから初めてboundary_nodeに隣接するpathを見つける
            //それをpathに入れる、
         
        }
    }

    //boundary_nodeじゃないcomp_pathのノードから
    //comp_pathのノードと隣接する && boundary_nodeじゃない && この領域内(pg->nodesでチェック) && 既存のcomp_pathに内包されない
    //ならば、

    return comp_path;
}

//boundary_node以外のbefore repairのbest path内に含まれてないノードが仕様に存在するパスのエンドノードの場合は、そいつも追加する必要がある
unsigned int find_minimal_path_sets_coverting_cnodes()
{
    vector<string> path_spec;

    // cout << "start to find_minimal_path_sets_coverting_cnodes()" << endl;
    // cout << "vec_group size: " << vec_path_group.size() << endl;

    unsigned int comp = 0;

    for(int i=0;i<vec_path_group.size();i++){
        struct path_group *pg = &vec_path_group[i];
        unordered_map<string, bool> cnode_map = pg->connected_node;

        //考えること
        //1. boundary
        //2. nullからパスが新しく生えた時

        //pg->path_listには短い順に入っている
        //boundary nodeを含んでいるパスを順に追加していく
        //boundary nodeが全部埋まったら、ok
        //部分パスがあったら圧縮 (2-4は2-4-7に含まれるので、2-4-7だけで良い)


        //boundaryに含まれているノード
        //boudaryに含まれているノードを含む最長パスを仕様にする
        //含まれているboundaryノードはチェック
        //最長パスの部分パスから到達可能な同path groupのパスを見つけて仕様に入れる

        vector<vector<string>> path_list = pg->path_list;

        //path_listから、1つでもboundary-nodesを含むパスを取り出して、パス集合を作る
        // cout << "------------------" << endl;
        // cout << "start to create_minimal_path_sets()" << endl;
        vector<vector<string>> minimal_spec = create_cnode_minimal_path_set(pg);
        comp += minimal_spec.size();
        
        // cout << endl;
        cout << "minimal path set" << endl;
        for(int j=0;j<minimal_spec.size();j++){
            add_spec(minimal_spec[j], pg->prefix);
        }
        // cout << "------------------" << endl;
    }

    // cout << "done find_minimal_path_sets_coverting_cnodes()" << endl;

    return comp;
}

unsigned int find_ospf_minimal_path_sets_covering_cnodes()
{
    vector<string> path_spec;

    // cout << "start to find_minimal_path_sets_coverting_cnodes()" << endl;
    // cout << "vec_group size: " << vec_path_group.size() << endl;

    unsigned int comp = 0;

    vector<vector<string>> all_spec;

    for(int i=0;i<vec_path_group.size();i++){
        struct path_group *pg = &vec_path_group[i];
        unordered_map<string, bool> cnode_map = pg->connected_node;

        //考えること
        //1. boundary
        //2. nullからパスが新しく生えた時

        //pg->path_listには短い順に入っている
        //boundary nodeを含んでいるパスを順に追加していく
        //boundary nodeが全部埋まったら、ok
        //部分パスがあったら圧縮 (2-4は2-4-7に含まれるので、2-4-7だけで良い)


        //boundaryに含まれているノード
        //boudaryに含まれているノードを含む最長パスを仕様にする
        //含まれているboundaryノードはチェック
        //最長パスの部分パスから到達可能な同path groupのパスを見つけて仕様に入れる

        vector<vector<string>> path_list = pg->path_list;

        //path_listから、1つでもboundary-nodesを含むパスを取り出して、パス集合を作る
        // cout << "------------------" << endl;
        // cout << "start to create_minimal_path_sets()" << endl;
        vector<vector<string>> minimal_spec = create_cnode_minimal_path_set(pg);
        for(int t=0;t<minimal_spec.size();t++){
            all_spec.push_back(minimal_spec[t]);
        }
        /*
        minimal_spec = delete_subpaths(minimal_spec);
        comp += minimal_spec.size();
        
        // cout << endl;
        // cout << "minimal path set" << endl;
        for(int j=0;j<minimal_spec.size();j++){
            add_spec(minimal_spec[j], pg->prefix);
        }
       // cout << "------------------" << endl;
        */
    }

    sort(all_spec.begin(), all_spec.end());

    all_spec = delete_subpaths(all_spec);
    comp = all_spec.size();

    // cout << "minimal path set" << endl;
    for(int j=0;j<all_spec.size();j++){
        add_spec(all_spec[j], vec_path_group[0].prefix);
    }

    // cout << "------------------" << endl;


    // cout << "done find_minimal_path_sets_coverting_cnodes()" << endl;

    return comp;
}


//boundary_node以外のbefore repairのbest path内に含まれてないノードが仕様に存在するパスのエンドノードの場合は、そいつも追加する必要がある
unsigned int create_no_se_spec()
{
    vector<string> path_spec;

    // cout << "start to create_no_se_spec()" << endl;
    // cout << "vec_group size: " << vec_path_group.size() << endl;

    unsigned int comp = 0;

    for(int i=0;i<vec_path_group.size();i++){
        struct path_group *pg = &vec_path_group[i];
        list<string> bn = pg->boundary_nodes;
        unordered_map<string, bool> b_nodes = pg->boundary_node;

        //考えること
        //1. boundary
        //2. nullからパスが新しく生えた時

        //pg->path_listには短い順に入っている
        //boundary nodeを含んでいるパスを順に追加していく
        //boundary nodeが全部埋まったら、ok
        //部分パスがあったら圧縮 (2-4は2-4-7に含まれるので、2-4-7だけで良い)


        //boundaryに含まれているノード
        //boudaryに含まれているノードを含む最長パスを仕様にする
        //含まれているboundaryノードはチェック
        //最長パスの部分パスから到達可能な同path groupのパスを見つけて仕様に入れる

        vector<vector<string>> path_list = pg->path_list;

        //path_listから、1つでもboundary-nodesを含むパスを取り出して、パス集合を作る
        // cout << "------------------" << endl;
        // cout << "start to create_boundary_path_sets()" << endl;
        vector<vector<string>> bn_spec = create_boundary_path_sets(pg);
        comp += bn_spec.size();
        
        // cout << endl;
        // cout << "boundary path set" << endl;
        for(int i=0;i<bn_spec.size();i++){
            add_spec(bn_spec[i], pg->prefix);
        }
        // cout << "------------------" << endl;

        //path_setのパスから直接到達可能なノードを含むパスを探す
        //boundary_pathを延伸してpathになりうる、かつ、boundary_pathに内包されていない
        //その場合はbefore_repairのpathを追加する
        // cout << "------------------" << endl;
        // cout << "start to create_neighbor_path_sets()" << endl;
        vector<vector<string>> complementary_spec = create_neighbor_path_sets(pg, bn_spec);


        // cout << endl;
        // cout << "neighbor path set" << endl;
        for(int i=0;i<complementary_spec.size();i++){
            add_spec(complementary_spec[i], pg->prefix);
        }
         // cout << "------------------" << endl;

        comp += complementary_spec.size();
    }

    // cout << "done create_no_se_spec()" << endl;

    return comp;
}

void init_vec_path_group()
{
    vector<struct path_group> tmp_vec_path_group;
    vec_path_group = tmp_vec_path_group;
    unordered_map<string, vector<string>> tmp_topo;
    se_topo = tmp_topo;
}

void new_create_side_effect_path_group_per_prefix()
{
    init_vec_path_group();

    total_number_of_compressed_path.clear();
    total_number_of_uncompressed_path.clear();
    
    cout << "start to create side effect group" << endl;
    vector<vector<unsigned int>> path_vec;

    struct v4_route r;

    unsigned int uncomp = 0;

    //side effect infoからbefore repairのパスを取り出す
    for(auto itr=cchange_to_se_info.begin();itr!=cchange_to_se_info.end();itr++){
        vector<side_effect_info> sei = itr->second;
        
        vector<unsigned int> prev_path;
        for(int i=0;i<sei.size();i++){
            vector<unsigned int> b_path = sei[i].before_repair_path;

            if(prev_path == b_path){
                continue;
            }

            prev_path = b_path;

            r = sei[i].route;

            // cout << "Prefix: ";
            cur_prefix = bitset_to_readable_addr_str(sei[i].route.prefix, sei[i].route.prefix_len);
            
            // cout << "before repair path: ";
            print_as_path(b_path);

            path_vec.push_back(b_path);

            uncomp++;
        }
    }

    cout << "uncomp size: " << uncomp << endl;

    total_number_of_uncompressed_path.push_back(uncomp);


    //pathを短い順にする for path grouping
    cout << "do sort" << endl;
   sort(path_vec.begin(),path_vec.end(),[](const auto& a, const auto& b){
    return a.size() < b.size();
   });

   
   cout << "create_path_group" << endl;
    create_path_group(path_vec);
    cout << "record" << endl;
    record_nodes_per_group();

    /*
    create_se_subgraph();

    //boundary_nodeを計算する
    compute_boundary_nodes();

    //boundary path setとneighbor path setを計算して、no se specを作る
    unsigned int comp = create_no_se_spec();
    */

    //new !
    //仕様中のパスにおいて、自分を含むパス内のノード以外と他のノードと隣接するノードを見つける
    cout << "compute connected" << endl;
    compute_connected_nodes();
    //そのノードを全てカバーする最小数のpath setを見つける
    /*find_minimal_path_set_covering_connected_nodes()*/

    cout << "find minmal" << endl;
    unsigned int comp = find_minimal_path_sets_coverting_cnodes();

    cout << "comp size: " << comp << endl;

    total_number_of_compressed_path.push_back(comp);

    compressed_rate.push_back((double)uncomp/comp);
}

//全部集めてから一気にこれを回す？
void new_create_ospf_side_effect_path_group_per_prefix()
{
    init_vec_path_group();

    total_number_of_compressed_path.clear();
    total_number_of_uncompressed_path.clear();
    
    // cout << "start to create side effect group" << endl;
    vector<vector<unsigned int>> path_vec;

    struct v4_route r;

    unsigned int uncomp = 0;
    unordered_map<string, bool> tmp_map;

    //side effect infoからbefore repairのパスを取り出す
    for(auto itr=cchange_to_se_info.begin();itr!=cchange_to_se_info.end();itr++){
        vector<side_effect_info> sei = itr->second;
        
        vector<unsigned int> prev_path;
        for(int i=0;i<sei.size();i++){
            vector<unsigned int> b_path = sei[i].before_repair_path;

            if(prev_path == b_path){
                continue;
            }
            //test
            string a;
            for(int i=0;i<b_path.size();i++){
                a += to_string(b_path[i]);
            }
            if(tmp_map[a] == true){
                cout << "before best path is duplicated" << endl;
                exit(-1);
            }
            tmp_map[a] = true;

            prev_path = b_path;

            r = sei[i].route;

            // cout << "Prefix: ";
            cur_prefix = bitset_to_readable_addr_str(sei[i].route.prefix, sei[i].route.prefix_len);
            
            // cout << "before repair path: ";
            print_as_path(b_path);

            path_vec.push_back(b_path);

            uncomp++;
        }
    }

    cout << "uncomp size: " << uncomp << endl;

    total_number_of_uncompressed_path.push_back(uncomp);


    //pathを短い順にする for path grouping
    cout << "do sort" << endl;
   sort(path_vec.begin(),path_vec.end(),[](const auto& a, const auto& b){
    return a.size() < b.size();
   });

   cout << "do create_pathg" << endl;
    create_path_group(path_vec);
    cout << "do record" << endl;
    record_nodes_per_group();

    /*
    create_se_subgraph();

    //boundary_nodeを計算する
    compute_boundary_nodes();

    //boundary path setとneighbor path setを計算して、no se specを作る
    unsigned int comp = create_no_se_spec();
    */

    //new !
    //仕様中のパスにおいて、自分を含むパス内のノード以外と他のノードと隣接するノードを見つける
    cout << "do compute connected node" << endl;
    compute_connected_nodes();
    //そのノードを全てカバーする最小数のpath setを見つける
    /*find_minimal_path_set_covering_connected_nodes()*/

    cout << "find ospf minimal path" << endl;
    unsigned int comp = find_ospf_minimal_path_sets_covering_cnodes();

    cout << "comp size: " << comp << endl;

    total_number_of_compressed_path.push_back(comp);

    compressed_rate.push_back((double)uncomp/comp);
}


void create_side_effect_path_group_per_prefix()
{
    init_vec_path_group();

    total_number_of_compressed_path.clear();
    total_number_of_uncompressed_path.clear();
    
    // cout << "start to create side effect group" << endl;
    vector<vector<unsigned int>> path_vec;

    struct v4_route r;

    unsigned int uncomp = 0;

    //side effect infoからbefore repairのパスを取り出す
    for(auto itr=cchange_to_se_info.begin();itr!=cchange_to_se_info.end();itr++){
        vector<side_effect_info> sei = itr->second;
        
        vector<unsigned int> prev_path;
        for(int i=0;i<sei.size();i++){
            vector<unsigned int> b_path = sei[i].before_repair_path;

            if(prev_path == b_path){
                continue;
            }

            prev_path = b_path;

            r = sei[i].route;

            // cout << "Prefix: ";
            cur_prefix = bitset_to_readable_addr_str(sei[i].route.prefix, sei[i].route.prefix_len);
            
            // cout << "before repair path: ";
            print_as_path(b_path);

            path_vec.push_back(b_path);

            uncomp++;
        }
    }

    // cout << "uncomp size: " << uncomp << endl;

    total_number_of_uncompressed_path.push_back(uncomp);


    //pathを短い順にする for path grouping
   sort(path_vec.begin(),path_vec.end(),[](const auto& a, const auto& b){
    return a.size() < b.size();
   });

 
    create_path_group(path_vec);
    record_nodes_per_group();

    create_se_subgraph();

    //boundary_nodeを計算する
    compute_boundary_nodes();

    //boundary path setとneighbor path setを計算して、no se specを作る
    unsigned int comp = create_no_se_spec();

    //new !
    //仕様中のパスにおいて、自分を含むパス内のノード以外と他のノードと隣接するノードを見つける
    /*compute_connected_nodes()*/
    //そのノードを全てカバーする最小数のpath setを見つける
    /*find_minimal_path_set_covering_connected_nodes()*/

    // cout << "comp size: " << comp << endl;

    total_number_of_compressed_path.push_back(comp);

    compressed_rate.push_back(uncomp/comp);
}

void missing_route_path_search(struct bgp_table_elms cur_first, struct bgp_table_elms diff_first, struct router *cr, bitset<32> addr, int pl, vector<config_change> *cc)
{
    // cout << "Start to check spec to config change as  a missng route issue !" << endl;

    // cout << "Search best path before repair" << endl;
    
    print_as_number(cur_first);


    cur_first.as_path.push_back(cr->bgp.as_number);

    bool first_flag = false;

    string config_changes = "";
    vector<side_effect_info> se_info;
    
    for(int th=0;th<cur_first.as_path.size();th++){
        string rtr = to_string(cur_first.as_path[th]);
        struct router *rtr_st = name_to_router[rtr];

        if(!rtr_st->bgp_diff.import_filter.empty()){
            for(auto bif=rtr_st->bgp_diff.import_filter.begin();bif!=rtr_st->bgp_diff.import_filter.end();bif++){
                if(bif->second.size() == 0){
                    continue;
                }

                        if(bif->second[0].action == "set route_map drop"){
                            // cout << "-----Missing route cause------" << endl;
                            // cout << "router " << rtr << endl;
                            // cout << "peer: " << bif->first << endl;
                            // cout << "filter name: " << bif->second[0].action << endl;
                            // cout << "------------------------" << endl;

                            //config_changes += insert_config_changes(rtr,bif->second[0].action,"non");
                            struct config_change change;
                            change.router = rtr_st;
                            change.type = bif->second[0].action;
                            change.value = "non";

                            cc->push_back(change);


                            //struct side_effect_info sei;
                            //sei.router = rtr_st;
                            //sei.se_affected_router = cr;
                            //sei.type = bif->second[0].action;

                            /*
                            struct v4_route r;
                            r.prefix = addr;
                            r.prefix_len = pl;
                            sei.route = r;

                            sei.before_repair_path = cur_first.as_path;
                            sei.after_repair_path = diff_first.as_path;

                            se_info.push_back(sei);
                            */

                            if(first_flag == false){
                                first_flag = true;

                                //virtually simulate ?
                                //1. copy bgp_diff to tmp_bgp_diff
                                for(auto itr=name_to_router.begin();itr!=name_to_router.end();itr++){
                                    struct router *cr = itr->second;
                                    list<struct bgp_table_elms> table = itr->second->bgp.loc_rib_table[addr];
                                    cr->virtual_bgp_diff.loc_rib_table[addr] = table;
                                }

                                //2. run simulate with tmp_bgp_diff
                                unordered_map<unsigned int, vector<filter>> tmp_filter = rtr_st->virtual_bgp_diff.import_filter;
                                //test_5_22
                                rtr_st->virtual_bgp_diff.import_filter.clear();
                                struct bgp_message vir_bm;
                                vector<unsigned int > asp = cur_first.as_path;
                                asp.pop_back();
                                create_bm(&vir_bm,cur_first.adv_route, asp, 0);
                                //virtual_simulate(vir_bm,rtr_st,addr, cur_first.as_path, th);
                                //3. check if best path before and after repair are not same
                                //4. search best path after repair and print the priority
                                rtr_st->virtual_bgp_diff.import_filter = tmp_filter;
                            }
                        }
                    }
                }
            }


    /*
    if(!config_changes.empty()){
        if(cchange_to_se_info[config_changes].size() == 0){
            cchange_to_se_info[config_changes] = se_info;
        }
        else{
            for(int s=0;s<se_info.size();s++){
                cchange_to_se_info[config_changes].push_back(se_info[s]);    
            }
        }
    }

    // cout << "go to show_se_info() at missing_route_path_search()" << endl;
    show_se_info(config_changes);
    */
}

//deletionの対応？
//bestがなぜ負けたのか or afterがなぜ買ったのか
void preference_path_search(struct bgp_table_elms cur_first, struct bgp_table_elms diff_first, unsigned int cur_rtr_asn, bitset<32> addr, int pl, int diff_num, vector<config_change> *cc)
{
    // cout << "Start to " << __func__ << endl;

    // cout << "Search best path before repair" << endl;

    int primary_cause_idx = -1;
    
    //beforeが負けるconfigが入った時用
    for(int acc=0;acc<cur_first.as_path.size();acc++){
        string rtr = to_string(cur_first.as_path[acc]);
        struct router *rtr_st = name_to_router[rtr];

        if(!rtr_st->bgp.import_filter.empty()){
            for(auto bif=rtr_st->bgp.import_filter.begin();bif!=rtr_st->bgp.import_filter.end();bif++){
                if(bif->second.size() == 0){
                    continue;
                }

                // cout << "router " << rtr << endl;
                // cout << "peer: " << bif->first << endl;
                // cout << "filter name: " << bif->second[0].action << endl;

                if(primary_cause_idx ==  -1){
                    primary_cause_idx = acc;
                }
            }
        }
    }

    // cout << "Search best path after repair" << endl;

    diff_first.as_path.push_back(cur_rtr_asn);
    //print_as_number(diff_first);

    int cur_diff = 0;

    //rotuer_name.config_type.value:~~~
    //string config_changes = "";
    //vector<side_effect_info> se_info;

    for(int df=diff_first.as_path.size()-1;df>=0;df--){
        string rtr = to_string(diff_first.as_path[df]);
        struct router *diff_rtr_st = name_to_router[rtr];

        // cout << "visit at " << rtr << endl;

        if(!diff_rtr_st->bgp_diff.import_filter.empty()){
            for(auto bif=diff_rtr_st->bgp_diff.import_filter.begin();bif!=diff_rtr_st->bgp_diff.import_filter.end();bif++){
                if(bif->second.size() == 0){
                    continue;
                }

                if(cur_diff >= diff_num){
                    // cout << "current diff value equal to the diff_num, so end !" << endl;
                    break;
                }

                //この値でいいかチェック
                cur_diff += bif->second[0].action_value_numeric;
                // cout << "added value: " << bif->second[0].action_value_numeric;
                // cout << " cur_diff value: " << cur_diff << endl;

                //そのルーターのbest path before repairとafter repairがそのpriorityによって決まっていたら要因として出力
                list<struct bgp_table_elms> tmp_table_before_repair = diff_rtr_st->bgp.loc_rib_table[addr];
                list<struct bgp_table_elms> tmp_table_after_repair = diff_rtr_st->bgp_diff.loc_rib_table[addr];

                if(tmp_table_before_repair.empty() || tmp_table_after_repair.empty()){
                    continue;
                }

                struct bgp_table_elms tmp_best_before_repair = tmp_table_before_repair.front();
                struct bgp_table_elms tmp_best_after_repair = tmp_table_after_repair.front();

                //// cout << "tmp_best_before_reapir: ";
                //show_as_path(tmp_best_before_repair);

               // // cout << "tmp_best_after_reapir: ";
                //show_as_path(tmp_best_after_repair);


                //pathが一緒じゃなくて、このpriorityがafter repairの要因になっている
                if(!is_path_equal(tmp_best_before_repair.as_path,tmp_best_after_repair.as_path)){
                    //after repairのribのpriorityがこのaction_value_numericで決まっていたらのチェック
                    //cur_diff== か、curdiffが小さいか (ospfなら足し算がいる)
                    //matchk関数を作る？

                    // cout << "lp after repair: " << tmp_best_after_repair.lp << endl;
                    if(tmp_best_after_repair.lp == bif->second[0].action_value_numeric){
                        //side_effect_count++;
                        // cout << "-----Priority cause------" << endl;
                        // cout << "router: " << rtr << endl;
                        // cout << "peer: " << bif->first << endl;
                        // cout << "filter name: " << bif->second[0].action << endl;
                        // cout << "filter numeric: " << bif->second[0].action_value_numeric << endl;
                        // cout << "------------------------" << endl;

                        //メモする
                        //config_changesに積もっていかないか (多分起きない)
                        //config_changes += insert_config_changes(rtr,bif->second[0].action,to_string(bif->second[0].action_value_numeric));
                        struct config_change change;
                        change.router = diff_rtr_st;
                        change.type = "local preference";
                        change.value = to_string(bif->second[0].action_value_numeric);

                        cc->push_back(change);

                        //このルーターのこのconfigのこの値
                        /*
                        struct side_effect_info sei;
                        sei.router = diff_rtr_st;
                        sei.se_affected_router = name_to_router[to_string(cur_rtr_asn)];
                        sei.type = "local preference";
                        sei.action_value_numeric = bif->second[0].action_value_numeric;

                        struct v4_route r;
                        r.prefix = addr;
                        r.prefix_len = pl;
                        sei.route = r;

                        sei.before_repair_path = tmp_best_before_repair.as_path;
                        sei.after_repair_path = tmp_best_after_repair.as_path;

                        se_info.push_back(sei);
                        // cout << "sei_size: " << se_info.size() << endl;
                        */
                    }
                }
            }
        }      
    }

    /*
    if(!config_changes.empty()){
        //// cout << "config changes: " << config_changes << endl;
        //// cout << "size: " << cchange_to_se_info[config_changes].size() << endl;
        if(cchange_to_se_info[config_changes].size() == 0){
            cchange_to_se_info[config_changes] = se_info;
        }
        else{
            for(int s=0;s<se_info.size();s++){
                cchange_to_se_info[config_changes].push_back(se_info[s]);    
            }
        }
    }
    
    // cout << "go to show_se_info()" << endl;
    show_se_info(config_changes);
    */
    
    //// cout << "popback" << endl;
    diff_first.as_path.pop_back();
}

bool is_new_advertised_prefix(struct router *router)
{
    if(router->bgp.new_advertised_prefix == true){
        return true;
    }
}

bool is_as_path_same(vector<unsigned int> missing_path, vector<unsigned int> cur_path, int idx)
{
    if(idx != cur_path.size()-1){
        return false;
    }

    for(int i=0;i<=idx;i++){
        if(missing_path[i] != cur_path[i]){
            return false;
        }
    }

    return true;
}


struct bgp_table_elms is_there_missing_route(vector<unsigned int> missing_path, struct router *router, bitset<32> addr, int idx)
{
    // cout << "start to do is_there_missing_route()" << endl;
    struct bgp_table_elms tmp_elm;

    if(router->bgp_diff.loc_rib_table[addr].empty()){
        return tmp_elm;
    }

    list<struct bgp_table_elms> table_after_repair = router->bgp_diff.loc_rib_table[addr];
    
    for(auto itr=table_after_repair.begin();itr!=table_after_repair.end();itr++){
        vector<unsigned int> cur_path = itr->as_path;
        print_as_path(cur_path);

        cur_path.push_back(router->bgp.as_number);

        if(is_as_path_same(missing_path, cur_path, idx)){
            //return table_after_repair.front();
            return *itr;
        }
    }

    return tmp_elm;
}


void empty_rib_path_search(struct bgp_table_elms diff_first, bitset<32> addr, int pl, vector<config_change> *cc)
{
    //1.新しく広報されて生えたケース
    //2.Preferenceで落ちていて、Preferenceで勝ってた経路がblockされていた
    //3.単にblockされていた

    //
    bool t = is_new_advertised_prefix(as_number_to_router[diff_first.as_path[0]][0]);
   // bool t = is_new_advertised_prefix(name_to_router[to_string(diff_first.as_path[0])]);
    if(t == true){
        //1.新しく広報されて生えたケース
        struct config_change change;
        //struct router *r = name_to_router[to_string(diff_first.as_path[0])];
        struct router *r = as_number_to_router[diff_first.as_path[0]][0];

        change.router = r;
        change.type = "new advertised prefix";
        change.value = r->bgp.new_route.prefix.to_string() + "/" + to_string(r->bgp.new_route.prefix_len);

        cc->push_back(change);

        return;
    }

    for(int th=diff_first.as_path.size()-1;th>=0;th--){
        /*
        string rtr = to_string(diff_first.as_path[th]);
        struct router *diff_rtr_st = name_to_router[rtr];
        */
       struct router *diff_rtr_st = as_number_to_router[diff_first.as_path[th]][0];
       string rtr = diff_rtr_st->name;

        //2.Preferenceで落ちていて、Preferenceで勝ってた経路がblockされていた
        //3.単にblockされていた <- 今はここだけ
        if(!diff_rtr_st->bgp_diff.import_filter.empty()){
            for(auto bif=diff_rtr_st->bgp_diff.import_filter.begin();bif!=diff_rtr_st->bgp_diff.import_filter.end();bif++){
                if(bif->second.size() == 0){
                    continue;
                }

                if(bif->second[0].action == "set route_map drop"){
                    // cout << "-----Missing route cause------" << endl;
                    // cout << "router " << rtr << endl;
                    // cout << "peer: " << bif->first << endl;
                    // cout << "filter name: " << bif->second[0].action << endl;
                    // cout << "------------------------" << endl;

                    //config_changes += insert_config_changes(rtr,bif->second[0].action,"non");
                    struct config_change change;
                    change.router = diff_rtr_st;
                    change.type = bif->second[0].action;
                    change.value = "non";

                    cc->push_back(change);

                    return;
                }
            }
        }
    }
}

bool check_duplicate_confic_change(vector<config_change> cc, struct config_change cur_cc)
{
    for(int i=0;i<cc.size();i++){
        if(cc[i].router->name == cur_cc.router->name){
            if(cc[i].type == cur_cc.type){
                if(cc[i].value == cur_cc.type){
                    return true;
                }
            }
        }
    }

    return false;
}

vector<pair<string, unsigned int>> gather_ospf_diff_cost(vector<router *> path)
{
    vector<pair<string, unsigned int>> cost_vec;

    // cout << "gather_ospf_diff_cost() " << endl;

    for(int i=0;i<path.size()-1;i++){
        struct router *cur_router = path[i];
        struct router *next_router = path[i+1];

        if(cur_router->ospf_diff.diff_peer_cost_map[next_router->name] > 0){
            pair<string, unsigned int> tmp_pair;
            tmp_pair.first = cur_router->name + "-" + next_router->name;
            tmp_pair.second = cur_router->ospf_diff.diff_peer_cost_map[next_router->name];   

            // cout << "link: " << tmp_pair.first << " cost: " << tmp_pair.second << endl;

            cost_vec.push_back(tmp_pair);
        }
    }

    return cost_vec;
}

std::string get_name1(std::string full_name) {
    // 1. 最初のハイフンの位置を探す
    size_t first_hyphen_pos = full_name.find('-');

    // ハイフンが見つからなかった場合は、元の文字列をそのまま返す
    if (first_hyphen_pos == std::string::npos) {
        return full_name;
    }

    // 2. 最初のハイフンの次の文字が数字かどうかをチェックする
    //    文字列の範囲外アクセスを防ぐチェックも同時に行う
    if (first_hyphen_pos + 1 < full_name.length() && isdigit(full_name[first_hyphen_pos + 1])) {
        
        // 3. 数字だった場合、2個目のハイフンを探す
        //    (検索開始位置を最初のハイフンの次からに指定)
        size_t second_hyphen_pos = full_name.find('-', first_hyphen_pos + 1);

        // 2個目のハイフンが見つかった場合
        if (second_hyphen_pos != std::string::npos) {
            // 先頭から2個目のハイフンの前までを返す
            return full_name.substr(0, second_hyphen_pos);
        } else {
            // 2個目のハイフンが見つからない場合 (例: "agg3-3") は、
            // 元の文字列をそのまま返す
            return full_name;
        }
    }

    // 4. 最初のハイフンの次が数字でなかった場合は、従来通りの動作
    //    (最初のハイフンの前までを返す)
    return full_name.substr(0, first_hyphen_pos);
}

std::string get_name2(std::string full_name) {
    // 1. 最初のハイフンの位置を探す
    size_t first_hyphen_pos = full_name.find('-');

    // ハイフンが見つからない場合は、2個目のデバイス名はないので空文字列を返す
    if (first_hyphen_pos == std::string::npos) {
        return "";
    }

    // 2. 最初のハイフンの次の文字が数字かどうかをチェックする
    if (first_hyphen_pos + 1 < full_name.length() && isdigit(full_name[first_hyphen_pos + 1])) {
        
        // 3. 数字だった場合、2個目のハイフンを探す
        size_t second_hyphen_pos = full_name.find('-', first_hyphen_pos + 1);

        // 2個目のハイフンが見つかった場合
        if (second_hyphen_pos != std::string::npos) {
            // 2個目のハイフンの後ろの部分文字列を返す
            return full_name.substr(second_hyphen_pos + 1);
        } else {
            // 2個目のハイフンが見つからない (例: "agg3-3")場合は、
            // 2個目のデバイス名はないので空文字列を返す
            return "";
        }
    }

    // 4. 最初のハイフンの次が数字でなかった場合は、
    //    最初のハイフンの後ろの部分文字列を返す
    return full_name.substr(first_hyphen_pos + 1);
}


vector<struct config_change> new_search_valid_repaired_ospf_ecmp_config(vector<vector<struct router *>> before_repair_sp, vector<vector<struct router *>> after_repair_sp,struct v4_route route)
{
    vector<config_change> cc;
    
    bitset<32> p = route.prefix;
    int l = route.prefix_len;

    bool f1,f2 = false;

    for(int j=0;j<before_repair_sp.size();j++){
        f1 = true;
        vector<router *> tmp_before_repair_sp = before_repair_sp[j];
        vector<pair<string, unsigned int>> before_repair_cost_vec;

        before_repair_cost_vec = gather_ospf_diff_cost(tmp_before_repair_sp);
        
        for(int i=0;i<before_repair_cost_vec.size();i++){
            cout << "link: " << before_repair_cost_vec[i].first << ", cost: " << before_repair_cost_vec[i].second << endl;
    
            string link = before_repair_cost_vec[i].first;
            string dev1 = get_name1(link);
            if(dev1 == link){
                // cout << "get_name1" << endl;
                exit(-1);
            }
            string dev2 = get_name2(link);
            if(dev2 == link){
                // cout << "get_name2" << endl;
                exit(-1);
            }
    
            struct router *tmp_router = name_to_router[dev1];
            // cout << "dev1: " << dev1 << " dev2: " << dev2 << endl;
            unsigned int cost_before_repair = tmp_router->ospf.peer_cost_map[dev2];
    
            struct config_change change;
            change.router = tmp_router;
            change.type = "ospf_cost";
            change.value = to_string(cost_before_repair) + "-" +  to_string(before_repair_cost_vec[i].second);
    
            if(!check_duplicate_confic_change(cc, change)){
                cc.push_back(change);
            }
            else{
                // cout << "dup!" << endl;
            }
        }
    }

    for(int j=0;j<after_repair_sp.size();j++){
        f2 = true;
        vector<router *> tmp_after_repair_sp = after_repair_sp[j];
        vector<pair<string, unsigned int>> after_repair_cost_vec;
        
        after_repair_cost_vec = gather_ospf_diff_cost(tmp_after_repair_sp);

        for(int i=0;i<after_repair_cost_vec.size();i++){
            cout << "link: " << after_repair_cost_vec[i].first << " , cost: " << after_repair_cost_vec[i].second << endl;
    
            string link = after_repair_cost_vec[i].first;
            string dev1 = get_name1(link);
            if(dev1 == link){
                // cout << "get_name1" << endl;
                exit(-1);
            }
            string dev2 = get_name2(link);
            if(dev2 == link){
                // cout << "get_name2" << endl;
                exit(-1);
            }
    
            struct router *tmp_router = name_to_router[dev1];
            unsigned int cost_before_repair = tmp_router->ospf.peer_cost_map[dev2];
    
            struct config_change change;
            change.router = tmp_router;
            change.type = "ospf_cost";
            change.value = to_string(cost_before_repair) + "-" +  to_string(after_repair_cost_vec[i].second);
    
            if(!check_duplicate_confic_change(cc, change)){
                cc.push_back(change);
            }
            else{
                // cout << "dup!" << endl;
            }
        }   
    }
    
    if(cc.size()==0){
        cout << "cc.size() == 0 !!!" << endl;
        cout << f1 << endl;
        cout << f2 << endl;
        //side_effect_cnt--;
        //exit(-1);
    }

    return cc;
}

vector<struct config_change> new_search_valid_repaired_ospf_config(vector<struct router *> before_repair_sp, vector<struct router *> after_repair_sp,struct v4_route route)
{   
    vector<vector<config_change>> vcc;
    vector<config_change> cc;

    // cout << "Start to " << __func__ << endl;

    bitset<32> p = route.prefix;
    int l = route.prefix_len;

     //一番シンプルなのは、before/after repairのshortest pathを調べて、コスト変化を見つけること
    //与えられたパスを探索して、変化したコストを何らかの形でまとめて返す関数

    vector<pair<string, unsigned int>> before_repair_cost_vec;
    vector<pair<string, unsigned int>> after_repair_cost_vec;

    before_repair_cost_vec = gather_ospf_diff_cost(before_repair_sp);
    after_repair_cost_vec = gather_ospf_diff_cost(after_repair_sp);

    //stringにする
    // cout << "-----Cost diffrence cause at Search_valid_config------" << endl;

    // cout << "Changes at shortest path before repair" << endl;
    for(int i=0;i<before_repair_cost_vec.size();i++){
        // cout << "link: " << before_repair_cost_vec[i].first << ", cost: " << before_repair_cost_vec[i].second << endl;

        string link = before_repair_cost_vec[i].first;
        string dev1 = get_name1(link);
        if(dev1 == link){
            // cout << "get_name1" << endl;
            exit(-1);
        }
        string dev2 = get_name2(link);
        if(dev2 == link){
            // cout << "get_name2" << endl;
            exit(-1);
        }

        struct router *tmp_router = name_to_router[dev1];
        // cout << "dev1: " << dev1 << " dev2: " << dev2 << endl;
        unsigned int cost_before_repair = tmp_router->ospf.peer_cost_map[dev2];

        struct config_change change;
        change.router = tmp_router;
        change.type = "ospf_cost";
        change.value = to_string(cost_before_repair) + "-" +  to_string(before_repair_cost_vec[i].second);

        if(!check_duplicate_confic_change(cc, change)){
            cc.push_back(change);
        }
        else{
            // cout << "dup!" << endl;
        }
    }

    // cout << "Changes at shortest path after repair" << endl;
    for(int i=0;i<after_repair_cost_vec.size();i++){
        // cout << "link: " << after_repair_cost_vec[i].first << " , cost: " << after_repair_cost_vec[i].second << endl;

        string link = after_repair_cost_vec[i].first;
        string dev1 = get_name1(link);
        if(dev1 == link){
            // cout << "get_name1" << endl;
            exit(-1);
        }
        string dev2 = get_name2(link);
        if(dev2 == link){
            // cout << "get_name2" << endl;
            exit(-1);
        }

        struct router *tmp_router = name_to_router[dev1];
        unsigned int cost_before_repair = tmp_router->ospf.peer_cost_map[dev2];

        struct config_change change;
        change.router = tmp_router;
        change.type = "ospf_cost";
        change.value = to_string(cost_before_repair) + "-" +  to_string(after_repair_cost_vec[i].second);

        if(!check_duplicate_confic_change(cc, change)){
            cc.push_back(change);
        }
        else{
            // cout << "dup!" << endl;
        }
    }
    // cout << "-------------------" << endl;

    // cout << "cc size: " << cc.size() << endl;

    if(cc.size()==0){
        // cout << "cc.size() == 0 !!!" << endl;
        exit(-1);
    }

    return cc;
}

vector<struct config_change> search_valid_repaired_ospf_config(vector<struct router *> vr, struct v4_route route)
{
    vector<vector<config_change>> vcc;
    vector<config_change> cc;

    // cout << "do not use" << endl;
    exit(-1);

    //各ルーターのvccを集めてみる？
    //違いがあったら、アラート出して、後で考える

    // cout << "Start to " << __func__ << endl;

    bitset<32> p = route.prefix;
    int l = route.prefix_len;

    //how to 打ち切り

     bool preference_done_flag = false;
     bool route_map_don_flag = false;
     bool cost_done_flag = false;

    for(int i=vr.size()-1;i>=0;i--){
        //after repairのtraceを逆向きに探索して、だけじゃダメで、他のパスがブロックとかもありえる -> after repairのbest path上のルーターで見えるはず
        //手順はse解析と基本的に同じで、逆に言えば、before repairの前のbest pathが別のパスの場合は、そのパスを探索して、そのパスが負けた原因を探す
        //before repairのベストパスが存在しない場合は、after repairのbest pathが存在できるようになった理由を探すことにする

        //vrには仕様で指定されたパスのノードが入っている
        //もし、仕様で指定される前にvr[i]で、ベストパスが別のパスになっているならば、そのノードの変更を引き起こしたcofnig changeを探し出す必要がある
        //そのためには、そのbefore repairのベストパスを探索して、そのbest path changeの原因となっているconfig changeを見つけ出す必要がある
        //もしvr[i]のbefore repair pathが空の場合は、after repairのバスのどこかでblockされているか優先度的に負けてる、それを探し出す

        //diffは、router->ospf_diff.diff_peer_costに入っている

        // cout << "current cc_vec size: " << cc.size() << endl;

        //only for scalability eval
        //if(i == 0 || preference_done_flag == true){
        //if(i == 0 || preference_done_flag == true || route_map_don_flag == true){
        if(i == 0 || cost_done_flag == true){
            break;
        }

        //vector<config_change> cc;

        struct router *cr = vr[i];
        if(cr == NULL){
            // cout << "N(LL)" << endl;
            exit(-1);
        }
        // cout << "visit router " << cr->name << endl;

        //vr[i]における、before repairとafter repairのベストパスが同じかを調べる
        //同じならcontinue, なぜならno changeなので
        //もし異なるなら、before repairが存在するか空なのかで場合わけ
        //もし存在するならば、そのパスを探索して、そのchangeの原因を見つける
        //もし存在しないならば、存在するところまで、遡る
        //存在するところが見つかれば、そこからなぜ消えたのかを見つける
        //全部見つからない場合は、単純にconfigが入ってなかったことになるので、after repairにおけるconfig changeの中で、それを実現したものを紐づける

        if(cr->bgp.loc_rib_table[p].empty()){
            //before repairのribが存在しない && after repairでは新しく生えている
            //(i)元々ACLとかで経路広報がblockされている可能性がある or (ii) 新しく広報され出した
            //diffで、deletionがあったことを記録しているようにすれば、after repairのtraceを探索すれば見つけられる
            //つまり、after repairのtraceを探索して、before repairと異なり、なぜ新しく経路が生えたのかを調べる必要がある
            if(cr->bgp_diff.loc_rib_table[p].empty()){
                continue;
            }

            // cout << "go empty_rib_path_search()" << endl;
            // cout << "We should not come here now" << endl;
            exit(-1);
            empty_rib_path_search(cr->bgp_diff.loc_rib_table[p].front(), route.prefix, route.prefix_len, &cc);
        }
        else{
            if(cr->bgp_diff.loc_rib_table[p].empty()){
                //仕様がblockの時 or src->dstまで一本道しかなくて、dropされた時、はありえる
                //この時は、blockを実現したpreferenceかroute mapを探す
                //とりあえず、評価ではroute mapの削除を追加するので、それを見つける

                //↑ ospfではありえない？
                //リンクコストの変化しか評価しないので、いったん飛ばす
                // cout << "go cr->bgp_diff.loc_rib_table[p].empty()" << endl;
                // cout << "We should not come here for ospf" << endl;
                exit(-1);
            }
            else{
                //存在すらなら、普通に到達性の仕様
                //// cout << "we are here" << endl;

                //best pathがbefore afterで同じかをチェック
                struct bgp_table_elms best_elm_before_repair = cr->bgp.loc_rib_table[p].front();
                struct bgp_table_elms best_elm_after_repair = cr->bgp_diff.loc_rib_table[p].front();

                //同じならcontinue
                if(best_elm_before_repair.as_path == best_elm_after_repair.as_path){
                    // cout << "best paths before and after repair are same" << endl;
                    continue;
                }
                else{
                    //異なるなら, best path changeの原因を見つけるために、before repairのpathを探索する
                    
                    // cout << "best paths differ" << endl;

                    //なぜ変わってしまったのかの原因を見つける (preference or いないか)
                    //ここにきてる時点で、before/afterも空じゃないし、パスが同じじゃない
                    list<bgp_table_elms> table_after_repair = cr->bgp_diff.loc_rib_table[p];
                    //ospf change
                    int diff = ospf_find_the_best_route(best_elm_before_repair, &table_after_repair);
                    //int diff = new_find_the_best_route_2(best_elm_before_repair, &table_after_repair);

                    //一番シンプルなのは、before/after repairのshortest pathを調べて、コスト変化を見つけること
                    //与えられたパスを探索して、変化したコストを何らかの形でまとめて返す関数
                    
                    //shortest path before repairがrib after repairにいるか調べる
                    //shortest path after repairがrib before repairにいるか調べる

                    //両方いるなら、コストを調べられる
                    //両方いない場合は、shortest path before repairとafter repairを探索して、コスト変化を見つける
                    //片方だけいる場合は、いる方はコスト変化をしているか調べて、しているならコスト変化を探す、いない方はパスを探索してコスト変化を探る

                    if(diff > -1){
                        //両方ともいる場合

                        //shortest path before repairのin rib before repairとin rib after repairでのコストの差を比べる
                        //shortest path after repairのin rib before repairとin rib after repairでのコストの差を比べる

                        //shortest path after repairのコストが減っていて、shortest path before repairのコストが増えている場合
                        //その両方のコスト変化が、shortest pathの変化に貢献

                        //shortest path before repairのコストだけが増えている
                        //shortest path before repairを探索して、コスト増加したところを見つける

                        //shortest path after repairのコストだけが減っている
                        //shortest path after repairを探索して、コスト減少したところを見つける



                        // cout << "Cost diff: " << diff << endl;
                        
                        /*
                        int lpv = best_elm_after_repair.lp;
                        vector<unsigned int> bpar = best_elm_after_repair.as_path;
                        bpar.push_back(cr->bgp.as_number);
                        */
                       //ospf change
                       //unsigned int cost_after_repair = best_elm_after_repair.total_cost;
                       //vector<unsigned int> bpar = best_elm_after_repair.as_path;
                       //bpar.push_back(cr->bgp.as_number);
                       unsigned int cost_before_repair = best_elm_before_repair.total_cost;
                       vector<unsigned int> bpbr = best_elm_before_repair.as_path;
                       bpbr.push_back(cr->bgp.as_number);

                        // cout << "ospf cost issue (cost diff)" << endl;
                        //ospf change
                        //print_as_path(bpar);
                        print_as_path(bpbr);

                        // cout << "ith: " << i << endl;

                        bool flag_done = false;

                        unsigned int cur_cost = 0;

                        for(int j=bpbr.size()-1;j>=0;j--){

                            struct router *before_router = as_number_to_router[bpbr[j]][0];
                            string rtr = before_router->name;

                            // cout << "v2: visit router " << rtr << endl;

                            if(!before_router->ospf_diff.diff_peer_cost_map.empty()){
                                //before_router->ospf_diff.diff_peer_cost_map[]
                            }

                            //ospf diff
                            if(!before_router->ospf_diff.diff_peer_cost_map.empty()){
                                for(auto ospfd=before_router->ospf_diff.diff_peer_cost_map.begin();ospfd!=before_router->ospf_diff.diff_peer_cost_map.end();ospfd++){
                                    //costが0じゃなく、pathに従っている
                                    //costの合算

                                    string peer = ospfd->first;
                                    unsigned int peer_cost = ospfd->second;

                                    if(peer_cost == 0 || peer != before_router->name){
                                        continue;
                                    }
                                    // cout << "total cost: " << cost_before_repair << endl;
                                    // cout << "current cost: " << cur_cost << endl;
                                    // cout << "ospfd->second (cost): " << ospfd->second << endl;

                                    if(ospfd->second > 0){
                                        unsigned int cost_before_repiar = 0;

                                        int tt = 0;
                                        for(tt=0;tt<before_router->ospf.peer_info.size();i++){
                                            if(before_router->ospf.peer_info[tt].peer_name == ospfd->first){
                                                cost_before_repiar = before_router->ospf.peer_info[tt].cost;
                                            }
                                        }

                                        if(tt == before_router->ospf.peer_info.size()){
                                            // cout << "fatal error at tt == diff_rtr_st->ospf.peer_info.size()" << endl;
                                        }

                                        // cout << "-----Cost difefrence cause at Search_valid_config------" << endl;
                                        // cout << "router: " << rtr << endl;
                                        // cout << "peer: " << ospfd->first << endl;
                                        // cout << "Cost before repair: " << cost_before_repiar << endl;
                                        // cout << "Cost after repair: " << ospfd->second << endl;
                                        // cout << "------------------------" << endl;

                                        cur_cost += ospfd->second;

                                        struct config_change change;
                                        change.router = before_router;
                                        change.type = "ospf_cost";
                                        change.value = to_string(cost_before_repiar) + "->" + to_string(ospfd->second);

                                        if(!check_duplicate_confic_change(cc, change) &&  preference_done_flag == false){
                                            cc.push_back(change);
                                        }

                                        // cout << "cc size: " << cc.size() << endl;
                                        /*
                                        flag_done = true;
                                        preference_done_flag = true;
                                        
                                        break;
                                        */
                                    }
                                }
                            }
                        
                            //ospf change
                            /*
                            if(!diff_rtr_st->bgp_diff.import_filter.empty()){
                                for(auto bif=diff_rtr_st->bgp_diff.import_filter.begin();bif!=diff_rtr_st->bgp_diff.import_filter.end();bif++){
                                    if(bif->second.size() == 0){
                                        continue;
                                    }

                                   // cout << "lp after repair: " << lpv << endl;
                                   if(lpv == bif->second[0].action_value_numeric){
                                        // cout << "-----Priority cause at Search_valid_config------" << endl;
                                        // cout << "router: " << rtr << endl;
                                        // cout << "peer: " << bif->first << endl;
                                        // cout << "filter name: " << bif->second[0].action << endl;
                                        // cout << "filter numeric: " << bif->second[0].action_value_numeric << endl;
                                        // cout << "------------------------" << endl;

                                        struct config_change change;
                                        change.router = diff_rtr_st;
                                        change.type = "set local_pref";
                                        //change.type = "set local_pref";
                                        change.value = to_string(bif->second[0].action_value_numeric);

                                        if(!check_duplicate_confic_change(cc, change) &&  preference_done_flag == false){
                                            cc.push_back(change);
                                        }

                                        // cout << "cc size: " << cc.size() << endl;
                                        flag_done = true;
                                        preference_done_flag = true;
                                        
                                        break;
                                   }
                                }
                            }
                            */       
                        }
                    }
                    else{
                        //いない(beforeのベストがafterにいない)
                        //どこでdropされたか探す

                        vector<unsigned int> cur_first = best_elm_before_repair.as_path;

                        if(cur_first[cur_first.size()-1] != cr->bgp.as_number){
                            cur_first.push_back(cr->bgp.as_number);
                        }
                        //cur_first.push_back(cr->bgp.as_number);

                        // cout << "missing issue" << endl;

                        bool flag_done = false;

                        for(int th=cur_first.size()-1;th>=0;th--){
                            /*
                            string rtr = to_string(cur_first[th]);
                            struct router *rtr_st = name_to_router[rtr];
                            */

                           if(flag_done == true){
                                //break;
                           }

                            struct router *rtr_st = as_number_to_router[cur_first[th]][0];
                            string rtr = rtr_st->name;

                            // cout << "visit router " << rtr_st->name << endl;

                            //発信源の真隣がblockしたケース to do

                            //cur_firstがbest pathなルーターを見つける, endは削る
                            //why this happen

                            //このルーターのafter repairにいるかみる
                            struct bgp_table_elms elm = is_there_missing_route(cur_first, rtr_st, p, th);
                            // cout << "done is_there_missing route()" << endl;
                            if(!elm.as_path.empty()){
                                //いる場合は、preferenceで負けてる　→ そのpreferenceが何で、どこでついたかを探す
                                // cout << "priority issue at router " << rtr_st->bgp.as_number << endl;
                                struct bgp_table_elms cur_top = rtr_st->bgp_diff.loc_rib_table[p].front();

                                //check what types of priorty (lp, as_path_len)
                                //その原因を探す
                                //config_chang to seに入れる

                                if(cur_top.lp > elm.lp){
                                    // cout << "the cause is local preference difference" << endl;

                                    for(auto bif=rtr_st->bgp_diff.import_filter.begin();bif!=rtr_st->bgp_diff.import_filter.end();bif++){
                                        if(bif->second.size() == 0){
                                            continue;
                                        }

                                        if(cur_top.lp == bif->second[0].action_value_numeric){
                                            // cout << "-----Priority cause at search valid repaired config------" << endl;
                                            // cout << "router: " << rtr << endl;
                                            // cout << "peer: " << bif->first << endl;
                                            // cout << "filter name: " << bif->second[0].action << endl;
                                            // cout << "filter numeric: " << bif->second[0].action_value_numeric << endl;
                                            // cout << "------------------------" << endl;

                                            //メモする
                                            //config_changesに積もっていかないか (多分起きない)

                                            struct config_change change;
                                            change.router = rtr_st;
                                            change.type = "set local_pref";
                                            //change.type = "set local_pref";
                                            change.value = to_string(bif->second[0].action_value_numeric);

                                            if(!check_duplicate_confic_change(cc, change) &&  preference_done_flag == false){
                                                cc.push_back(change);
                                            }   

                                            preference_done_flag = true;
                                            route_map_don_flag = true;
                                            flag_done = true;
                                        }
                                    }
                                }
                            }
                            else{
                                // cout << "We should not come here for ospf at search_valid_repaired_ospf_config()" << endl;
                                exit(-1);
                            }
                            /*
                            else{
                                //いない場合は、routem mapで落とされてないかチェック
                                if(!rtr_st->bgp_diff.import_filter.empty()){
                                    for(auto bif=rtr_st->bgp_diff.import_filter.begin();bif!=rtr_st->bgp_diff.import_filter.end();bif++){
                                        if(bif->second.size() == 0){
                                            continue;
                                        }

                                        //// cout << "router: " << rtr_st->name << endl;

                                        if(bif->second[0].action == "set route_map drop"){
                                            // cout << "-----Missing route cause at search valid config------" << endl;
                                            // cout << "router " << rtr << endl;
                                            // cout << "peer: " << bif->first << endl;
                                            // cout << "filter name: " << bif->second[0].action << endl;
                                            // cout << "------------------------" << endl;

                                            struct config_change change;
                                            change.router = rtr_st;
                                            change.type = bif->second[0].action;
                                            change.value = "non";

                                            if(!check_duplicate_confic_change(cc, change)){
                                                cc.push_back(change);
                                            }

                                            //cc.push_back(change);
                                            flag_done = true;
                                            route_map_don_flag = true;
                                        }
                                    }
                                }
                            }
                            */
                        }
                    }
                }
            }
        }
    }
    //vector<config_change> o_vcc;

    return cc;
}

vector<struct config_change> search_valid_repaired_config(vector<struct router *> vr, struct v4_route route)
{
    vector<vector<config_change>> vcc;
    vector<config_change> cc;

    //各ルーターのvccを集めてみる？
    //違いがあったら、アラート出して、後で考える

    // cout << "Start to " << __func__ << endl;

    bitset<32> p = route.prefix;
    int l = route.prefix_len;

    //how to 打ち切り

     bool preference_done_flag = false;
     bool route_map_don_flag = false;

    for(int i=vr.size()-1;i>=0;i--){
        //after repairのtraceを逆向きに探索して、だけじゃダメで、他のパスがブロックとかもありえる -> after repairのbest path上のルーターで見えるはず
        //手順はse解析と基本的に同じで、逆に言えば、before repairの前のbest pathが別のパスの場合は、そのパスを探索して、そのパスが負けた原因を探す
        //before repairのベストパスが存在しない場合は、after repairのbest pathが存在できるようになった理由を探すことにする

        //vrには仕様で指定されたパスのノードが入っている
        //もし、仕様で指定される前にvr[i]で、ベストパスが別のパスになっているならば、そのノードの変更を引き起こしたcofnig changeを探し出す必要がある
        //そのためには、そのbefore repairのベストパスを探索して、そのbest path changeの原因となっているconfig changeを見つけ出す必要がある
        //もしvr[i]のbefore repair pathが空の場合は、after repairのバスのどこかでblockされているか優先度的に負けてる、それを探し出す

        // cout << "current cc_vec size: " << cc.size() << endl;

        //only for scalability eval
        //if(i == 0 || preference_done_flag == true){
        if(i == 0 || preference_done_flag == true || route_map_don_flag == true){
        //if(i == 0){
            break;
        }

        //vector<config_change> cc;

        struct router *cr = vr[i];
        if(cr == NULL){
            // cout << "N(LL)" << endl;
            exit(-1);
        }
        // cout << "visit router " << cr->name << endl;

        //vr[i]における、before repairとafter repairのベストパスが同じかを調べる
        //同じならcontinue, なぜならno changeなので
        //もし異なるなら、before repairが存在するか空なのかで場合わけ
        //もし存在するならば、そのパスを探索して、そのchangeの原因を見つける
        //もし存在しないならば、存在するところまで、遡る
        //存在するところが見つかれば、そこからなぜ消えたのかを見つける
        //全部見つからない場合は、単純にconfigが入ってなかったことになるので、after repairにおけるconfig changeの中で、それを実現したものを紐づける

        if(cr->bgp.loc_rib_table[p].empty()){
            //before repairのribが存在しない && after repairでは新しく生えている
            //(i)元々ACLとかで経路広報がblockされている可能性がある or (ii) 新しく広報され出した
            //diffで、deletionがあったことを記録しているようにすれば、after repairのtraceを探索すれば見つけられる
            //つまり、after repairのtraceを探索して、before repairと異なり、なぜ新しく経路が生えたのかを調べる必要がある
            if(cr->bgp_diff.loc_rib_table[p].empty()){
                continue;
            }

            // cout << "go empty_rib_path_search()" << endl;
            empty_rib_path_search(cr->bgp_diff.loc_rib_table[p].front(), route.prefix, route.prefix_len, &cc);
            // cout << "We should not come here now" << endl;
            exit(-1);
        }
        else{
            if(cr->bgp_diff.loc_rib_table[p].empty()){
                //仕様がblockの時 or src->dstまで一本道しかなくて、dropされた時、はありえる
                //この時は、blockを実現したpreferenceかroute mapを探す
                //とりあえず、評価ではroute mapの削除を追加するので、それを見つける

                // cout << "cr->bgp_diff.loc_rib_table[p].empty() at search_valid_repaired_config" << endl;

                struct bgp_table_elms best_elm_before_repair = cr->bgp.loc_rib_table[p].front();
                vector<unsigned int> cur_first = best_elm_before_repair.as_path;

                if(cur_first[cur_first.size()-1] != cr->bgp.as_number){
                     cur_first.push_back(cr->bgp.as_number);
                }

                // cout << "missing issue" << endl;

                        bool flag_done = false;

                        for(int th=cur_first.size()-1;th>=0;th--){
                            /*
                            string rtr = to_string(cur_first[th]);
                            struct router *rtr_st = name_to_router[rtr];
                            */

                           if(flag_done == true){
                                //break;
                           }

                            struct router *rtr_st = as_number_to_router[cur_first[th]][0];
                            string rtr = rtr_st->name;

                            // cout << "visit router " << rtr_st->name << endl;

                            //発信源の真隣がblockしたケース to do

                            //cur_firstがbest pathなルーターを見つける, endは削る
                            //why this happen

                            //このルーターのafter repairにいるかみる
                            struct bgp_table_elms elm = is_there_missing_route(cur_first, rtr_st, p, th);
                            // cout << "done is_there_missing route()" << endl;
                            if(!elm.as_path.empty()){
                                //いる場合は、preferenceで負けてる　→ そのpreferenceが何で、どこでついたかを探す
                                // cout << "priority issue at router " << rtr_st->bgp.as_number << endl;
                                struct bgp_table_elms cur_top = rtr_st->bgp_diff.loc_rib_table[p].front();

                                //check what types of priorty (lp, as_path_len)
                                //その原因を探す
                                //config_chang to seに入れる

                                if(cur_top.lp > elm.lp){
                                    // cout << "the cause is local preference difference" << endl;

                                    for(auto bif=rtr_st->bgp_diff.import_filter.begin();bif!=rtr_st->bgp_diff.import_filter.end();bif++){
                                        if(bif->second.size() == 0){
                                            continue;
                                        }

                                        if(cur_top.lp == bif->second[0].action_value_numeric){
                                            // cout << "-----Priority cause at search valid repaired config------" << endl;
                                            // cout << "router: " << rtr << endl;
                                            // cout << "peer: " << bif->first << endl;
                                            // cout << "filter name: " << bif->second[0].action << endl;
                                            // cout << "filter numeric: " << bif->second[0].action_value_numeric << endl;
                                            // cout << "------------------------" << endl;

                                            //メモする
                                            //config_changesに積もっていかないか (多分起きない)

                                            struct config_change change;
                                            change.router = rtr_st;
                                            change.type = "set local_pref";
                                            //change.type = "set local_pref";
                                            change.value = to_string(bif->second[0].action_value_numeric);

                                            if(!check_duplicate_confic_change(cc, change) &&  preference_done_flag == false){
                                                cc.push_back(change);
                                            }   

                                            preference_done_flag = true;
                                            route_map_don_flag = true;
                                            flag_done = true;
                                        }
                                    }
                                }
                            }
                            else{
                                //いない場合は、routem mapで落とされてないかチェック
                                if(!rtr_st->bgp_diff.import_filter.empty()){
                                    for(auto bif=rtr_st->bgp_diff.import_filter.begin();bif!=rtr_st->bgp_diff.import_filter.end();bif++){
                                        if(bif->second.size() == 0){
                                            continue;
                                        }

                                        //// cout << "router: " << rtr_st->name << endl;

                                        if(bif->second[0].action == "set route_map drop"){
                                            // cout << "-----Missing route cause at search valid config------" << endl;
                                            // cout << "router " << rtr << endl;
                                            // cout << "peer: " << bif->first << endl;
                                            // cout << "filter name: " << bif->second[0].action << endl;
                                            // cout << "------------------------" << endl;

                                            struct config_change change;
                                            change.router = rtr_st;
                                            change.type = bif->second[0].action;
                                            change.value = "non";

                                            if(!check_duplicate_confic_change(cc, change)){
                                                cc.push_back(change);
                                            }

                                            //cc.push_back(change);
                                            flag_done = true;
                                            route_map_don_flag = true;
                                        }
                                    }
                                }
                            }
                        }
                //いや、こっちはありえない、なぜなら仕様で指定したpathがblockじゃないなら、通るので
                /*
                // cout << "We should not come here now" << endl;
                exit(-1);
                struct bgp_table_elms best_elm_before_repair = cr->bgp.loc_rib_table[p].front();
                vector<unsigned int> basp = best_elm_before_repair.as_path;

                for(int v=basp.size()-1;v>=0;v--){
                    
                }
                */
            }
            else{
                //存在すらなら、普通に到達性の仕様
                //// cout << "we are here" << endl;
                // cout << "NUll check" << endl;
                // cout << "bgp_loc_rib: " << cr->bgp.loc_rib_table[p].empty() << endl;
                 // cout << "diff bgp_loc_rib: " << cr->bgp_diff.loc_rib_table[p].empty() << endl;
                

                //best pathがbefore afterで同じかをチェック
                struct bgp_table_elms best_elm_before_repair = cr->bgp.loc_rib_table[p].front();
                struct bgp_table_elms best_elm_after_repair = cr->bgp_diff.loc_rib_table[p].front();

                //同じならcontinue
                if(best_elm_before_repair.as_path == best_elm_after_repair.as_path){
                    // cout << "best paths before and after repair are same" << endl;
                    continue;
                }
                else{
                    //異なるならば、チェンジの原因を見つけるために、before repairのpathを探索する
                    
                    // cout << "best paths differ" << endl;

                    //なぜ変わってしまったのかの原因を見つける (preference or いないか)
                    //ここにきてる時点で、before/afterも空じゃないし、パスが同じじゃない
                    list<bgp_table_elms> table_after_repair = cr->bgp_diff.loc_rib_table[p];
                    int diff = new_find_the_best_route_2(best_elm_before_repair, &table_after_repair);

                    if(diff > -1){
                        //いる、preference
                        //どのpreferenceか、lpかas pathをとりあえず
                        //とりあえずlpと考えてみる(lp以外だと今は判定していない)

                        int lpv = best_elm_after_repair.lp;
                        vector<unsigned int> bpar = best_elm_after_repair.as_path;
                        bpar.push_back(cr->bgp.as_number);

                        // cout << "preference issue" << endl;
                        print_as_path(bpar);

                        // cout << "ith: " << i << endl;

                        bool flag_done = false;

                        for(int j=i;j>=0;j--){
                            if(j < 0){
                            //if(j < 0 || flag_done == true){
                                break;
                            }

                            /*
                            string rtr = to_string(bpar[j]);
                            struct router *diff_rtr_st = name_to_router[rtr];
                            */
                            struct router *diff_rtr_st = as_number_to_router[bpar[j]][0];
                            string rtr = diff_rtr_st->name;

                            // cout << "v2: visit router " << rtr << endl;

                            if(!diff_rtr_st->bgp_diff.import_filter.empty()){
                                for(auto bif=diff_rtr_st->bgp_diff.import_filter.begin();bif!=diff_rtr_st->bgp_diff.import_filter.end();bif++){
                                    if(bif->second.size() == 0){
                                        continue;
                                    }
                                    /*
                                    if(cur_diff >= diff_num){
                                        // cout << "current diff value equal to the diff_num, so end !" << endl;
                                        break;
                                    }
                                    */
                                   
                                   // cout << "lp after repair: " << lpv << endl;
                                   if(lpv == bif->second[0].action_value_numeric){
                                        // cout << "-----Priority cause at Search_valid_config------" << endl;
                                        // cout << "router: " << rtr << endl;
                                        // cout << "peer: " << bif->first << endl;
                                        // cout << "filter name: " << bif->second[0].action << endl;
                                        // cout << "filter numeric: " << bif->second[0].action_value_numeric << endl;
                                        // cout << "------------------------" << endl;

                                        struct config_change change;
                                        change.router = diff_rtr_st;
                                        change.type = "set local_pref";
                                        //change.type = "set local_pref";
                                        change.value = to_string(bif->second[0].action_value_numeric);

                                        if(!check_duplicate_confic_change(cc, change) &&  preference_done_flag == false){
                                            cc.push_back(change);
                                        }

                                        // cout << "cc size: " << cc.size() << endl;
                                        flag_done = true;
                                        preference_done_flag = true;
                                        
                                        break;
                                   }
                                }
                            }
                        }
                    }
                    else{
                        //いない(beforeのベストがafterにいない)
                        //どこでdropされたか探す

                        vector<unsigned int> cur_first = best_elm_before_repair.as_path;

                        if(cur_first[cur_first.size()-1] != cr->bgp.as_number){
                            cur_first.push_back(cr->bgp.as_number);
                        }
                        //cur_first.push_back(cr->bgp.as_number);

                        // cout << "missing issue" << endl;

                        bool flag_done = false;

                        for(int th=cur_first.size()-1;th>=0;th--){
                            /*
                            string rtr = to_string(cur_first[th]);
                            struct router *rtr_st = name_to_router[rtr];
                            */

                           if(flag_done == true){
                                //break;
                           }

                            struct router *rtr_st = as_number_to_router[cur_first[th]][0];
                            string rtr = rtr_st->name;

                            // cout << "visit router " << rtr_st->name << endl;

                            //発信源の真隣がblockしたケース to do

                            //cur_firstがbest pathなルーターを見つける, endは削る
                            //why this happen

                            //このルーターのafter repairにいるかみる
                            struct bgp_table_elms elm = is_there_missing_route(cur_first, rtr_st, p, th);
                            // cout << "done is_there_missing route()" << endl;
                            if(!elm.as_path.empty()){
                                //いる場合は、preferenceで負けてる　→ そのpreferenceが何で、どこでついたかを探す
                                // cout << "priority issue at router " << rtr_st->bgp.as_number << endl;
                                struct bgp_table_elms cur_top = rtr_st->bgp_diff.loc_rib_table[p].front();

                                //check what types of priorty (lp, as_path_len)
                                //その原因を探す
                                //config_chang to seに入れる

                                if(cur_top.lp > elm.lp){
                                    // cout << "the cause is local preference difference" << endl;

                                    for(auto bif=rtr_st->bgp_diff.import_filter.begin();bif!=rtr_st->bgp_diff.import_filter.end();bif++){
                                        if(bif->second.size() == 0){
                                            continue;
                                        }

                                        if(cur_top.lp == bif->second[0].action_value_numeric){
                                            // cout << "-----Priority cause at search valid repaired config------" << endl;
                                            // cout << "router: " << rtr << endl;
                                            // cout << "peer: " << bif->first << endl;
                                            // cout << "filter name: " << bif->second[0].action << endl;
                                            // cout << "filter numeric: " << bif->second[0].action_value_numeric << endl;
                                            // cout << "------------------------" << endl;

                                            //メモする
                                            //config_changesに積もっていかないか (多分起きない)

                                            struct config_change change;
                                            change.router = rtr_st;
                                            change.type = "set local_pref";
                                            //change.type = "set local_pref";
                                            change.value = to_string(bif->second[0].action_value_numeric);

                                            if(!check_duplicate_confic_change(cc, change) &&  preference_done_flag == false){
                                                cc.push_back(change);
                                            }   

                                            preference_done_flag = true;
                                            route_map_don_flag = true;
                                            flag_done = true;
                                        }
                                    }
                                }
                            }
                            else{
                                //いない場合は、routem mapで落とされてないかチェック
                                if(!rtr_st->bgp_diff.import_filter.empty()){
                                    for(auto bif=rtr_st->bgp_diff.import_filter.begin();bif!=rtr_st->bgp_diff.import_filter.end();bif++){
                                        if(bif->second.size() == 0){
                                            continue;
                                        }

                                        //// cout << "router: " << rtr_st->name << endl;

                                        if(bif->second[0].action == "set route_map drop"){
                                            // cout << "-----Missing route cause at search valid config------" << endl;
                                            // cout << "router " << rtr << endl;
                                            // cout << "peer: " << bif->first << endl;
                                            // cout << "filter name: " << bif->second[0].action << endl;
                                            // cout << "------------------------" << endl;

                                            struct config_change change;
                                            change.router = rtr_st;
                                            change.type = bif->second[0].action;
                                            change.value = "non";

                                            if(!check_duplicate_confic_change(cc, change)){
                                                cc.push_back(change);
                                            }

                                            //cc.push_back(change);
                                            flag_done = true;
                                            route_map_don_flag = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //vector<config_change> o_vcc;

    return cc;
        /*

        if(cr->bgp.loc_rib_table[p].empty()){
            //before repairのribが存在しない && after repairでは新しく生えている
            //(i)元々ACLとかで経路広報がblockされている可能性がある or (ii) 新しく広報され出した
            //diffで、deletionがあったことを記録しているようにすれば、after repairのtraceを探索すれば見つけられる
            //つまり、after repairのtraceを探索して、before repairと異なり、なぜ新しく経路が生えたのかを調べる必要がある

            empty_rib_path_search(cr->bgp_diff.loc_rib_table[p].front(), route.prefix, route.prefix_len, &cc);
        }
        else{
            struct bgp_table_elms best_elm_before_repair = cr->bgp.loc_rib_table[p].front();
            //空をチェック、はありえル、blockの仕様の時
            list<bgp_table_elms> table_after_repair = cr->bgp_diff.loc_rib_table[p];

            //before repairのパスが存在する && before repairからafter repairではbest pathが変わっている
            //before repairのbest pathがafter repairに存在するかを調べる
            int diff = new_find_the_best_route(best_elm_before_repair, &table_after_repair);

            //存在する場合はpreference issue, 存在しない場合はmissing route issue、それぞれで原因を調べる
            if(diff > -1){
                //preference issue
                preference_path_search(best_elm_before_repair, table_after_repair.front(), cr->bgp.as_number, route.prefix, route.prefix_len, diff, &cc);

            }
            else{
                //missing route issue
                missing_route_path_search(best_elm_before_repair, table_after_repair.front(), cr, route.prefix, route.prefix_len, &cc);
            }
        }

        vcc.push_back(cc);
    }*/
}

void infer_config_repair_from_spec_for_scalability_eval_for_diff_rib_empty(struct specification spec)
{
    //vector<unsigned int> best_path_before_repair = best_entry_before_repair.as_path;
    //vector<unsigned int> best_path_after_repair = best_entry_after_repair.as_path;

    //after repairは与えられた仕様のパスを満たすと仮定できる
    //仕様のパスを探索して、そのpath changeを誘発したconfig changeをマップする

    //config changeしたノードを取得
    //vector<struct router *> vec_router = get_router_for_given_spec(spec);

    vector<struct router*> vec_router;

    // cout << "spec path size: " << spec.forwarding_path.size() << endl;

    for(int i=0;i<spec.forwarding_path.size();i++){
        //unsigned int cur_rtr = spec.forwarding_path[i];
        //struct router *r = name_to_router[to_string(cur_rtr)];

        //struct router *r = as_number_to_router[spec.forwarding_path[i]][0];
        // cout << "router name: " << spec.forwarding_path[i] << endl;
        //struct router *r = name_to_router[spec.forwarding_path[i]];
        struct router *r = as_number_to_router[stol(spec.forwarding_path[i])][0];
        vec_router.push_back(r);
    }

    //each routerにおいて、after repairのtraceを辿って、config changeを探す
    //q1: 全部が全部作用するのか
    //q2: vec_routerのrouterごとに該当するconfig_changeが違ったら？
    vector<struct config_change> vcc = search_valid_repaired_config(vec_router,spec.route);
    // cout << "vcc size: " << vcc.size() << endl;

    string ip_prefix = spec.route.prefix.to_string()+"/"+to_string(spec.route.prefix_len);
    string path = get_string_path_str(spec.forwarding_path);
    //string path = spec.forwarding_path;
    string string_spec = ip_prefix+":!"+path;

    // cout << "spec: " << string_spec << endl;

    //specとconfig changeを紐づける 
    for(int i=0;i<vcc.size();i++){
        spec_to_config_changes[string_spec].push_back(vcc[i]); 
    }
}

void infer_ospf_ecmp_config_repair_from_spec_for_scalability_eval(struct specification spec, vector<vector<struct router *>> before_repair_shortest_path, vector<vector<struct router *>> after_repair_shortest_path)
{
    //vector<unsigned int> best_path_before_repair = best_entry_before_repair.as_path;
    //vector<unsigned int> best_path_after_repair = best_entry_after_repair.as_path;

    //after repairは与えられた仕様のパスを満たすと仮定できる
    //仕様のパスを探索して、そのpath changeを誘発したconfig changeをマップする

    //config changeしたノードを取得
    //vector<struct router *> vec_router = get_router_for_given_spec(spec);

    vector<vector<struct router*>> after_repair_sp = after_repair_shortest_path;
    vector<vector<struct router*>> before_repair_sp = before_repair_shortest_path;

    //// cout << "spec path size: " << spec.forwarding_path.size() << endl;

    //before_repair_vecにあって、after_repair_vecにないものを探す
    //before_repair_vecになくて、after_repair_vecにあるものを探す
    //そのパスを追加して、search_validにかける

    /*
    unordered_map<int, bool> checked;


    for(int i=0;i<before_repair_shortest_path.size();i++){
        vector<unsigned int> cur_before_shortest_path = before_repair_shortest_path[i];
        bool f = false;
        for(int j=0;j<after_repair_shortest_path.size();j++){
            if(cur_before_shortest_path == after_repair_shortest_path[j]){
                checked[j] = true;
                f = true;
                break;
            }
        }

        if(f == false){
            vector<router *> brsp;
            for(int k=0;k<before_repair_shortest_path[i].size();k++){
                struct router *r = as_number_to_router[before_repair_shortest_path[i][k]][0];
                brsp.push_back(r);
            }

            before_repair_sp.push_back(brsp);
        }
    }

    for(int i=0;i<after_repair_shortest_path.size();i++){
        if(checked[i] == true){
            continue;
        }

        vector<router *> arsp;
        for(int k=0;k<after_repair_shortest_path[i].size();k++){
            struct router *r = as_number_to_router[after_repair_shortest_path[i][k]][0];
            arsp.push_back(r);
        }
        after_repair_sp.push_back(arsp);
    }
    */

    //each routerにおいて、after repairのtraceを辿って、config changeを探す
    //q1: 全部が全部作用するのか
    //q2: vec_routerのrouterごとに該当するconfig_changeが違ったら？
    vector<struct config_change> vcc = new_search_valid_repaired_ospf_ecmp_config(before_repair_sp, after_repair_sp, spec.route);
    // cout << "vcc size: " << vcc.size() << endl;

    string ip_prefix = spec.route.prefix.to_string()+"/"+to_string(spec.route.prefix_len);
    string path = get_string_path_str(spec.forwarding_path);
    //string path = spec.forwarding_path;
    string string_spec = ip_prefix+":"+path;

    // cout << "spec: " << string_spec << endl;

    //specとconfig changeを紐づける 
    for(int i=0;i<vcc.size();i++){
        spec_to_config_changes[string_spec].push_back(vcc[i]); 
    }
}

void infer_ospf_config_repair_from_spec_for_scalability_eval(struct specification spec, vector<unsigned int> before_repair_shortest_path, vector<unsigned int> after_repair_shortest_path)
{
    //vector<unsigned int> best_path_before_repair = best_entry_before_repair.as_path;
    //vector<unsigned int> best_path_after_repair = best_entry_after_repair.as_path;

    //after repairは与えられた仕様のパスを満たすと仮定できる
    //仕様のパスを探索して、そのpath changeを誘発したconfig changeをマップする

    //config changeしたノードを取得
    //vector<struct router *> vec_router = get_router_for_given_spec(spec);

    vector<struct router*> after_repair_sp;
    vector<struct router*> before_repair_sp;

    //// cout << "spec path size: " << spec.forwarding_path.size() << endl;

    for(int i=0;i<spec.forwarding_path.size();i++){
        //unsigned int cur_rtr = spec.forwarding_path[i];
        //struct router *r = name_to_router[to_string(cur_rtr)];

        //struct router *r = as_number_to_router[spec.forwarding_path[i]][0];
        //// cout << "router name: " << spec.forwarding_path[i] << endl;
        //struct router *r = name_to_router[spec.forwarding_path[i]];
        struct router *r = as_number_to_router[stol(spec.forwarding_path[i])][0];
        after_repair_sp.push_back(r);
    }

    for(int i=0;i<before_repair_shortest_path.size();i++){
        //// cout << "router name: " << before_repair_shortest_path[i] << endl;

        struct router *r = as_number_to_router[before_repair_shortest_path[i]][0];
        before_repair_sp.push_back(r);
    } 

    //each routerにおいて、after repairのtraceを辿って、config changeを探す
    //q1: 全部が全部作用するのか
    //q2: vec_routerのrouterごとに該当するconfig_changeが違ったら？
    vector<struct config_change> vcc = new_search_valid_repaired_ospf_config(before_repair_sp, after_repair_sp, spec.route);
    // cout << "vcc size: " << vcc.size() << endl;

    string ip_prefix = spec.route.prefix.to_string()+"/"+to_string(spec.route.prefix_len);
    string path = get_string_path_str(spec.forwarding_path);
    //string path = spec.forwarding_path;
    string string_spec = ip_prefix+":"+path;

    // cout << "spec: " << string_spec << endl;

    //specとconfig changeを紐づける 
    for(int i=0;i<vcc.size();i++){
        spec_to_config_changes[string_spec].push_back(vcc[i]); 
    }
}

void infer_config_repair_from_spec_for_scalability_eval(struct specification spec)
{
    //vector<unsigned int> best_path_before_repair = best_entry_before_repair.as_path;
    //vector<unsigned int> best_path_after_repair = best_entry_after_repair.as_path;

    //after repairは与えられた仕様のパスを満たすと仮定できる
    //仕様のパスを探索して、そのpath changeを誘発したconfig changeをマップする

    //config changeしたノードを取得
    //vector<struct router *> vec_router = get_router_for_given_spec(spec);

    vector<struct router*> vec_router;

    // cout << "spec path size: " << spec.forwarding_path.size() << endl;

    for(int i=0;i<spec.forwarding_path.size();i++){
        //unsigned int cur_rtr = spec.forwarding_path[i];
        //struct router *r = name_to_router[to_string(cur_rtr)];

        //struct router *r = as_number_to_router[spec.forwarding_path[i]][0];
        // cout << "router name: " << spec.forwarding_path[i] << endl;
        //struct router *r = name_to_router[spec.forwarding_path[i]];
        struct router *r = as_number_to_router[stol(spec.forwarding_path[i])][0];
        vec_router.push_back(r);
    }

    //each routerにおいて、after repairのtraceを辿って、config changeを探す
    //q1: 全部が全部作用するのか
    //q2: vec_routerのrouterごとに該当するconfig_changeが違ったら？
    vector<struct config_change> vcc = search_valid_repaired_config(vec_router,spec.route);
    // cout << "vcc size: " << vcc.size() << endl;

    string ip_prefix = spec.route.prefix.to_string()+"/"+to_string(spec.route.prefix_len);
    string path = get_string_path_str(spec.forwarding_path);
    //string path = spec.forwarding_path;
    string string_spec = ip_prefix+":"+path;

    // cout << "spec: " << string_spec << endl;

    //specとconfig changeを紐づける 
    for(int i=0;i<vcc.size();i++){
        spec_to_config_changes[string_spec].push_back(vcc[i]); 
    }
}

void infer_config_repair_from_spec(struct specification spec, struct bgp_table_elms best_entry_before_repair, struct bgp_table_elms best_entry_after_repair)
{
    //vector<unsigned int> best_path_before_repair = best_entry_before_repair.as_path;
    //vector<unsigned int> best_path_after_repair = best_entry_after_repair.as_path;

    //after repairは与えられた仕様のパスを満たすと仮定できる
    //仕様のパスを探索して、そのpath changeを誘発したconfig changeをマップする

    //config changeしたノードを取得
    //vector<struct router *> vec_router = get_router_for_given_spec(spec);

    vector<struct router*> vec_router;

    for(int i=0;i<spec.forwarding_path.size();i++){
        //unsigned int cur_rtr = spec.forwarding_path[i];
        //struct router *r = name_to_router[to_string(cur_rtr)];
        struct router *r = name_to_router[spec.forwarding_path[i]];
        vec_router.push_back(r);
    }

    //each routerにおいて、after repairのtraceを辿って、config changeを探す
    //q1: 全部が全部作用するのか
    //q2: vec_routerのrouterごとに該当するconfig_changeが違ったら？
    vector<struct config_change> vcc = search_valid_repaired_config(vec_router,spec.route);

    string ip_prefix = spec.route.prefix.to_string()+"/"+to_string(spec.route.prefix_len);
    string path = get_string_path_str(spec.forwarding_path);
    string string_spec = ip_prefix+":"+path;

    //specとconfig changeを紐づける 
    for(int i=0;i<vcc.size();i++){
        spec_to_config_changes[string_spec].push_back(vcc[i]); 
    }
}

int print_ospf_vec_asn(vector<unsigned int> v, bool after)
{
    unsigned int c = 0;
    // cout << "AS_PATH (size " << v.size() << "): ";
    for(int i=0;i<v.size();i++){
        //// cout << v[i] << " (" << as_number_to_router[v[i]][0]->name  << ") ";
        // cout << as_number_to_router[v[i]][0]->name  << "";
        if(i!=v.size()-1){
            struct router *tmpr = as_number_to_router[v[i]][0];
            string next = as_number_to_router[v[i+1]][0]->name;
            unsigned int cost = tmpr->ospf.peer_cost_map[next];
            unsigned int diff_cost = tmpr->ospf_diff.diff_peer_cost_map[next];

            if(diff_cost > 0 && after == true){
                // cout << "-" << diff_cost << "-";
                c += diff_cost;
            }
            else{
                // cout << "-" << cost << "-";
                c += cost;
            }
        }
    }
    // cout <<  " : total_cost " << c << endl;

    return c;
}

void print_vec_router(vector<struct router *> rv)
{
    //cout << "AS_PATH (size " << v.size() << "): ";
    for(int i=0;i<rv.size();i++){
        cout << rv[i]->name + " ";
    }
    cout << endl;
}

void print_vec_asn(vector<unsigned int> v)
{
    return;
    cout << "AS_PATH (size " << v.size() << "): ";
    for(int i=0;i<v.size();i++){
        cout << v[i] << " (" << as_number_to_router[v[i]][0]->name  << ") ";
    }
    cout << endl;
}

void show_spec_to_se()
{
    // cout << endl;
    // cout << "Start to show maps from spec to side effect" << endl;

    string prev_cc = "";

    for(auto itr=spec_to_config_changes.begin();itr!=spec_to_config_changes.end();itr++){
        string spec = itr->first;
        vector<struct config_change> ccv = itr->second;

        string s_cc;
        for(int i=0;i<ccv.size();i++){
            struct config_change cc = ccv[i];
            s_cc += cc.router->name+"."+cc.type+"."+cc.value+":";
        }

        if(s_cc == prev_cc){
            continue;
        }
        prev_cc = s_cc;

        // cout << "change: " << s_cc << endl;

        if(!cchange_to_se_info[s_cc].empty()){
            // cout << "--------------------" << endl; 

            // cout << "spec: " << spec << endl;
            // cout << "config_change: " << s_cc << endl;


            vector<side_effect_info> seiv = cchange_to_se_info[s_cc];


            for(int i=0;i<seiv.size();i++){
                struct side_effect_info sei = seiv[i];
                
                // cout << endl;
                // cout << "idx " << i << endl;
                // cout << "name: router " << sei.router->name << endl;
                // cout << "best path before repair: ";
                print_vec_asn(sei.before_repair_path);
                // cout << "best path after repair: ";
                print_vec_asn(sei.after_repair_path);
            }

            // cout << "--------------------" << endl;
        }
        else{
            // cout << "No map from spec to se" << endl;
            exit(-1);
        }

        // cout << endl;
    }
    // cout << endl;
}

void create_dummy_specification(string dummy, struct router *r, string type, string value)
{
    struct config_change cc;

    cc.router = r;
    cc.type = type;
    cc.value = value;

    spec_to_config_changes[dummy].push_back(cc);
}

void show_as_path(struct bgp_table_elms bte)
{
    return;
    // cout << "AS_PATH (size " << bte.as_path.size() << "): ";
    for(int i=0;i<bte.as_path.size();i++){
        // cout << bte.as_path[i] << " ";
    }
    // cout << endl;
}

void read_topology(string path)
{
    std::ifstream ifs(path);
    string line;

    unordered_map<string, bool> is_used;

    // cout << "read network topology:" << endl;

    while(getline(ifs, line)){
        string src;
        vector<string> dst_vec;

        stringstream iss(line);
        
        int i=0;
        string d;

        while(iss >> d){
            if(i == 0){
                src = d;
            }
            else{
                dst_vec.push_back(d);
            }
            i+=1;
        }

        for(int i=0;i<dst_vec.size();i++){
            zoo_topo[src].push_back(dst_vec[i]);
            // cout << "src: " << src << " dst: " << dst_vec[i] << endl;
        }
    }
}


void show_current_que(priority_queue<dTuple, vector<dTuple>, compare_bgp_message_tuple> que)
{
    return;
    while(!que.empty()){
        dTuple dt = que.top();
        que.pop();

        struct bgp_message bm = get<0>(dt);
        struct bgp_message diff_bm = get<1>(dt);
        struct router *router = get<2>(dt);

        // cout << "router: " << router->name << endl;
        // cout << "bm.path: ";
        for(int i=0;i<bm.as_path.size();i++){
            // cout << bm.as_path[i] << " ";
        }
        // cout << endl;

        // cout << "diff_bm.path: ";
        for(int i=0;i<diff_bm.as_path.size();i++){
            // cout << diff_bm.as_path[i] << " ";
        }
        // cout << endl;
    }
}


vector<router> read_network()
{
    vector<router> vec_router;

    // cout << "create router instances:" << endl;

    for(auto itr=zoo_topo.begin();itr!=zoo_topo.end();itr++){
        struct router r;
        r.name = itr->first;
        r.bgp.loc_rib = new radix_tree;
        r.bgp.loc_rib_diff = new radix_tree;

        vec_router.push_back(r);

        // cout << "router " << r.name << " is added." << endl;
    }

    // cout << "total number of routers: " << vec_router.size() << endl;

    return vec_router;
}

void read_test_specification()
{
    struct specification spec;

    // cout << "do not use this function" << endl;
    exit(-1);
    
    for(unsigned int i=0;i<10;i++){
        unsigned int a = 1000;
        //spec.forwarding_path.push_back(a+i);
    }

    for(unsigned int i=0;i<1000;i++){
        vec_specs.push_back(spec);
    }
}

void read_specification_for_scalability_eval()
{
    struct specification spec;

    //infer_config_repair_from_spec_for_scalability_eval(spec);
}

void set_name_to_router(vector<router> *vec_router)
{
    for(int i=0;i<vec_router->size();i++){
        name_to_router[(*vec_router)[i].name] = &(*vec_router)[i];
    }
}

bool is_next_as_included(vector<unsigned int> as_path, unsigned int next_as)
{
    for(int i=0;i<as_path.size();i++){
        if(as_path[i] == next_as){
            return true;
        }
    }
    return false;
}


bool is_as_path_included(vector<unsigned int> as_path, unsigned int as_number)
{
    // cout << "input as_number: " << as_number << endl;
    // cout << "as_path: ";
    for(int i=0;i<as_path.size();i++){
        // cout << as_path[i] << " ";
        if(as_path[i] == as_number){
            return true;
        }
    }
    // cout << endl;
    return false;
}

/*
void create_diff_bgp_message(struct bgp_message *cur_message)
{
    cur_message->diff_message = new bgp_message;
    cur_message->diff_message->adv_route = cur_message->adv_route;
    cur_message->diff_message->as_path = cur_message->as_path;
    cur_message->diff_message->community = cur_message->community;
    cur_message->diff_message->lp = cur_message->lp;
    cur_message->diff_message->type = cur_message->type;
}
*/

bool is_prefix_include(bitset<32> left, int left_prefixlen, bitset<32> right, int right_prefixlen)
{
    if(right_prefixlen == 0){
        //// cout << "1" << endl;
        return true;
    }
    else if(left_prefixlen == 0 && right_prefixlen != 0){
        //// cout << "2" << endl;
        return false;
    }
    else if(left_prefixlen != 0 && right_prefixlen != 0/*right_prefixlen <= left_prefixlen*/){
        //// cout << "3" << endl;
        int base_prefix = right_prefixlen;
        /*
        if(right_prefixlen < left_prefixlen){
            base_prefix = right_prefixlen;
        }
        else{
            base_prefix = left_prefixlen;
        }*/

        int range = 32 - base_prefix;

        for(int i=0;i<range;i++){
            right.reset(i);
            left.reset(i);
        }

        if(right == left){
            return true;
        }
        
        return false;
    }
    else{
        // cout << "alert !!!" << endl;
    }

    //// cout << "4" << endl;
    return false;
}

bool is_prefix_same(bitset<32> left, int left_prefixlen, bitset<32> right, int right_prefixlen)
{
    if(left_prefixlen == right_prefixlen && left == right){
        return true;
     }

    return false;
}

bool is_include(struct header_constraint *left, struct header_constraint *right, int type)
{
    //// cout << left->dst_addr << endl;
    //// cout << right->dst_addr << endl;
    //// cout << "src"<<endl;

    if(is_prefix_include(left->src_addr,left->src_prefixlen, right->src_addr, right->src_prefixlen)){
        if(is_prefix_include(left->dst_addr,left->dst_prefixlen, right->dst_addr, right->dst_prefixlen)){
            return true;
        }
    }

    return false;
}


bool is_path_given_in_spec(struct v4_route *route, vector<unsigned int> as_path)
{
    vector<unsigned int> spec_as_path;

    for(int i=0;i<vec_specs.size();i++){
        if(is_prefix_include(route->prefix, route->prefix_len, vec_specs[i].route.prefix, vec_specs[i].route.prefix_len)){
            if(is_str_path_equal(convert_upath_to_spath(as_path), vec_specs[i].forwarding_path)){
                is_spec_satisfied[i] = true;
                return true;
            }
        }
    }

    // for test
    return true;
    return false;
}

vector<struct specification> read_unsatisfied_spec()
{
    vector<struct specification> unsatisfied_spec;

    for(int i=0;i<vec_specs.size();i++){
        if(is_spec_satisfied[i] == false){
            unsatisfied_spec.push_back(vec_specs[i]);
        }
    }

    return unsatisfied_spec;
}

vector<router> define_network()
{
    vector<router> vec_router;

    //define router
    struct interface i1,i2,i3;
    i1.name = "i1";
    i2.name = "i2";
    i3.name = "i3";

    struct router r1;
    r1.name = "r1";
    r1.infs.push_back(i1);
    r1.infs.push_back(i2);
    r1.bgp.loc_rib = new radix_tree;
    r1.bgp.loc_rib_diff = new radix_tree;

    struct router r2;
    r2.name = "r2";
    r2.infs.push_back(i1);
    r2.infs.push_back(i2);
    r2.bgp.loc_rib = new radix_tree;
    r2.bgp.loc_rib_diff = new radix_tree;

    struct router r3;
    r3.name = "r3";
    r3.infs.push_back(i1);
    r3.infs.push_back(i2);
    r3.infs.push_back(i3);
    r3.bgp.loc_rib = new radix_tree;
    r3.bgp.loc_rib_diff = new radix_tree;


    struct router r4;
    r4.name = "r4";
    r4.infs.push_back(i1);
    r4.infs.push_back(i2);
    r4.infs.push_back(i3);
    r4.bgp.loc_rib = new radix_tree;
    r4.bgp.loc_rib_diff = new radix_tree;


    vec_router.push_back(r1);
    vec_router.push_back(r2);
    vec_router.push_back(r3);
    vec_router.push_back(r4);

    //define topology
    topology[r1.name+"i1"] = r2.name+"i1";
    topology[r2.name+"i1"] = r1.name+"i1";

    topology[r1.name+"i2"] = r3.name+"i1";
    topology[r3.name+"i1"] = r1.name+"i2";

    topology[r2.name+"i2"] = r4.name+"i1";
    topology[r4.name+"i1"] = r2.name+"i2";

    topology[r3.name+"i2"] = r4.name+"i2";
    topology[r4.name+"i2"] = r3.name+"i2";

    return vec_router;
}

void set_import_filter(vector<router> *vec_router)
{
    //R1とR3はif community == 2ならdrop

    /*
    for(int i=0;i<vec_router->size();i++){
        if((*vec_router)[i].name == "r1"){
            struct filter f;
            f.action = "match community drop";
            f.action_value = "2";

            (*vec_router)[i].bgp.import_filter[3].push_back(f);
        }
        else if((*vec_router)[i].name == "r3"){
            struct filter f;
            f.action = "match community drop";
            f.action_value = "2";

            (*vec_router)[i].bgp.import_filter[4].push_back(f);
        }
    }
    */

    for(int i=0;i<vec_router->size();i++){
        if((*vec_router)[i].name == "r1"){
            struct filter f;
            f.action = "set local_pref";
            f.action_value_numeric = 400;

            (*vec_router)[i].bgp.import_filter[200].push_back(f);
        }
        else if((*vec_router)[i].name == "r2"){
            struct filter f;
            f.action = "set local_pref";
            f.action_value_numeric = 300;

            (*vec_router)[i].bgp.import_filter[300].push_back(f);
        }
    }
}

void set_export_filter(vector<router> *vec_router)
{
    //r2のi1とi2にset community 2

    
    for(int i=0;i<vec_router->size();i++){
        /*
        if((*vec_router)[i].name != "r2"){
            continue;
        }
        
        struct filter f;
        f.prefix = "0.0.0.0/0";
        f.action = "set community";
        f.action_value = "2";

        (*vec_router)[i].bgp.export_filter[4].push_back(f);
        */
    }
}

void set_ibgp_peer_example(vector<router> *vec_router)
{
    (*vec_router)[0].bgp.i_peer_relation.push_back(&(*vec_router)[1]);
    (*vec_router)[1].bgp.i_peer_relation.push_back(&(*vec_router)[0]);
}

void set_ebgp_peer_example(vector<router> *vec_router)
{
    //router R1 peers AS 200
    (*vec_router)[0].bgp.e_peer_relation[200] = &(*vec_router)[2];
    (*vec_router)[2].bgp.e_peer_relation[100] = &(*vec_router)[0];

    (*vec_router)[1].bgp.e_peer_relation[300] = &(*vec_router)[3];
    (*vec_router)[3].bgp.e_peer_relation[100] = &(*vec_router)[1];

    (*vec_router)[2].bgp.e_peer_relation[300] = &(*vec_router)[3];
    (*vec_router)[3].bgp.e_peer_relation[200] = &(*vec_router)[2];
}

void set_ebgp_peer(vector<router> *vec_router)
{
    //隣接するノードにはる
    for(int i=0;i<vec_router->size();i++){
        // cout << "node: " << (*vec_router)[i].name << endl;  
        vector<string> neigh = zoo_topo[(*vec_router)[i].name];
        // cout << "number of neighbors: " << neigh.size() << endl;

        for(int j=0;j<neigh.size();j++){
         // cout << "nei router: " << neigh[j] << endl;
            struct router *r = name_to_router[neigh[j]];

            (*vec_router)[i].bgp.e_peer_relation[r->bgp.as_number] = r;
            //test_03_30
            /*
            struct filter f;
            f.action = "set local_pref";
            f.action_value_numeric = 100;
            (*vec_router)[i].bgp.import_filter[r->bgp.as_number].push_back(f);
            */
            // cout << "router " << (*vec_router)[i].name << " (AS " << (*vec_router)[i].bgp.as_number << ") peers router " << r->name << " (AS " << r->bgp.as_number << ")" << endl;
        }
    }
}

void set_ibgp_peer(vector<router> *vec_router)
{

}

void set_bgp_peer(vector<router> *vec_router)
{
    /*
    set_ebgp_peer_example(vec_router);
    set_ibgp_peer_example(vec_router);
    */

    set_ibgp_peer(vec_router);

    set_ebgp_peer(vec_router);
}

void set_diff_config(vector<router> *vec_router)
{
    (*vec_router)[1].bgp_diff.as_number = (*vec_router)[1].bgp.as_number;
    (*vec_router)[1].bgp_diff.adj_rib_in = (*vec_router)[1].bgp.adj_rib_in;
    (*vec_router)[1].bgp_diff.import_filter = (*vec_router)[1].bgp.import_filter;
    (*vec_router)[1].bgp_diff.loc_rib = new radix_tree;
    (*vec_router)[1].bgp_diff.adj_rib_out = (*vec_router)[1].bgp.adj_rib_out;
    (*vec_router)[1].bgp_diff.export_filter = (*vec_router)[1].bgp.export_filter;
    (*vec_router)[1].bgp_diff.advertised_v4_route = (*vec_router)[1].bgp.advertised_v4_route;
    (*vec_router)[1].bgp_diff.e_peer_relation = (*vec_router)[1].bgp.e_peer_relation;
    (*vec_router)[1].bgp_diff.i_peer_relation = (*vec_router)[1].bgp.i_peer_relation;

    //add diff
    struct filter f;
    f.action = "set local_pref";
    f.action_value_numeric = 500;

    (*vec_router)[1].bgp_diff.import_filter[300].push_back(f);

    for(int i=0;i<vec_router->size();i++){
        (*vec_router)[i].bgp_diff.loc_rib = new radix_tree;
        (*vec_router)[i].bgp_diff.as_number = (*vec_router)[i].bgp.as_number;
        (*vec_router)[i].bgp_diff.e_peer_relation = (*vec_router)[i].bgp.e_peer_relation;
        (*vec_router)[i].bgp_diff.i_peer_relation = (*vec_router)[i].bgp.i_peer_relation;
    }
    /*
    for(int i=0;i<vec_router->size();i++){
        (*vec_router)[i].bgp_diff.adj_rib_in = (*vec_router)[i].bgp.adj_rib_in;
        (*vec_router)[i].bgp_diff.import_filter = (*vec_router)[i].bgp.import_filter;
        (*vec_router)[i].bgp_diff.loc_rib = new radix_tree;
        (*vec_router)[i].bgp_diff.adj_rib_out = (*vec_router)[i].bgp.adj_rib_out;
        (*vec_router)[i].bgp_diff.export_filter = (*vec_router)[i].bgp.export_filter;
        (*vec_router)[i].bgp_diff.advertised_v4_route = (*vec_router)[i].bgp.advertised_v4_route;
        //(*vec_router)[i].bgp_diff.peer_relation = (*vec_router)[i].bgp.peer_relation;
    }
    */

    /*
    struct filter f;
    f.prefix = "0.0.0.0/0";
    f.action = "set local_pref";
    f.action_value_numeric = 500;

    pair<filter, bool> filter_diff;
    filter_diff.first = f;
    filter_diff.second = false;
    */

    //should use import/export filter

    //(*vec_router)[1].bgp_diff.export_filter[4].clear();
}


void run_bgp_decision_process(vector<router> *vec_router)
{
    /*
        for each router
            run bgp_decision process
    */
}

void advertised_route_diff_check(struct bgp_message bm, struct router *router, unsigned int as_number)
{
    if(router->bgp_diff.advertised_v4_route.size() > 0){

    }
}

void import_filter_diff_check(struct bgp_message *bm, struct bgp_message *diff_bm ,struct router *router, unsigned int as_number)
{
    vector<filter> vec_filter = router->bgp_diff.import_filter[as_number];

    //bmのprefixに関連するdiffのみに修正

    if(vec_filter.size() > 0){
        msg_id++;
    
        bm->unique_id = msg_id;
        //// cout << "bm->unique_id " << bm->unique_id << endl;
        *diff_bm = *bm;
        diff_traverse_history = traverse_history;

        //// cout << "diff_happen at import filter at router " << router->name << endl;
    }
}

void export_filter_diff_check(struct bgp_message *bm, struct bgp_message *diff_bm, struct router *router, unsigned int as_number)
{
    vector<filter> vec_filter = router->bgp_diff.export_filter[as_number];

    //to_do: custmize for prefix-based check
    if(vec_filter.size() > 0){
        msg_id++;
        bm->unique_id = msg_id;

        *diff_bm = *bm;
        diff_traverse_history = traverse_history;

        //// cout << "diff_happen at export filter at router " << router->name << endl;
    }
}

bool apply_default_diff_import_filter(struct bgp_message *bm, struct router *router, unsigned int as_number)
{
    /*
    if(router->is_bgp_diff == false){
        return apply_import_filter(bm,router,as_number);
    }
    */

    vector<filter> f = router->bgp_diff.default_import_filter[as_number];

   /*
    if(f.size()>0){
        // cout << "apply diff import filter at router " << router->name << " (AS " << router->bgp.as_number << ")" << endl;
    }
    */

    for(int i=0;i<f.size();i++){
        if(f[i].action == "set community"){
            //prefix matching
            if(true){
                //set community
                bm->community = stoi(f[i].action_value);
            }
        }
        else if(f[i].action == "set route_map drop"){
            bm->sonzai = false;
            bm->terminated = true;
            bm->terminated_point = router->name;

            // cout << "dropped !" << endl;

            return false;
        }
        else if(f[i].action == "match community drop"){
            if(bm->community == stoi(f[i].action_value)){
                // cout << router->name << endl;
                return false;
            }
        }
        else if(f[i].action == "set local_pref"){
            bm->lp = f[i].action_value_numeric;
            //// cout << "new lp: " << bm->lp << endl;
        }
    }

    return true;
}

bool apply_diff_import_filter(struct bgp_message *bm, struct router *router, unsigned int as_number)
{
    /*
    if(router->is_bgp_diff == false){
        return apply_import_filter(bm,router,as_number);
    }
    */

    vector<filter> f = router->bgp_diff.import_filter[as_number];

    if(f.size()>0){
        // cout << "apply diff import filter at router " << router->name << " (AS " << router->bgp.as_number << ")" << endl;
    }

    for(int i=0;i<f.size();i++){
        if(f[i].action == "set community"){
            //prefix matching
            if(true){
                //set community
                bm->community = stoi(f[i].action_value);
            }
        }
        else if(f[i].action == "set route_map drop"){
            bm->sonzai = false;
            bm->terminated = true;
            bm->terminated_point = router->name;

            //return false;
        }
        else if(f[i].action == "match community drop"){
            if(bm->community == stoi(f[i].action_value)){
                // cout << router->name << endl;
                return false;
            }
        }
        else if(f[i].action == "set local_pref"){
            bm->lp = f[i].action_value_numeric;
            //// cout << "new lp: " << bm->lp << endl;
        }
    }

    return true;
}

bool apply_default_import_filter(struct bgp_message *bm, struct bgp_message *diff_bm, struct router *router, unsigned int as_number)
{
    vector<filter> f = router->bgp.default_import_filter[as_number];

    //dropするときはterminate

    for(int i=0;i<f.size();i++){
        if(f[i].action == "set community"){
            //prefix matching
            if(true){
                //set community
                bm->community = stoi(f[i].action_value);
            }
        }
        else if(f[i].action == "set route_map drop"){
            bm->sonzai = false;
            bm->terminated = true;
            bm->terminated_point = router->name;

            // cout << "dropped !" << endl;

            return false;
        }
        else if(f[i].action == "match community drop"){
            if(bm->community == stoi(f[i].action_value)){
                // cout << router->name << endl;
                return false;
            }
        }
        else if(f[i].action == "set local_pref"){
            bm->lp = f[i].action_value_numeric;   
        }
    }

    return true;
}

bool apply_import_filter(struct bgp_message *bm, struct bgp_message *diff_bm, struct router *router, unsigned int as_number)
{
    vector<filter> f = router->bgp.import_filter[as_number];

    //filterに引っ掛かったら、bm->terminated == trueにする

    if(diff_bm->sonzai == false){
        import_filter_diff_check(bm, diff_bm,router,as_number);
    }

    //dropするときはterminate

    for(int i=0;i<f.size();i++){
        if(f[i].action == "set community"){
            //prefix matching
            if(true){
                //set community
                bm->community = stoi(f[i].action_value);
            }
        }
        else if(f[i].action == "match community drop"){
            if(bm->community == stoi(f[i].action_value)){
                // cout << router->name << endl;
                return false;
            }
        }
        else if(f[i].action == "set local_pref"){
            bm->lp = f[i].action_value_numeric;   
        }
    }

    if(diff_bm->sonzai == true && diff_bm->terminated == false){
        apply_diff_import_filter(diff_bm,router,as_number);
        //test_03_25
    }

    return true;
}

void new_import_filter_diff_check(struct bgp_message *bm, struct router *router, unsigned int as_number)
{
    vector<filter> vec_filter = router->bgp_diff.import_filter[as_number];

    //to_do: custmize for prefix-based check
    if(vec_filter.size() > 0){
        dPair dp{*bm, router};
        dif_pq.push(dp);
    }
}

bool new_apply_import_filter(struct bgp_message *bm,struct router *router, unsigned int as_number)
{
    vector<filter> f = router->bgp.import_filter[as_number];

    //dropするときはterminate

    new_import_filter_diff_check(bm, router, as_number);

    for(int i=0;i<f.size();i++){
        if(f[i].action == "set community"){
            //prefix matching
            if(true){
                //set community
                bm->community = stoi(f[i].action_value);
            }
        }
        else if(f[i].action == "set route_map drop"){
            bm->sonzai = false;
            bm->terminated = true;
            bm->terminated_point = router->name;

            // cout << "dropped !" << endl;

            return false;
        }
        else if(f[i].action == "match community drop"){
            if(bm->community == stoi(f[i].action_value)){
                // cout << router->name << endl;
                return false;
            }
        }
        else if(f[i].action == "set local_pref"){
            bm->lp = f[i].action_value_numeric;   
        }
    }

    return true;
}

bool new_apply_default_import_filter(struct bgp_message *bm,struct router *router, unsigned int as_number)
{
    vector<filter> f = router->bgp.default_import_filter[as_number];

    //dropするときはterminate

    new_import_filter_diff_check(bm, router, as_number);

    for(int i=0;i<f.size();i++){
        if(f[i].action == "set community"){
            //prefix matching
            if(true){
                //set community
                bm->community = stoi(f[i].action_value);
            }
        }
        else if(f[i].action == "match community drop"){
            if(bm->community == stoi(f[i].action_value)){
                // cout << router->name << endl;
                return false;
            }
        }
        else if(f[i].action == "set local_pref"){
            bm->lp = f[i].action_value_numeric;   
        }
    }

    return true;
}


bool apply_diff_export_filter(struct bgp_message *bdm, struct router *router,unsigned int as_number)
{
    /*
    if(router->is_bgp_diff == false){
        return apply_export_filter(bm,router,as_number);
    }
    */

    vector<filter> f = router->bgp_diff.export_filter[as_number];

    /*
    if(global_dss.diff == false){
        export_filter_diff_check(*bm,router,as_number);
    }*/

    for(int i=0;i<f.size();i++){
        if(f[i].action == "set community"){
            //prefix matching
            if(true){
                //set community
                bdm->community = stoi(f[i].action_value);
            }
        }
        else if(f[i].action == "set route_map drop"){
            bdm->sonzai = false;
            bdm->terminated = true;
            bdm->terminated_point = router->name;
        }
        else if(f[i].action == "set local_pref"){
            bdm->lp = f[i].action_value_numeric;   
            //// cout << "new lp: " << bdm->lp << endl;
        }
    }

    return true;
}

bool apply_export_filter(struct bgp_message *bm, struct bgp_message *diff_bm, struct router *router,unsigned int as_number)
{
    vector<filter> f = router->bgp.export_filter[as_number];

    if(diff_bm->sonzai == false){
        export_filter_diff_check(bm,diff_bm,router,as_number);
    }

    //to_do:dropするときはterminate

    for(int i=0;i<f.size();i++){
        if(f[i].action == "set community"){
            //prefix matching
            if(true){
                //set community
                bm->community = stoi(f[i].action_value);
            }
        }
        else if(f[i].action == "set route_map drop"){
            bm->sonzai = false;
            bm->terminated = true;
            bm->terminated_point = router->name;
        }
        else if(f[i].action == "set local_pref"){
            bm->lp = f[i].action_value_numeric;   
        }
    }

    if(diff_bm->sonzai == true &&  diff_bm->terminated == false){
        apply_diff_export_filter(diff_bm,router,as_number);
        //test_03_25
    }

    return true;
}



void new_export_filter_diff_check(struct bgp_message *bm, struct router *router, unsigned int as_number)
{
    vector<filter> vec_filter = router->bgp_diff.export_filter[as_number];

    //to_do: custmize for prefix-based check
    if(vec_filter.size() > 0){
        // cout << "you should not come here this time !!!!" << endl;

        dPair dp{*bm, router};
        dif_pq.push(dp);
    }
}

bool new_apply_export_filter(struct bgp_message *bm, struct router *router,unsigned int as_number)
{
    vector<filter> f = router->bgp.export_filter[as_number];

    //diff exist

    /*
    if(diff_bm->sonzai == false){
        export_filter_diff_check(bm,diff_bm,router,as_number);
    }*/

    //if diff
   new_export_filter_diff_check(bm, router, as_number);

    for(int i=0;i<f.size();i++){
        if(f[i].action == "set community"){
            //prefix matching
            if(true){
                //set community
                bm->community = stoi(f[i].action_value);
            }
        }
        else if(f[i].action == "set route_map drop"){
            bm->sonzai = false;
            bm->terminated = true;
            bm->terminated_point = router->name;
            
            //test_05_21
            return false;
        }
        else if(f[i].action == "set local_pref"){
            bm->lp = f[i].action_value_numeric;   
        }
    }

    /*
    if(diff_bm->sonzai == true &&  diff_bm->terminated == false){
        apply_diff_export_filter(diff_bm,router,as_number);
        //test_03_25
    }
    */


    return true;
}



bool check_diff_route_preference(string cur_node, struct bgp_message cur_bm)
{
    if(diff_traverse_history.find(cur_node) != diff_traverse_history.end()){
        if(diff_traverse_history[cur_node].lp != 0 || cur_bm.lp != 0){
            if(cur_bm.lp <= diff_traverse_history[cur_node].lp){
                return false;
            }

            return true;
        }
        else if(cur_bm.as_path.size() >= diff_traverse_history[cur_node].as_path.size()){
            return false;
        }
    }

    return true;
}


void add_diff_new_advertised_prefix(string str_router, unsigned int as_number, string ip_addr, int plen)
{
    struct v4_route v4r;
    v4r.prefix = std::bitset<32>(get_string_to_uint_ipv4_addr(ip_addr));
    v4r.prefix_len = plen;

    struct router *router = name_to_router[str_router];

    pair<unsigned int, vector<v4_route>> adv_route;
    adv_route.first = as_number;
    adv_route.second.push_back(v4r);

    router->bgp_diff.advertised_v4_route.push_back(adv_route);
}


void add_new_advertised_prefix(string str_router, unsigned int as_number, string ip_addr, int plen)
{
    struct v4_route v4r;
    v4r.prefix = std::bitset<32>(get_string_to_uint_ipv4_addr(ip_addr));
    v4r.prefix_len = plen;

    struct router *router = name_to_router[str_router];

    pair<unsigned int, vector<v4_route>> adv_route;
    adv_route.first = as_number;
    adv_route.second.push_back(v4r);

    router->bgp.advertised_v4_route.push_back(adv_route);
}

/*
void rec_route_i_propagation_dfs(struct bgp_message bm, struct router *router, unordered_map<string, bool> is_visited)
{
    if(is_visited[router->name] == true){
        return;
    }

    // cout << "ibgp visited at " << router->name << endl;

    unsigned int prev_as_number = bm.as_path[bm.as_path.size()-1]; 

    //store bgp_adj_rib_in
    router->bgp.adj_rib_in[prev_as_number].push_back(bm.adv_route);

    //check import filter
    if(apply_import_filter(&bm, router, prev_as_number)){
        //store bgp_loc_rib_in
        struct route_data data;
        data.prefix = bm.adv_route.prefix;
        data.prefixlen = bm.adv_route.prefix_len;
        //add parameter
        data.weight = 0;
        data.local_pref = bm.lp;
        bitset_radix_tree_add_w_pref(router->bgp.loc_rib, data,bm.adv_route.prefix_len, bm.adv_route.prefix);

        //set as number
        //bm.as_path.push_back(router->bgp.as_number);
        print_as_numbers(bm);
        // cout << "local_pref: " << bm.lp << endl;

        for(int i = 0;i<router->bgp.i_peer_relation.size();i++){
            router->bgp.adj_rib_out[router->bgp.i_peer_relation[i]->bgp.as_number].push_back(bm.adv_route);
            if(apply_export_filter(&bm, router,router->bgp.i_peer_relation[i]->bgp.as_number)){
                struct router *next_router = router->bgp.i_peer_relation[i];
                // cout << "i_peer_relation: " << next_router->name << endl;
                // cout << "AS number: " << next_router->bgp.as_number << endl;
                is_visited[router->name] = true;
                rec_route_i_propagation_dfs(bm, next_router,is_visited);
            }
        }
    }
}
*/



bool test_que_check(dTuple dt, priority_queue<dTuple, vector<dTuple>, compare_bgp_message_tuple> *que)
{
    priority_queue<dTuple, vector<dTuple>, compare_bgp_message_tuple> tmp_que = *que;
    while(!tmp_que.empty()){
        dTuple tmp_dt = tmp_que.top();
        tmp_que.pop();

        struct bgp_message bm = get<0>(dt);
        struct bgp_message diff_bm = get<1>(dt);
        struct router *router = get<2>(dt);

        struct bgp_message tmp_bm = get<0>(tmp_dt);
        struct bgp_message tmp_diff_bm = get<1>(tmp_dt);
        struct router *tmp_router = get<2>(tmp_dt);

        if(router->name == tmp_router->name && bm.as_path == tmp_bm.as_path && diff_bm.as_path == tmp_diff_bm.as_path){
            return true;
        }
    }
    return false;
}

static int test_count = 0;
bool is_exist_this_path(list<struct bgp_table_elms> *loc_rib_table, vector<unsigned int> as_path, vector<unsigned int> cur_best_path, bool is_diff, string cur_node, struct router *router, priority_queue<dTuple, vector<dTuple>, compare_bgp_message_tuple> *que)
{
    bool deleted = false;
    bool begin = false;

    /*
    // cout << "is_exist_this_path start" << endl;
    // cout << "At router " << router->name << endl;
    // cout << "loc_rib_table size: " << loc_rib_table->size() << endl;
    */

    for(auto itr=loc_rib_table->begin();itr!=loc_rib_table->end();){
        /*
            // cout << "canditate path: ";
            for(int i=0;i<itr->as_path.size();i++){
                // cout << itr->as_path[i] << " ";
            }     
            // cout << endl;
            */
        if(is_path_equal(itr->as_path, as_path)){
            //withdrawal
            
            /*
            // cout << "Path deleted at router " << router->name << endl;
            // cout << "the path: ";
            for(int i=0;i<itr->as_path.size();i++){
                // cout << itr->as_path[i] << " ";
            }
            // cout << endl;
            */

            if(itr==loc_rib_table->begin()){
                begin = true;
            }

            itr = loc_rib_table->erase(itr);
            deleted = true;

            //// cout << "Router " << router->name << " loc_rib_table size: " << loc_rib_table->size() << endl;
            
        }
        else{
            itr++;
        }
    }

    //代わりに新しいのを入れる

    //// cout << "deleted: " << deleted << " begin: " << begin << endl;
    //eBPP peerを入れる、ダブりは落とす処理を入れる
    if(loc_rib_table->size() > 0  && deleted == true && begin == true){
        struct bgp_message bm;
        struct bgp_message diff_bm;
        bm = loc_rib_table->begin()->prev_bgp_message;
        diff_bm = loc_rib_table->begin()->prev_diff_bgp_message;

        if(loc_rib_table->begin()->in_out == 2){
            return begin;
        }

        //// cout << "new que at is_exist_this_path" << endl;
        //// cout << "bm.as_path: ";

        /*  
        for(int i=0;i<bm.as_path.size();i++){
            // cout << bm.as_path[i] << " ";
        }
        // cout << endl;

        // cout << "diff_bm.as_path: ";
        for(int i=0;i<diff_bm.as_path.size();i++){
            // cout << diff_bm.as_path[i] << " ";
        }
        // cout << endl;
        */
        
        for(int i=0;i<router->bgp.advertised_peers.size();i++){
            /*
            if(router->bgp.received_peers[router->bgp.advertised_peers[i]] == true){
                // cout << "already received from router " << router->bgp.advertised_peers[i] << endl;
                // cout << "no bgp update we can send" << endl;
                continue;
            }
            */

           /*
            // cout << "send a new update route for e_peer_relation router " << router->bgp.advertised_peers[i] << endl;
            if(is_next_as_included(cur_best_path, router->bgp.advertised_peers[i])){
                // cout << "next as is included in cur_best_path" << endl;
                // cout << "cur_node: " << cur_node << endl;   
                continue;
            }

            if(is_next_as_included(as_path, router->bgp.advertised_peers[i])){
                // cout << "next as is included in as_path" << endl;
                continue;
            }
            */

            dTuple dt{bm, diff_bm, router->bgp.e_peer_relation[router->bgp.advertised_peers[i]]};
            /*
            if(test_que_check(dt, que)){
                continue;
            }*/

            //// cout << "added at router " << router->bgp.advertised_peers[i] << endl;
            que->push(dt);
        }
    }

    return deleted;
}

bool is_exist_this_path_diff(list<struct bgp_table_elms> *loc_rib_table, vector<unsigned int> as_path, vector<unsigned int> cur_best_path, bool is_diff, string cur_node, struct router *router, priority_queue<dTuple, vector<dTuple>, compare_bgp_message_tuple> *que)
{
    bool deleted = false;
    bool begin = false;


    for(auto itr=loc_rib_table->begin();itr!=loc_rib_table->end();){
        /*
            // cout << "canditate path: ";
            for(int i=0;i<itr->as_path.size();i++){
                // cout << itr->as_path[i] << " ";
            }     
            // cout << endl;
            */
        if(is_path_equal(itr->as_path, as_path)){
            //withdrawal
            /*
            // cout << "Path deleted at router " << router->name << endl;
            // cout << "the path: ";
            for(int i=0;i<itr->as_path.size();i++){
                // cout << itr->as_path[i] << " ";
            }
            // cout << endl;
            */

            if(itr==loc_rib_table->begin()){
                begin = true;
            }

            itr = loc_rib_table->erase(itr);
            deleted = true;

            //// cout << "Router " << router->name << " loc_rib_table size: " << loc_rib_table->size() << endl;
            
        }
        else{
            itr++;
        }
    }

    //代わりに新しいのを入れる

    //// cout << "deleted: " << deleted << " begin: " << begin << endl;
    //eBPP peerを入れる、ダブりは落とす処理を入れる
    if(loc_rib_table->size() > 0  && deleted == true && begin == true){
        struct bgp_message bm;
        struct bgp_message diff_bm;
        bm = loc_rib_table->begin()->prev_bgp_message;
        diff_bm = loc_rib_table->begin()->prev_diff_bgp_message;

        if(loc_rib_table->begin()->in_out == 2){
            return begin;
        }

        /*
        // cout << "new que at is_exist_this_path" << endl;
        // cout << "bm.as_path: ";

        for(int i=0;i<bm.as_path.size();i++){
            // cout << bm.as_path[i] << " ";
        }
        // cout << endl;
        */

        /*
        // cout << "diff_bm.as_path: ";
        for(int i=0;i<diff_bm.as_path.size();i++){
            // cout << diff_bm.as_path[i] << " ";
        }
        // cout << endl;
        */

        for(int i=0;i<router->bgp_diff.advertised_peers.size();i++){
            /*
            if(router->bgp_diff.received_peers[router->bgp_diff.advertised_peers[i]] == true){
                // cout << "already received from router " << router->bgp_diff.advertised_peers[i] << endl;
                // cout << "no bgp update we can send" << endl;
                continue;
            }
            */

            /*
            // cout << "send a new update route for e_peer_relation router " << router->bgp_diff.advertised_peers[i] << endl;
            if(is_next_as_included(cur_best_path, router->bgp_diff.advertised_peers[i])){
                // cout << "next as is included in cur_best_path" << endl;
                // cout << "cur_node: " << cur_node << endl;   
                continue;
            }

            if(is_next_as_included(as_path, router->bgp_diff.advertised_peers[i])){
                // cout << "next as is included in as_path" << endl;
                continue;
            }
            */

            dTuple dt{bm, diff_bm, router->bgp_diff.e_peer_relation[router->bgp_diff.advertised_peers[i]]};
            /*
            if(test_que_check(dt, que)){
                continue;
            }*/

            // cout << "added at router " << router->bgp_diff.advertised_peers[i] << endl;
            que->push(dt);
        }
    }

    return deleted;
}


bool check_route_preference(string cur_node, struct bgp_message cur_bm)
{
    if(traverse_history.find(cur_node) != traverse_history.end()){
        if(traverse_history[cur_node].lp != 0 || cur_bm.lp != 0){
            if(cur_bm.lp <= traverse_history[cur_node].lp){
                return false;
            }

            return true;
        }
        else if(cur_bm.as_path.size() >= traverse_history[cur_node].as_path.size()){
            return false;
        }
    }

    return true;
}

bool delete_elm_from_table(list<struct bgp_table_elms> *rib_table, unsigned int asn)
{  
    if(rib_table->size() == 0){
        return false;
    }

    auto itr = rib_table->begin();
    for(;itr!=rib_table->end();itr++){
        if(itr->as_path[itr->as_path.size()-1] == asn){
            bool t = false;
            if(itr == rib_table->begin()){
                t = true;
            }
            rib_table->erase(itr);
            return t;
        }
    }

    return false;
}

//to_do:in_out == 3の時の処理
bool add_elm_to_table(list<struct bgp_table_elms> *cur_rib_table, struct bgp_table_elms bte)
{
    if(cur_rib_table->size() == 0){
        cur_rib_table->push_front(bte);
        if(bte.in_out == 2){
            // cout << "---------------" << endl;
            // cout << "errorrrrrrrrrr at in_out 2" << endl;
            // cout << "---------------" << endl;
            exit(1);
            return false;
        }
        return true;
    }
    else{
        if(bte.in_out == 2){
            // cout << "---------------" << endl;
            // cout << "errorrrrrrrrrr at in_out 2" << endl;
            // cout << "---------------" << endl;
            exit(1);
            cur_rib_table->push_back(bte);
            return false;
        }

        auto itr = cur_rib_table->begin();
        for(;itr!=cur_rib_table->end();itr++){
            if(itr->in_out == 2 && bte.in_out != 2){
                if(itr == cur_rib_table->begin()){
                    cur_rib_table->insert(itr, bte);
                    return true;
                }
                cur_rib_table->insert(itr, bte);
                return false;
            }

            if(itr->lp != 0 || bte.lp != 0){
                if(itr->lp < bte.lp){
                    if(itr == cur_rib_table->begin()){
                        cur_rib_table->insert(itr, bte);
                        return true;
                    }
                    cur_rib_table->insert(itr, bte);
                    return false;
                }
            }
            else if(itr->as_path.size() > bte.as_path.size()){
                if(itr == cur_rib_table->begin()){
                    cur_rib_table->insert(itr, bte);
                    return true;
                }
                cur_rib_table->insert(itr, bte);
                return false;
            }
        }
        if(itr == cur_rib_table->end()){
            cur_rib_table->insert(itr, bte);
            return false;
        }
    }

    // cout << "you should not see this" << endl;
    return false;
}

bool add_elm_to_ospf_table_ecmp(list<struct bgp_table_elms> *cur_rib_table, struct bgp_table_elms bte)
{
    //// cout << "added bte:" << endl;
    //print_as_number_to_router(bte);

    if(cur_rib_table->size() == 0){
        //// cout << "cur_rib_table->size() == 0" << endl;
        cur_rib_table->push_front(bte);
        cur_rib_table->front().vec_path.push_back(bte.as_path);
        return true;
    }
    else{
        auto itr = cur_rib_table->begin();
        for(;itr!=cur_rib_table->end();itr++){
            if(itr->total_cost == bte.total_cost){
                itr->vec_path.push_back(bte.as_path);
                if(itr == cur_rib_table->begin()){
                    return true;
                }
                return false;
            }
            if(itr->total_cost > bte.total_cost){
                /*
                // cout << "itr->total_cost > bte.total_cost" << endl;
                // cout << "itr->total cost: " << itr->total_cost << endl;
                // cout << "bte.total_cost: " << bte.total_cost << endl;
                */
               cur_rib_table->insert(itr, bte);
               cur_rib_table->front().vec_path.push_back(bte.as_path);
                if(itr == cur_rib_table->begin()){
                    return true;
                }
                return false;
            }
        }
        if(itr == cur_rib_table->end()){
            //// cout << "itr == cur_rib_table->end()" << endl;
            cur_rib_table->insert(itr, bte);
            cur_rib_table->back().vec_path.push_back(bte.as_path);
            return false;
        }
    }

    // cout << "you should not see this" << endl;
    return false;
}

bool add_elm_to_ospf_table(list<struct bgp_table_elms> *cur_rib_table, struct bgp_table_elms bte)
{
    //// cout << "added bte:" << endl;
    print_as_number_to_router(bte);
    if(cur_rib_table->size() == 0){
        //// cout << "cur_rib_table->size() == 0" << endl;
        cur_rib_table->push_front(bte);
        return true;
    }
    else{
        auto itr = cur_rib_table->begin();
        for(;itr!=cur_rib_table->end();itr++){
            if(itr->total_cost > bte.total_cost){
                /*
                // cout << "itr->total_cost > bte.total_cost" << endl;
                // cout << "itr->total cost: " << itr->total_cost << endl;
                // cout << "bte.total_cost: " << bte.total_cost << endl;
                */
                if(itr == cur_rib_table->begin()){
                    cur_rib_table->insert(itr, bte);
                    return true;
                }
                cur_rib_table->insert(itr, bte);
                return false;
            }
        }
        if(itr == cur_rib_table->end()){
            //// cout << "itr == cur_rib_table->end()" << endl;
            cur_rib_table->insert(itr, bte);
            return false;
        }
    }

    // cout << "you should not see this" << endl;
    return false;
}

bool add_elm_to_diff_table(list<struct bgp_table_elms> *cur_rib_table, struct bgp_table_elms bte)
{
    if(cur_rib_table->size() == 0){
        cur_rib_table->push_front(bte);
        if(bte.in_out == 2){
            // cout << "---------------" << endl;
            // cout << "errorrrrrrrrrr at in_out 2" << endl;
            // cout << "---------------" << endl;
            exit(1);
            return false;
        }
        return true;
    }
    else{
        if(bte.in_out == 2){
            cur_rib_table->push_back(bte);
            // cout << "---------------" << endl;
            // cout << "errorrrrrrrrrr at in_out 2" << endl;
            // cout << "---------------" << endl;
            exit(1);
            return false;
        }
        
        auto itr = cur_rib_table->begin();
        for(;itr!=cur_rib_table->end();itr++){
            if(itr->in_out == 2 && bte.in_out != 2){
                // cout << "---------------" << endl;
                // cout << "errorrrrrrrrrr at in_out 2" << endl;
                // cout << "---------------" << endl;
                exit(1);
                    /*
                if(itr == cur_rib_table->begin()){
                    cur_rib_table->insert(itr, bte);
                    return true;
                }
                cur_rib_table->insert(itr, bte);
                return false;
                */
            }

            if(itr->lp != 0 || bte.lp != 0){
                if(itr->lp < bte.lp){
                    if(itr == cur_rib_table->begin()){
                        cur_rib_table->insert(itr, bte);
                        return true;
                    }
                    cur_rib_table->insert(itr, bte);
                    return false;
                }
            }
            else if(itr->as_path.size() > bte.as_path.size()){
                 if(itr == cur_rib_table->begin()){
                    cur_rib_table->insert(itr, bte);
                    return true;
                }
                cur_rib_table->insert(itr, bte);
                return false;
            }
        }
        if(itr == cur_rib_table->end()){
            cur_rib_table->push_back(bte);
            return false;
        }
    }

    // cout << "you should not see this" << endl;

    return false;
}

bool add_elm_to_diff_ospf_table_ecmp(list<struct bgp_table_elms> *cur_rib_table, struct bgp_table_elms bte)
{
    //// cout << "added diff bte:" << endl;
    //print_as_number_to_router(bte);

    if(cur_rib_table->size() == 0){
        //// cout << "cur_rib_table->size() == 0" << endl;
        cur_rib_table->push_front(bte);
        cur_rib_table->front().vec_path.push_back(bte.as_path);
        return true;
    }
    else{       
        auto itr = cur_rib_table->begin();
        for(;itr!=cur_rib_table->end();itr++){
            if(itr->total_cost == bte.total_cost){
                itr->vec_path.push_back(bte.as_path);
                if(itr == cur_rib_table->begin()){
                    return true;
                }
                return false;
            }
            if(itr->total_cost > bte.total_cost){
                /*
                // cout << "itr->total_cost > bte.total_cost" << endl;
                // cout << "itr->total cost: " << itr->total_cost << endl;
                // cout << "bte.total_cost: " << bte.total_cost << endl;
                */
               cur_rib_table->insert(itr, bte);
               cur_rib_table->front().vec_path.push_back(bte.as_path);
                if(itr == cur_rib_table->begin()){
                    return true;
                }
                //cur_rib_table->insert(itr, bte);
                return false;
            }
        }
        if(itr == cur_rib_table->end()){
            //// cout << "itr == cur_rib_table->end()" << endl;
            //// cout << "itr == cur_rib_table->end()" << endl;
            cur_rib_table->push_back(bte);
            cur_rib_table->back().vec_path.push_back(bte.as_path);
            return false;
        }
    }

    // cout << "you should not see this" << endl;

    return false;
}

bool add_elm_to_diff_ospf_table(list<struct bgp_table_elms> *cur_rib_table, struct bgp_table_elms bte)
{
    //// cout << "added diff bte:" << endl;
    print_as_number_to_router(bte);
    if(cur_rib_table->size() == 0){
        //// cout << "cur_rib_table->size() == 0" << endl;
        cur_rib_table->push_front(bte);
        return true;
    }
    else{       
        auto itr = cur_rib_table->begin();
        for(;itr!=cur_rib_table->end();itr++){
            if(itr->total_cost > bte.total_cost){
                /*
                // cout << "itr->total_cost > bte.total_cost" << endl;
                // cout << "itr->total cost: " << itr->total_cost << endl;
                // cout << "bte.total_cost: " << bte.total_cost << endl;
                */
                if(itr == cur_rib_table->begin()){
                    cur_rib_table->insert(itr, bte);
                    return true;
                }
                //cur_rib_table->insert(itr, bte);
                return false;
            }
        }
        if(itr == cur_rib_table->end()){
            //// cout << "itr == cur_rib_table->end()" << endl;
            //// cout << "itr == cur_rib_table->end()" << endl;
            //cur_rib_table->push_back(bte);
            return false;
        }
    }

    // cout << "you should not see this" << endl;

    return false;
}

unordered_map<string, bool> set_visited_table(unordered_map<string, bool> is_visited, vector<unsigned int> as_path)
{
    for(int i=0;i<as_path.size();i++){
        is_visited[to_string(as_path[i])] = true;
    }

    return is_visited;
}

pair<bool, bool> new_add_diff_bgp_table(struct router *router, struct bgp_message *bm)
{
    struct bgp_table_elms bte;

    pair<bool, bool> is_this_route_first;

    bte.adv_route = bm->adv_route;
    bte.as_path = bm->as_path;
    bte.community = bm->community;
    bte.lp = bm->lp;
    bte.id = bm->unique_id;
    bte.sonzai = bm->sonzai;
    bte.terminated = bm->terminated;
    bte.terminated_point = bm->terminated_point;


    list<struct bgp_table_elms> *cur_diff_rib_table = &router->bgp_diff.loc_rib_table[bte.adv_route.prefix];
    
    
    vector<unsigned int> before_diff_best_path;
    if(cur_diff_rib_table->size() > 0){
        before_diff_best_path = cur_diff_rib_table->front().as_path;
    }

    if(bte.as_path.size() == 0){
        // cout << "as_path is 0" << endl;
    }

    bool deleted = delete_elm_from_table(cur_diff_rib_table, bte.as_path[bte.as_path.size()-1]);

    if(bte.terminated != true){
        //テーブルの上から今受け取った要素を追加
        is_this_route_first.first = add_elm_to_diff_table(cur_diff_rib_table,bte);   
    }

    //テーブルの上から今受け取った要素を追加
    //is_this_route_first.second = add_elm_to_diff_table(cur_diff_rib_table,bte);

    if(cur_diff_rib_table->empty()){
        //消しに行く処理？
        is_this_route_first.first = false;
        return is_this_route_first;
    }

    vector<unsigned int> after_diff_best_path = cur_diff_rib_table->front().as_path;
    
    //追加前と後のbest pathのパスが同じではなかったら、bgp updateを送る
    //need to change add bgp tableのoutput

    //if(deleted == true || before_diff_best_path != after_diff_best_path){
    if(before_diff_best_path != after_diff_best_path){

        //set after best path 
        //if(is_this_route_first.first == false && cur_diff_rib_table->front().in_out != 2){  
        struct bgp_table_elms new_elm = cur_diff_rib_table->front();

        bm->adv_route = new_elm.adv_route;
        bm->as_path = new_elm.as_path;
        bm->community = new_elm.community;
        bm->lp = new_elm.lp;
        bm->unique_id = new_elm.id;
        bm->sonzai = true;
        bm->terminated = false;
        bm->terminated_point = new_elm.terminated_point;
        //}

        is_this_route_first.first = true;
    }
    else{
        is_this_route_first.first = false;
    }

    return is_this_route_first;    
}

pair<bool, bool> add_diff_ospf_table(struct router *router, struct bgp_message *bm)
{
    struct bgp_table_elms bte;

    pair<bool, bool> is_this_route_first;

    bte.adv_route = bm->adv_route;
    bte.as_path = bm->as_path;
    bte.community = bm->community;
    bte.lp = bm->lp;
    bte.id = bm->unique_id;
    bte.sonzai = bm->sonzai;
    bte.terminated = bm->terminated;
    bte.terminated_point = bm->terminated_point;
    bte.total_cost = bm->total_cost;

    //// cout << "start to add_diff_ospf_table" << endl;

    list<struct bgp_table_elms> *cur_diff_rib_table = &router->bgp_diff.loc_rib_table[bte.adv_route.prefix];
    
    
    vector<unsigned int> before_diff_best_path;
    if(cur_diff_rib_table->size() > 0){
        before_diff_best_path = cur_diff_rib_table->front().as_path;
    }

    if(bte.as_path.size() == 0){
        // cout << "as_path is 0" << endl;
    }

    //bool deleted = delete_elm_from_table(cur_diff_rib_table, bte.as_path[bte.as_path.size()-1]);

    if(bte.terminated != true){
        //テーブルの上から今受け取った要素を追加
        //// cout << "add for add_elm_to_diff_ospf_table" << endl; 
        is_this_route_first.first = add_elm_to_diff_ospf_table_ecmp(cur_diff_rib_table,bte);
        //is_this_route_first.first = add_elm_to_diff_ospf_table(cur_diff_rib_table,bte);

        //is_this_route_first.first = add_elm_to_diff_table(cur_diff_rib_table,bte);   
    }

    //テーブルの上から今受け取った要素を追加
    //is_this_route_first.second = add_elm_to_diff_table(cur_diff_rib_table,bte);

    if(cur_diff_rib_table->empty()){
        //消しに行く処理？
        is_this_route_first.first = false;
        return is_this_route_first;
    }

    vector<unsigned int> after_diff_best_path = cur_diff_rib_table->front().as_path;
    
    //追加前と後のbest pathのパスが同じではなかったら、bgp updateを送る
    //need to change add bgp tableのoutput

    //if(deleted == true || before_diff_best_path != after_diff_best_path){
    if(before_diff_best_path != after_diff_best_path){

        //set after best path 
        //if(is_this_route_first.first == false && cur_diff_rib_table->front().in_out != 2){  
        struct bgp_table_elms new_elm = cur_diff_rib_table->front();

        bm->adv_route = new_elm.adv_route;
        bm->as_path = new_elm.as_path;
        bm->community = new_elm.community;
        bm->lp = new_elm.lp;
        bm->unique_id = new_elm.id;
        bm->sonzai = true;
        bm->terminated = false;
        bm->terminated_point = new_elm.terminated_point;
        //}

        is_this_route_first.first = true;
    }
    else{
        is_this_route_first.first = false;
    }

    return is_this_route_first;    
}

pair<bool, bool> virtual_new_add_diff_bgp_table(struct router *router, struct bgp_message *bm)
{
    struct bgp_table_elms bte;

    pair<bool, bool> is_this_route_first;

    bte.adv_route = bm->adv_route;
    bte.as_path = bm->as_path;
    bte.community = bm->community;
    bte.lp = bm->lp;
    bte.id = bm->unique_id;
    bte.sonzai = bm->sonzai;
    bte.terminated = bm->terminated;
    bte.terminated_point = bm->terminated_point;


    list<struct bgp_table_elms> *cur_diff_rib_table = &router->virtual_bgp_diff.loc_rib_table[bte.adv_route.prefix];
    
    
    vector<unsigned int> before_diff_best_path;
    if(cur_diff_rib_table->size() > 0){
        before_diff_best_path = cur_diff_rib_table->front().as_path;
    }

    if(bte.as_path.size() == 0){
        // cout << "as_path is 0" << endl;
    }

    bool deleted = delete_elm_from_table(cur_diff_rib_table, bte.as_path[bte.as_path.size()-1]);

    if(bte.terminated != true){
        //テーブルの上から今受け取った要素を追加
        is_this_route_first.second = add_elm_to_diff_table(cur_diff_rib_table,bte);   
    }

    //テーブルの上から今受け取った要素を追加
    //is_this_route_first.second = add_elm_to_diff_table(cur_diff_rib_table,bte);

    if(cur_diff_rib_table->empty()){
        //消しに行く処理？
        is_this_route_first.first = false;
        return is_this_route_first;
    }
    vector<unsigned int> after_diff_best_path = cur_diff_rib_table->front().as_path;
    
    //追加前と後のbest pathのパスが同じではなかったら、bgp updateを送る
    //need to change add bgp tableのoutput

    //if(deleted == true || before_diff_best_path != after_diff_best_path){
    if(before_diff_best_path != after_diff_best_path){

        //set after best path 
        if(is_this_route_first.second == false && cur_diff_rib_table->front().in_out != 2){  
            struct bgp_table_elms new_elm = cur_diff_rib_table->front();

            bm->adv_route = new_elm.adv_route;
            bm->as_path = new_elm.as_path;
            bm->community = new_elm.community;
            bm->lp = new_elm.lp;
            bm->unique_id = new_elm.id;
            bm->sonzai = true;
            bm->terminated = false;
            bm->terminated_point = new_elm.terminated_point;
        }
        is_this_route_first.first = true;
    }
    else{
        is_this_route_first.first = false;
    }

    return is_this_route_first;    
}


pair<bool, bool> new_add_bgp_table(struct router *router, struct bgp_message *bm)
{
    struct bgp_table_elms bte;

    pair<bool, bool> is_this_route_first;

    bte.adv_route = bm->adv_route;
    bte.as_path = bm->as_path;
    bte.community = bm->community;
    bte.lp = bm->lp;
    bte.id = bm->unique_id;
    bte.sonzai = bm->sonzai;
    bte.terminated = bm->terminated;
    bte.terminated_point = bm->terminated_point;

    list<struct bgp_table_elms> *cur_rib_table = &router->bgp.loc_rib_table[bte.adv_route.prefix];  
    
    //1.最初に今のbest pathを変数に記録
    vector<unsigned int> before_best_path;
    if(cur_rib_table->size() > 0){
        before_best_path = cur_rib_table->front().as_path;
    }

    bool deleted = delete_elm_from_table(cur_rib_table,bte.as_path[bte.as_path.size()-1]);

    //テーブルの上から今受け取った要素を追加
    is_this_route_first.first = add_elm_to_table(cur_rib_table, bte);

    //追加後のbest pathを記録
    vector<unsigned int> after_best_path = cur_rib_table->front().as_path;

    //追加前と後のbest pathのパスが同じではなかったら、bgp updateを送る
    //need to change add bgp tableのoutput

    if(deleted == true || before_best_path != after_best_path){
        //set after best path 
        if(is_this_route_first.first == false && cur_rib_table->front().in_out != 2){  
            struct bgp_table_elms new_elm = cur_rib_table->front();

            bm->adv_route = new_elm.adv_route;
            bm->as_path = new_elm.as_path;
            bm->community = new_elm.community;
            bm->lp = new_elm.lp;
            bm->unique_id = new_elm.id;
            bm->sonzai = true;
            bm->terminated = false;
            bm->terminated_point = new_elm.terminated_point;
        }

        is_this_route_first.first = true;
    }
    else{
        is_this_route_first.first = false;
    }

    return is_this_route_first;    
}


pair<bool, bool> add_ospf_table(struct router *router, struct bgp_message *bm)
{
    struct bgp_table_elms bte;

    pair<bool, bool> is_this_route_first;

    bte.adv_route = bm->adv_route;
    bte.as_path = bm->as_path;
    bte.community = bm->community;
    bte.lp = bm->lp;
    bte.id = bm->unique_id;
    bte.sonzai = bm->sonzai;
    bte.terminated = bm->terminated;
    bte.terminated_point = bm->terminated_point;
    bte.total_cost = bm->total_cost;

    list<struct bgp_table_elms> *cur_rib_table = &router->bgp.loc_rib_table[bte.adv_route.prefix];  
    
    //1.最初に今のbest pathを変数に記録
    vector<unsigned int> before_best_path;
    if(cur_rib_table->size() > 0){
        before_best_path = cur_rib_table->front().as_path;
    }

    //bool deleted = delete_elm_from_table(cur_rib_table,bte.as_path[bte.as_path.size()-1]);

    
    //テーブルの上から今受け取った要素を追加
    //add_elm_to_ospf
    is_this_route_first.first = add_elm_to_ospf_table_ecmp(cur_rib_table, bte);
    //is_this_route_first.first = add_elm_to_ospf_table(cur_rib_table, bte);

    //is_this_route_first.first = add_elm_to_table(cur_rib_table, bte);

    //追加後のbest pathを記録
    vector<unsigned int> after_best_path = cur_rib_table->front().as_path;

    //追加前と後のbest pathのパスが同じではなかったら、bgp updateを送る
    //need to change add bgp tableのoutput

    if(before_best_path != after_best_path){
        //set after best path 
        //caution ! 注意！
        if(is_this_route_first.first == false && cur_rib_table->front().in_out != 2){  
            struct bgp_table_elms new_elm = cur_rib_table->front();

            bm->adv_route = new_elm.adv_route;
            bm->as_path = new_elm.as_path;
            bm->community = new_elm.community;
            bm->lp = new_elm.lp;
            bm->unique_id = new_elm.id;
            bm->sonzai = true;
            bm->terminated = false;
            bm->terminated_point = new_elm.terminated_point;
        }

        is_this_route_first.first = true;
    }
    else{
        is_this_route_first.first = false;
    }

    return is_this_route_first;    
}


pair<bool, bool> new_add_ospf_diff_table(struct router *router, struct bgp_message *bm)
{
    struct bgp_table_elms bte;

    pair<bool, bool> is_this_route_first;

    bte.adv_route = bm->adv_route;
    bte.as_path = bm->as_path;
    bte.community = bm->community;
    bte.lp = bm->lp;
    bte.id = bm->unique_id;
    bte.sonzai = bm->sonzai;
    bte.terminated = bm->terminated;
    bte.terminated_point = bm->terminated_point;
    bte.total_cost = bm->total_cost;

    list<struct bgp_table_elms> *cur_rib_table = &router->bgp_diff.loc_rib_table[bte.adv_route.prefix];  
    
    //1.最初に今のbest pathを変数に記録
    vector<unsigned int> before_best_path;
    if(cur_rib_table->size() > 0){
        before_best_path = cur_rib_table->front().as_path;
    }

    bool deleted = delete_elm_from_table(cur_rib_table,bte.as_path[bte.as_path.size()-1]);

    //テーブルの上から今受け取った要素を追加
    //add_elm_to_ospf
    is_this_route_first.first = add_elm_to_ospf_table_ecmp(cur_rib_table, bte);
    //is_this_route_first.first = add_elm_to_ospf_table(cur_rib_table, bte);
    
    //is_this_route_first.first = add_elm_to_table(cur_rib_table, bte);

    //追加後のbest pathを記録
    vector<unsigned int> after_best_path = cur_rib_table->front().as_path;

    //追加前と後のbest pathのパスが同じではなかったら、bgp updateを送る
    //need to change add bgp tableのoutput

    if(before_best_path != after_best_path){
        //set after best path 
        //caution ! 注意！
        if(is_this_route_first.first == false && cur_rib_table->front().in_out != 2){  
            struct bgp_table_elms new_elm = cur_rib_table->front();

            bm->adv_route = new_elm.adv_route;
            bm->as_path = new_elm.as_path;
            bm->community = new_elm.community;
            bm->lp = new_elm.lp;
            bm->unique_id = new_elm.id;
            bm->sonzai = true;
            bm->terminated = false;
            bm->terminated_point = new_elm.terminated_point;
        }

        is_this_route_first.first = true;
    }
    else{
        is_this_route_first.first = false;
    }

    return is_this_route_first;    
}



pair<bool, bool> add_bgp_table(struct router *router, struct bgp_message *bm, struct bgp_message *diff_bm, priority_queue<dTuple, vector<dTuple>, compare_bgp_message_tuple> *que)
{
    //最初に今のbest pathを変数に記録
    //受け取ったbgp updateのas pathの最後のAS numberが合致するエントリをbgp tableから削除
    //テーブルの上から今受け取った要素を追加
    //追加後のbest pathを記録
    //追加前と後のbest pathのパスが同じではなかったら、bgp updateを送る

    struct bgp_table_elms bte;
    struct bgp_table_elms diff_bte;

    unsigned int readv_id = 1;

    pair<bool, bool> is_this_route_first;

    bte.adv_route = bm->adv_route;
    bte.as_path = bm->as_path;
    bte.community = bm->community;
    bte.lp = bm->lp;
    bte.id = bm->unique_id;
    bte.sonzai = bm->sonzai;
    bte.terminated = bm->terminated;
    bte.terminated_point = bm->terminated_point;
    if(diff_bm->sonzai == true){
        diff_bte.adv_route = diff_bm->adv_route;
        diff_bte.as_path = diff_bm->as_path;
        diff_bte.community = diff_bm->community;
        diff_bte.lp = diff_bm->lp;
        diff_bte.id = diff_bm->unique_id;
        diff_bte.sonzai = diff_bm->sonzai;
        diff_bte.terminated = diff_bm->terminated;
        diff_bte.terminated_point = diff_bm->terminated_point;
    }
    else if(diff_bm->sonzai == false &&  diff_bm->terminated == false){
        diff_bte.adv_route = bm->adv_route;
        diff_bte.as_path = bm->as_path;
        diff_bte.community = bm->community;
        diff_bte.lp = bm->lp;
        diff_bte.id = bm->unique_id;
        diff_bte.sonzai = bm->sonzai;
        diff_bte.terminated = bm->terminated;
        diff_bte.terminated_point = bm->terminated_point;
    }

    //↑ ここで追加するとき、bteとdiff_bteのステートは以後変わる

    //test_04_01
    bte.prev_bgp_message = *bm;
    bte.prev_diff_bgp_message = *diff_bm;

    diff_bte.prev_bgp_message = *bm;
    diff_bte.prev_diff_bgp_message = *diff_bm;

    //ue koko

    list<struct bgp_table_elms> *cur_rib_table = &router->bgp.loc_rib_table[bte.adv_route.prefix];
    list<struct bgp_table_elms> *cur_diff_rib_table = &router->bgp_diff.loc_rib_table[bte.adv_route.prefix];

    //1.最初に今のbest pathを変数に記録
    vector<unsigned int> before_best_path;
    if(cur_rib_table->size() > 0){
        before_best_path = cur_rib_table->front().as_path;
    }


    vector<unsigned int> before_diff_best_path;
    if(cur_diff_rib_table->size() > 0){
        before_diff_best_path = cur_diff_rib_table->front().as_path;
    }

    //受け取ったbgp updateのas pathの最後のAS numberが合致するエントリをbgp tableから削除
    //テーブルの上から今受け取った要素を追加
    //追加後のbest pathを記録
    //追加前と後のbest pathのパスが同じではなかったら、bgp updateを送る

    if(bm->sonzai == true && bm->terminated == false){
        //do: 4/30
        //受け取ったbgp updateのas pathの最後のAS numberが合致するエントリをbgp tableから削除
        bool deleted = delete_elm_from_table(cur_rib_table,bte.as_path[bte.as_path.size()-1]);

        //テーブルの上から今受け取った要素を追加
        is_this_route_first.first = add_elm_to_table(cur_rib_table, bte);

        //追加後のbest pathを記録
        vector<unsigned int> after_best_path = cur_rib_table->front().as_path;

        //追加前と後のbest pathのパスが同じではなかったら、bgp updateを送る
        //need to change add bgp tableのoutput

        if(deleted == true || before_best_path != after_best_path){
            //set after best path 
            if(is_this_route_first.first == false && cur_rib_table->front().in_out != 2){  
                struct bgp_table_elms new_elm = cur_rib_table->front();

                bm->adv_route = new_elm.adv_route;
                bm->as_path = new_elm.as_path;
                bm->community = new_elm.community;
                bm->lp = new_elm.lp;
                bm->unique_id = new_elm.id;
                bm->sonzai = true;
                bm->terminated = false;
                bm->terminated_point = new_elm.terminated_point;
            }

            is_this_route_first.first = true;
        }
        else{
            is_this_route_first.first = false;
        }

        /*
        if(is_this_route_first.first == true && cur_rib_table->size() > 1){
            auto itr = cur_rib_table->begin();
            itr++;
            //queからitr->as_path+router->nameを含むものを取り除く
            unordered_map<string, bool> is_visited;

            is_visited[to_string(cur_rib_table->begin()->as_path[cur_rib_table->begin()->as_path.size()-1])] = true;
            is_visited[router->name] = true;
        }
        */

        if(diff_bm->sonzai == true && diff_bm->terminated == false){
            diff_bte.in_out = 1;

            //do: 4/30
            //受け取ったbgp updateのas pathの最後のAS numberが合致するエントリをbgp tableから削除
            //// cout << "here1" << endl;
            bool deleted = delete_elm_from_table(cur_diff_rib_table, diff_bte.as_path[diff_bte.as_path.size()-1]);

            is_this_route_first.second = add_elm_to_diff_table(cur_diff_rib_table,diff_bte);

            //追加後のbest pathを記録
            vector<unsigned int> after_diff_best_path = cur_diff_rib_table->front().as_path;

            if(deleted == true || before_diff_best_path != after_diff_best_path){
            //set after best path 
                if(is_this_route_first.second == false && cur_diff_rib_table->front().in_out != 2){  
                    struct bgp_table_elms new_elm = cur_diff_rib_table->front();

                    diff_bm->adv_route = new_elm.adv_route;
                    diff_bm->as_path = new_elm.as_path;
                    diff_bm->community = new_elm.community;
                    diff_bm->lp = new_elm.lp;
                    diff_bm->unique_id = new_elm.id;
                    diff_bm->sonzai = true;
                    diff_bm->terminated = false;
                    diff_bm->terminated_point = new_elm.terminated_point;
                }
                    is_this_route_first.second = true;
                }
            else{
                is_this_route_first.second = false;
            }

            /*
            if(is_this_route_first.second == true && cur_diff_rib_table->size() > 1){
                auto itr = cur_diff_rib_table->begin();
                itr++;
                if(itr->in_out == 1){
                    unordered_map<string, bool> is_visited;

                    unsigned int recieved_router = cur_diff_rib_table->begin()->as_path[cur_diff_rib_table->begin()->as_path.size()-1]; 
                    is_visited[to_string(recieved_router)] = true;
                    is_visited[router->name] = true;
                }
            }
            */
            //add_withdrawal_map(bte, diff_bte, router->bgp.as_number, bte_is_withdrawal, diff_bte_is_withdrawal);
        }
        else if(diff_bm->sonzai == true && diff_bm->terminated == true){

            diff_bte.in_out = 2;
            is_this_route_first.second = add_elm_to_diff_table(cur_diff_rib_table, diff_bte);

             //bmだけ止める
            //add_withdrawal_map(bte, diff_bte, router->bgp.as_number, bte_is_withdrawal, diff_bte_is_withdrawal);
        }
        else if(diff_bm->sonzai == false && diff_bm->terminated == false){ //diff_bm->sonzai == false && diff_bm->terminated == false
            diff_bte.in_out = 1;

            //do: 4/30
            //受け取ったbgp updateのas pathの最後のAS numberが合致するエントリをbgp tableから削除
            //bool deleted = delete_elm_from_table(cur_diff_rib_table, bte.as_path[bte.as_path.size()-1]);

            is_this_route_first.second = add_elm_to_diff_table(cur_diff_rib_table, bte);

            //追加後のbest pathを記録
            vector<unsigned int> after_diff_best_path = cur_diff_rib_table->front().as_path;

            if(deleted == true || before_diff_best_path != after_diff_best_path){
            //set after best path 
                if(is_this_route_first.second == false && cur_diff_rib_table->front().in_out != 2){  
                    struct bgp_table_elms new_elm = cur_diff_rib_table->front();

                    diff_bm->adv_route = new_elm.adv_route;
                    diff_bm->as_path = new_elm.as_path;
                    diff_bm->community = new_elm.community;
                    diff_bm->lp = new_elm.lp;
                    diff_bm->unique_id = new_elm.id;
                    //diff_bm->sonzai = true;
                    //diff_bm->terminated = false;
                    diff_bm->terminated_point = new_elm.terminated_point;
                }
                is_this_route_first.second = true;
            }
            else{
                is_this_route_first.second = false;
            }


            /*
            if(is_this_route_first.second == true && cur_diff_rib_table->size() > 1){
                auto itr = cur_diff_rib_table->begin();
                itr++;
                if(itr->in_out == 1){

                    unordered_map<string, bool> is_visited;

                    unsigned int recieved_router = cur_diff_rib_table->begin()->as_path[cur_diff_rib_table->begin()->as_path.size()-1]; 
                    is_visited[to_string(recieved_router)] = true;
                    is_visited[router->name] = true;
                }
            }
            */
        }
        else{ //diff_bm->sonzai == false && diff_bm->terminated == true
            is_this_route_first.second = false;
        }
    }
    else if(diff_bm->sonzai == true && diff_bm->terminated == false){
        //bmが死んでてdiffは生きてる
        
        diff_bte.in_out = 1;

        //do: 4/30
        //受け取ったbgp updateのas pathの最後のAS numberが合致するエントリをbgp tableから削除
        //// cout << "here4" << endl;
        bool deleted = delete_elm_from_table(cur_diff_rib_table, diff_bte.as_path[diff_bte.as_path.size()-1]);

        is_this_route_first.second = add_elm_to_diff_table(cur_diff_rib_table,diff_bte);

        //追加後のbest pathを記録
        vector<unsigned int> after_diff_best_path = cur_diff_rib_table->front().as_path;

         if(deleted == true || before_diff_best_path != after_diff_best_path){
            //set after best path 
                if(is_this_route_first.second == false && cur_diff_rib_table->front().in_out != 2){  
                    struct bgp_table_elms new_elm = cur_diff_rib_table->front();

                    diff_bm->adv_route = new_elm.adv_route;
                    diff_bm->as_path = new_elm.as_path;
                    diff_bm->community = new_elm.community;
                    diff_bm->lp = new_elm.lp;
                    diff_bm->unique_id = new_elm.id;
                    diff_bm->sonzai = true;
                    diff_bm->terminated = false;
                    diff_bm->terminated_point = new_elm.terminated_point;
                }
                    is_this_route_first.second = true;
                }
            else{
                is_this_route_first.second = false;
            }

        /*
        if(is_this_route_first.second == true && cur_diff_rib_table->size() > 1){
            auto itr = cur_diff_rib_table->begin();
            itr++;

            if(itr->in_out == 1){
                unordered_map<string, bool> is_visited;
                unsigned int recieved_router = cur_diff_rib_table->begin()->as_path[cur_diff_rib_table->begin()->as_path.size()-1];     
                is_visited[to_string(recieved_router)] = true;
                is_visited[router->name] = true;
            }
            
            //add_withdrawal_map(bte, diff_bte, router->bgp.as_number, bte_is_withdrawal, diff_bte_is_withdrawal);
        }
        */
        //// cout << "diff_bgp_table add w/o prev_conf" << endl;

        //withdrawalされてなくて、死んだbmを追加
        //to_do: 本当に追加すべきか (現状は余分)
        if(bm->sonzai == true && bm->terminated == true){
            bte.in_out = 2;
            //is_this_route_first.first = add_elm_to_table(cur_rib_table, bte);
        }
    }
    else{
        is_this_route_first.first = false;
        is_this_route_first.second = false;
    }

    /*
    if(bte_is_withdrawal == true){
        // cout << "withdrawal_bte: ";
        for(int i=0;i<withdrawal_bte.as_path.size();i++){
            // cout << withdrawal_bte.as_path[i] << " ";
        }
        // cout << endl;
    }


    if(diff_bte_is_withdrawal == true){
        // cout << "withdrawal_diff_bte: ";
        for(int i=0;i<withdrawal_diff_bte.as_path.size();i++){
            // cout << withdrawal_diff_bte.as_path[i] << " ";
        }
        // cout << endl;
    }
    */

    //永遠にwithdrawalし続けるのはおかしくないか？
    //add_withdrawal_map(withdrawal_bte, withdrawal_diff_bte, bte_is_withdrawal, diff_bte_is_withdrawal);

   /*
    withdrawal_bte.as_path.push_back(router->bgp.as_number);
    withdrawal_diff_bte.as_path.push_back(router->bgp.as_number);
    delete_withdrawal(withdrawal_bte, withdrawal_diff_bte, bte_is_withdrawal, diff_bte_is_withdrawal, que);
    */


    return is_this_route_first;
}



void show_as_path_only(struct bgp_table_elms bte)
{
    for(int i=0;i<bte.as_path.size();i++){
        // cout << bte.as_path[i] << " ";
    }
    // cout << endl;
}

void show_no_route_reachable_difference(struct bgp_table_elms bte, struct bgp_table_elms diff_bte)
{
    // cout << "Previous bgp message is dropped at router " << bte.as_path[bte.as_path.size()-1] << endl;
    // cout << "This drop is caused by " << endl;
}

void show_preference_difference(struct bgp_table_elms bte, struct bgp_table_elms diff_bte)
{
    if(diff_bte.lp != 0 || bte.lp != 0){
        if(bte.lp <= diff_bte.lp){
            // cout << "local pref of the past preferred path: " << bte.lp << endl;
            // cout << "local pref of the current preferred path: " << diff_bte.lp << endl;
        }
    }
    else if(bte.as_path.size() >= diff_bte.as_path.size()){
        // cout << "AS_path length of the past preferred path: " << bte.as_path.size() << endl;
        // cout << "AS_path length of the current preferred path: " << diff_bte.as_path.size() << endl;
    }
    else{
        // cout << "Arrival issues ?" << endl;
    }
}

void show_preference_difference_w_same_id(struct bgp_table_elms bte, struct bgp_table_elms diff_bte)
{
    if(diff_bte.lp != 0 || bte.lp != 0){
        if(bte.lp <= diff_bte.lp){
            // cout << "local pref of the old id: " << bte.lp << endl;
            // cout << "local pref of the current id: " << diff_bte.lp << endl;
        }
    }
    else if(bte.as_path.size() >= diff_bte.as_path.size()){
        // cout << "AS_path length of the old id: " << bte.as_path.size() << endl;
        // cout << "AS_path length of the current id: " << diff_bte.as_path.size() << endl;
    }
    else{
        // cout << "Arrival issues ?" << endl;
    }
}

bool compare_path(vector<unsigned int> spec_path, vector<unsigned int> bte_path)
{
    if(spec_path.size() != bte_path.size()){
        return false;
    }

    for(int i=0;i<spec_path.size();i++){
        if(spec_path[i] != bte_path[i]){
            return false;
        }
    }

    return true;
}


bool compare_str_path(vector<string> spec_path, vector<string> bte_path)
{
    if(spec_path.size() != bte_path.size()){
        return false;
    }

    for(int i=0;i<spec_path.size();i++){
        if(spec_path[i] != bte_path[i]){
            return false;
        }
    }

    return true;
}

bool is_in_path_specs(struct bgp_table_elms bte)
{
    for(int i=0;i<vec_specs.size();i++){
        //to_do_path check
        if(compare_str_path(vec_specs[i].forwarding_path, convert_upath_to_spath(bte.as_path))){
            return true;
        }
    }

    return false;
}

bool is_bte_path_same(struct bgp_table_elms bte, struct bgp_table_elms diff_bte)
{
    if(bte.as_path.size() != diff_bte.as_path.size()){
        return false;
    }

    for(int i=0;i<bte.as_path.size();i++){
        if(bte.as_path[i] != diff_bte.as_path[i]){
            return false;
        }
    }

    return true;
}

void reset_bgp_parameters(struct bgp_message *bm, struct bgp_message *diff_bm)
{
    if(bm->terminated != false){
        bm->lp = 0;
    }

    if(diff_bm->sonzai == true && diff_bm->terminated == false){
        diff_bm->lp = 0;
    }
}

void reset_single_bgp_parameters(struct bgp_message *bm)
{
    //if(bm->terminated != false){
    //    bm->lp = 0;
    //}

    bm->lp = 0;
}

void check_route_num(vector<router> *vec_router, string router_name, bitset<32> addr, int idx)
{
    // cout << "idx: " << idx << endl;
    // cout << "check_route_num" << endl;
    int counter = 0;
    struct router *router;
    for(int i=0;i<vec_router->size();i++){
        if((*vec_router)[i].bgp_diff.loc_rib_table[addr].size() > 0 ){
            counter++;
        }
        else{
            router = &(*vec_router)[i];
        }
    }
    // cout << "counter: " << counter << endl;

    return;
    if(counter < 753){
        // cout << "error at check_route_num" << endl;
        // cout << "incomplete router: " << router->name << endl;
        // cout << "advertised router: " << router_name << endl;
        // cout << "router->bgp_diff.loc_rib_table[addr].size(): " << router->bgp_diff.loc_rib_table[addr].size() << endl;
        sleep(100);
    }
}

int find_the_best_route(struct bgp_table_elms bestr, list<struct bgp_table_elms> *diff_table)
{
    int idx = 0;
    for(auto itr=diff_table->begin();itr!=diff_table->end();itr++){
        if(bestr.as_path == itr->as_path){
            return idx;
        }
        idx++;
    }

    return -1;
}


static unsigned int side_effect_count = 0;

void virtual_simulate(struct bgp_message bm, struct router *router, bitset<32> addr, vector<unsigned int> best_path_before_repair, int idx)
{
    priority_queue<dPair, vector<dPair>, compare_bgp_message> vir_pq;
    dPair dp{bm, router};
    vir_pq.push(dp);

    // cout << "virtual simulation for side effect analysis" << endl;

    while(!vir_pq.empty()){
        tuple<bgp_message, struct router *> cur_diff_pair;

        cur_diff_pair = vir_pq.top();
        vir_pq.pop();

        struct bgp_message diff_bm;
        struct router *diff_router;
        diff_bm = get<0>(cur_diff_pair);
        diff_router = get<1>(cur_diff_pair);

        diff_bm.sonzai = true;
        diff_bm.terminated = false;

        //print_as_numbers(diff_bm);

        bool flag=false;    

        for(int t=0;t<diff_bm.as_path.size();t++){
            if(diff_bm.as_path[t] == diff_router->bgp_diff.as_number){
                flag = true;
                break;
            }
        }

        if(flag==true){
            continue;
        }
                
        unsigned int prev_as_number = diff_bm.as_path[diff_bm.as_path.size()-1];

        diff_router->virtual_bgp_diff.adj_rib_in[prev_as_number].push_back(diff_bm.adv_route);

        if(apply_diff_import_filter(&diff_bm, diff_router, prev_as_number)){

            //入れる先を変える: virtual_bgp_diff        
            pair<bool, bool> is_this_route_first = virtual_new_add_diff_bgp_table(diff_router,&diff_bm);

            //ここでbest_routeじゃなかったらstop
            if(is_this_route_first.first == false){
                continue;
            }

            if(diff_bm.sonzai == true && diff_bm.terminated == false){
                diff_bm.as_path.push_back(diff_router->virtual_bgp_diff.as_number);
            }

            if(diff_bm.sonzai == true && diff_bm.terminated == false){
                for(auto itr = diff_router->virtual_bgp_diff.e_peer_relation.begin();itr!=diff_router->virtual_bgp_diff.e_peer_relation.end();itr++){
                    if(itr->first == prev_as_number){
                        continue;
                    }   

                    diff_router->virtual_bgp_diff.adj_rib_out[itr->first].push_back(diff_bm.adv_route);

                    reset_single_bgp_parameters(&diff_bm);

                    if(new_apply_export_filter(&diff_bm, diff_router, itr->first)){
       
                        pair<bgp_message, struct router *> next_tuple{diff_bm, itr->second};
                                        
                        if(diff_bm.sonzai == true && diff_bm.terminated == false){
                            diff_router->virtual_bgp_diff.advertised_peers.push_back(itr->second->virtual_bgp_diff.as_number);
                            itr->second->virtual_bgp_diff.received_peers[diff_router->virtual_bgp_diff.as_number] = true;

                            vir_pq.push(next_tuple);
                        }
                    }
                }   
            }
        }
    }

    //3. check if best path before and after repair are not same
    list<struct bgp_table_elms> tmp_table_after_repair = router->bgp_diff.loc_rib_table[addr];
    list<struct bgp_table_elms> virtual_tmp_table_after_repair = router->virtual_bgp_diff.loc_rib_table[addr];

    if(!tmp_table_after_repair.empty() && !virtual_tmp_table_after_repair.empty()){
        struct bgp_table_elms tmp_best_after_repair = tmp_table_after_repair.front();
        struct bgp_table_elms virtual_tmp_best_after_repair = virtual_tmp_table_after_repair.front();

        if(!is_path_equal(tmp_best_after_repair.as_path,virtual_tmp_best_after_repair.as_path)){
            for(int i=0;i<virtual_tmp_best_after_repair.as_path.size();i++){
                /*
                string rtr = to_string(virtual_tmp_best_after_repair.as_path[i]);
                struct router *diff_rtr_st = name_to_router[rtr];
                */
               struct router *diff_rtr_st = as_number_to_router[virtual_tmp_best_after_repair.as_path[i]][0];
               string rtr = diff_rtr_st->name;

                if(!diff_rtr_st->virtual_bgp_diff.import_filter.empty()){
                    for(auto bif=diff_rtr_st->virtual_bgp_diff.import_filter.begin();bif!=diff_rtr_st->virtual_bgp_diff.import_filter.end();bif++){
                        if(bif->second.size() == 0){
                            continue;
                        }

                        // cout << "-----Future priority cause------" << endl;
                        // cout << "router: " << rtr << endl;
                        // cout << "peer: " << bif->first << endl;
                        // cout << "filter name: " << bif->second[0].action << endl;
                        // cout << "filter numeric: " << bif->second[0].action_value_numeric << endl;
                        // cout << "------------------------" << endl;
                    }
                }
            }
        }
    }
    else if(tmp_table_after_repair.empty() && !virtual_tmp_table_after_repair.empty()){
        struct bgp_table_elms virtual_tmp_best_after_repair = virtual_tmp_table_after_repair.front();
        for(int i=0;i<virtual_tmp_best_after_repair.as_path.size();i++){
            /*
            string rtr = to_string(virtual_tmp_best_after_repair.as_path[i]);
            struct router *diff_rtr_st = name_to_router[rtr];
            */
           struct router *diff_rtr_st = as_number_to_router[virtual_tmp_best_after_repair.as_path[i]][0];
           string rtr = diff_rtr_st->name;

            if(!diff_rtr_st->virtual_bgp_diff.import_filter.empty()){
                for(auto bif=diff_rtr_st->virtual_bgp_diff.import_filter.begin();bif!=diff_rtr_st->virtual_bgp_diff.import_filter.end();bif++){
                    if(bif->second.size() == 0){
                        continue;
                    }

                    // cout << "-----Future priority cause------" << endl;
                    // cout << "router: " << rtr << endl;
                    // cout << "peer: " << bif->first << endl;
                    // cout << "filter name: " << bif->second[0].action << endl;
                    // cout << "filter numeric: " << bif->second[0].action_value_numeric << endl;
                    // cout << "------------------------" << endl;
                }
            }
        }
    }
    /*
    for(int i=idx+1;i<best_path_before_repair.size();i++){
        string rtr = to_string(best_path_before_repair[i]);
        struct router *diff_rtr_st = name_to_router[rtr];

        if(!diff_rtr_st->virtual_bgp_diff.import_filter.empty()){
            for(auto bif=diff_rtr_st->virtual_bgp_diff.import_filter.begin();bif!=diff_rtr_st->virtual_bgp_diff.import_filter.end();bif++){
                if(bif->second.size() == 0){
                    continue;
                }

                list<struct bgp_table_elms> tmp_table_after_repair = diff_rtr_st->bgp_diff.loc_rib_table[addr];
                list<struct bgp_table_elms> virtual_tmp_table_after_repair = diff_rtr_st->virtual_bgp_diff.loc_rib_table[addr];

                if(tmp)                
            }
        }
    }*/
    //4. search best path after repair and print the priority
}



void se_analysis_null_rib_after_repair(struct bgp_table_elms cur_first, struct router *cr, bitset<32> addr)
{
    //いなかったら探しに行く
    // cout << "The cause of this side effect is a missng route issue !" << endl;

    //dropされたルールを探しに行く、同じseかをチェック
    // cout << "Search best path before repair" << endl;
    cur_first.as_path.push_back(cr->bgp.as_number);
    print_as_number(cur_first);

    // cout << "Best path after repair is null !" << endl;

    for(int th=0;th<cur_first.as_path.size();th++){
        /*
        string rtr = to_string(cur_first.as_path[th]);
        struct router *rtr_st = name_to_router[rtr];
        */
       struct router *rtr_st = as_number_to_router[cur_first.as_path[th]][0];
       string rtr = rtr_st->name;

        //diffのどこで落ちてるかチェック
        if(!rtr_st->bgp_diff.import_filter.empty()){
            for(auto bif=rtr_st->bgp_diff.import_filter.begin();bif!=rtr_st->bgp_diff.import_filter.end();bif++){
                if(bif->second.size() == 0){
                    continue;
                }

                if(bif->second[0].action == "set route_map drop"){
                    side_effect_count++;
                    // cout << "-----Missing route cause------" << endl;
                    // cout << "router " << rtr << endl;
                    // cout << "peer: " << bif->first << endl;
                    // cout << "filter name: " << bif->second[0].action << endl;
                    // cout << "------------------------" << endl;

                    //virtually simulate ?
                    //1. copy bgp_diff to tmp_bgp_diff
                    for(auto itr=name_to_router.begin();itr!=name_to_router.end();itr++){
                        struct router *cr = itr->second;
                        list<struct bgp_table_elms> table = itr->second->bgp.loc_rib_table[addr];
                        cr->virtual_bgp_diff.loc_rib_table[addr] = table;
                    }

                    //2. run simulate with tmp_bgp_diff
                    unordered_map<unsigned int, vector<filter>> tmp_filter = rtr_st->virtual_bgp_diff.import_filter;
                    //test_5_22
                    rtr_st->virtual_bgp_diff.import_filter.clear();
                    struct bgp_message vir_bm;
                    vector<unsigned int > asp = cur_first.as_path;
                    asp.pop_back();
                    create_bm(&vir_bm,cur_first.adv_route, asp, 0);
                    virtual_simulate(vir_bm,rtr_st,addr, cur_first.as_path, th);
                    //3. check if best path before and after repair are not same
                    //4. search best path after repair and print the priority
                    rtr_st->virtual_bgp_diff.import_filter = tmp_filter;
                }
            }
        }
    }
    cur_first.as_path.pop_back();
}

void show_se_info(string config_changes)
{
    vector<side_effect_info> sei = cchange_to_se_info[config_changes];
    
    if(sei.size() == 0){
        return;
    }

     // cout << "--------------" << endl;

    // cout << "config changes: " << config_changes << endl;
    // cout << "sei_size: " << sei.size() << endl;

    for(int i=0;i<sei.size();i++){
        // cout << "router causing se: " << sei[i].router->name << endl;
        // cout << "rotuer afected by se: " << sei[i].se_affected_router->name << endl; 
        //cout << "type: " << sei[i].type << endl;
    }

    // cout << "--------------" << endl;
}

string insert_config_changes(string router, string filter_name, string action_value)
{
    string ans = router+"."+filter_name+"."+action_value+":";
    return ans;
}


/*
bool check_best_path_before_repair(vector<unsigned int> missing_path, struct router *router, bitset<32> addr)
{
    list<struct bgp_table_elms> table_before_repair = router->bgp.loc_rib_table[addr];
    struct bgp_table_elms best_path = table_before_repair.front();

    if(is_as_path_same(missing_path, best_path.as_path)){
        return true;
    }

    return false;
}
*/

void se_missing_route_path_search(struct bgp_table_elms cur_first, struct bgp_table_elms diff_first, struct router *cr, bitset<32> addr, int pl)
{
    // cout << "The cause of this side effect is a missng route issue !" << endl;

    // cout << "Search best path before repair" << endl;

    
    cur_first.as_path.push_back(cr->bgp.as_number);

    print_as_number(cur_first);

    bool first_flag = false;

    string config_changes = "";
    vector<side_effect_info> se_info;

    //crで自分で落としている可能性もある
    //その対応を書く

    //逆順にするべし
    for(int th=cur_first.as_path.size()-1;th>=0;th--){
        /*
        string rtr = to_string(cur_first.as_path[th]);
        struct router *rtr_st = name_to_router[rtr];
        */
        struct router *rtr_st = as_number_to_router[cur_first.as_path[th]][0];
        string rtr = rtr_st->name;

        // cout << "visit router " << rtr_st->bgp.as_number << endl;

        //発信源の真隣がblockしたケース to do

        //cur_firstがbest pathなルーターを見つける, endは削る
        //why this happen

        //このルーターのafter repairにいるかみる
        struct bgp_table_elms elm = is_there_missing_route(cur_first.as_path, rtr_st, addr, th);
        // cout << "done is_there_missing_route" << endl;
        if(!elm.as_path.empty()){
            //いる場合は、preferenceで負けてる　→ そのpreferenceが何で、どこでついたかを探す
            // cout << "priority issue at router " << rtr_st->bgp.as_number << endl;
            struct bgp_table_elms cur_top = rtr_st->bgp_diff.loc_rib_table[addr].front();

            //check what types of priorty (lp, as_path_len)
            //その原因を探す
            //config_chang to seに入れる

            if(cur_top.lp > elm.lp){
                // cout << "the cause is local preference difference" << endl;

                for(auto bif=rtr_st->bgp_diff.import_filter.begin();bif!=rtr_st->bgp_diff.import_filter.end();bif++){
                    if(bif->second.size() == 0){
                        continue;
                    }

                    if(cur_top.lp == bif->second[0].action_value_numeric){
                        // cout << "-----Priority cause------" << endl;
                        // cout << "router: " << rtr << endl;
                        // cout << "peer: " << bif->first << endl;
                        // cout << "filter name: " << bif->second[0].action << endl;
                        // cout << "filter numeric: " << bif->second[0].action_value_numeric << endl;
                        // cout << "------------------------" << endl;

                        //メモする
                        //config_changesに積もっていかないか (多分起きない)
                        config_changes += insert_config_changes(rtr,bif->second[0].action,to_string(bif->second[0].action_value_numeric));

                        //このルーターのこのconfigのこの値
                        struct side_effect_info sei;
                        sei.router = rtr_st;
                        //sei.se_affected_router = name_to_router[to_string(cr->bgp.as_number)];
                        sei.se_affected_router = as_number_to_router[cr->bgp.as_number][0];
                        sei.type = "local preference";
                        sei.action_value_numeric = bif->second[0].action_value_numeric;

                        struct v4_route r;
                        r.prefix = addr;
                        r.prefix_len = pl;
                        sei.route = r;

                        //tmp_best_before_repair.as_path.push_back(cur_rtr_asn);
                        //sei.before_repair_path = tmp_best_before_repair.as_path;
                        //tmp_best_after_repair.as_path.push_back(cur_rtr_asn);
                        //sei.after_repair_path = tmp_best_after_repair.as_path;

                        sei.before_repair_path = cur_first.as_path;
                        vector<unsigned int> empty_path;
                        //sei.after_repair_path = cur_top.as_path;
                        sei.after_repair_path = diff_first.as_path;
                        if(sei.after_repair_path[sei.after_repair_path.size()-1] != cr->bgp.as_number){
                            sei.after_repair_path.push_back(cr->bgp.as_number);
                        }


                        se_info.push_back(sei);
                    }
                }
            }
        }
        else{
            //いない場合は、routem mapで落とされてないかチェック
            if(!rtr_st->bgp_diff.import_filter.empty()){
                for(auto bif=rtr_st->bgp_diff.import_filter.begin();bif!=rtr_st->bgp_diff.import_filter.end();bif++){
                    if(bif->second.size() == 0){
                        continue;
                    }

                    //// cout << "router: " << rtr_st->name << endl;

                    if(bif->second[0].action == "set route_map drop"){
                        side_effect_count++;
                        // cout << "-----Missing route cause------" << endl;
                        // cout << "router " << rtr << endl;
                        // cout << "peer: " << bif->first << endl;
                        // cout << "filter name: " << bif->second[0].action << endl;
                        // cout << "------------------------" << endl;

                        config_changes += insert_config_changes(rtr,bif->second[0].action,"non");

                        struct side_effect_info sei;
                        sei.router = rtr_st;
                        sei.se_affected_router = cr;
                        sei.type = bif->second[0].action;

                        struct v4_route r;
                        r.prefix = addr;
                        r.prefix_len = pl;
                        sei.route = r;

                        sei.before_repair_path = cur_first.as_path;
                        if(sei.before_repair_path[sei.before_repair_path.size()-1] != cr->bgp.as_number){
                            sei.before_repair_path.push_back(cr->bgp.as_number);
                        }

                        sei.after_repair_path = diff_first.as_path;
                        
                        if(sei.after_repair_path.size() == 0){
                            // cout << "No route" << endl;
                        }
                        else if(sei.after_repair_path[sei.after_repair_path.size()-1] != cr->bgp.as_number){
                            sei.after_repair_path.push_back(cr->bgp.as_number);
                        }

                        se_info.push_back(sei);


                        if(first_flag == false){
                            first_flag = true;

                            //virtually simulate ?
                            //1. copy bgp_diff to tmp_bgp_diff
                            for(auto itr=name_to_router.begin();itr!=name_to_router.end();itr++){
                                struct router *cr = itr->second;
                                list<struct bgp_table_elms> table = itr->second->bgp.loc_rib_table[addr];
                                cr->virtual_bgp_diff.loc_rib_table[addr] = table;
                            }

                            //2. run simulate with tmp_bgp_diff
                            unordered_map<unsigned int, vector<filter>> tmp_filter = rtr_st->virtual_bgp_diff.import_filter;
                            //test_5_22
                            rtr_st->virtual_bgp_diff.import_filter.clear();
                            struct bgp_message vir_bm;
                            vector<unsigned int > asp = cur_first.as_path;
                            asp.pop_back();
                            create_bm(&vir_bm,cur_first.adv_route, asp, 0);
                            virtual_simulate(vir_bm,rtr_st,addr, cur_first.as_path, th);
                            //3. check if best path before and after repair are not same
                            //4. search best path after repair and print the priority
                            rtr_st->virtual_bgp_diff.import_filter = tmp_filter;
                        }
                    }
                 }

            }
            //落とされてなかったら、ループを回す
            continue;
        }
    }

    if(!config_changes.empty()){
        if(cchange_to_se_info[config_changes].size() == 0){
            cchange_to_se_info[config_changes] = se_info;
        }
        else{
            for(int s=0;s<se_info.size();s++){
                cchange_to_se_info[config_changes].push_back(se_info[s]);    
            }
        }
    }

    // cout << "go to show_se_info() at missing_route_path_search()" << endl;
    show_se_info(config_changes);


    return;

        /* here to
        if(!rtr_st->bgp_diff.import_filter.empty()){
            for(auto bif=rtr_st->bgp_diff.import_filter.begin();bif!=rtr_st->bgp_diff.import_filter.end();bif++){
                if(bif->second.size() == 0){
                    continue;
                }
                */
               /*
                if(!rtr_st->bgp_diff.import_filter.empty()){
                    for(auto bif=rtr_st->bgp_diff.import_filter.begin();bif!=rtr_st->bgp_diff.import_filter.end();bif++){
                        if(bif->second.size() == 0){
                            continue;
                        }
                        // cout << "we are here" << endl;
                        // cout << "router: " << rtr_st->name << endl;

                        if(bif->second[0].action == "set route_map drop"){
                            side_effect_count++;
                            // cout << "-----Missing route cause------" << endl;
                            // cout << "router " << rtr << endl;
                            // cout << "peer: " << bif->first << endl;
                            // cout << "filter name: " << bif->second[0].action << endl;
                            // cout << "------------------------" << endl;

                            config_changes += insert_config_changes(rtr,bif->second[0].action,"non");

                            struct side_effect_info sei;
                            sei.router = rtr_st;
                            sei.se_affected_router = cr;
                            sei.type = bif->second[0].action;

                            struct v4_route r;
                            r.prefix = addr;
                            r.prefix_len = pl;
                            sei.route = r;

                            sei.before_repair_path = cur_first.as_path;
                            sei.before_repair_path.push_back(cr->bgp.as_number);
                            sei.after_repair_path = diff_first.as_path;
                            sei.after_repair_path.push_back(cr->bgp.as_number);

                            se_info.push_back(sei);

                            if(first_flag == false){
                                first_flag = true;

                                //virtually simulate ?
                                //1. copy bgp_diff to tmp_bgp_diff
                                for(auto itr=name_to_router.begin();itr!=name_to_router.end();itr++){
                                    struct router *cr = itr->second;
                                    list<struct bgp_table_elms> table = itr->second->bgp.loc_rib_table[addr];
                                    cr->virtual_bgp_diff.loc_rib_table[addr] = table;
                                }

                                //2. run simulate with tmp_bgp_diff
                                unordered_map<unsigned int, vector<filter>> tmp_filter = rtr_st->virtual_bgp_diff.import_filter;
                                //test_5_22
                                rtr_st->virtual_bgp_diff.import_filter.clear();
                                struct bgp_message vir_bm;
                                vector<unsigned int > asp = cur_first.as_path;
                                asp.pop_back();
                                create_bm(&vir_bm,cur_first.adv_route, asp, 0);
                                virtual_simulate(vir_bm,rtr_st,addr, cur_first.as_path, th);
                                //3. check if best path before and after repair are not same
                                //4. search best path after repair and print the priority
                                rtr_st->virtual_bgp_diff.import_filter = tmp_filter;
                            }
                        }
                        else if(bif->second[0].action == ""){

                        }
                    }
                }
            }
     //   }
   // }

    if(!config_changes.empty()){
        if(cchange_to_se_info[config_changes].size() == 0){
            cchange_to_se_info[config_changes] = se_info;
        }
        else{
            for(int s=0;s<se_info.size();s++){
                cchange_to_se_info[config_changes].push_back(se_info[s]);    
            }
        }
    }

    // cout << "go to show_se_info() at missing_route_path_search()" << endl;
    show_se_info(config_changes);
    here*/
}

vector<unsigned int> convert_routervec_to_asvec(vector<struct router *> rv)
{
    vector<unsigned int> asvec;
    for(int i=0;i<rv.size();i++){
        asvec.push_back(rv[i]->bgp.as_number);
    }
    return asvec;
}

bool check_se_info_dup(vector<struct side_effect_info> seiv, struct side_effect_info sei)
{
    for(int i=0;i<seiv.size();i++){
        if(seiv[i].router->name == sei.router->name){
            if(seiv[i].before_repair_path == sei.before_repair_path){
                if(seiv[i].after_repair_path == sei.after_repair_path){
                    return false;
                }
            }
        }
    }

    return true;
}

void ospf_ecmp_path_search(vector<vector<struct router *>> before_repair_sp, vector<vector<struct router *>> after_repair_sp, bitset<32> addr, int pl)
{
    // cout << "Start to " << __func__ << endl;

    //bitset<32> p = route.prefix;
    //int l = route.prefix_len;

     //一番シンプルなのは、before/after repairのshortest pathを調べて、コスト変化を見つけること
    //与えられたパスを探索して、変化したコストを何らかの形でまとめて返す関数

    vector<pair<string, unsigned int>> before_repair_cost_vec;
    vector<pair<string, unsigned int>> after_repair_cost_vec;

    string config_changes = "";
    string ospf_action = "ospf_cost";
    vector<side_effect_info> se_info;

    //vector<unsigned int> tmp_before_asvec = convert_routervec_to_asvec(before_repair_sp);
    //vector<unsigned int> tmp_after_asvec = convert_routervec_to_asvec(after_repair_sp);

    for(int j=0;j<before_repair_sp.size();j++){
        vector<router *> tmp_before_repair_sp = before_repair_sp[j];
        vector<pair<string, unsigned int>> before_repair_cost_vec;

        before_repair_cost_vec = gather_ospf_diff_cost(tmp_before_repair_sp);
        
        for(int i=0;i<before_repair_cost_vec.size();i++){
            // cout << "link: " << before_repair_cost_vec[i].first << ", cost: " << before_repair_cost_vec[i].second << endl;
    
            string link = before_repair_cost_vec[i].first;
            string dev1 = get_name1(link);
            if(dev1 == link){
                // cout << "get_name1" << endl;
                exit(-1);
            }
            string dev2 = get_name2(link);
            if(dev2 == link){
                // cout << "get_name2" << endl;
                exit(-1);
            }

            struct router *tmp_router = name_to_router[dev1];
            unsigned int cost_before_repair = tmp_router->ospf.peer_cost_map[dev2];
            unsigned int cost_after_repair = before_repair_cost_vec[i].second;
            string value = to_string(cost_before_repair) + "-" +  to_string(cost_after_repair);

            string tmp_change = insert_config_changes(dev1,ospf_action,value);
            config_changes += tmp_change;
            
            //test
            /*
            struct side_effect_info sei;
            sei.router = tmp_router;
            sei.se_affected_router = tmp_before_repair_sp[tmp_before_repair_sp.size()-1];
            sei.type = ospf_action;
            sei.action_value_numeric = cost_after_repair;

            struct v4_route r;
            r.prefix = addr;
            r.prefix_len = pl;
            sei.route = r;

            vector<unsigned int> tmp_before_asvec = convert_routervec_to_asvec(tmp_before_repair_sp);
            sei.before_repair_path = tmp_before_asvec;
            //sei.after_repair_path = tmp_after_asvec;

            se_info.push_back(sei);
            */
        }
    }

    for(int j=0;j<after_repair_sp.size();j++){
        vector<router *> tmp_after_repair_sp = after_repair_sp[j];
        vector<pair<string, unsigned int>> after_repair_cost_vec;
        
        after_repair_cost_vec = gather_ospf_diff_cost(tmp_after_repair_sp);

        for(int i=0;i<after_repair_cost_vec.size();i++){
            // cout << "link: " << after_repair_cost_vec[i].first << " , cost: " << after_repair_cost_vec[i].second << endl;
    
            string link = after_repair_cost_vec[i].first;
            string dev1 = get_name1(link);
            if(dev1 == link){
                // cout << "get_name1" << endl;
                exit(-1);
            }
            string dev2 = get_name2(link);
            if(dev2 == link){
                // cout << "get_name2" << endl;
                exit(-1);
            }

            struct router *tmp_router = name_to_router[dev1];
            unsigned int cost_before_repair = tmp_router->ospf.peer_cost_map[dev2];
            unsigned int cost_after_repair = after_repair_cost_vec[i].second;
            string value = to_string(cost_before_repair) + "-" +  to_string(cost_after_repair);

            string tmp_change = insert_config_changes(dev1,ospf_action,value);
            config_changes += tmp_change;
            
            //test
            /*
            struct side_effect_info sei;
            sei.router = tmp_router;
            sei.se_affected_router = tmp_after_repair_sp[tmp_after_repair_sp.size()-1];
            sei.type = ospf_action;
            sei.action_value_numeric = cost_after_repair;

            struct v4_route r;
            r.prefix = addr;
            r.prefix_len = pl;
            sei.route = r;

            //sei.before_repair_path = tmp_before_asvec;
            vector<unsigned int> tmp_after_asvec = convert_routervec_to_asvec(tmp_after_repair_sp);
            sei.after_repair_path = tmp_after_asvec;

            se_info.push_back(sei);
            */
        }
    }

    //test
    if(before_repair_sp.size() != 0){
        struct side_effect_info sei;
        struct v4_route r;
        r.prefix = addr;
        r.prefix_len = pl;
        sei.route = r;
        
        for(int ff=0;ff<before_repair_sp.size();ff++){
            vector<unsigned int> tmp_before_asvec = convert_routervec_to_asvec(before_repair_sp[ff]);
            sei.before_repair_path = tmp_before_asvec;
            se_info.push_back(sei);
        }
    }
    else{
        //test
        cout << "before repair path is 0" << endl;
        return;
    }


    //ospf change
    if(!config_changes.empty()){
        // cout << "config changes: " << config_changes << endl;
        //// cout << "size: " << cchange_to_se_info[config_changes].size() << endl;
        if(cchange_to_se_info[config_changes].size() == 0){
            cchange_to_se_info[config_changes] = se_info;
        }
        else{
            for(int s=0;s<se_info.size();s++){
                if(check_se_info_dup(cchange_to_se_info[config_changes], se_info[s])){
                    cchange_to_se_info[config_changes].push_back(se_info[s]);
                }
            }
        }
    }

    // cout << "go to show_se_info()" << endl;
    show_se_info(config_changes);
}

void ospf_path_search(vector<struct router *> before_repair_sp, vector<struct router *> after_repair_sp, bitset<32> addr, int pl)
{
    // cout << "Start to " << __func__ << endl;

    //bitset<32> p = route.prefix;
    //int l = route.prefix_len;

     //一番シンプルなのは、before/after repairのshortest pathを調べて、コスト変化を見つけること
    //与えられたパスを探索して、変化したコストを何らかの形でまとめて返す関数

    vector<pair<string, unsigned int>> before_repair_cost_vec;
    vector<pair<string, unsigned int>> after_repair_cost_vec;

    before_repair_cost_vec = gather_ospf_diff_cost(before_repair_sp);
    after_repair_cost_vec = gather_ospf_diff_cost(after_repair_sp);

    string config_changes = "";
    string ospf_action = "ospf_cost";
    vector<side_effect_info> se_info;

    vector<unsigned int> tmp_before_asvec = convert_routervec_to_asvec(before_repair_sp);
    vector<unsigned int> tmp_after_asvec = convert_routervec_to_asvec(after_repair_sp);

    // cout << "-----Cost difference cause at Ospf_Path_Search------" << endl;

    // cout << "Changes at shortest path before repair" << endl;
    for(int i=0;i<before_repair_cost_vec.size();i++){
        // cout << "link: " << before_repair_cost_vec[i].first << ", cost: " << before_repair_cost_vec[i].second << endl;    

        string link = before_repair_cost_vec[i].first;
        string dev1 = get_name1(link);
        if(dev1 == link){
            // cout << "get_name1" << endl;
            exit(-1);
        }
        string dev2 = get_name2(link);
        if(dev2 == link){
            // cout << "get_name2" << endl;
            exit(-1);
        }

        struct router *tmp_router = name_to_router[dev1];
        unsigned int cost_before_repair = tmp_router->ospf.peer_cost_map[dev2];
        unsigned int cost_after_repair = before_repair_cost_vec[i].second;
        string value = to_string(cost_before_repair) + "-" +  to_string(cost_after_repair);

        string tmp_change = insert_config_changes(dev1,ospf_action,value);
        config_changes += tmp_change;

        struct side_effect_info sei;
        sei.router = tmp_router;
        sei.se_affected_router = before_repair_sp[before_repair_sp.size()-1];
        sei.type = ospf_action;
        sei.action_value_numeric = cost_after_repair;

        struct v4_route r;
        r.prefix = addr;
        r.prefix_len = pl;
        sei.route = r;

        sei.before_repair_path = tmp_before_asvec;
        sei.after_repair_path = tmp_after_asvec;

        se_info.push_back(sei);
        // cout << "sei_size: " << se_info.size() << endl;
    }

    // cout << "Changes at shortest path after repair" << endl;
    for(int i=0;i<after_repair_cost_vec.size();i++){
        // cout << "link: " << after_repair_cost_vec[i].first << ", cost: " << after_repair_cost_vec[i].second << endl;

        string link = after_repair_cost_vec[i].first;
        string dev1 = get_name1(link);
        if(dev1 == link){
            // cout << "get_name1" << endl;
            exit(-1);
        }
        string dev2 = get_name2(link);
        if(dev2 == link){
            // cout << "get_name2" << endl;
            exit(-1);
        }
        
        struct router *tmp_router = name_to_router[dev1];
        unsigned int cost_before_repair = tmp_router->ospf.peer_cost_map[dev2];
        unsigned int cost_after_repair = after_repair_cost_vec[i].second;
        string value = to_string(cost_before_repair) + "-" +  to_string(cost_after_repair);

        string tmp_change = insert_config_changes(dev1,ospf_action,value);
        config_changes += tmp_change;

        struct side_effect_info sei;
        sei.router = tmp_router;
        sei.se_affected_router = before_repair_sp[before_repair_sp.size()-1];
        sei.type = ospf_action;
        sei.action_value_numeric = cost_after_repair;

        struct v4_route r;
        r.prefix = addr;
        r.prefix_len = pl;
        sei.route = r;

        sei.before_repair_path = tmp_before_asvec;
        sei.after_repair_path = tmp_after_asvec;

        se_info.push_back(sei);
        // cout << "sei_size: " << se_info.size() << endl;
    }

    //ospf change
    if(!config_changes.empty()){
        // cout << "config changes: " << config_changes << endl;
        //// cout << "size: " << cchange_to_se_info[config_changes].size() << endl;
        if(cchange_to_se_info[config_changes].size() == 0){
            cchange_to_se_info[config_changes] = se_info;
        }
        else{
            for(int s=0;s<se_info.size();s++){
                if(check_se_info_dup(cchange_to_se_info[config_changes], se_info[s])){
                    cchange_to_se_info[config_changes].push_back(se_info[s]);
                }
            }
        }
    }

    // cout << "go to show_se_info()" << endl;
    show_se_info(config_changes);
}

//deletionの対応？
void se_preference_path_search(struct bgp_table_elms cur_first, struct bgp_table_elms diff_first, unsigned int cur_rtr_asn, bitset<32> addr, int pl, int diff_num)
{
    // cout << "Search best path before repair" << endl;

    int primary_cause_idx = -1;
    
    for(int acc=0;acc<cur_first.as_path.size();acc++){
        /*
        string rtr = to_string(cur_first.as_path[acc]);
        struct router *rtr_st = name_to_router[rtr];
        */
        struct router *rtr_st = as_number_to_router[cur_first.as_path[acc]][0];
        string rtr = rtr_st->name;

        if(!rtr_st->bgp.import_filter.empty()){
            for(auto bif=rtr_st->bgp.import_filter.begin();bif!=rtr_st->bgp.import_filter.end();bif++){
                if(bif->second.size() == 0){
                    continue;
                }

                // cout << "router " << rtr << endl;
                // cout << "peer: " << bif->first << endl;
                // cout << "filter name: " << bif->second[0].action << endl;

                if(primary_cause_idx ==  -1){
                    primary_cause_idx = acc;
                }
            }
        }
    }

    // cout << "Search best path after repair" << endl;

    diff_first.as_path.push_back(cur_rtr_asn);
    //print_as_number(diff_first);

    int cur_diff = 0;

    //rotuer_name.config_type.value:~~~
    string config_changes = "";
    vector<side_effect_info> se_info;

    bool done_flag = false;

    //string prev_config_change = "";

    //for(int df=0;df<diff_first.as_path.size();df++){
    for(int df=diff_first.as_path.size()-1;df>=0;df--){
        /*
        string rtr = to_string(diff_first.as_path[df]);
        struct router *diff_rtr_st = name_to_router[rtr];
        */
        struct router *diff_rtr_st = as_number_to_router[diff_first.as_path[df]][0];
        string rtr = diff_rtr_st->name;

        // cout << "visit at " << rtr << endl;

        /*only for scale test*/
        if(done_flag == true){
            break;
        }

        if(!diff_rtr_st->bgp_diff.import_filter.empty()){
            for(auto bif=diff_rtr_st->bgp_diff.import_filter.begin();bif!=diff_rtr_st->bgp_diff.import_filter.end();bif++){
                if(bif->second.size() == 0){
                    continue;
                }
                
                /*
                if(cur_diff >= diff_num){
                    // cout << "current diff value equal to the diff_num, so end !" << endl;
                    break;
                }
                */

                //この値でいいかチェック
                cur_diff += bif->second[0].action_value_numeric;
                // cout << "added value: " << bif->second[0].action_value_numeric;
                // cout << " cur_diff value: " << cur_diff << endl;

                //そのルーターのbest path before repairとafter repairがそのpriorityによって決まっていたら要因として出力
                list<struct bgp_table_elms> tmp_table_before_repair = diff_rtr_st->bgp.loc_rib_table[addr];
                list<struct bgp_table_elms> tmp_table_after_repair = diff_rtr_st->bgp_diff.loc_rib_table[addr];

                if(tmp_table_before_repair.empty() || tmp_table_after_repair.empty()){
                    continue;
                }

                struct bgp_table_elms tmp_best_before_repair = tmp_table_before_repair.front();
                struct bgp_table_elms tmp_best_after_repair = tmp_table_after_repair.front();

                //// cout << "tmp_best_before_reapir: ";
                //show_as_path(tmp_best_before_repair);

                //// cout << "tmp_best_after_reapir: ";
                //show_as_path(tmp_best_after_repair);


                //pathが一緒じゃなくて、このpriorityがafter repairの要因になっている
                if(!is_path_equal(tmp_best_before_repair.as_path,tmp_best_after_repair.as_path)){
                    //after repairのribのpriorityがこのaction_value_numericで決まっていたらのチェック
                    //cur_diff== か、curdiffが小さいか (ospfなら足し算がいる)
                    //matchk関数を作る？

                    // cout << "lp after repair: " << tmp_best_after_repair.lp << endl;
                    if(tmp_best_after_repair.lp == bif->second[0].action_value_numeric){
                        //side_effect_count++;
                        // cout << "-----Priority cause------" << endl;
                        // cout << "router: " << rtr << endl;
                        // cout << "peer: " << bif->first << endl;
                        // cout << "filter name: " << bif->second[0].action << endl;
                        // cout << "filter numeric: " << bif->second[0].action_value_numeric << endl;
                        // cout << "------------------------" << endl;

                        //メモする
                        //config_changesに積もっていかないか (多分起きない)
                        string tmp_change = insert_config_changes(rtr,bif->second[0].action,to_string(bif->second[0].action_value_numeric));
                        /*
                        if(tmp_change == prev_config_change){
                            continue;
                        }

                        prev_config_change = tmp_change;
                        */

                        config_changes += tmp_change;

                        //このルーターのこのconfigのこの値
                        struct side_effect_info sei;
                        sei.router = diff_rtr_st;
                        //sei.se_affected_router = name_to_router[to_string(cur_rtr_asn)];
                        sei.se_affected_router = as_number_to_router[cur_rtr_asn][0];
                        sei.type = "local preference";
                        sei.action_value_numeric = bif->second[0].action_value_numeric;

                        struct v4_route r;
                        r.prefix = addr;
                        r.prefix_len = pl;
                        sei.route = r;

                        tmp_best_before_repair.as_path.push_back(cur_rtr_asn);    
                        sei.before_repair_path = tmp_best_before_repair.as_path;
                        tmp_best_after_repair.as_path.push_back(cur_rtr_asn);
                        sei.after_repair_path = tmp_best_after_repair.as_path;

                        se_info.push_back(sei);
                        // cout << "sei_size: " << se_info.size() << endl;

                        done_flag = true;
                        
                        /*
                        if(cur_diff >= diff_num){
                            // cout << "current diff value equal to the diff_num, so end !" << endl;
                            break;
                        }*/
                    }
                }
            }
        }      
    }

    //メモする

    if(!config_changes.empty()){
        // cout << "config changes: " << config_changes << endl;
        //// cout << "size: " << cchange_to_se_info[config_changes].size() << endl;
        if(cchange_to_se_info[config_changes].size() == 0){
            cchange_to_se_info[config_changes] = se_info;
        }
        else{
            for(int s=0;s<se_info.size();s++){
                cchange_to_se_info[config_changes].push_back(se_info[s]);    
            }
        }
    }
    
    // cout << "go to show_se_info()" << endl;
    show_se_info(config_changes);
    
    //// cout << "popback" << endl;
    diff_first.as_path.pop_back();
}


void create_spec_to_ospf_config_change_map(vector<unsigned int> bp_br, vector<unsigned int> bp_ar, struct v4_route r)
{
    //inferを呼び出す
    struct specification spec;
    spec.forwarding_path = convert_upath_to_svpath(bp_ar);
    spec.route = r;


    // cout << "create_spec_to_ospf_config_change_map()" << endl;

    // cout << "shortest path before repair" << endl;
    print_ospf_vec_asn(bp_br, false);
    // cout << "repaired-shortest path before repair"<< endl;
    int c1 = print_ospf_vec_asn(bp_br, true);

    // cout << "shortest path after repair" << endl;
    int c2 = print_ospf_vec_asn(bp_ar, true);

    if(c2 > c1){
        // cout << "c2 > c1" << endl;
        exit(-1);
    }

    infer_ospf_config_repair_from_spec_for_scalability_eval(spec, bp_br, bp_ar);
}

void create_spec_to_ospf_ecmp_config_change_map(vector<vector<struct router *>> vec_best_path_before_repair, vector<vector<struct router *>> vec_best_path_after_repair, struct v4_route r)
{
    //inferを呼び出す
    struct specification spec;
    vector<string> src_dst;

    if(vec_best_path_after_repair.empty()){
        src_dst.push_back(vec_best_path_before_repair[0][0]->name);
        src_dst.push_back(vec_best_path_before_repair[0][vec_best_path_before_repair[0].size()-1]->name);
    }   
    else{
        src_dst.push_back(vec_best_path_after_repair[0][0]->name);
        src_dst.push_back(vec_best_path_after_repair[0][vec_best_path_after_repair[0].size()-1]->name);
    }

    //spec.forwarding_path = convert_upath_to_svpath(vec_best_path_after_repair[0]);
    spec.forwarding_path = src_dst;
    spec.route = r;


    // cout << "create_spec_to_ospf_config_change_map()" << endl;

    // cout << "shortest path before repair" << endl;
    //print_ospf_vec_asn(bp_br, false);

    // cout << "repaired-shortest path before repair"<< endl;
    /*
    int c1 = print_ospf_vec_asn(bp_br, true);

    // cout << "shortest path after repair" << endl;
    int c2 = print_ospf_vec_asn(bp_ar, true);

    if(c2 > c1){
        // cout << "c2 > c1" << endl;
        exit(-1);
    }
    */

    cout << "go infer" << endl;
    infer_ospf_ecmp_config_repair_from_spec_for_scalability_eval(spec,vec_best_path_before_repair, vec_best_path_after_repair);
}

void create_spec_to_config_change_map(vector<unsigned int> bp_br, vector<unsigned int> bp_ar, struct v4_route r)
{
    //inferを呼び出す
    struct specification spec;
    spec.forwarding_path = convert_upath_to_svpath(bp_ar);
    spec.route = r;

    // cout << "create_spec_to_config_change_map()" << endl;

    // cout << "route: " << endl;
    
    print_vec_asn(bp_ar);
    
    infer_config_repair_from_spec_for_scalability_eval(spec);
}

void create_spec_to_ospf_config_change_map_for_diff_rib_empty(vector<unsigned int> bp_br, vector<unsigned int> bp_ar, struct v4_route r)
{
    //inferを呼び出す
    struct specification spec;
    spec.forwarding_path = convert_upath_to_svpath(bp_br);
    spec.route = r;

    // cout << "create_spec_to_config_change_map()" << endl;

    // cout << "route: " << endl;
    
    print_vec_asn(bp_br);
    
    //infer_ospf_config_repair_from_spec_for_scalability_eval_for_diff_rib_empty(spec);
}


void create_spec_to_config_change_map_for_diff_rib_empty(vector<unsigned int> bp_br, vector<unsigned int> bp_ar, struct v4_route r)
{
    //inferを呼び出す
    struct specification spec;
    spec.forwarding_path = convert_upath_to_svpath(bp_br);
    spec.route = r;

    // cout << "create_spec_to_config_change_map()" << endl;

    // cout << "route: " << endl;
    
    print_vec_asn(bp_br);
    
    infer_config_repair_from_spec_for_scalability_eval_for_diff_rib_empty(spec);
}


//1. sorce of side effectが同一のグループに分ける
//2. どの仕様がどの副作用を生み出したかの関係性計算
//3. 仕様両立性について

static unsigned int new_se_count = 0;
static unsigned int new_per_prefix_se_counter=0;
static vector<unsigned int> new_per_prefix_se_counter_vec;
bool new_diff_route_propagation_generalized_dijkstra(struct router *router)
{
    //1. simulation
    //1.1 diffに打ちたるまでそのままnormal_ribとdiff_rib作りながら進む (normal ribのqueとdiff ribのque)
    //1.2 diffにあったら、routing processをそれぞれのメッセージでスプリットして、それぞれのメッセージでribを作りながらsimulate

    //2 ribのbest pathのdiffをチェック
    //2.1 diffがあったらspecで指定されているかチェック

    //3 diffが出た原因の特定
    //3.1 原因ごとにdiiff_graphを探索


    bool t = false;

    for(int i=0;i<router->bgp.advertised_v4_route.size();i++){
        for(int j=0;j<router->bgp.advertised_v4_route[i].second.size();j++){
            int pl = router->bgp.advertised_v4_route[i].second[j].prefix_len;
            // cout << "advertised prefix: " << router->bgp.advertised_v4_route[i].second[j].prefix.to_string() + "/" + to_string(router->bgp.advertised_v4_route[i].second[j].prefix_len) << endl;

            unordered_map<string, struct bgp_message> th;
            traverse_history = th;
            diff_traverse_history = th;

            struct bgp_message first_bm;
            first_bm.lp = 0;
            first_bm.community = 0;
            first_bm.adv_route = router->bgp.advertised_v4_route[i].second[j];
            first_bm.as_path.push_back(router->bgp.as_number);
            first_bm.sonzai = true;
            first_bm.unique_id = 0;
            first_bm.terminated = false;


            print_as_numbers(first_bm);

            priority_queue<dPair, vector<dPair>, compare_bgp_message> norm_pq;
            //priority_queue<dPair, vector<dPair>, compare_bgp_message> dif_pq;
            dif_pq = norm_pq;


            //norm_pqとグラフで続ける
            //diffを見つけたらdiff_pqにグラフのvisited(visitedはas_pathに入っている)とともに入れる
            //norm_pq終わったら、diff_pqに写って探索する、グラフに入れる
            //ribチェック

            if(new_apply_export_filter(&first_bm,  router, router->bgp.advertised_v4_route[i].first)){
                router->bgp.adj_rib_out[router->bgp.advertised_v4_route[i].first].push_back(router->bgp.advertised_v4_route[i].second[j]);
                
                reset_single_bgp_parameters(&first_bm);

                for(auto itr=router->bgp.e_peer_relation.begin();itr!=router->bgp.e_peer_relation.end();itr++){
                    struct router *next_router = router->bgp.e_peer_relation[itr->first];
                    traverse_history[next_router->name] = first_bm;

                    tuple<bgp_message, struct router *> tmp_pair{first_bm, next_router};
                    
                    router->bgp.advertised_peers.push_back(next_router->bgp.as_number);

                    next_router->bgp.received_peers[router->bgp.as_number] = true;

                    norm_pq.push(tmp_pair); 
                }

                while(!norm_pq.empty()){
                    tuple<bgp_message, struct router *> cur_pair;

                    cur_pair = norm_pq.top();
                    norm_pq.pop();

                    struct bgp_message cur_bm;
                    struct router *cur_router;
                    cur_bm = get<0>(cur_pair);
                    cur_router = get<1>(cur_pair);

                    bool flag=false;    

                    if(cur_bm.sonzai == true && cur_bm.terminated == false){
                        for(int as=0;as<cur_bm.as_path.size();as++){
                            if(cur_bm.as_path[as] == cur_router->bgp.as_number){
                                flag = true;
                                break;
                            }
                        }

                        if(flag==true){
                            continue;
                        }
                

                        unsigned int prev_as_number = cur_bm.as_path[cur_bm.as_path.size()-1]; 

                        //store bgp_adj_rib_in
                        cur_router->bgp.adj_rib_in[prev_as_number].push_back(cur_bm.adv_route);

                        if(apply_default_diff_import_filter(&cur_bm, cur_router, prev_as_number) == false){
                            continue;
                        }

                        if(new_apply_import_filter(&cur_bm, cur_router, prev_as_number)){

                            pair<bool, bool> is_this_route_first = new_add_bgp_table(cur_router,&cur_bm);

                            //test_03_17
                            if(cur_bm.terminated != true){
                                all_pair_reachability[cur_bm.as_path[0]][cur_router->bgp.as_number] = true;
                            }

                            print_as_numbers(cur_bm);
                            //ここでbest_routeじゃなかったらstop
                            if(is_this_route_first.first == false){
                                continue;
                            }

                            if(cur_bm.sonzai == true && cur_bm.terminated == false){
                                cur_bm.as_path.push_back(cur_router->bgp.as_number);
                            }

                            if(cur_bm.sonzai == true && cur_bm.terminated == false){
                                for(auto itr = cur_router->bgp.e_peer_relation.begin();itr!=cur_router->bgp.e_peer_relation.end();itr++){
                                    if(itr->first == prev_as_number){
                                        continue;
                                    }   

                                    cur_router->bgp.adj_rib_out[itr->first].push_back(cur_bm.adv_route);

                                    reset_single_bgp_parameters(&cur_bm);

                                    if(new_apply_export_filter(&cur_bm, cur_router, itr->first)){
       
                                        pair<bgp_message, struct router *> next_tuple{cur_bm, itr->second};
                                        
                                        if(cur_bm.sonzai == true && cur_bm.terminated == false){
                                            cur_router->bgp.advertised_peers.push_back(itr->second->bgp.as_number);
                                            itr->second->bgp.received_peers[cur_router->bgp.as_number] = true;

                                            norm_pq.push(next_tuple);
                                        }
                                    }
                                }   
                            }
                        }
                    }
                }
            }

            //rib copy
            // cout << "Prefix addr: " << router->bgp.advertised_v4_route[i].second[j].prefix << endl;
            for(auto itr=name_to_router.begin();itr!=name_to_router.end();itr++){
                struct router *cr = itr->second;
                list<struct bgp_table_elms> table = itr->second->bgp.loc_rib_table[router->bgp.advertised_v4_route[i].second[j].prefix];
                cr->bgp_diff.loc_rib_table[router->bgp.advertised_v4_route[i].second[j].prefix] = table;
            }

            //start diff_pq
            if(dif_pq.size() != 0){
                // cout << "diff start !" << endl;
            }

            while(!dif_pq.empty()){
                tuple<bgp_message, struct router *> cur_diff_pair;

                cur_diff_pair = dif_pq.top();
                dif_pq.pop();

                struct bgp_message diff_bm;
                struct router *diff_router;
                diff_bm = get<0>(cur_diff_pair);
                diff_router = get<1>(cur_diff_pair);

                diff_bm.sonzai = true;
                diff_bm.terminated = false;

                print_as_numbers(diff_bm);

                bool flag=false;    

                for(int t=0;t<diff_bm.as_path.size();t++){
                    if(diff_bm.as_path[t] == diff_router->bgp_diff.as_number){
                        flag = true;
                        break;
                    }
                }

                if(flag==true){
                    continue;
                }
                
                unsigned int prev_as_number = diff_bm.as_path[diff_bm.as_path.size()-1];

                diff_router->bgp_diff.adj_rib_in[prev_as_number].push_back(diff_bm.adv_route);

                //この時点でribから消しておく
                //preferenceが下がる->他のhigh priorityのやつのupdateをqueに入れる
                //preferenceが上がる->もしbest pathならそのupdateをqueに入れる

                //for test with default import filter
                if(apply_default_diff_import_filter(&diff_bm, diff_router, prev_as_number) == false){
                    continue;
                }

                if(apply_diff_import_filter(&diff_bm, diff_router, prev_as_number)){
                    
                    pair<bool, bool> is_this_route_first = new_add_diff_bgp_table(diff_router,&diff_bm);

                    if(diff_bm.terminated != true){
                        all_pair_reachability[diff_bm.as_path[0]][diff_router->bgp_diff.as_number] = true;
                    }

                    print_as_numbers(diff_bm);
                    //ここでbest_routeじゃなかったらstop
                    if(is_this_route_first.first == false){
                        continue;
                    }


                    if(diff_bm.sonzai == true && diff_bm.terminated == false){
                        diff_bm.as_path.push_back(diff_router->bgp_diff.as_number);

                        for(auto itr = diff_router->bgp_diff.e_peer_relation.begin();itr!=diff_router->bgp_diff.e_peer_relation.end();itr++){
                            if(itr->first == prev_as_number){
                                continue;
                            }   

                            diff_router->bgp_diff.adj_rib_out[itr->first].push_back(diff_bm.adv_route);

                            reset_single_bgp_parameters(&diff_bm);

                            if(new_apply_export_filter(&diff_bm, diff_router, itr->first)){
       
                                pair<bgp_message, struct router *> next_tuple{diff_bm, itr->second};
                                        
                                if(diff_bm.sonzai == true && diff_bm.terminated == false){
                                    diff_router->bgp_diff.advertised_peers.push_back(itr->second->bgp_diff.as_number);
                                    itr->second->bgp_diff.received_peers[diff_router->bgp_diff.as_number] = true;

                                    dif_pq.push(next_tuple);
                                }
                            }
                        }   
                    }
                }     
            }

            //与えられたspecは全部満たされているか

            //diffが生じているノードとそのパスを特定する
            //原因を特定して、両方に保持する
            //全体を通して、同じ原因のグループを作る(これが1つ目)
            //副作用をなくす仕様と既存仕様間の静的けんさ(これが2つ目)

            //check_side effect

            int test_flag = 0;
            unordered_map<string, unordered_map<string, bool>> se_checker;

            new_per_prefix_se_counter = 0;

            //diff あるやつ
            priority_queue<prisePair> pri_sep;

            //route propagationした順番に辿っていく

            //differential simulationをやる時
            //とりあえず、prefixごとに
            //diff-traceを作った後に、経路広報順にafter repairのグラフを探索する
            //副作用の関係グラフを作る
            //どれとどれが消えれば、どの副作用は追加しなくて良いか
            
            
            //// cout << "go to the side effect analysis" << endl;
            for(auto itr=name_to_router.begin();itr!=name_to_router.end();itr++){
                struct router *cr = itr->second;
                //// cout << "current router: " << cr->name << endl;
                bitset<32> addr = router->bgp.advertised_v4_route[i].second[j].prefix;

                /*
                if(cr->bgp_diff.loc_rib_table[addr].empty()){
                    //// cout << "No elements, No change happen." << endl;
                    if(!cr->bgp.loc_rib_table[addr].empty()){
                        // cout << "Before diff rib is empty" << endl;
                        // cout << "Side-effect occurs at router " << cr->name << endl;

                        list<struct bgp_table_elms> table = cr->bgp.loc_rib_table[addr];
                        struct bgp_table_elms cur_first = table.front();

                        new_per_prefix_se_counter++;

                        exit(1);
                        se_analysis_null_rib_after_repair(cur_first, cr, addr);

                        continue;
                    }
                    else{
                        // cout << "No elements, No change happen." << endl;
                        continue;
                    }
                }*/

                if(cr->bgp_diff.loc_rib_table[addr].empty()){
                    if(!cr->bgp.loc_rib_table[addr].empty()){
                        // cout << "--------------------------------" << endl;
                        // cout << "Side Effect " << side_effect_cnt << endl;
                        // cout << "router " << cr->bgp.as_number << endl;

                        side_effect_cnt++;

                        list<struct bgp_table_elms> table = cr->bgp.loc_rib_table[addr];
                        struct bgp_table_elms cur_first = table.front();

                        struct bgp_table_elms diff_empty;
                        diff_empty.empty = true;

                        // cout << "bgp_diff.loc_rib_table is empty !" << endl;

                        /*
                        vector<unsigned int> tmp_bp_br = cur_first.as_path;
                        tmp_bp_br.push_back(cr->bgp.as_number);
                        vector<unsigned int> tmp_bp_ar = diff_first.as_path;
                        tmp_bp_ar.push_back(cr->bgp.as_number);
                        // cout << "----------------" << endl;
                        create_spec_to_config_change_map(tmp_bp_br, tmp_bp_ar, router->bgp.advertised_v4_route[i].second[j]);
                        // cout << "create_spec_to_config_change_map is done" << endl;
                        // cout << "---------------------" << endl;
                        // cout << endl;
                        */
                        vector<unsigned int> tmp_bp_br = cur_first.as_path;
                        tmp_bp_br.push_back(cr->bgp.as_number);
                        vector<unsigned int> empty_bp_ar;
                        empty_bp_ar.push_back(-1);

                        // cout << "----------------" << endl;
                        create_spec_to_config_change_map_for_diff_rib_empty(tmp_bp_br, empty_bp_ar, router->bgp.advertised_v4_route[i].second[j]);
                        // cout << "create_spec_to_config_change_map is done" << endl;
                        // cout << "---------------------" << endl;


                        
                        se_missing_route_path_search(cur_first, diff_empty,cr,addr,pl);

                        // cout << "--------------------------------" << endl;
                    }
                    else{
                        // cout << "No elements, No change happen." << endl;
                    }
                    continue;
                }


                list<struct bgp_table_elms> table = cr->bgp.loc_rib_table[addr];
                struct bgp_table_elms cur_first = table.front();

                list<struct bgp_table_elms> diff_table = cr->bgp_diff.loc_rib_table[addr];
                struct bgp_table_elms diff_first = diff_table.front();

                if(is_path_equal(cur_first.as_path, diff_first.as_path)){ 
                    /*
                    // cout << "Same preffered route, no change happen at router " << cr->name << endl;
                    print_uint_path(cur_first.as_path);
                    // cout << endl;
                    */
                    continue;
                }
                else{
                    // cout << "--------------------------------" << endl;
                    // cout << "Side Effect " << side_effect_cnt << endl;
                    // cout << "router " << cr->bgp.as_number << endl;

                    side_effect_cnt++;

                    // cout << "best path before repair: ";
                    print_as_number(cur_first);

                    // cout << "best path after repair: ";
                    print_as_number(diff_first); 

                    new_per_prefix_se_counter++;

                    /*
                    for scalability-testings
                    */
                   vector<unsigned int> tmp_bp_br = cur_first.as_path;
                   tmp_bp_br.push_back(cr->bgp.as_number);
                   vector<unsigned int> tmp_bp_ar = diff_first.as_path;
                   tmp_bp_ar.push_back(cr->bgp.as_number);
                   // cout << "----------------" << endl;
                    create_spec_to_config_change_map(tmp_bp_br, tmp_bp_ar, router->bgp.advertised_v4_route[i].second[j]);
                    // cout << "create_spec_to_config_change_map is done" << endl;
                    // cout << "---------------------" << endl;
                    // cout << endl;
                    //


                    //cur_firstがdiff_ribにいるかをチェック
                    // cout << "router: " << cr->name << endl;
                    int best_idx = new_find_the_best_route(cur_first, &diff_table);
                    if(best_idx > -1){
                        // cout << "diff is " << best_idx << endl;
                        se_preference_path_search(cur_first, diff_first,cr->bgp.as_number, addr, pl, best_idx);
                        
                        t = true;
                        //priority
                        /*
                        prisePair tmp_sep;
                        tmp_sep.first = diff_first.as_path.size();
                        tmp_sep.second = cr;

                        pri_sep.push(tmp_sep);
                        */
                        
                    }
                    else{
                        //missing route
                        // cout << "goto se_misssing_route_path_search" << endl;
                        se_missing_route_path_search(cur_first, diff_first,cr, addr, pl);

                        t = true;
                    }
                }
            }
        }
    }

    return t;
}


bool ospf_diff_route_propagation_generalized_dijkstra(struct router *router)
{
    bool t = false;

    for(int i=0;i<router->bgp.advertised_v4_route.size();i++){
        for(int j=0;j<router->bgp.advertised_v4_route[i].second.size();j++){
            int pl = router->bgp.advertised_v4_route[i].second[j].prefix_len;
            // cout << "advertised prefix: " << router->bgp.advertised_v4_route[i].second[j].prefix.to_string() + "/" + to_string(router->bgp.advertised_v4_route[i].second[j].prefix_len) << endl;

            unordered_map<string, struct bgp_message> th;
            traverse_history = th;
            diff_traverse_history = th;

            struct bgp_message first_bm;
            first_bm.lp = 0;
            first_bm.community = 0;

            first_bm.adv_route = router->bgp.advertised_v4_route[i].second[j];

            first_bm.as_path.push_back(router->bgp.as_number);
            first_bm.sonzai = true;
            first_bm.unique_id = 0;
            first_bm.terminated = false;
            first_bm.total_cost = 0;


            priority_queue<dPair, vector<dPair>, compare_ospf_message> norm_pq;

            //初期地点のqueへの追加
            //e_peer -> zoo_topo[router->name]のneighbor
            /*
            for(auto itr=router->bgp.e_peer_relation.begin();itr!=router->bgp.e_peer_relation.end();itr++){
                struct router *next_router = router->bgp.e_peer_relation[itr->first];
                traverse_history[next_router->name] = first_bm;

                tuple<bgp_message, struct router *> tmp_pair{first_bm, next_router};
                    
                router->bgp.advertised_peers.push_back(next_router->bgp.as_number);

                next_router->bgp.received_peers[router->bgp.as_number] = true;

                norm_pq.push(tmp_pair); 
            }
            */

            //// cout << "start router: " << router->name << endl;

            for(int f=0;f<router->ospf.peer_info.size();f++){
                unsigned int cost = router->ospf.peer_info[f].cost;
                string peer_name = router->ospf.peer_info[f].peer_name;
                struct router *next_router = name_to_router[peer_name];

                //// cout << "first_added_router: " << next_router->name << endl;

                first_bm.total_cost = cost;
                
                tuple<bgp_message, struct router *> tmp_pair{first_bm, next_router};

                norm_pq.push(tmp_pair);
            }

            //dif_ospf_pq = norm_pq;

            while(!norm_pq.empty()){
                tuple<bgp_message, struct router *> cur_pair;

                cur_pair = norm_pq.top();
                norm_pq.pop();

                struct bgp_message cur_bm;
                struct router *cur_router;
                cur_bm = get<0>(cur_pair);
                cur_router = get<1>(cur_pair);

                //// cout << "cur_router: " << cur_router->name << endl;
                //// cout << "cost: " << cur_bm.total_cost << endl;

                cur_bm.sonzai = true;
                cur_bm.terminated = true;

                bool flag=false;    

                for(int as=0;as<cur_bm.as_path.size();as++){
                    if(cur_bm.as_path[as] == cur_router->bgp.as_number){
                        flag = true;
                        break;
                    }
                }

                if(flag==true){
                    continue;
                }
                

                unsigned int prev_as_number = cur_bm.as_path[cur_bm.as_path.size()-1]; 

                pair<bool, bool> is_this_route_first = add_ospf_table(cur_router,&cur_bm);
                //pair<bool, bool> is_this_route_first = new_add_bgp_table(cur_router,&cur_bm);

            
                //all_pair_reachability[cur_bm.as_path[0]][cur_router->bgp.as_number] = true;

                print_as_numbers(cur_bm);
                //ここでbest_routeじゃなかったらstop
                if(is_this_route_first.first == false){
                    continue;
                }

                cur_bm.as_path.push_back(cur_router->bgp.as_number);

                for(int ie=0;ie<cur_router->ospf.peer_info.size();ie++){
                    struct bgp_message tmp_next_bm = cur_bm;

                    string next = cur_router->ospf.peer_info[ie].peer_name;
                    unsigned int cost = cur_router->ospf.peer_info[ie].cost;
                    struct router *next_router = name_to_router[next];

                    // cout << "Next: " << next << endl;;

                    if(next_router->bgp.as_number == prev_as_number){
                        continue;
                    }

                    tmp_next_bm.total_cost += cost;

                    //// cout << "cost: " << cost << endl;
                    //// cout << "cur_cost: " << cur_bm.total_cost << endl;

                    pair<bgp_message, struct router *> next_tuple{tmp_next_bm, next_router};

                    norm_pq.push(next_tuple);
                }

                /*
                for(auto itr = cur_router->bgp.e_peer_relation.begin();itr!=cur_router->bgp.e_peer_relation.end();itr++){
                    if(itr->first == prev_as_number){
                        continue;
                    }   

                    pair<bgp_message, struct router *> next_tuple{cur_bm, itr->second};
                                        
                    cur_router->bgp.advertised_peers.push_back(itr->second->bgp.as_number);
                    itr->second->bgp.received_peers[cur_router->bgp.as_number] = true;

                    norm_pq.push(next_tuple);
                } 
                */
            }

            //// cout << "Prefix addr: " << router->bgp.advertised_v4_route[i].second[j].prefix << endl;
            //ospf change
            /*
            for(auto itr=name_to_router.begin();itr!=name_to_router.end();itr++){
                struct router *cr = itr->second;
                list<struct bgp_table_elms> table = itr->second->bgp.loc_rib_table[router->bgp.advertised_v4_route[i].second[j].prefix];
                cr->bgp_diff.loc_rib_table[router->bgp.advertised_v4_route[i].second[j].prefix] = table;
            }
            */

            for(int l=0;l<router->ospf_diff.peer_info.size();l++){
                string peer_name = router->ospf_diff.peer_info[l].peer_name;
                struct router *next_router = name_to_router[peer_name];
                unsigned int cost = 0;
                if(router->ospf_diff.diff_peer_cost_map[peer_name] > 0){
                    cost = router->ospf_diff.diff_peer_cost_map[peer_name];
                }
                else{
                    cost = router->ospf_diff.peer_info[l].cost;
                }

                //// cout << "first_added_router for diff: " << next_router->name << endl;

                first_bm.total_cost = cost;
                
                tuple<bgp_message, struct router *> tmp_pair{first_bm, next_router};

                dif_ospf_pq.push(tmp_pair);
            }

            //start diff_pq
            if(dif_ospf_pq.size() != 0){
                // cout << endl;
                // cout << "----------------" << endl;
                // cout << "diff start !" << endl;
            }

            while(!dif_ospf_pq.empty()){
                tuple<bgp_message, struct router *> cur_diff_pair;

                cur_diff_pair = dif_ospf_pq.top();
                dif_ospf_pq.pop();

                struct bgp_message diff_bm;
                struct router *diff_router;
                diff_bm = get<0>(cur_diff_pair);
                diff_router = get<1>(cur_diff_pair);

                //// cout << "----------------" << endl;
                //// cout << "cur_router: " << diff_router->name << endl;
                //// cout << "cost: " << diff_bm.total_cost << endl;


                diff_bm.sonzai = true;
                diff_bm.terminated = false;

                print_as_numbers(diff_bm);

                bool flag=false;

                for(int t=0;t<diff_bm.as_path.size();t++){
                    if(diff_bm.as_path[t] == diff_router->bgp_diff.as_number){
                        flag = true;
                        break;
                    }
                }

                if(flag==true){
                    continue;
                }
                
                unsigned int prev_as_number = diff_bm.as_path[diff_bm.as_path.size()-1];
                
                //pair<bool, bool> is_this_route_first = new_add_ospf_diff_table(diff_router,&diff_bm);
                pair<bool, bool> is_this_route_first = add_diff_ospf_table(diff_router,&diff_bm);
                //pair<bool, bool> is_this_route_first = new_add_diff_bgp_table(diff_router,&diff_bm);

                //all_pair_reachability[diff_bm.as_path[0]][diff_router->bgp_diff.as_number] = true;

                print_as_numbers(diff_bm);
                //ここでbest_routeじゃなかったらstop
                if(is_this_route_first.first == false){
                    continue;
                }

                diff_bm.as_path.push_back(diff_router->bgp_diff.as_number);
                
                for(int fa=0;fa<diff_router->ospf_diff.peer_info.size();fa++){
                    struct bgp_message tmp_next_bm = diff_bm;

                    string next = diff_router->ospf_diff.peer_info[fa].peer_name;
                    struct router *next_router = name_to_router[next];

                    if(next_router->bgp.as_number == prev_as_number){
                        continue;
                    }

                    //// cout << "Next: " << next << endl;;
                    
                    unsigned int cost = 0;
                    if(diff_router->ospf_diff.diff_peer_cost_map[next] > 0){
                        cost = diff_router->ospf_diff.diff_peer_cost_map[next];
                        //// cout << "diff cost: " << cost << endl;
                        //cost = diff_router->ospf_diff.peer_info[i].cost;
                    }
                    else{
                        cost = diff_router->ospf_diff.peer_info[fa].cost;
                        //// cout << "before-repair-cost: " << cost << endl;
                    }
                    
                    //unsigned int cost = diff_router->ospf_diff.peer_info[i].cost;

                    tmp_next_bm.total_cost += cost;

                    //// cout << "diff_bm.total_cost: " << tmp_next_bm.total_cost << endl;

                    pair<bgp_message, struct router *> next_tuple{tmp_next_bm, next_router};

                    dif_ospf_pq.push(next_tuple);
                }
                /*
                for(auto itr = diff_router->bgp_diff.e_peer_relation.begin();itr!=diff_router->bgp_diff.e_peer_relation.end();itr++){
                    if(itr->first == prev_as_number){
                        continue;
                    }   
       
                    pair<bgp_message, struct router *> next_tuple{diff_bm, itr->second};
                                        
                    diff_router->bgp_diff.advertised_peers.push_back(itr->second->bgp_diff.as_number);
                    itr->second->bgp_diff.received_peers[diff_router->bgp_diff.as_number] = true;

                    dif_pq.push(next_tuple);
                }*/
            }

            int test_flag = 0;
            unordered_map<string, unordered_map<string, bool>> se_checker;

            new_per_prefix_se_counter = 0;
            priority_queue<prisePair> pri_sep;

            
            //// cout << "go to the side effect analysis" << endl;
            for(auto itr=name_to_router.begin();itr!=name_to_router.end();itr++){
                struct router *cr = itr->second;
                //// cout << "current router: " << cr->name << endl;
                bitset<32> addr = router->bgp.advertised_v4_route[i].second[j].prefix;
                
                //ospf change to_do!
                if(cr->bgp_diff.loc_rib_table[addr].empty()){
                    if(!cr->bgp.loc_rib_table[addr].empty()){
                        // cout << "We should not come here for ospf (!cr->bgp.loc_rib_table[addr].empty()))" << endl;
                        exit(-1);
                    }
                    else{
                        // cout << "No elements, No change happen." << endl;
                        //// cout << "Ftatal error !" << endl;
                        //exit(-1);
                    }
                    continue;
                }


                list<struct bgp_table_elms> table = cr->bgp.loc_rib_table[addr];
                struct bgp_table_elms cur_first = table.front();
                vector<vector<unsigned int>> vec_elm;
                if(cur_first.vec_path.size() != 0){
                    vec_elm = cur_first.vec_path;
                    sort(vec_elm.begin(), vec_elm.end());
                    
                    //cout << "vec_elm is found at router " << cr->name << endl;
                    //cout << "tmp_vec_elm.size(): " << vec_elm.size() << endl;
                }
                else{
                    cout << "No vec_elm" << endl;
                    exit(-1);
                }

                list<struct bgp_table_elms> diff_table = cr->bgp_diff.loc_rib_table[addr];
                struct bgp_table_elms diff_first = diff_table.front();
                vector<vector<unsigned int>> diff_vec_elm;
                if(diff_first.vec_path.size() != 0){
                    diff_vec_elm = diff_first.vec_path;
                    sort(diff_vec_elm.begin(), diff_vec_elm.end());

                    //cout << "diff_vec_elm is found at router " << cr->name << endl;
                    //cout << "tmp_vec_elm.size(): " << diff_vec_elm.size() << endl;
                }
                else{
                    cout << "No vec_elm" << endl;
                    exit(-1);
                }

                //if(is_path_equal(cur_first.as_path, diff_first.as_path)){
                if(is_multiple_path_equal(vec_elm, diff_vec_elm)){
                    
                    //cout << "Same preffered route, no change happen at router " << cr->name << endl;
                    /*
                    //print_uint_path(cur_first.as_path);
                    // cout << endl;
                    */
                    continue;
                }
                else{
                     /*
                    for scalability-testings
                    */
                   vector<unsigned int> tmp_bp_br = cur_first.as_path;
                   tmp_bp_br.push_back(cr->bgp.as_number);
                   vector<unsigned int> tmp_bp_ar = diff_first.as_path;
                   tmp_bp_ar.push_back(cr->bgp.as_number);
                   
                   for(int k=0;k<vec_elm.size();k++){
                        vec_elm[k].push_back(cr->bgp.as_number);
                   }

                   for(int k=0;k<diff_vec_elm.size();k++){
                        diff_vec_elm[k].push_back(cr->bgp.as_number);
                   }
                   
                   /*
                   cout << "diff happen" << endl;
                   cout << "before repair size: " << vec_elm.size() << endl;
                   for(int a=0;a<vec_elm.size();a++){
                        print_vec_asn(vec_elm[a]);
                   }

                   cout << "after repair size: " << diff_vec_elm.size() << endl;
                   for(int a=0;a<diff_vec_elm.size();a++){
                    print_vec_asn(diff_vec_elm[a]);
               }
                    */


                    /*only for now*/
                    //for multipath
                    /*
                    if(print_ospf_vec_asn(tmp_bp_br, true) ==  print_ospf_vec_asn(tmp_bp_ar, true)){
                        continue;
                    }
                    */
                    /**/
                    //cout << "--------------------------------" << endl;
                    //cout << "Side Effect " << side_effect_cnt << endl;
                    //cout << "router " << cr->bgp.as_number << endl;

                    side_effect_cnt++;
                    
                    //// cout << "best path before repair: ";
                    //print_as_number(cur_first);

                    //// cout << "best path after repair: ";
                    //print_as_number(diff_first); 

                    new_per_prefix_se_counter++;

                    unordered_map<int, bool> checked;

                    vector<vector<struct router*>> before_repair_sp;
                    vector<vector<struct router*>> after_repair_sp;

                    for(int i=0;i<vec_elm.size();i++){
                        vector<unsigned int> cur_before_shortest_path = vec_elm[i];
                        bool f = false;
                        for(int j=0;j<diff_vec_elm.size();j++){
                            if(cur_before_shortest_path == diff_vec_elm[j]){
                                checked[j] = true;
                                f = true;
                                break;
                            }
                        }
                
                        if(f == false){
                            vector<struct router *> brsp;
                            for(int k=0;k<vec_elm[i].size();k++){
                                struct router *r = as_number_to_router[vec_elm[i][k]][0];
                                brsp.push_back(r);
                            }
                
                            before_repair_sp.push_back(brsp);
                        }
                    }

                    //cout << "here" << endl;

                    for(int i=0;i<diff_vec_elm.size();i++){
                        if(checked[i] == true){
                            continue;
                        }
                
                        vector<struct router *> arsp;
                        for(int k=0;k<diff_vec_elm[i].size();k++){
                            struct router *r = as_number_to_router[diff_vec_elm[i][k]][0];
                            arsp.push_back(r);
                        }
                        after_repair_sp.push_back(arsp);
                    }

                    /*
                    if( after_repair_sp.size() > 0){
                        continue;
                    }*/

                    if(before_repair_sp.empty() && after_repair_sp.empty()){
                        cout << "empty to do" << endl;
                        continue;
                    }
                    cout << "--------" << endl;
                    cout << "before_size: " << before_repair_sp.size() << endl;
                    for(int ta=0;ta<before_repair_sp.size();ta++){
                        print_vec_router(before_repair_sp[ta]);
                    }
                    cout << "after_size: "  << after_repair_sp.size() << endl;
                    for(int ta=0;ta<after_repair_sp.size();ta++){
                        print_vec_router(after_repair_sp[ta]);
                    }
                    cout << "--------" << endl;

                   // cout << "----------------" << endl;
                   //ospf_change
                   create_spec_to_ospf_ecmp_config_change_map(before_repair_sp, after_repair_sp, router->bgp.advertised_v4_route[i].second[j]);
                    //create_spec_to_ospf_config_change_map(tmp_bp_br, tmp_bp_ar, router->bgp.advertised_v4_route[i].second[j]);
                     cout << "create_spec_to_config_change_map is done" << endl;
                    // cout << "---------------------" << endl;
                    //
                    
                    //convert 

                    //vector<struct router*> after_repair_sp;
                    //vector<struct router*> before_repair_sp;
                   //vector<vector<struct router*>> before_repair_sp;
                   //vector<vector<struct router*>> after_repair_sp;
                    
                   /*
                   for(int e=0;e<vec_elm.size();e++){
                        vector<unsigned int> tmp_bp_br = vec_elm[e];
                        vector<struct router*> tmp_r_vec;
                        for(int d=0;d<tmp_bp_br.size();d++){
                            struct router *r = as_number_to_router[tmp_bp_br[d]][0];
                            tmp_r_vec.push_back(r);
                        }
                        before_repair_sp.push_back(tmp_r_vec);
                   }

                   for(int e=0;e<diff_vec_elm.size();e++){
                        vector<unsigned int> tmp_bp_ar = diff_vec_elm[e];
                        vector<struct router*> diff_tmp_r_vec;
                        for(int d=0;d<tmp_bp_ar.size();d++){
                            struct router *r = as_number_to_router[tmp_bp_ar[d]][0];
                            diff_tmp_r_vec.push_back(r);
                        }
                        after_repair_sp.push_back(diff_tmp_r_vec);
                    }
                    */
               

                    /*
                    for(int ea=0;ea<tmp_bp_ar.size();ea++){
                        //// cout << "router name: " << tmp_bp_ar[ea] << endl;
                        //struct router *r = name_to_router[spec.forwarding_path[i]];
                        struct router *r = as_number_to_router[tmp_bp_ar[ea]][0];
                        after_repair_sp.push_back(r);
                    }

                    for(int ea=0;ea<tmp_bp_br.size();ea++){
                        //// cout << "router name: " << tmp_bp_br[ea] << endl;

                        struct router *r = as_number_to_router[tmp_bp_br[ea]][0];
                        before_repair_sp.push_back(r);
                    }   
                    */


                    ospf_ecmp_path_search(before_repair_sp, after_repair_sp, addr, pl);
                    //ospf_path_search(before_repair_sp, after_repair_sp, addr, pl);
                    t = true;
                }
            }
        }
    }

    return t;
}

static unsigned int se_count = 0;
static unsigned int per_prefix_se_counter=0;
static vector<unsigned int> per_prefix_se_counter_vec;
void diff_route_propagation_generalized_dijkstra(struct router *router)
{
    //1. simulation
    //1.1 diffに打ちたるまでそのままnormal_ribとdiff_rib作りながら進む (normal ribのqueとdiff ribのque)
    //1.2 diffにあったら、routing processをそれぞれのメッセージでスプリットして、それぞれのメッセージでribを作りながらsimulate

    //2 ribのbest pathのdiffをチェック
    //2.1 diffがあったらspecで指定されているかチェック

    //3 diffが出た原因の特定
    //3.1 原因ごとにdiiff_graphを探索

    for(int i=0;i<router->bgp.advertised_v4_route.size();i++){
        for(int j=0;j<router->bgp.advertised_v4_route[i].second.size();j++){
            // cout << "advertised prefix: " << router->bgp.advertised_v4_route[i].second[j].prefix.to_string() + "/" + to_string(router->bgp.advertised_v4_route[i].second[j].prefix_len) << endl;

            unordered_map<string, struct bgp_message> th;
            traverse_history = th;
            diff_traverse_history = th;

            struct bgp_message first_bm;
            first_bm.lp = 0;
            first_bm.community = 0;
            first_bm.adv_route = router->bgp.advertised_v4_route[i].second[j];
            first_bm.as_path.push_back(router->bgp.as_number);
            first_bm.sonzai = true;
            first_bm.unique_id = 0;
            first_bm.terminated = false;


            print_as_numbers(first_bm);

            priority_queue<dPair, vector<dPair>, compare_bgp_message> norm_pq;
            //priority_queue<dPair, vector<dPair>, compare_bgp_message> dif_pq;
            dif_pq = norm_pq;


            //norm_pqとグラフで続ける
            //diffを見つけたらdiff_pqにグラフのvisited(visitedはas_pathに入っている)とともに入れる
            //norm_pq終わったら、diff_pqに写って探索する、グラフに入れる
            //ribチェック

            if(new_apply_export_filter(&first_bm,  router, router->bgp.advertised_v4_route[i].first)){
                router->bgp.adj_rib_out[router->bgp.advertised_v4_route[i].first].push_back(router->bgp.advertised_v4_route[i].second[j]);
                
                reset_single_bgp_parameters(&first_bm);

                for(auto itr=router->bgp.e_peer_relation.begin();itr!=router->bgp.e_peer_relation.end();itr++){
                    struct router *next_router = router->bgp.e_peer_relation[itr->first];
                    traverse_history[next_router->name] = first_bm;

                    tuple<bgp_message, struct router *> tmp_pair{first_bm, next_router};
                    
                    router->bgp.advertised_peers.push_back(next_router->bgp.as_number);

                    next_router->bgp.received_peers[router->bgp.as_number] = true;

                    norm_pq.push(tmp_pair); 
                }

                while(!norm_pq.empty()){
                    tuple<bgp_message, struct router *> cur_pair;

                    cur_pair = norm_pq.top();
                    norm_pq.pop();

                    struct bgp_message cur_bm;
                    struct router *cur_router;
                    cur_bm = get<0>(cur_pair);
                    cur_router = get<1>(cur_pair);

                    bool flag=false;    

                    if(cur_bm.sonzai == true && cur_bm.terminated == false){
                        for(int as=0;as<cur_bm.as_path.size();as++){
                            if(cur_bm.as_path[as] == cur_router->bgp.as_number){
                                flag = true;
                                break;
                            }
                        }

                        if(flag==true){
                            continue;
                        }
                

                        unsigned int prev_as_number = cur_bm.as_path[cur_bm.as_path.size()-1]; 

                        //store bgp_adj_rib_in
                        cur_router->bgp.adj_rib_in[prev_as_number].push_back(cur_bm.adv_route);

                        if(new_apply_import_filter(&cur_bm, cur_router, prev_as_number)){

                            pair<bool, bool> is_this_route_first = new_add_bgp_table(cur_router,&cur_bm);

                            //test_03_17
                            if(cur_bm.terminated != true){
                                all_pair_reachability[cur_bm.as_path[0]][cur_router->bgp.as_number] = true;
                            }

                            print_as_numbers(cur_bm);
                            //ここでbest_routeじゃなかったらstop
                            if(is_this_route_first.first == false){
                                continue;
                            }

                            if(cur_bm.sonzai == true && cur_bm.terminated == false){
                                cur_bm.as_path.push_back(cur_router->bgp.as_number);
                            }

                            if(cur_bm.sonzai == true && cur_bm.terminated == false){
                                for(auto itr = cur_router->bgp.e_peer_relation.begin();itr!=cur_router->bgp.e_peer_relation.end();itr++){
                                    if(itr->first == prev_as_number){
                                        continue;
                                    }   

                                    cur_router->bgp.adj_rib_out[itr->first].push_back(cur_bm.adv_route);

                                    reset_single_bgp_parameters(&cur_bm);

                                    if(new_apply_export_filter(&cur_bm, cur_router, itr->first)){
       
                                        pair<bgp_message, struct router *> next_tuple{cur_bm, itr->second};
                                        
                                        if(cur_bm.sonzai == true && cur_bm.terminated == false){
                                            cur_router->bgp.advertised_peers.push_back(itr->second->bgp.as_number);
                                            itr->second->bgp.received_peers[cur_router->bgp.as_number] = true;

                                            norm_pq.push(next_tuple);
                                        }
                                    }
                                }   
                            }
                        }
                    }
                }
            }

            //rib copy
            // cout << "Prefix addr: " << router->bgp.advertised_v4_route[i].second[j].prefix << endl;
            for(auto itr=name_to_router.begin();itr!=name_to_router.end();itr++){
                struct router *cr = itr->second;
                list<struct bgp_table_elms> table = itr->second->bgp.loc_rib_table[router->bgp.advertised_v4_route[i].second[j].prefix];
                cr->bgp_diff.loc_rib_table[router->bgp.advertised_v4_route[i].second[j].prefix] = table;
            }

            //start diff_pq
            if(dif_pq.size() != 0){
                // cout << "diff start !" << endl;
            }

            while(!dif_pq.empty()){
                tuple<bgp_message, struct router *> cur_diff_pair;

                cur_diff_pair = dif_pq.top();
                dif_pq.pop();

                struct bgp_message diff_bm;
                struct router *diff_router;
                diff_bm = get<0>(cur_diff_pair);
                diff_router = get<1>(cur_diff_pair);

                diff_bm.sonzai = true;
                diff_bm.terminated = false;

                print_as_numbers(diff_bm);

                bool flag=false;    

                for(int t=0;t<diff_bm.as_path.size();t++){
                    if(diff_bm.as_path[t] == diff_router->bgp_diff.as_number){
                        flag = true;
                        break;
                    }
                }

                if(flag==true){
                    continue;
                }
                
                unsigned int prev_as_number = diff_bm.as_path[diff_bm.as_path.size()-1];

                diff_router->bgp_diff.adj_rib_in[prev_as_number].push_back(diff_bm.adv_route);

                //この時点でribから消しておく
                //preferenceが下がる->他のhigh priorityのやつのupdateをqueに入れる
                //preferenceが上がる->もしbest pathならそのupdateをqueに入れる

                if(apply_diff_import_filter(&diff_bm, diff_router, prev_as_number)){
                    
                    pair<bool, bool> is_this_route_first = new_add_diff_bgp_table(diff_router,&diff_bm);

                    if(diff_bm.terminated != true){
                        all_pair_reachability[diff_bm.as_path[0]][diff_router->bgp_diff.as_number] = true;
                    }

                    print_as_numbers(diff_bm);
                    //ここでbest_routeじゃなかったらstop
                    if(is_this_route_first.first == false){
                        continue;
                    }


                    if(diff_bm.sonzai == true && diff_bm.terminated == false){
                        diff_bm.as_path.push_back(diff_router->bgp_diff.as_number);

                        for(auto itr = diff_router->bgp_diff.e_peer_relation.begin();itr!=diff_router->bgp_diff.e_peer_relation.end();itr++){
                            if(itr->first == prev_as_number){
                                continue;
                            }   

                            diff_router->bgp_diff.adj_rib_out[itr->first].push_back(diff_bm.adv_route);

                            reset_single_bgp_parameters(&diff_bm);

                            if(new_apply_export_filter(&diff_bm, diff_router, itr->first)){
       
                                pair<bgp_message, struct router *> next_tuple{diff_bm, itr->second};
                                        
                                if(diff_bm.sonzai == true && diff_bm.terminated == false){
                                    diff_router->bgp_diff.advertised_peers.push_back(itr->second->bgp_diff.as_number);
                                    itr->second->bgp_diff.received_peers[diff_router->bgp_diff.as_number] = true;

                                    dif_pq.push(next_tuple);
                                }
                            }
                        }   
                    }
                }      
            }

            //check_side effect

            int test_flag = 0;
            unordered_map<string, unordered_map<string, bool>> se_checker;

            per_prefix_se_counter = 0;

            for(auto itr=name_to_router.begin();itr!=name_to_router.end();itr++){
                struct router *cr = itr->second;
                bitset<32> addr = router->bgp.advertised_v4_route[i].second[j].prefix;

                if(cr->bgp_diff.loc_rib_table[addr].empty()){
                    //// cout << "No elements, No change happen." << endl;

                    if(!cr->bgp.loc_rib_table[addr].empty()){
                        // cout << "Side-effect occurs at router " << cr->name << endl;

                        list<struct bgp_table_elms> table = cr->bgp.loc_rib_table[addr];
                        struct bgp_table_elms cur_first = table.front();

                        per_prefix_se_counter++;

                        se_analysis_null_rib_after_repair(cur_first, cr, addr);

                        continue;
                    }
                    else{
                        // cout << "No elements, No change happen." << endl;
                        continue;
                    }
                }

                list<struct bgp_table_elms> table = cr->bgp.loc_rib_table[addr];
                list<struct bgp_table_elms> diff_table = cr->bgp_diff.loc_rib_table[addr];

                struct bgp_table_elms cur_first = table.front();
                struct bgp_table_elms diff_first = diff_table.front();

                /*if(is_path_given_in_spec(&router->bgp.advertised_v4_route[i].second[j], cur_first.as_path)){
                    // cout << "The cause of this side effect is a specification issue !" << endl;
                }*/

                //if(cur_first.id == diff_first.id){
                if(is_path_equal(cur_first.as_path, diff_first.as_path)){ 
                    //// cout << "Same preffered route, no change happen at router " << cr->name << endl;
                }
                else{
                    // cout << "Side-effect occurs at router " << cr->name << endl;

                    //test_05_22
                    //cur_first.as_path.push_back(cr->bgp.as_number);
                    //diff_first.as_path.push_back(cr->bgp.as_number);

                    // cout << "best path before repair: ";
                    print_as_number(cur_first);

                    // cout << "best path after repair: ";
                    print_as_number(diff_first); 

                    per_prefix_se_counter++;

                    //cur_firstがdiff_ribにいるかをチェック
                    int best_idx = find_the_best_route(cur_first, &diff_table);
                    if(best_idx != -1){
                        //いたらpriority issueをprint
                        // cout << "The cause of this side effect is a preference issue !" << endl;
                        test_flag = 1;

                        //priorityの原因を特定、同じseかをチェック
                        // cout << "Search best path before repair" << endl;
                        print_as_number(cur_first);

                        //1. identify primary cause (what do the types of the priority make the difference?)

                        int primary_cause_idx = -1;
                        for(int acc=0;acc<cur_first.as_path.size();acc++){
                            /*
                            string rtr = to_string(cur_first.as_path[acc]);
                            struct router *rtr_st = name_to_router[rtr];
                            */
                            struct router *rtr_st = as_number_to_router[cur_first.as_path[acc]][0];
                            string rtr = rtr_st->name;

                            if(!rtr_st->bgp.import_filter.empty()){
                                for(auto bif=rtr_st->bgp.import_filter.begin();bif!=rtr_st->bgp.import_filter.end();bif++){
                                    if(bif->second.size() == 0){
                                        continue;
                                    }
                                    // cout << "router " << rtr << endl;
                                    // cout << "peer: " << bif->first << endl;
                                    // cout << "filter name: " << bif->second[0].action << endl;

                                    if(primary_cause_idx ==  -1){
                                        primary_cause_idx = acc;
                                    }
                                }
                            }
                        }

                        // cout << "Search best path after repair" << endl;
                        print_as_number(diff_first);

                        diff_first.as_path.push_back(cr->bgp.as_number);
                        //print_as_number(diff_first);

                        for(int df=0;df<diff_first.as_path.size();df++){
                            /*
                            string rtr = to_string(diff_first.as_path[df]);
                            struct router *diff_rtr_st = name_to_router[rtr];
                            */
                           struct router *diff_rtr_st = as_number_to_router[diff_first.as_path[df]][0];
                           string rtr = diff_rtr_st->name;

                            //// cout << "visit at " << rtr << endl;

                            if(!diff_rtr_st->bgp_diff.import_filter.empty()){
                                for(auto bif=diff_rtr_st->bgp_diff.import_filter.begin();bif!=diff_rtr_st->bgp_diff.import_filter.end();bif++){
                                    if(bif->second.size() == 0){
                                        continue;
                                    }
                                    //そのルーターのbest path before repairとafter repairがそのpriorityによって決まっていたら要因として出力
                                    list<struct bgp_table_elms> tmp_table_before_repair = diff_rtr_st->bgp.loc_rib_table[addr];
                                    list<struct bgp_table_elms> tmp_table_after_repair = diff_rtr_st->bgp_diff.loc_rib_table[addr];

                                    if(tmp_table_before_repair.empty() || tmp_table_after_repair.empty()){
                                        continue;
                                    }


                                    struct bgp_table_elms tmp_best_before_repair = tmp_table_before_repair.front();
                                    struct bgp_table_elms tmp_best_after_repair = tmp_table_after_repair.front();

                                    //pathが一緒じゃなくて、このpriorityがafter repairの要因になっている
                                    if(!is_path_equal(tmp_best_before_repair.as_path,tmp_best_after_repair.as_path)){
                                        if(tmp_best_after_repair.lp == bif->second[0].action_value_numeric){
                                            side_effect_count++;
                                            // cout << "-----Priority cause------" << endl;
                                            // cout << "router: " << rtr << endl;
                                            // cout << "peer: " << bif->first << endl;
                                            // cout << "filter name: " << bif->second[0].action << endl;
                                            // cout << "filter numeric: " << bif->second[0].action_value_numeric << endl;
                                            // cout << "------------------------" << endl;

                                            se_checker[rtr][bif->second[0].action] = true;
                                        }
                                    }

                                    //// cout << "router " << rtr << endl;
                                    //// cout << "peer: " << bif->first << endl;
                                    //// cout << "filter name: " << bif->second[0].action << endl;
                                    //// cout << "filter numeric: " << bif->second[0].action_value_numeric << endl;

                                    
                                }
                            }                      
                        }

                        //// cout << "popback" << endl;
                        diff_first.as_path.pop_back();
                        //print_as_number(diff_first);
                    }
                    else{
                        //いなかったら探しに行く
                        // cout << "The cause of this side effect is a missng route issue !" << endl;

                        //dropされたルールを探しに行く、同じseかをチェック
                        // cout << "Search best path before repair" << endl;
                        print_as_number(cur_first);

                        cur_first.as_path.push_back(cr->bgp.as_number);

                        bool first_flag = false;

                        for(int th=0;th<cur_first.as_path.size();th++){
                            /*
                            string rtr = to_string(cur_first.as_path[th]);
                            struct router *rtr_st = name_to_router[rtr];
                            */
                            struct router *rtr_st = as_number_to_router[cur_first.as_path[th]][0];
                            string rtr = rtr_st->name;

                            //diffのどこで落ちてるかチェック
                            if(!rtr_st->bgp_diff.import_filter.empty()){
                                for(auto bif=rtr_st->bgp_diff.import_filter.begin();bif!=rtr_st->bgp_diff.import_filter.end();bif++){
                                    if(bif->second.size() == 0){
                                        continue;
                                    }

                                    if(bif->second[0].action == "set route_map drop"){
                                        side_effect_count++;
                                        // cout << "-----Missing route cause------" << endl;
                                        // cout << "router " << rtr << endl;
                                        // cout << "peer: " << bif->first << endl;
                                        // cout << "filter name: " << bif->second[0].action << endl;
                                        // cout << "------------------------" << endl;

                                        se_checker[rtr][bif->second[0].action] = true;

                                        if(first_flag == false){
                                            first_flag = true;

                                            //virtually simulate ?
                                            //1. copy bgp_diff to tmp_bgp_diff
                                             for(auto itr=name_to_router.begin();itr!=name_to_router.end();itr++){
                                                struct router *cr = itr->second;
                                                list<struct bgp_table_elms> table = itr->second->bgp.loc_rib_table[router->bgp.advertised_v4_route[i].second[j].prefix];
                                                cr->virtual_bgp_diff.loc_rib_table[router->bgp.advertised_v4_route[i].second[j].prefix] = table;
                                            }

                                            //2. run simulate with tmp_bgp_diff
                                            unordered_map<unsigned int, vector<filter>> tmp_filter = rtr_st->virtual_bgp_diff.import_filter;
                                            //test_5_22
                                            rtr_st->virtual_bgp_diff.import_filter.clear();
                                            struct bgp_message vir_bm;
                                            vector<unsigned int > asp = cur_first.as_path;
                                            asp.pop_back();
                                            create_bm(&vir_bm,cur_first.adv_route, asp, 0);
                                            virtual_simulate(vir_bm,rtr_st,router->bgp.advertised_v4_route[i].second[j].prefix, cur_first.as_path, th);
                                            //3. check if best path before and after repair are not same
                                            //4. search best path after repair and print the priority
                                            rtr_st->virtual_bgp_diff.import_filter = tmp_filter;
                                        }
                                    }
                                    /*
                                    else if(bif->second[0].action == "set local_pref"){
                                       //そのルーターのbest path before repairとafter repairがそのpriorityによって決まっていたら要因として出力
                                        list<struct bgp_table_elms> tmp_table_before_repair = rtr_st->bgp.loc_rib_table[addr];
                                        list<struct bgp_table_elms> tmp_table_after_repair = rtr_st->bgp_diff.loc_rib_table[addr];

                                        if(tmp_table_before_repair.empty() || tmp_table_after_repair.empty()){
                                            continue;
                                        }

                                        struct bgp_table_elms tmp_best_before_repair = tmp_table_before_repair.front();
                                        struct bgp_table_elms tmp_best_after_repair = tmp_table_after_repair.front();

                                        //pathが一緒じゃなくて、このpriorityがafter repairの要因になっている
                                        if(!is_path_equal(tmp_best_before_repair.as_path,tmp_best_after_repair.as_path)){
                                            if(tmp_best_after_repair.lp == bif->second[0].action_value_numeric){
                                                // cout << "-----Future priority cause------" << endl;
                                                // cout << "router: " << rtr << endl;
                                                // cout << "peer: " << bif->first << endl;
                                                // cout << "filter name: " << bif->second[0].action << endl;
                                                // cout << "filter numeric: " << bif->second[0].action_value_numeric << endl;
                                                // cout << "------------------------" << endl;
                                            }
                                        }
                                    }
                                    */
                                }
                            }
                        }

                        cur_first.as_path.pop_back();
                    }
                }
            }

            int secc = 0;
            for(auto sec=se_checker.begin();sec!=se_checker.end();sec++){
                secc++;
                se_count++;
            }
            sec_vec.push_back(secc);

            // cout << "Per prefix SE count: " << per_prefix_se_counter << endl;
            per_prefix_se_counter_vec.push_back(per_prefix_se_counter);
        }
    }
}

int print_se_average_per_prefix()
{
    int num = 0;

    for(int i=0;i<sec_vec.size();i++){
        num += sec_vec[i];
        //// cout <<"sec[i]: " << sec_vec[i] << " ";
    }
    //// cout << endl;

    return num/sec_vec.size();
}

int print_per_prefix_se_num()
{
    int num=0;

    for(int i=0;i<per_prefix_se_counter_vec.size();i++){
        num += per_prefix_se_counter_vec[i];
    }

    return num/per_prefix_se_counter_vec.size();
}

//test_04_02
bitset<32> test_0402_addr;
void route_propagation_generalized_dijkstra(struct router *router)
{
    msg_id = 0;

    for(int i=0;i<router->bgp.advertised_v4_route.size();i++){
        for(int j=0;j<router->bgp.advertised_v4_route[i].second.size();j++){
            // cout << "advertised prefix: " << router->bgp.advertised_v4_route[i].second[j].prefix.to_string() + "/" + to_string(router->bgp.advertised_v4_route[i].second[j].prefix_len) << endl;
            test_0402_addr = router->bgp.advertised_v4_route[i].second[j].prefix;

            unordered_map<string, struct bgp_message> th;
            traverse_history = th;
            diff_traverse_history = th;

            struct bgp_message first_bm;
            first_bm.lp = 0;
            first_bm.community = 0;
            first_bm.adv_route = router->bgp.advertised_v4_route[i].second[j];
            first_bm.as_path.push_back(router->bgp.as_number);
            first_bm.sonzai = true;
            first_bm.unique_id = 0;
            first_bm.terminated = false;

            struct bgp_message first_diff_bm;
            first_diff_bm.lp = 0;
            first_diff_bm.community = 0;
            first_diff_bm.sonzai = false;
            first_diff_bm.unique_id = 0;
            first_diff_bm.terminated = false;
            
            /*
            msg_id++;

            bm.unique_id = msg_id;
            diff_bm.unique_id = msg_id;
            */
            //to do: consider ibgp && peer diff
            /*
            if(router->bgp.e_peer_relation[router->bgp.advertised_v4_route[i].first] == NULL){
                // cout << "no peer " <<  endl;
                continue;
            }
            */



            print_as_numbers(first_bm);

            if(apply_export_filter(&first_bm, &first_diff_bm, router, router->bgp.advertised_v4_route[i].first)){
                router->bgp.adj_rib_out[router->bgp.advertised_v4_route[i].first].push_back(router->bgp.advertised_v4_route[i].second[j]);
                if(first_diff_bm.sonzai == true && first_diff_bm.terminated == false){
                    router->bgp_diff.adj_rib_out[router->bgp.advertised_v4_route[i].first].push_back(router->bgp.advertised_v4_route[i].second[j]);
                }
                
                //priority_queue<dPair, vector<dPair>, compare_bgp_message> pq;
                priority_queue<dTuple, vector<dTuple>, compare_bgp_message_tuple> pq;
                
                reset_bgp_parameters(&first_bm, &first_diff_bm);

                //to_do: bmが死んでるときはdiffだけで回す
                for(auto itr=router->bgp.e_peer_relation.begin();itr!=router->bgp.e_peer_relation.end();itr++){
                    struct router *next_router = router->bgp.e_peer_relation[itr->first];
                    //test_03_17
                    traverse_history[next_router->name] = first_bm;
                    if(first_diff_bm.sonzai == true && first_diff_bm.terminated == false){
                        diff_traverse_history = traverse_history;
                    }

                    tuple<bgp_message, bgp_message, struct router *> tmp_pair{first_bm, first_diff_bm, next_router};
                    
                    router->bgp.advertised_peers.push_back(next_router->bgp.as_number);
                    router->bgp_diff.advertised_peers.push_back(next_router->bgp_diff.as_number);

                    next_router->bgp.received_peers[router->bgp.as_number] = true;
                    next_router->bgp_diff.received_peers[router->bgp_diff.as_number] = true;

                    pq.push(tmp_pair); 
                }

                while(!pq.empty()){
                    tuple<bgp_message, bgp_message, struct router *> cur_pair;

                    cur_pair = pq.top();
                    pq.pop();

                    struct bgp_message cur_bm;
                    struct bgp_message cur_diff_bm;
                    struct router *cur_router;
                    cur_bm = get<0>(cur_pair);
                    cur_diff_bm = get<1>(cur_pair);
                    cur_router = get<2>(cur_pair);

                    //// cout << "check_withdrawal_map" << endl;

                    //check_withdrawal_map(&cur_bm, &cur_diff_bm);
                    //// cout << "withdrawal_path size: " << withdrawal_path.size() << endl;
                    //// cout << "withdrawal_diff_path size: " << withdrawal_diff_path.size() << endl;

                    //// cout << "check_withdrawal_map end" << endl;

                    //// cout << "--------------------------------" << endl;

                    if(cur_bm.sonzai == false && cur_bm.terminated == true && cur_diff_bm.sonzai == false){
                        // cout << "cur_bm is dead at router " << cur_router->name << endl;
                        continue;
                    }

                    /*
                    // cout << "cur_bm: ";
                    print_as_numbers(cur_bm);
                    // cout << endl;
                    // cout << "cur_diff_bm: ";
                    print_as_numbers(cur_diff_bm);
                    // cout << endl;
                    */
                    
                    bool flag=false;    
                    //cur_bmが生きててdiffも生きてるか、cur_bmのみ生きてるか、diffが死んでるように見える
                    if(cur_bm.sonzai == true && cur_bm.terminated == false){
                        for(int i=0;i<cur_bm.as_path.size();i++){
                            if(cur_bm.as_path[i] == cur_router->bgp.as_number){
                                flag = true;
                                break;
                            }
                        }

                        if(flag==true){
                            continue;
                        }
                        //to_do: consider cur_diff_bm?
                

                        unsigned int prev_as_number = cur_bm.as_path[cur_bm.as_path.size()-1]; 

                        //store bgp_adj_rib_in
                        cur_router->bgp.adj_rib_in[prev_as_number].push_back(cur_bm.adv_route);
                        if(cur_diff_bm.sonzai == true && cur_diff_bm.terminated == false){
                            cur_router->bgp_diff.adj_rib_in[prev_as_number].push_back(cur_bm.adv_route);
                        }
                        
                        //to_do: filterでcur_bmだけ落とされてcur_diff_bmだけ生き残った時の場合分け
                        //to_do: traverse_historyのdiffも作る
                        if(apply_import_filter(&cur_bm, &cur_diff_bm ,cur_router, prev_as_number)){
                            struct route_data data;
                            data.prefix = cur_bm.adv_route.prefix;
                            data.prefixlen = cur_bm.adv_route.prefix_len;
                            data.as_number = cur_bm.as_path;
                            //add parameter
                            data.weight = 0;
                            data.local_pref = cur_bm.lp;
    
                            //ここでpreferenceのチェックできるはず
                            //次のtodo
                            //bitset_radix_tree_add_w_pref(cur_router->bgp.loc_rib, data,cur_bm.adv_route.prefix_len, cur_bm.adv_route.prefix);
                            pair<bool, bool> is_this_route_first = add_bgp_table(cur_router,&cur_bm,&cur_diff_bm, &pq);

                            if(cur_diff_bm.sonzai == true && cur_diff_bm.terminated == false){
                                struct route_data data2;
                                data2.prefix = cur_diff_bm.adv_route.prefix;
                                data2.prefixlen = cur_diff_bm.adv_route.prefix_len;
                                data2.as_number = cur_diff_bm.as_path;
                                //add parameter
                                data2.weight = 0;
                                data2.local_pref = cur_diff_bm.lp;
                                //bitset_radix_tree_add_w_pref(cur_router->bgp_diff.loc_rib, data2, cur_diff_bm.adv_route.prefix_len,cur_diff_bm.adv_route.prefix);
                            }
       
                            //test_03_17
                            if(!(cur_bm.terminated == true && cur_diff_bm.terminated == true)){
                                all_pair_reachability[cur_bm.as_path[0]][cur_router->bgp.as_number] = true;
                            }

                            print_as_numbers(cur_bm);
                            //ここでbest_routeじゃなかったらstop
                            if(is_this_route_first.first == false && is_this_route_first.second == false){
                                continue;
                            }
                            else if(is_this_route_first.first == true && is_this_route_first.second == false){
                                
                                //cur_diff_bmは優先度で負けた、sonzaiはtrueだがterminatedもtrue
                                if(cur_diff_bm.sonzai == true){
                                    cur_diff_bm.terminated = true;
                                }
                                //ue koko
                               //ここでterminateすると、復活した時に進めなくなる

                            }
                            else if(is_this_route_first.first == false && is_this_route_first.second == true){
                                
                                //cur_bmは優先度で負けた、sonzaiはtrueだがterminatedもtrue
                                if(cur_bm.sonzai == true){
                                    cur_bm.terminated = true;
                                }
                                //ここでterminateすると、復活した時に進めなくなる
                                //ue koko
                            }
                            //true, trueはそのまま続行


                            if(cur_bm.sonzai == true && cur_bm.terminated == false){
                                cur_bm.as_path.push_back(cur_router->bgp.as_number);
                            }

                            if(cur_diff_bm.sonzai == true && cur_diff_bm.terminated == false){
                                cur_diff_bm.as_path.push_back(cur_router->bgp_diff.as_number);
                            }

                            if(cur_bm.sonzai == true && cur_bm.terminated == false){
                                for(auto itr = cur_router->bgp.e_peer_relation.begin();itr!=cur_router->bgp.e_peer_relation.end();itr++){
                                    if(itr->first == prev_as_number){
                                        continue;
                                    }   

                                    /*
                                    if(cur_router->bgp.received_peers[itr->second->bgp.as_number] == true){
                                        continue;
                                    }*/

                                    //to_do: adj_rib_in/outの上書き, not push back
                                    cur_router->bgp.adj_rib_out[itr->first].push_back(cur_bm.adv_route);
                                    if(cur_diff_bm.sonzai == true && cur_diff_bm.terminated == false){
                                        cur_router->bgp_diff.adj_rib_out[itr->first].push_back(cur_diff_bm.adv_route);
                                    }

                                    reset_bgp_parameters(&cur_bm, &cur_diff_bm);

                                    if(apply_export_filter(&cur_bm, &cur_diff_bm, cur_router, itr->first)){
                                        //to do: dropの処理, terminated
       
                                        tuple<bgp_message, bgp_message, struct router *> next_tuple{cur_bm,cur_diff_bm,itr->second};
                                        
                                        if(cur_bm.sonzai == true && cur_bm.terminated == false){
                                            cur_router->bgp.advertised_peers.push_back(itr->second->bgp.as_number);
                                            itr->second->bgp.received_peers[cur_router->bgp.as_number] = true;

                                            if((cur_diff_bm.sonzai == true && cur_diff_bm.terminated == false) || (cur_diff_bm.sonzai == false && cur_diff_bm.terminated == false)){
                                                cur_router->bgp_diff.advertised_peers.push_back(itr->second->bgp_diff.as_number);
                                                itr->second->bgp_diff.received_peers[cur_router->bgp_diff.as_number] = true;
                                            }

                                            pq.push(next_tuple);
                                        }
                                        else if(cur_diff_bm.sonzai == true && cur_diff_bm.terminated == false){
                                            cur_router->bgp_diff.advertised_peers.push_back(itr->second->bgp_diff.as_number);
                                            itr->second->bgp_diff.received_peers[cur_router->bgp_diff.as_number] = true;

                                            pq.push(next_tuple);
                                        }
                                    }
                                }   
                            }
                            else if(cur_diff_bm.sonzai == true && cur_diff_bm.terminated == false){
                                for(auto itr = cur_router->bgp_diff.e_peer_relation.begin();itr!=cur_router->bgp_diff.e_peer_relation.end();itr++){
                                    if(itr->first == prev_as_number){
                                        continue;
                                    }

                                    /*    
                                    if(cur_router->bgp_diff.received_peers[itr->second->bgp_diff.as_number] == true){
                                        continue;
                                    }
                                    */
                            
                                    cur_router->bgp_diff.adj_rib_out[itr->first].push_back(cur_diff_bm.adv_route);
        
                                    reset_bgp_parameters(&cur_bm, &cur_diff_bm);

                                    if(apply_diff_export_filter(&cur_diff_bm, cur_router, itr->first)){
                                        //to do: dropの処理, terminated

                                        tuple<bgp_message, bgp_message, struct router *> next_tuple{cur_bm,cur_diff_bm,itr->second};
            
                                        if(cur_diff_bm.terminated == false){
                                            cur_router->bgp_diff.advertised_peers.push_back(itr->second->bgp_diff.as_number);
                                            itr->second->bgp_diff.received_peers[cur_router->bgp_diff.as_number] = true;

                                            pq.push(next_tuple);
                                        }
                                    }
                                } 
                            }
                        }
                    }
                    //cur_bmが死んでるように見えて(or 死んでて)、diff_bmだけ生きてる
                    else if(cur_diff_bm.sonzai == true && cur_diff_bm.terminated == false){
                        for(int i=0;i<cur_diff_bm.as_path.size();i++){
                            if(cur_diff_bm.as_path[i] == cur_router->bgp_diff.as_number){
                                flag = true;
                                break;
                            }
                        }

                        if(flag==true){
                            continue;
                        }

                        /*
                        // cout << "visited at router " << cur_router->name << " ";
                        if(cur_bm.terminated == false && cur_diff_bm.sonzai == true && cur_diff_bm.terminated == false){
                            // cout << "with bm and diff_bm" << endl;
                        }
                        else if(cur_bm.terminated == false && (cur_diff_bm.sonzai == false || cur_diff_bm.sonzai == true && cur_diff_bm.terminated == true)){
                            // cout << "with bm only" << endl;
                        }
                        else if(cur_bm.terminated == true && cur_diff_bm.sonzai == true && cur_diff_bm.terminated == false){
                            // cout << "with diff_bm only" << endl;
                        }
                        else {
                            // cout << "fatal error at bm types (terminated) !!!" << endl;
                        }
                        */
                        
                        unsigned int prev_as_number = cur_diff_bm.as_path[cur_diff_bm.as_path.size()-1];

                        cur_router->bgp_diff.adj_rib_in[prev_as_number].push_back(cur_diff_bm.adv_route);

                        if(apply_diff_import_filter(&cur_diff_bm,cur_router,prev_as_number)){
                            struct route_data data2;
                            data2.prefix = cur_diff_bm.adv_route.prefix;
                            data2.prefixlen = cur_diff_bm.adv_route.prefix_len;
                            data2.as_number = cur_diff_bm.as_path;
                            //add parameter
                            data2.weight = 0;
                            data2.local_pref = cur_diff_bm.lp;

                            //bitset_radix_tree_add_w_pref(cur_router->bgp_diff.loc_rib, data2, cur_diff_bm.adv_route.prefix_len,cur_diff_bm.adv_route.prefix);
                            pair<bool, bool> is_this_diffroute_first = add_bgp_table(cur_router,&cur_bm,&cur_diff_bm, &pq);

                            cur_diff_bm.as_path.push_back(cur_router->bgp.as_number);

                            //to_do: maybe
                            /*
                            // cout << "diff_AS: ";
                            print_as_numbers(cur_diff_bm);
                            */

                            if(is_this_diffroute_first.first == false && is_this_diffroute_first.second == false){
                                continue;
                            }

                            if(cur_diff_bm.terminated == false){
                                for(auto itr = cur_router->bgp_diff.e_peer_relation.begin();itr!=cur_router->bgp_diff.e_peer_relation.end();itr++){
                                    if(itr->first == prev_as_number){
                                        continue;
                                    }

                                    /*
                                    if(cur_router->bgp_diff.received_peers[itr->second->bgp_diff.as_number] == true){
                                        continue;
                                    }*/

                                    cur_router->bgp_diff.adj_rib_out[itr->first].push_back(cur_diff_bm.adv_route);
                                    
                                    reset_bgp_parameters(&cur_bm, &cur_diff_bm);

                                    if(apply_diff_export_filter(&cur_diff_bm, cur_router, itr->first)){
                                        //to do: dropの処理, terminated
                                        /*
                                        bool dif = check_diff_route_preference(itr->second->name,cur_diff_bm);
                                        
                                        if(dif == false){
                                            continue;
                                        }
                                        else{
                                            diff_traverse_history[itr->second->name] = cur_diff_bm;
                                        }
                                        */

                                        tuple<bgp_message, bgp_message, struct router *> next_tuple{cur_bm,cur_diff_bm,itr->second};
            
                                        if(cur_diff_bm.terminated == false){
                                            cur_router->bgp_diff.advertised_peers.push_back(itr->second->bgp_diff.as_number);
                                            itr->second->bgp_diff.received_peers[cur_router->bgp_diff.as_number] = true;

                                            pq.push(next_tuple);
                                        }
                                    }
                                }
                            }
                        }                        
                    }
                }
            }
        }
    }
}



void check_side_effect(vector<router> *vec_router, bitset<32> addr)
{
    // cout << "start to check side-effect !^^!" << endl;

    for(int i=0;i<vec_router->size();i++){
        if((*vec_router)[i].bgp_diff.loc_rib_table[addr].empty()){
            //// cout << "No elements, No change happen." << endl;

            continue;
        }

        list<struct bgp_table_elms> *table = &(*vec_router)[i].bgp.loc_rib_table[addr];
        list<struct bgp_table_elms> *tmp_diff_table = &(*vec_router)[i].bgp_diff.loc_rib_table[addr];

        list<struct bgp_table_elms> diff_table = (*vec_router)[i].bgp_diff.loc_rib_table[addr];

        //diff_tableにin_out == 2だったら入れない
        //// cout << "------------Test--------------" << endl;
        /*
        for(auto itr=table->begin();itr!=table->end();itr++){
            if(itr->in_out == 2){
                //// cout << "in_out is 2, so skip" << endl;
                continue;
            }
            add_elm_to_diff_table(&diff_table, *itr);
        }  
        */

        //// cout << "diff_table_added" << endl;

        /*
        for(auto itr=tmp_diff_table->begin();itr!=tmp_diff_table->end();itr++){
            add_elm_to_diff_table(&diff_table, *itr);
        }*/

        //// cout << "------------Test--------------" << endl;
        
        //tableの先頭と同じidを持つdiff_tableの要素が存在するかチェック
        struct bgp_table_elms cur_first = table->front();
        struct bgp_table_elms diff_first = diff_table.front();

        //if(cur_first.id == diff_first.id){
        if(is_path_equal(cur_first.as_path, diff_first.as_path)){
            //// cout << "Same preffered route, no change happen at router " << (*vec_router)[i].name << endl;

            /*
            // cout << "cur_path ";
            show_as_path(cur_first);

            // cout << "diff_path: ";
            show_as_path(diff_first);
            */

            continue;
        }

        // cout << "Maybe side-effect at router " << (*vec_router)[i].name << endl;
        //// cout << "cur_path (id " << cur_first.id << ") ";
        //show_as_path(cur_first);
        //// cout << "diff_path (id " << diff_first.id << ") ";
        //show_as_path(diff_first);

        bool flag = false;

        side_effect_count++;


        if(cur_first.id == 0){
            //dif_firstのidを探しに行く
        
            auto itr=table->begin();
            for(;itr!=table->end();itr++){
                if(itr->id != diff_first.id){
                    continue;
                }
                
                if(is_in_path_specs(diff_first) == true){
                    break;
                }

                if(itr->in_out != 2){
                    // cout << "-------------------------------" << endl;
                    // cout << "We have side-effect in repair." << endl;
                    // cout << "Type is route preference change." << endl;
                    // cout << "The past first priority path: ";
                    show_as_path_only(cur_first);
                    // cout << "The current first priority path: ";
                    show_as_path_only(diff_first);
                    // cout << "The preference difference comes from... " << endl;
                    show_preference_difference_w_same_id(*itr, diff_first);
                    // cout << "-------------------------------" << endl;
                    break;
                }
                else{
                    // cout << "-------------------------------" << endl;
                    // cout << "We have side-effect in repair." << endl;
                    // cout << "Type is route preference change." << endl;
                    // cout << "The past first priority path: ";
                    show_as_path_only(cur_first);
                    // cout << "The current first priority path: ";
                    show_as_path_only(diff_first);
                    // cout << "The preference difference comes from.." << endl;
                    show_preference_difference(cur_first, diff_first);
                    //show_preference
                    // cout << "The current path is newly emerged after repair. " << endl;
                    show_no_route_reachable_difference(*itr, diff_first);
                    // cout << "-------------------------------" << endl;
                    break;
                }
                //return;
            }

            if(itr == table->end()){
                // cout << "-------------------------------" << endl;
                // cout << "We have side-effect in repair." << endl;
                // cout << "Type is route preference change." << endl;
                // cout << "The first priority path with different id is ";
                show_as_path_only(cur_first);
                //// cout << "The first priority path is None (i.e., no route reachable)." << endl;
                // cout << "The current first priority path: ";
                show_as_path_only(diff_first);
                // cout << "The preference difference comes from... " << endl;
                show_preference_difference(cur_first, diff_first);
                // cout << "-------------------------------" << endl;
            }

            //side_effect_count++;

            continue;
            //return;
        }

        for(auto itr=diff_table.begin();itr!=diff_table.end();itr++){
            // cout << "diff_table_iteration." << endl;

            if(cur_first.id != itr->id){
                continue;
            }
            
            //show_as_path(cur_first);
            //show_as_path(*itr);

            //同じidかつpathが一緒なら優先度問題、diff_tableの同じidの要素とtopの要素のpreferenceの取得
            if(is_bte_path_same(cur_first,*itr)){
                //diff_firstのパスが副作用リストに存在するかチェック
                if(is_in_path_specs(diff_first) == true){
                    break;
                }
                //存在しない場合は、
                //side-effect happen
                // cout << "-------------------------------" << endl;
                // cout << "We have side-effect in repair." << endl;
                // cout << "Type is route preference change." << endl;
                // cout << "The current first priority path: ";
                show_as_path_only(diff_first);
                // cout << "The preference difference comes from... " << endl;
                show_preference_difference(*itr, diff_first);
                // cout << "-------------------------------" << endl;

                flag = true;
                //side_effect_count++;
                break;
            }
            else if(itr->in_out == 1){
                //pathは異なるけど、inは一緒、つまりpathひんまげ

                //current fist priority pathをshow
                //どこで曲ったかを説明

                flag = true;
                // side_effect_count++;
                break;
            }
            else if(itr->in_out == 2){
             //同じidかつin_outがdelならdrop問題、top要素の出力とどこでdropしたかを出力
                // cout << "-------------------------------" << endl;
                // cout << "We have side-effect in repair." << endl;
                // cout << "Type is advertisement drop." << endl;
                // cout << "The first priority path: ";
                show_as_path_only(diff_first);
                // cout << "The old preferred path is dropped at ";
                show_as_path_only(*itr);
                //to_do: filter以外
                // cout << "The config line is route_map at ";
                // cout << itr->as_path[itr->as_path.size()-1] << endl;
                //dropしたconfigの出力
                // cout << "-------------------------------" << endl;

                flag = true;
                //side_effect_count++;
                break;
            }
            else if(itr->in_out == 3){
             //同じidかつpathが違うならパスチェンジング問題、changeしたconfigとtopの要素のパスの取得
             // cout << "-------------------------------" << endl;
                // cout << "We have side-effect in repair." << endl;
                // cout << "Type is path changing by route_map." << endl;
                // cout << "The first priority path: ";
                show_as_path_only(diff_first);
                // cout << "The old preferred path is dropped at ";
                //show_as_path_only(*itr);
                //to_do: filter以外
                //// cout << "The config line is route_map at ";
                //// cout << itr->as_path[itr->as_path.size()-1] << endl;
                //dropしたconfigの出力
                // cout << "-------------------------------" << endl;

                flag = true;
                //side_effect_count++;
                break;
            }
            else{
                // cout << "Error: invalid in_out value: " << itr->in_out << endl;
                break;
            }
        }

        if(flag == false){
            // cout << "No elements, No change happen." << endl;
        }
    }
}


void clear_advertised_peers(vector<router> *vec_router)
{
    for(int i=0;i<vec_router->size();i++){
        (*vec_router)[i].bgp.advertised_peers.clear();
        (*vec_router)[i].bgp_diff.advertised_peers.clear();
    }
}

void clear_received_peers(vector<router> *vec_router)
{
    for(int i=0;i<vec_router->size();i++){
        (*vec_router)[i].bgp.received_peers.clear();
        (*vec_router)[i].bgp_diff.received_peers.clear();
    }
}

void run_bgp_simulation_per_route(vector<router> *vec_router)
{
    for(int i=0;i<vec_router->size();i++){
        if((*vec_router)[i].bgp.advertised_v4_route.size() == 0){
            continue;
        }

        clear_advertised_peers(vec_router);
        clear_received_peers(vec_router);

        // cout << endl;
        // cout << "--------------------------------------------------" <<endl;
        // cout << "router " << (*vec_router)[i].name << " starts to announce ip prefix" << endl;
        // cout << endl;
        //route_propagation_dfs(&(*vec_router)[i]); 
        route_propagation_generalized_dijkstra(&(*vec_router)[i]);

        check_side_effect(vec_router,(*vec_router)[i].bgp.advertised_v4_route[0].second[0].prefix);

        // cout << "side-effect count: " << side_effect_count << endl;

        check_route_num(vec_router, (*vec_router)[i].name, test_0402_addr,i);
        
    }

     //test_03_19
    // // cout << "start diff-based simulation." << endl;
     //test_03_19
    //unordered_map<string, vector<unsigned int>> history;
    // run_diff_bgp_simulation_per_route(vec_router, history);

     //diff-check
     //check_side_effect(vec_router);
}

void clear_spec_to_cchange_to_se_info()
{
    unordered_map<string, vector<struct config_change>> tmp_spec_to_cchange_to_se_info;
    spec_to_config_changes = tmp_spec_to_cchange_to_se_info;

    unordered_map<string, vector<struct side_effect_info>> tmp_spec_to_side_effect_info;
    cchange_to_se_info = tmp_spec_to_side_effect_info;
}

void print_spec(struct specification spec)
{
    string ip_prefix = spec.route.prefix.to_string()+"/"+to_string(spec.route.prefix_len);
    string path = get_string_path_str(spec.forwarding_path);
    //string path = spec.forwarding_path;
    string string_spec = ip_prefix+":"+path;
    //string dev_path = ", " + get_string_dev_path(spec.forwarding_path);
    //string string_spec = path;

    ofstream outputfile("spec_list.txt", std::ios::app);
    outputfile<< path << endl;
    //outputfile<< dev_path << endl;
}

void reset_for_ospf()
{
    //show spec list & reset
    ofstream outputfile("spec_list.txt", std::ios::app);
    
    if(spec_list.size() > 0){
        outputfile<< cur_prefix << endl;
    }
    
    for(int i=0;i<spec_list.size();i++){
        print_spec(spec_list[i]);
    }
    outputfile.close();
    
    vector<struct specification> tmp_sl;
    spec_list.clear();
    
    //spec to config changes
    spec_to_config_changes.clear(); 

    //cchange_to_se_info
    cchange_to_se_info.clear();

    //vec_path_group
    vec_path_group.clear();
}

void reset_for_bgp_per_prefix_adv()
{
    //show spec list & reset
    ofstream outputfile("spec_list.txt", std::ios::app);
    
    if(spec_list.size() > 0){
        outputfile<< cur_prefix << endl;
    }
    
    for(int i=0;i<spec_list.size();i++){
        print_spec(spec_list[i]);
    }
    outputfile.close();
    
    vector<struct specification> tmp_sl;
    spec_list.clear();
    
    //spec to config changes
    spec_to_config_changes.clear(); 

    //cchange_to_se_info
    cchange_to_se_info.clear();

    //vec_path_group
    vec_path_group.clear();

    se_topo.clear();
}

void create_differential_graph_per_route(vector<router> *vec_router)
{
    for(int i=0;i<vec_router->size();i++){
        if((*vec_router)[i].bgp.advertised_v4_route.size() == 0){
            continue;
        }

        clear_advertised_peers(vec_router);
        clear_received_peers(vec_router);
        //clear_spec_to_cchange_to_se_info();

        // cout << endl;
        // cout << "--------------------------------------------------" <<endl;
        // cout << "router " << (*vec_router)[i].name << " starts to announce ip prefix" << endl;
        // cout << endl;


        //diff_route_propagation_generalized_dijkstra(&(*vec_router)[i]);

        bool t = new_diff_route_propagation_generalized_dijkstra(&(*vec_router)[i]);

        if(t == true){
            // cout << "!!!!!!!!!!!" << endl;
            //create_side_effect_path_group_per_prefix();
            new_create_side_effect_path_group_per_prefix();
            // cout << "!!!!!!!!!!!" << endl;
        }

        
        show_spec_to_se();

        spec_analy_and_show();

        //reset for bgp ?
        // cout << "do reset !" << endl;
        reset_for_bgp_per_prefix_adv();
        
        //for test
        if(i == 0){
            //break;
        }
        

        //check_side_effect(vec_router,(*vec_router)[i].bgp.advertised_v4_route[0].second[0].prefix);

        //// cout << "side-effect count: " << side_effect_count << endl;

        //check_route_num(vec_router, (*vec_router)[i].name, test_0402_addr,i);
    }
}

void create_ospf_differential_graph_per_route(vector<router> *vec_router)
{
    for(int i=0;i<vec_router->size();i++){
        if((*vec_router)[i].bgp.advertised_v4_route.size() == 0){
            continue;
        }

        //cout << "loop idx: " << i << endl;

        clear_advertised_peers(vec_router);
        clear_received_peers(vec_router);
        //clear_spec_to_cchange_to_se_info();

        // cout << endl;
        // cout << "--------------------------------------------------" <<endl;
        // cout << "router " << (*vec_router)[i].name << " starts to announce ip prefix" << endl;
        // cout << endl;


        //diff_route_propagation_generalized_dijkstra(&(*vec_router)[i]);

        bool t = ospf_diff_route_propagation_generalized_dijkstra(&(*vec_router)[i]);

        //ospf_change

        /*
        if(t == true){
            // cout << "!!!!!!!!!!!" << endl;
            //create_side_effect_path_group_per_prefix();
            new_create_ospf_side_effect_path_group_per_prefix();
            // cout << "!!!!!!!!!!!" << endl;
        }

        show_spec_to_se();

        spec_analy_and_show();

        //reset for bgp ?
        //// cout << "do reset !" << endl;
        reset_for_bgp_per_prefix_adv();
        */

        //show_spec_to_se();
        
        //for test
        if(i == 0){
           //break;
        }
        

        //check_side_effect(vec_router,(*vec_router)[i].bgp.advertised_v4_route[0].second[0].prefix);

        //// cout << "side-effect count: " << side_effect_count << endl;

        //check_route_num(vec_router, (*vec_router)[i].name, test_0402_addr,i);
    }

    new_create_ospf_side_effect_path_group_per_prefix();
    show_spec_to_se();
    spec_analy_and_show();
    //reset_for_bgp_per_prefix_adv();
}


/*
void route_propagation_diff_dfs(struct router *router)
{
    unordered_map<string, bool> is_visited;
}

void run_diff_bgp_simulation_per_route(vector<router> *vec_router)
{
    for(int i=0;i<vec_router->size();i++){
        if((*vec_router)[i].bgp.advertised_v4_route_diff.size() == 0){
            continue;
        }

        route_propagation_diff_dfs(&(*vec_router)[i]);
    }
}
*/

void run_differential_graph_simulation(vector<router> *vec_router)
{
    create_differential_graph_per_route(vec_router);

   run_bgp_decision_process(vec_router);
}

void run_ospf_differential_graph_simulation(vector<router> *vec_router)
{
    create_ospf_differential_graph_per_route(vec_router);
    //run_bgp_decision_process(vec_router);
}

void run_simulation(vector<router> *vec_router)
{
   run_bgp_simulation_per_route(vec_router);

   run_bgp_decision_process(vec_router);
}

void free_bgp()
{
    for(auto itr=as_number_to_router.begin();itr!=as_number_to_router.end();itr++){
        for(int i=0;i<itr->second.size();i++){
            free(itr->second[i]->bgp.loc_rib);
            free(itr->second[i]->bgp.loc_rib_diff);
        }
    }
}

void free_static()
{
    as_number_to_router.clear();
    name_to_router.clear();
    topology.clear();
    se_topo.clear();
    zoo_topo.clear();
    traverse_history.clear();
    diff_traverse_history.clear();
    all_pair_reachability.clear();
    vec_specs.clear();
    is_spec_satisfied.clear();
    cur_prefix.clear();
    spec_to_config_changes.clear();
    cchange_to_se_info.clear();
    vec_path_group.clear();
    priority_queue<dPair, vector<dPair>, compare_bgp_message> dpq;
    dif_pq = dpq;
    priority_queue<dPair, vector<dPair>, compare_ospf_message> tmp_dif_ospf_pq;
    dif_ospf_pq = tmp_dif_ospf_pq;
    sec_vec.clear();
    spec_list.clear();
    side_effect_cnt = 0;
    total_number_of_compressed_path.clear();
    total_number_of_uncompressed_path.clear();
    compressed_rate.clear();
    msg_id = 0;
    side_effect_count = 0;
    new_se_count = 0;
    new_per_prefix_se_counter = 0;
    new_per_prefix_se_counter_vec.clear();
    se_count = 0;
    per_prefix_se_counter = 0;
    per_prefix_se_counter_vec.clear();
    dev_to_ospf_config.clear();
}

void set_advertised_route(vector<router> *vec_router)
{
    pair<unsigned int, vector<v4_route>> adv_route;

    struct v4_route r;
    r.prefix = std::bitset<32>(get_string_to_uint_ipv4_addr("1.0.0.0/16"));
    r.prefix_len = 16;

    adv_route.first = 100;
    adv_route.second.push_back(r);

    (*vec_router)[2].bgp.advertised_v4_route.push_back(adv_route);

    pair<unsigned int, vector<v4_route>> adv_route2;

    struct v4_route r1;
    r1.prefix = std::bitset<32>(get_string_to_uint_ipv4_addr("1.0.0.0/16"));
    r1.prefix_len = 16;

    adv_route2.first = 300;
    adv_route2.second.push_back(r1);

    (*vec_router)[2].bgp.advertised_v4_route.push_back(adv_route2);
}

void read_config_one_as_per_one_router(vector<router> *vec_router)
{
    unsigned int assigned_asn = 1;

    // cout << "Assign an AS number for each router" << endl;

    for(int i=0;i<vec_router->size();i++){
        /*
        (*vec_router)[i].bgp.as_number = assigned_asn;

        //setadvertised_prefix
        // cout << "router " << (*vec_router)[i].name << " is AS " << assigned_asn << endl;
        
        as_number_to_router[assigned_asn].push_back(&(*vec_router)[i]);

        assigned_asn++;
        */
        (*vec_router)[i].bgp.as_number = stoi((*vec_router)[i].name);

        //setadvertised_prefix
        // cout << "router " << (*vec_router)[i].name << " is AS " << stoi((*vec_router)[i].name) << endl;
        
        as_number_to_router[stoi((*vec_router)[i].name)].push_back(&(*vec_router)[i]);

        assigned_asn++;
    }
}

void new_read_config_one_as_per_one_router(vector<router> *vec_router)
{
    unsigned int assigned_asn = 100;

    // cout << "Assign an AS number for each router" << endl;

    for(int i=0;i<vec_router->size();i++){
        /*
        (*vec_router)[i].bgp.as_number = assigned_asn;

        //setadvertised_prefix
        // cout << "router " << (*vec_router)[i].name << " is AS " << assigned_asn << endl;
        
        as_number_to_router[assigned_asn].push_back(&(*vec_router)[i]);

        assigned_asn++;
        */
        (*vec_router)[i].bgp.as_number = assigned_asn;

        //setadvertised_prefix
        // cout << "router " << (*vec_router)[i].name << " is AS " << assigned_asn << endl;
        
        as_number_to_router[assigned_asn].push_back(&(*vec_router)[i]);

        assigned_asn++;
    }
}

static unsigned int first_addr = 1;
static unsigned int second_addr = 0;
void set_bgp_advertised_route(vector<router> *vec_router)
{
    for(int i=0;i<vec_router->size();i++){
        if(first_addr == 254){
            first_addr = 1;
            second_addr++;
        }

        string a = to_string(first_addr) + "." + to_string(second_addr) + ".0.0/24";

        struct v4_route r;
        r.prefix =  std::bitset<32>(get_string_to_uint_ipv4_addr(a));
        r.prefix_len = 24;

        pair<unsigned int, vector<v4_route>> adv_route;
        adv_route.first = (*vec_router)[i].bgp.as_number;
        adv_route.second.push_back(r);

        (*vec_router)[i].bgp.advertised_v4_route.push_back(adv_route);

        // cout << "router " << (*vec_router)[i].name << " (AS " << (*vec_router)[i].bgp.as_number << ") will announce ipv4 prefix " << a << endl;

        first_addr++;
    }
}

void set_ospf_advertised_route(vector<router> *vec_router)
{
    for(int i=0;i<vec_router->size();i++){
        string a = "100.0.0.0/24";

        struct v4_route r;
        r.prefix =  std::bitset<32>(get_string_to_uint_ipv4_addr(a));
        r.prefix_len = 24;

        pair<unsigned int, vector<v4_route>> adv_route;
        adv_route.first = (*vec_router)[i].bgp.as_number;
        adv_route.second.push_back(r);

        (*vec_router)[i].bgp.advertised_v4_route.push_back(adv_route);

        // cout << "router " << (*vec_router)[i].name << " (AS " << (*vec_router)[i].bgp.as_number << ") will announce ipv4 prefix " << a << endl;
    }
}

void set_diff_advertised_route(vector<router> *vec_router, int loop_para, int network_size)
{
    for(int i=0;i<loop_para;i++){
        std::mt19937 engine{ std::random_device{}() };
        std::uniform_int_distribution<int> dist(1, 254);

        string a,b,c,d;
        a = to_string(dist(engine));
        b = to_string(dist(engine));
        c = to_string(dist(engine));
        d = "0/24";

        string ad = a + "." + b + "." + c + "." + d;

        struct v4_route r;
        r.prefix =  std::bitset<32>(get_string_to_uint_ipv4_addr(ad));
        r.prefix_len = 24;

        std::mt19937 engine2{ std::random_device{}() };
        std::uniform_int_distribution<int> dist2(0, network_size-1);

        int device_idx = dist2(engine2);

        pair<unsigned int, vector<v4_route>> adv_route;
        adv_route.first = (*vec_router)[device_idx].bgp_diff.as_number;
        adv_route.second.push_back(r);

        (*vec_router)[device_idx].bgp_diff.advertised_v4_route.push_back(adv_route);

        // cout << "Diff: router " << (*vec_router)[device_idx].name << " (AS " << (*vec_router)[device_idx].bgp.as_number << ") will announce ipv4 prefix " << ad << endl;
    }
}

void set_config_diff(vector<router> *vec_router)
{
    for(int i=0;i<vec_router->size();i++){
        (*vec_router)[i].bgp_diff.loc_rib = new radix_tree;
        (*vec_router)[i].bgp_diff.as_number = (*vec_router)[i].bgp.as_number;
        (*vec_router)[i].bgp_diff.e_peer_relation = (*vec_router)[i].bgp.e_peer_relation;
        (*vec_router)[i].bgp_diff.i_peer_relation = (*vec_router)[i].bgp.i_peer_relation;
        (*vec_router)[i].bgp_diff.adj_rib_in = (*vec_router)[i].bgp.adj_rib_in;
        (*vec_router)[i].bgp_diff.adj_rib_out = (*vec_router)[i].bgp.adj_rib_out;
    }

    //int loop_para = 1000;
    //set_diff_advertised_route(vec_router, loop_para, vec_router->size());

    struct filter f;
    f.action = "set local_pref";
    f.action_value_numeric = 100;
    // cout << "add diff at router " << (*vec_router)[3].name << endl;
    (*vec_router)[3].bgp_diff.import_filter[2].push_back(f);
    //(*vec_router)[3].bgp_diff.import_filter[2].push_back(f);

    //ランダムに変更を加えるrouterを選ぶ
    //ランダムにその中のneighborを選ぶ
    //そのペアを記録して次回以降選ばないようにする
    //local prefを100→150にする
    //これを10→100回繰り返す
}

void set_diff_config_for_fmano25_lp_only(vector<router> *vec_router, unsigned int loop_para)
{
    for(int i=0;i<vec_router->size();i++){
        (*vec_router)[i].bgp_diff.loc_rib = new radix_tree;
        (*vec_router)[i].bgp_diff.as_number = (*vec_router)[i].bgp.as_number;
        (*vec_router)[i].bgp_diff.e_peer_relation = (*vec_router)[i].bgp.e_peer_relation;
        (*vec_router)[i].bgp_diff.i_peer_relation = (*vec_router)[i].bgp.i_peer_relation;
        (*vec_router)[i].bgp_diff.import_filter = (*vec_router)[i].bgp.import_filter;
        (*vec_router)[i].bgp_diff.import_filter = (*vec_router)[i].bgp.export_filter;

        (*vec_router)[i].virtual_bgp_diff.loc_rib = new radix_tree;
        (*vec_router)[i].virtual_bgp_diff.as_number = (*vec_router)[i].bgp.as_number;
        (*vec_router)[i].virtual_bgp_diff.e_peer_relation = (*vec_router)[i].bgp.e_peer_relation;
        (*vec_router)[i].virtual_bgp_diff.i_peer_relation = (*vec_router)[i].bgp.i_peer_relation;
        (*vec_router)[i].virtual_bgp_diff.import_filter = (*vec_router)[i].bgp.import_filter;
        (*vec_router)[i].virtual_bgp_diff.import_filter = (*vec_router)[i].bgp.export_filter;
    }

    //int loop_para = 1000;
    //set_diff_advertised_route(vec_router, loop_para, vec_router->size());

    /*
    struct filter f;
    f.action = "set local_pref";
    f.action_value_numeric = 100;
    // cout << "add diff at router " << (*vec_router)[1].name << endl;
    //router[1]はrouter 2, router[2]はrouter 0
    (*vec_router)[1].bgp_diff.import_filter[1].push_back(f);
    //(*vec_router)[3].bgp_diff.import_filter[2].push_back(f);
    */

   int loop_idx = 0;
   int tmp_loop_idx = 0;

   // cout << "-------------------------------------------------" << endl;
   while(loop_idx < loop_para){

        unordered_map<unsigned int, unordered_map<unsigned int, bool>> pair_record;
        
        //ランダムに変更を加えるrouterを選ぶ
        std::mt19937 engine{ std::random_device{}() };
        std::uniform_int_distribution<int> dist(0, vec_router->size()-1);
        int router_idx = dist(engine);

        //ランダムにその中のneighborを選ぶ
        vector<unsigned int> nei_vec;
        for(auto itr = (*vec_router)[router_idx].bgp.e_peer_relation.begin();itr!=(*vec_router)[router_idx].bgp.e_peer_relation.end();itr++){
            nei_vec.push_back(itr->first);
        }
        std::mt19937 engine2{ std::random_device{}() };
        std::uniform_int_distribution<int> dist2(0, nei_vec.size()-1);
        int neighbor_idx = dist2(engine2);
        
        if(pair_record[router_idx][neighbor_idx] == true){
            tmp_loop_idx++;
            continue;
        }

        //そのペアを記録して次回以降選ばないようにする
        pair_record[router_idx][neighbor_idx] = true;
        loop_idx++;

        //local prefを100→150にする
        //これを10→100回繰り返す
        struct filter f;
        f.action = "set local_pref";
        f.action_value_numeric = 150; //f.action_value_numeric = 100;
        (*vec_router)[router_idx].bgp_diff.import_filter[nei_vec[neighbor_idx]].push_back(f);
        //(*vec_router)[router_idx].bgp_diff.import_filter[nei_vec[neighbor_idx]].push_back(f);

        // cout << "diff configuration for import filter (set local_pref 150) is added at router " << (*vec_router)[router_idx].name << " (AS " << (*vec_router)[router_idx].bgp.as_number << ") from neighbor " << as_number_to_router[nei_vec[neighbor_idx]][0]->name << " (AS " << nei_vec[neighbor_idx] << ")" << endl;

        //string dummy_spec = "dummy"+(*vec_router)[router_idx].name;
        
        //create_dummy_specification(dummy_spec, &(*vec_router)[router_idx], f.action, to_string(f.action_value_numeric));
   }
   // cout << "-------------------------------------------------" << endl;
}

void set_diff_config_for_fmano25_route_map_only(vector<router> *vec_router, unsigned int loop_para)
{
    for(int i=0;i<vec_router->size();i++){
        (*vec_router)[i].bgp_diff.loc_rib = new radix_tree;
        (*vec_router)[i].bgp_diff.as_number = (*vec_router)[i].bgp.as_number;
        (*vec_router)[i].bgp_diff.e_peer_relation = (*vec_router)[i].bgp.e_peer_relation;
        (*vec_router)[i].bgp_diff.i_peer_relation = (*vec_router)[i].bgp.i_peer_relation;
        (*vec_router)[i].bgp_diff.import_filter = (*vec_router)[i].bgp.import_filter;
        (*vec_router)[i].bgp_diff.import_filter = (*vec_router)[i].bgp.export_filter;

        (*vec_router)[i].virtual_bgp_diff.loc_rib = new radix_tree;
        (*vec_router)[i].virtual_bgp_diff.as_number = (*vec_router)[i].bgp.as_number;
        (*vec_router)[i].virtual_bgp_diff.e_peer_relation = (*vec_router)[i].bgp.e_peer_relation;
        (*vec_router)[i].virtual_bgp_diff.i_peer_relation = (*vec_router)[i].bgp.i_peer_relation;
        (*vec_router)[i].virtual_bgp_diff.import_filter = (*vec_router)[i].bgp.import_filter;
        (*vec_router)[i].virtual_bgp_diff.import_filter = (*vec_router)[i].bgp.export_filter;
    }

    //int loop_para = 1000;
    //set_diff_advertised_route(vec_router, loop_para, vec_router->size());

    /*
    struct filter f;
    f.action = "set local_pref";
    f.action_value_numeric = 100;
    // cout << "add diff at router " << (*vec_router)[1].name << endl;
    //router[1]はrouter 2, router[2]はrouter 0
    (*vec_router)[1].bgp_diff.import_filter[1].push_back(f);
    //(*vec_router)[3].bgp_diff.import_filter[2].push_back(f);
    */

   int loop_idx = 0;
   int tmp_loop_idx = 0;

   // cout << "-------------------------------------------------" << endl;
   while(loop_idx < loop_para){

        unordered_map<unsigned int, unordered_map<unsigned int, bool>> pair_record;
        
        //ランダムに変更を加えるrouterを選ぶ
        std::mt19937 engine{ std::random_device{}() };
        std::uniform_int_distribution<int> dist(0, vec_router->size()-1);
        int router_idx = dist(engine);

        //ランダムにその中のneighborを選ぶ
        vector<unsigned int> nei_vec;
        for(auto itr = (*vec_router)[router_idx].bgp.e_peer_relation.begin();itr!=(*vec_router)[router_idx].bgp.e_peer_relation.end();itr++){
            nei_vec.push_back(itr->first);
        }

        std::mt19937 engine2{ std::random_device{}() };
        std::uniform_int_distribution<int> dist2(0, nei_vec.size()-1);
        int neighbor_idx = dist2(engine2);
        
        if(pair_record[router_idx][neighbor_idx] == true){
            tmp_loop_idx++;
            continue;
        }

        if((*vec_router)[router_idx].bgp_diff.import_filter[nei_vec[neighbor_idx]].size() != 0){
            continue;
        }

        //そのペアを記録して次回以降選ばないようにする
        pair_record[router_idx][neighbor_idx] = true;
        loop_idx++;

        //local prefを100→150にする
        //これを10→100回繰り返す
        struct filter f;
        f.action = "set route_map drop";
        //f.action_value_numeric = 50; //f.action_value_numeric = 100;
        //(*vec_router)[router_idx].bgp_diff.export_filter[nei_vec[neighbor_idx]].push_back(f);
        (*vec_router)[router_idx].bgp_diff.import_filter[nei_vec[neighbor_idx]].push_back(f);

        // cout << "diff configuration for route map is added at router " << (*vec_router)[router_idx].name << " from neighbor " << as_number_to_router[nei_vec[neighbor_idx]][0]->name << endl;

        string dummy_spec = "dummy"+(*vec_router)[router_idx].name;
        //create_dummy_specification(dummy_spec, &(*vec_router)[router_idx], f.action, "non");
   }
   // cout << "-------------------------------------------------" << endl;
}

void read_configuration(vector<router> *vec_router)
{
    //out << "we do not use now" << endl;
    //exit(-1);
    //read_config for one as per one router
    //read_config_one_as_per_one_router(vec_router);
    new_read_config_one_as_per_one_router(vec_router);

    //import/exportfilter

    //set_bgp peer
    set_bgp_peer(vec_router);

    //set advetised prefix
    set_bgp_advertised_route(vec_router);

    //set diffconfigz
    //set_config_diff(vec_router);
    //lp:5, 10, 15, 20
    //rmap: 5, 10, 15, 20
    //lp-rmap: 5, 10, 15, 20
    int lpara = 5;
    set_diff_config_for_fmano25_lp_only(vec_router, lpara);
    //set_diff_config_for_fmano25_route_map_only(vec_router, lpara);
}

void read_as_number_from_file(vector<router> *vec_router, string dir)
{
   vector<string> file_vec;

   //read file
   for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".cfg") {
            file_vec.push_back(entry.path().filename().string());
        }
    }

    for(int i=0;i<file_vec.size();i++){
        // cout << "----------------" << endl;
        string f_path = dir+file_vec[i];
        
        // cout << "read " << f_path << endl;

        int as_number = get_bgp_as_number(f_path);
        
        vector<struct RouteMapInfo> rmi = get_route_map_info(f_path);

        if(as_number == -1){
            // cout << "failed to read as number at " << f_path;
            exit(-1);
        }

        fs::path file_path(file_vec[i]);
        string dev_name = file_path.stem().string();
        struct router *r = name_to_router[dev_name];

        //(*vec_router)[i].bgp.as_number = assigned_asn;
        //setadvertised_prefix
        //// cout << "router " << (*vec_router)[i].name << " is AS " << assigned_asn << endl;
        //as_number_to_router[assigned_asn].push_back(&(*vec_router)[i]);
        
        r->bgp.as_number = as_number;
        // cout << "router " << r->name << " is AS " << r->bgp.as_number << endl;
        as_number_to_router[r->bgp.as_number].push_back(r);
        
        if(as_number_to_router[r->bgp.as_number].size() == 2){
            // cout << "multiple routers are the same AS" << endl;
            exit(-1);
        }

        if(rmi.size() > 0){
            // cout << "router " << r->name << " has route_map " << endl; 
            
            for(int rm_idx=0;rm_idx<rmi.size();rm_idx++){
                struct RouteMapInfo cur_rmi = rmi[rm_idx];

                // cout << "src: " << cur_rmi.src_node << " dst: " << cur_rmi.dst_node << " action: " << cur_rmi.action << endl;

                //(*vec_router)[router_idx].bgp_diff.import_filter[nei_vec[neighbor_idx]].push_back(f);
                struct filter f;
                f.action = "set route_map drop";

                struct router *dst_router = name_to_router[cur_rmi.dst_node];

                r->bgp.default_import_filter[dst_router->bgp.as_number].push_back(f);
                r->bgp_diff.default_import_filter[dst_router->bgp.as_number].push_back(f);
            }
        }
    }
    // cout << "----------------" << endl;
}

void read_zoo_configuration(vector<router> *vec_router, string dir)
{
    /*read_config for one as per one router*/
    //read_config_one_as_per_one_router(vec_router);
    //new_read_config_one_as_per_one_router(vec_router);
    read_as_number_from_file(vec_router, dir);
    //import/exportfilter

    //set_bgp peer
    set_bgp_peer(vec_router);

    //set advetised prefix
    set_bgp_advertised_route(vec_router);

    //set diffconfig
    //set_config_diff(vec_router);
    int lpara = 10;
    set_diff_config_for_fmano25_lp_only(vec_router, lpara);
    //set_diff_config_for_fmano25_route_map_only(vec_router, lpara);
}

void set_ospf_diff_cost(vector<router> *vec_router, int loop_para)
{
    //ospf_diffを作って、read_configの時にospf_diff = ospfをする
    for(int i=0;i<vec_router->size();i++){
        (*vec_router)[i].bgp_diff.loc_rib = new radix_tree;
        (*vec_router)[i].bgp_diff.as_number = (*vec_router)[i].bgp.as_number;
        (*vec_router)[i].bgp_diff.e_peer_relation = (*vec_router)[i].bgp.e_peer_relation;
        (*vec_router)[i].bgp_diff.i_peer_relation = (*vec_router)[i].bgp.i_peer_relation;
        (*vec_router)[i].bgp_diff.import_filter = (*vec_router)[i].bgp.import_filter;
        (*vec_router)[i].bgp_diff.import_filter = (*vec_router)[i].bgp.export_filter;

        //(*vec_router)[i].ospf_diff =  (*vec_router)[i].ospf;
    }


   int loop_idx = 0;
   int tmp_loop_idx = 0;

   // cout << "-------------------------------------------------" << endl;
   while(loop_idx < loop_para){
        unordered_map<unsigned int, unordered_map<unsigned int, bool>> pair_record;
            
        //ランダムに変更を加えるrouterを選ぶ
        std::mt19937 engine{ std::random_device{}() };
        std::uniform_int_distribution<int> dist(0, vec_router->size()-1);
        int router_idx = dist(engine);

        vector<string> nei_vec;
        for(int i=0;i<(*vec_router)[router_idx].ospf.peer_info.size();i++){
            string nei = (*vec_router)[router_idx].ospf.peer_info[i].peer_name;
            nei_vec.push_back(nei);
        }

        std::mt19937 engine2{ std::random_device{}() };
        std::uniform_int_distribution<int> dist2(0, nei_vec.size()-1);
        int neighbor_idx = dist2(engine2);

        if(pair_record[router_idx][neighbor_idx] == true){
            tmp_loop_idx++;
            continue;
        }

         //そのペアを記録して次回以降選ばないようにする
        pair_record[router_idx][neighbor_idx] = true;
        loop_idx++;

        //to be random
        unsigned int diff_cost = 10;
        struct router *nei_router = name_to_router[nei_vec[neighbor_idx]];

        (*vec_router)[router_idx].ospf_diff.diff_peer_cost_map[nei_vec[neighbor_idx]] = diff_cost;
        nei_router->ospf_diff.diff_peer_cost_map[(*vec_router)[router_idx].name] = diff_cost;
        //(*vec_router)[router_idx].bgp_diff.import_filter[nei_vec[neighbor_idx]].push_back(f);

        cout << "diff cost " <<  diff_cost << " is added between router " << (*vec_router)[router_idx].name << " (AS " << (*vec_router)[router_idx].bgp.as_number << ") and peer " << name_to_router[nei_vec[neighbor_idx]]->name << " (AS " << name_to_router[nei_vec[neighbor_idx]]->bgp.as_number << ")" << endl;
        // cout << "cost before repair is " << (*vec_router)[router_idx].ospf.peer_cost_map[nei_vec[neighbor_idx]] << endl;
   }

   // cout << "-------------------------------------------------" << endl;
}

void read_ospf_complementary_configuration(vector<router> *vec_router)
{
    //read_as_number
    new_read_config_one_as_per_one_router(vec_router);

    //advertised_route
    set_bgp_advertised_route(vec_router);
    //set_ospf_advertised_route(vec_router);
    
    //set_diff_config
    int num_of_change = 400;
    set_ospf_diff_cost(vec_router, num_of_change);
}

void show_num_of_reachability_pair()
{
    unsigned long long rp = 0;
    //ofstream outputfile("reach.txt");

    for(auto itr=all_pair_reachability.begin();itr!=all_pair_reachability.end();itr++){
        for(auto itr2=itr->second.begin();itr2!=itr->second.end();itr2++){
            if(itr2->second == false){
                continue;
            }
            //// cout << "pair< AS " << itr->first << ", AS " << itr2->first << ">" << endl;
            //string st = "pair< AS " + to_string(itr->first) + ", AS " + to_string(itr2->first) + ">";
            rp++;
            //outputfile <<st << endl;
        }
    }

    //outputfile.close();

    // cout << "total number of reachability pairs: " << rp << endl;
}

vector<router> set_configuration(vector<router> vec_router)
{
    /*
    for(int i=0;i<vec_router.size();i++){
        vec_router[i].bgp.as_number = i+1;
    }
    */

    vec_router[0].bgp.as_number = 100;
    vec_router[1].bgp.as_number = 100;
    vec_router[2].bgp.as_number = 200;
    vec_router[3].bgp.as_number = 300;

    //take parsed configs into variants each function

    //set advertised route
    set_advertised_route(&vec_router);

    //set import/outport filter
    set_import_filter(&vec_router);
    set_export_filter(&vec_router);

    set_bgp_peer(&vec_router);

    set_diff_config(&vec_router);
    //set_diff_config_for_fmano25(&vec_router);

    return vec_router;
}

void free_file(string path)
{
    ofstream ofs(path, std::ios::out | std::ios::trunc);

    if (!ofs.is_open()) {
        std::cerr << "Error: Could not open file " << path << std::endl;
        exit(-1);
    }

    ofs.close();
}

static bool isFirstCall = true;
void write_strings_to_file(string filename, string s1, string s2, string s3) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }
    if (!isFirstCall) {
        file << std::endl;
    }

    file << s1 << std::endl;
    file << s2 << std::endl;
    file << s3 << std::endl;

    isFirstCall = false;
}

static bool single_isFirstCall = true;
void write_single_string_to_file(string filename, string s1) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }
    if (!single_isFirstCall) {
        file << std::endl;
    }

    file << s1 << std::endl;
    single_isFirstCall = false;
}

double calculateAverage(vector<double> data) {
    if (data.empty()) {
        // cout << "data empty at calculate average" << endl;
        exit(-1);
        return 0.0;
    }

    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

static vector<double> stime_vec;
static vector<double> senum_vec;
static vector<double> senumpr_vec;
static vector<double> crate_vec;

int run_evaluation(string param_path, string rate_path, string nw_name, string ospf_flag)
{
    vector<router> vec_router;

    vector<string> topo_path_vec;
    //4
    topo_path_vec.push_back("arpanet_topo.txt");
    //20
    topo_path_vec.push_back("Oxford_topo.txt");
    //50
    topo_path_vec.push_back("surfnet_topo.txt");
    //74
    topo_path_vec.push_back("sinet_topo.txt");
    //158
    topo_path_vec.push_back("uscarrier_topo.txt");
    //754
    topo_path_vec.push_back("Kdl_topo.txt");

     //5
    topo_path_vec.push_back("arpanet_topo_new.txt");
    /*
     string topo_path = "arpanet_topo.txt";
      string topo_path = "sinet_topo.txt";
       string topo_path = "Oxford_topo.txt";
        string topo_path = "Kdl_topo.txt";
    */

   string arnes_topo = "./arnes/topo.txt";
   string conf_arnes = "./arnes/configs/";


   string colt_topo = "./colt/topo.txt";
   string conf_colt = "./colt/configs/";

    free_file("./spec_list.txt");
    //isFirstCall = true;
    //single_isFirstCall = true;

    auto start = chrono::system_clock::now();

    string topo_path;
    string conf_d;

    if(ospf_flag == "ospf"){
        topo_path = "./dna_networks/config2spec-networks-parsed/" + nw_name + "/topo.txt";
        conf_d = "./dna_networks/config2spec-networks-parsed/" + nw_name + "/configs/";
    }
    else{
        topo_path = "./" + nw_name + "/topo.txt";
        conf_d = "./" + nw_name + "/configs/";
    }

    /*
    if(nw_name == "colt"){
        topo_path = colt_topo;
        conf_d = conf_colt;
    }
    else if(nw_name == "arnes"){
        topo_path = arnes_topo;
        conf_d = conf_arnes;
    }
    */
    
    if(ospf_flag == "ospf"){
        read_ospf_file(conf_d);


        vec_router = read_network();

        set_name_to_router(&vec_router);   


        read_ospf_configuration();

        read_ospf_complementary_configuration(&vec_router);


        //ospf change
        run_ospf_differential_graph_simulation(&vec_router);

        //return 0;
    }
    else{
        read_topology(topo_path);


        vec_router = read_network();


        set_name_to_router(&vec_router);

        //// cout << "size: " << vec_router.size() << endl;

        if(nw_name == "kdl"){
            //vec_router = define_network();

            //vec_router = set_configuration(vec_router);
            read_configuration(&vec_router);
        }
        else{
            read_zoo_configuration(&vec_router, conf_d);
        }

        //read_test_specification();


        //run_differential_graph_simulation(&vec_router);
        run_differential_graph_simulation(&vec_router);
    }

    //ospf change
    //run_differential_graph_simulation(&vec_router);

    /*
    read_topology(topo_path);


    vec_router = read_network();


    set_name_to_router(&vec_router);

    //// cout << "size: " << vec_router.size() << endl;

    if(nw_name == "kdl"){
        //vec_router = define_network();

        //vec_router = set_configuration(vec_router);
        read_configuration(&vec_router);
    }
    else{
        read_zoo_configuration(&vec_router, conf_d);
    }

    //read_test_specification();


    run_differential_graph_simulation(&vec_router);
     */

    

    //conflict_check


    //show_spec_to_se();

    //for ospf
    //spec_analy_and_show();

    auto end2 = chrono::system_clock::now();
    auto dur2 = end2 - start;
    auto sec = chrono::duration_cast<chrono::milliseconds>(dur2).count();
    //// cout << "simulation time:  " << sec << endl;

    //// cout << "All pair reachability: " << all_pair_reachability.size() << endl;

    ofstream outputfile("measured_time.txt",std::ios::app);
    //outputfile<< sec << ", ";
    outputfile.close();

   //show_num_of_reachability_pair();
   
   // cout << "Total number of routers: " << vec_router.size() << endl;
   string simulation_time = "simulation time: " + to_string(sec);
   //// cout << "simulation time:  " << sec << endl;
   stime_vec.push_back(sec);

   string total_num_of_se = "total number of side effects: " + to_string(side_effect_cnt);
   //// cout << "Total number of side effect: " << side_effect_cnt << endl;
   senum_vec.push_back(side_effect_cnt);


   double t1 = (double) side_effect_cnt/vec_router.size();
   t1 = t1  * 10;
   double t2 = ceil(t1);
   double se_per_router  = t2/10.0;
   //// cout << "The number of side effect per router: " << se_per_router << endl;
   //// cout << "Path compression rate:" << endl;
    senumpr_vec.push_back(se_per_router);

   write_strings_to_file(param_path, simulation_time, total_num_of_se, to_string(se_per_router));


   vector<double> double_compressed_rate;

   //write_single_string_to_file(rate_path, "start");

   for(int i=0;i<compressed_rate.size();i++){
        double tmp1 = compressed_rate[i] * 10;
        double tmp2 = ceil(tmp1);
        double result  = tmp2/10.0;

        crate_vec.push_back(result);

        //auto res = std::fixed << std::setprecision(1) << result;
        stringstream res;
        res <<  std::fixed << std::setprecision(1) << result;
        string res_ss = res.str();

        //string rate = "rate " + to_string(i) + " : " + res_ss;
        string rate = res_ss;

        //// cout << "rate " << i << " : " << std::fixed << std::setprecision(1) << result << endl; 
        write_single_string_to_file(rate_path, rate);
   }
   //double d = se_count/vec_router.size();
   //// cout << " average number of side effects per an advertised prefix: " << d << endl;
   //// cout << "total number of side effects: " << se_count << endl;
   //// cout << "most big side effect num: " << side_effect_count << endl;
   //// cout << "SE count per prefix: " << print_per_prefix_se_num() << endl;
   //ofstream outputfile2("side_effect_count.txt",std::ios::app);
   //outputfile2 << side_effect_count << ", ";
  // outputfile2.close();

   //// cout << "test_count: " << test_count << endl;

    free_bgp();
    free_static();

    return 0;
}

int main(int argc, char* argv[])
{   
    string arnes_param_result = "./exp_result/arnes/parameter_change.txt";
    string arnes_compressed_rate_result = "./exp_result/arnes/compressed_rate.txt";

    string arnes_scale_sum =  "./exp_result/arnes/scale_sum.txt";
    string arnes_comp_rate_sum = "./exp_result/arnes/comp_rate_sum.txt";


    string colt_param_result = "./exp_result/colt/parameter_change.txt";
    string colt_compressed_rate_result = "./exp_result/colt/compressed_rate.txt";

    string colt_scale_sum =  "./exp_result/colt/scale_sum.txt";
    string colt_comp_rate_sum = "./exp_result/colt/comp_rate_sum.txt";

    string exp_result = "./exp_result/";
    string parameter_change = "parameter_change.txt";
    string compressed_rate = "compressed_rate.txt";
    string scale_sum = "scale_sum.txt";
    string comp_rate_sum = "comp_rate_sum.txt";

    //string target = "arnes";
    if(argc < 4){
        cout << "./main network_name param_name bgp-or-ospf" << endl;
        return 0;
    }

    string target_nw = argv[1];
    string a2 = argv[2];
    string ospf_flag = argv[3];
    string param =  "/" + a2 + "_";

    string run_eval1;
    string run_eval2;

    string f_scale_sum;
    string f_comp_rate_sum;

    if(ospf_flag == "ospf"){
        exp_result = "./ospf_exp_result/";
    }

    exp_result = exp_result + target_nw;
    run_eval1 = exp_result + param  + parameter_change;
    run_eval2 = exp_result  + param  + compressed_rate;
    f_scale_sum = exp_result + param  + scale_sum;
    f_comp_rate_sum = exp_result + param  + comp_rate_sum; 


    //各iterationにおけるデータを残す
    //別のファイルに、全iterationのresultを載せる
    int run_idx = 1;

    for(int i=0;i<run_idx;i++){
        run_evaluation(run_eval1, run_eval2, target_nw, ospf_flag);
    }

    return 0;
}