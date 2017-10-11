#pragma once

#include <opencv2/opencv.hpp>

/*!
 色情報
 */
struct Color
{
    std::string name; ///< 色名
    cv::Vec3b bgr; ///< RGB値
};

/*!
オプションファイルの情報
*/
struct Option
{
	std::vector<Color> colors; ///< 色情報
	std::map<std::string, std::string> clr2inst; ///< 色と命令のマップ
};

/*!
ブロック情報
*/
struct BlockInfo
{
	Color color; ///< ブロックの色
	cv::Rect rc; ///< ブロックの矩形
	cv::Rect color_area; ///< ブロック色判定領域
	cv::Vec3b ave; ///< 平均色
	int type; ///< 横幅: 1, 2, 3
};

/*!
@return オプションのデフォルト値
*/
Option getDefaultOption();

/*!
オプションをファイルに保存する
@param[in] path パス
@param[in] opt オプション
*/
void writeOption(std::string const & path, Option const & opt);

/*!
ファイルに保存したオプションを読み出す
@param[in] path パス
@return オプション
*/
Option readOption(std::string const & path);
