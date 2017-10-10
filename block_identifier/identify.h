#pragma once

#include "informations.hpp"

/*!
 カメラ画像取得後のメイン処理
 デバッグの都合でメイン関数に書くのやめた
 @param[in] m カメラ画像 or デバッグ画像
 @param[in] colors 判定する色の情報
 */
void identifyBlock(cv::Mat const & m, std::vector<Color> const & colors);
