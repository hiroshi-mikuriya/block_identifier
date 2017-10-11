#pragma once

#include "option.h"

/*!
ブロック情報を送信する
@param[in] opt オプション
@param[in] blockInfo ブロック情報
@param[in] address 送信先
@param[in] port ポート番号
*/
void sendTcp(Option const & opt, std::vector<BlockInfo> const & blockInfo, std::string const & address, int port);
