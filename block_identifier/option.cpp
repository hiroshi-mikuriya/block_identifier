#include "option.h"
#include <boost/serialization/string.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>

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
        
        template <class Archive>
        void serialize(Archive& ar, Block & v, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("color", v.color);
            ar & boost::serialization::make_nvp("width", v.width);
        }
        
        template <class Archive>
        void serialize(Archive& ar, Tuning & v, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("stud_threshold", v.stud_th);
            ar & boost::serialization::make_nvp("size_threshold", v.size_th);
            ar & boost::serialization::make_nvp("bin_threshold", v.bin_th);
            ar & boost::serialization::make_nvp("camera_width", v.camera_width);
            ar & boost::serialization::make_nvp("camera_height", v.camera_height);
            ar & boost::serialization::make_nvp("camera_ratio", v.camera_ratio);
            ar & boost::serialization::make_nvp("block_height", v.block_height);
            ar & boost::serialization::make_nvp("block_width", v.block_width);
        }
        
        template <class Archive>
        void serialize(Archive& ar, Option & v, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("color", v.colors);
            ar & boost::serialization::make_nvp("tuning", v.tune);
        }
    }
}

bool operator<(Block const & lv, Block const & rv)
{
    return lv.color != rv.color ? lv.color < rv.color : lv.width < rv.width;
}

Block BlockInfo::to_block()const
{
    return { this->color.name, this->width };
}

Option getDefaultOption()
{
    Option opt;
    opt.colors = {
#include "default_colors.hpp"
    };
    /*
    int stud_th; ///< 最上段ブロックのぼっちを除去する閾値
    int size_th; ///< ブロック幅判定閾値
    int bin_th; ///< ２値化閾値
    int camera_width; ///< カメラ横ピクセル数
    int camera_height; ///< カメラ縦ピクセル数
    double camera_ratio; ///< カメラ画像縮尺率
    int block_height; ///< ブロック高さ（縮尺前）
    int block_width; ///< ブロック幅（2ぼっち分）（縮尺前）
    */
    // opt.tune = { 40, 230, 200, 1280, 720, 0.5, 102, 150 }; // Duplo
    opt.tune = { 20, 190, 200, 1280, 720, 1.0, 20, 16 }; // Lego
    return opt;
}

void writeOption(std::string const & path, Option const & opt)
{
    std::ofstream ofs(path);
    boost::archive::xml_oarchive oa(ofs);
    oa << boost::serialization::make_nvp("option", opt);
}

Option readOption(std::string const & path)
{
    Option opt;
    std::ifstream ifs(path);
    boost::archive::xml_iarchive ia(ifs);
    ia >> boost::serialization::make_nvp("option", opt);
    return opt;
}

