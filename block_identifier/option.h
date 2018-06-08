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

typedef std::map<std::string, double> Params;

/*!
 ブロック識別のチューニングパラメータ
 */
struct Tuning
{
    int stud_th; ///< 最上段ブロックのぼっちを除去する閾値
    int size_th; ///< ブロック幅判定閾値
    int bin_th; ///< ２値化閾値
    int camera_width; ///< カメラ横ピクセル数
    int camera_height; ///< カメラ縦ピクセル数
    double camera_ratio; ///< カメラ画像縮尺率
    int block_height; ///< ブロック高さ（縮尺前）
    int block_width; ///< ブロック幅（2ぼっち分）（縮尺前）
    int get_block_width() const { return static_cast<int>(block_width * camera_ratio); }
    int get_block_height() const { return static_cast<int>(block_height * camera_ratio); }
};

/*!
 ブロック型
 */
struct Block
{
    std::string color; ///< 色名
    int width; ///< ブロック幅
};

/*!
Block型の比較演算子<br>
std::mapのキーにするために作成
*/
bool operator<(Block const & lv, Block const & rv);

/*!
オプションファイルの情報
*/
struct Option
{
    std::vector<Color> colors; ///< 色情報
    Tuning tune; ///< ブロック識別のチューニングパラメータ
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
    int width; ///< 横幅: 1, 2, 3
    Block to_block()const; ///< Block型へ変換する
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
