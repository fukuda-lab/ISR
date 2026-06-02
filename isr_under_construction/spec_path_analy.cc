#include <iostream>
#include <vector>
#include <string>
#include <utility> // std::pairのため

#include "./include/spec_path_analy.h"




/**
 * @brief target_pathがprefix_pathで始まるか（プレフィックスかどうか）を判定します。
 * * @param prefix_path プレフィックス候補のパス
 * @param target_path 判定対象のパス
 * @return bool target_pathがprefix_pathをプレフィックスとして持つ場合はtrue、そうでない場合はfalse
 */
bool is_prefix(std::vector<std::string>& prefix_path, std::vector<std::string>& target_path) {
    // プレフィックス候補の方が長ければ、プレフィックスにはなり得ない
    if (prefix_path.size() > target_path.size()) {
        return false;
    }

    // 先頭から要素を比較し、一つでも異なればプレフィックスではない
    for (size_t i = 0; i < prefix_path.size(); ++i) {
        if (prefix_path[i] != target_path[i]) {
            return false;
        }
    }

    // 全ての要素が一致したため、プレフィックスである
    return true;
}

/**
 * @brief パス仕様のベクターを受け取り、矛盾するパスのペアを検出します。
 * * @param vec_path_spec 検査対象のパス仕様のベクター
 * @return std::vector<std::pair<specification, specification>> 矛盾するパス仕様のペアのベクター
 */
std::vector<std::pair<specification, specification>> find_conflicts(
    std::vector<specification> vec_path_spec) {
    
    std::vector<specification> blocked_paths;
    std::vector<specification> permitted_paths;
    std::vector<std::pair<specification, specification>> conflict_path;

    // 1. パス仕様を block と permit のリストに分類する
    for (const auto& spec : vec_path_spec) {
        if (spec.block_or_permit) { // true == permit
            permitted_paths.push_back(spec);
        } else { // false == block
            blocked_paths.push_back(spec);
        }
    }

    // 2. 各ブロックパスが、いずれかの許可パスのプレフィックスになっていないか調べる
    for (auto& blocked_spec : blocked_paths) {
        for (auto& permitted_spec : permitted_paths) {
            // 3. プレフィックス関係にあるか判定する
            if (is_prefix(blocked_spec.forwarding_path, permitted_spec.forwarding_path)) {
                // 4. 矛盾が見つかったら結果に追加する
                conflict_path.push_back({blocked_spec, permitted_spec});

                cout << "conflict happens !" << endl;
            }
        }
    }

    return conflict_path;
}