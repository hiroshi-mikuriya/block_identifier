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
        void serialize(Archive& ar, Instruction::Param & v, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("name", v.name);
            ar & boost::serialization::make_nvp("type", v.type);
        }
        
        template <class Archive>
        void serialize(Archive& ar, Instruction & v, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("name", v.name);
            ar & boost::serialization::make_nvp("params", v.params);
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
            ar & boost::serialization::make_nvp("instruction", v.insts);
            ar & boost::serialization::make_nvp("color-instruction-map", v.clr2inst);
            ar & boost::serialization::make_nvp("tuning", v.tune);
        }
    }
}

const std::string Instruction::Param::Int = "int";
const std::string Instruction::Param::Double = "double";
const std::string Instruction::Param::String = "string";
const std::string Instruction::Param::Bitmap = "bitmap";

Option getDefaultOption()
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
    opt.insts = {
        { "object-clear", {} }, // 何も表示しない状態にする
        { "object-fill", {} }, // 赤で塗りつぶし
        { "object-ripple", {} }, // ランダムな位置に波紋
        { "object-mario", {} }, // マリ◯を表示
        { "object-mario-run1", {} }, // 走っている状態のマリ◯1
        { "object-mario-run2", {} }, // 走っている状態のマリ◯2
        { "object-mario-run-anime", {} }, // 走るマ◯オ(アニメ)
        { "object-mario-jump-anime", {} }, // ジャンプマ◯オ(アニメ)
        { "object-mario-runandjump-anime", {} }, // 走ってジャンプするマ◯オ(アニメ)
        { "object-drop-mushroom", {} }, // なんか巨大化できそうなキノコが落ちてくる
        { "object-bitmap", { { "bitmap", Instruction::Param::Bitmap } } }, // bitmap
        { "object-cube", {} }, // 森さんコンテンツのCUBE。四角が外に広がり続ける
        { "object-sphere", {} }, // 森さんコンテンツのSPHERE。丸が外に広がり続ける
        { "object-skewed-cube", {} }, // 森さんコンテンツのSKEWED
        { "object-skewed-sphere", {} }, // 森さんコンテンツのSKEWED
        { "object-repbang", {} }, // 森さんコンテンツのRepBang。散って集まってまた散って
        { "object-fireworks", {} }, // 森さんコンテンツのfireworks。花火
        { "object-balls", {} }, // 森さんコンテンツのballs。ボール群がCubeの中
        { "filter-clear", {} }, // フィルターをリセットする
        { "filter-wave", {} }, // 奥行きに対して振幅するフィルター
        { "filter-flat-wave", {} }, // 奥行きに対して振幅するフィルター。２次元にまるめる
        { "filter-hsv", {} }, // 奥行きに対して色を変えるフィルター
        { "filter-skewed", {} }, // y軸を中心に回る
        { "ctrl-loop", { { "count", Instruction::Param::Int } } }, // 省略可能。省略時は3と同じ。指定した場合は、ctrl-loopで挟まれた命令を繰り返し実行
        { "ctrl-overlap", { { "time", Instruction::Param::Int } } }, // オブジェクト間を重ねる秒数.
        { "ctrl-inout-effect", {} }, // オブジェクトの切り替わり時のエフェクト。徐々にアルファチャネルの値を変える
    };
    opt.clr2inst = std::map<std::string, std::string>{
        { "red", "object-ripple" },
        { "green", "ctrl-loop" },
        { "white", "object-mario-runandjump-anime" },
        { "blue", "object-cube" },
        { "aqua", "object-repbang" },
        { "yellow", "object-fireworks" },
    };
    opt.tune = { 40, 245, 80, 1280, 720, 0.5, 102, 150 };
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
