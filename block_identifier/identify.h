#pragma once

#include "option.h"

/*!
カメラ画像取得後のメイン処理
デバッグの都合でメイン関数に書くのやめた
@param[in] image カメラ画像 or デバッグ画像
@param[in] opt オプション
@param[in] blockInfo 判定したブロック情報の書き込み先
*/
void identifyBlock(
    cv::Mat const & image,
    Option const & opt,
    std::vector<BlockInfo> & blockInfo);
