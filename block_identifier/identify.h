#pragma once

#include "option.h"

/*!
カメラ画像取得後のメイン処理
デバッグの都合でメイン関数に書くのやめた
@param[in] image カメラ画像 or デバッグ画像
@param[in] colors 判定する色の情報
@param[in] blockInfo 判定したブロック情報の書き込み先
*/
void identifyBlock(
	cv::Mat const & image,
	std::vector<Color> const & colors,
	std::vector<BlockInfo> & blockInfo);
