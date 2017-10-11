#pragma once

#include <opencv2/opencv.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

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
inline Option getDefaultOption()
{
	Option opt;
	opt.colors = {
		{ "red", { 0x80, 0x80, 0xFF } },
		{ "green", { 0x80, 0xFF, 0x80 } },
		{ "white", { 0xFC, 0xFC, 0xFC } },
		{ "blue", { 0xFF, 0x80, 0x80 } },
		{ "aqua", { 0xFF, 0xE0, 0xC0 } },
		{ "yellow", { 0x80, 0xFF, 0xFF } },
	};
	opt.clr2inst = std::map<std::string, std::string>{
		{ "red", "object-ripple" },
		{ "green", "object-mario-run-anime" },
		{ "white", "object-mario-runandjump-anime 	" },
		{ "blue", "object-cube" },
		{ "aqua", "object-repbang" },
		{ "yellow", "object-fireworks" },
	};
	return opt;
}

namespace boost {
    namespace serialization {
        template <class Archive>
        void serialize(Archive& ar, cv::Vec3b & v, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("b", v[0]);
            ar & boost::serialization::make_nvp("g", v[1]);
            ar & boost::serialization::make_nvp("r", v[2]);
        }
        
        template <class Archive>
        void serialize(Archive& ar, Color & v, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("name", v.name);
            ar & boost::serialization::make_nvp("bgr", v.bgr);
        }
    }
}

