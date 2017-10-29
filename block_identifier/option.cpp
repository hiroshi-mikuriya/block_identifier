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
            ar & boost::serialization::make_nvp("instruction", v.insts);
            ar & boost::serialization::make_nvp("block-instruction-map", v.block2inst);
            ar & boost::serialization::make_nvp("tuning", v.tune);
        }
    }
}

const std::string Instruction::Param::Int = "int";
const std::string Instruction::Param::Double = "double";
const std::string Instruction::Param::String = "string";
const std::string Instruction::Param::Bitmap = "bitmap";

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
        { "red", { 0x80, 0x80, 0xFF } },
        { "green", { 0x80, 0xFF, 0x80 } },
        { "white", { 0xFC, 0xFC, 0xFC } },
        { "blue", { 0xFF, 0x80, 0x80 } },
        { "aqua", { 0xFF, 0xE0, 0xC0 } },
        { "yellow", { 0x80, 0xFF, 0xFF } },
    };
    opt.insts = {
        { /*ID*/ "object-clear", /*Param*/{} }, // 何も表示しない状態にする
        { /*ID*/ "object-fill", /*Param*/{} }, // 赤で塗りつぶし
        { /*ID*/ "object-ripple", /*Param*/{} }, // ランダムな位置に波紋
        { /*ID*/ "object-mario", /*Param*/{} }, // マリ◯を表示
        { /*ID*/ "object-mario-run1", /*Param*/{} }, // 走っている状態のマリ◯1
        { /*ID*/ "object-mario-run2", /*Param*/{} }, // 走っている状態のマリ◯2
        { /*ID*/ "object-mario-run-anime", /*Param*/{} }, // 走るマ◯オ(アニメ)
        { /*ID*/ "object-mario-jump-anime", /*Param*/{} }, // ジャンプマ◯オ(アニメ)
        { /*ID*/ "object-mario-runandjump-anime", /*Param*/{} }, // 走ってジャンプするマ◯オ(アニメ)
        { /*ID*/ "object-drop-mushroom", /*Param*/{} }, // なんか巨大化できそうなキノコが落ちてくる
        { /*ID*/ "object-bitmap", /*Param*/{ { "bitmap", Instruction::Param::Bitmap } } }, // bitmap
        { /*ID*/ "object-cube", /*Param*/{} }, // 森さんコンテンツのCUBE。四角が外に広がり続ける
        { /*ID*/ "object-sphere", /*Param*/{} }, // 森さんコンテンツのSPHERE。丸が外に広がり続ける
        { /*ID*/ "object-skewed-cube", /*Param*/{} }, // 森さんコンテンツのSKEWED
        { /*ID*/ "object-skewed-sphere", /*Param*/{} }, // 森さんコンテンツのSKEWED
        { /*ID*/ "object-repbang", /*Param*/{} }, // 森さんコンテンツのRepBang。散って集まってまた散って
        { /*ID*/ "object-fireworks", /*Param*/{} }, // 森さんコンテンツのfireworks。花火
        { /*ID*/ "object-balls", /*Param*/{} }, // 森さんコンテンツのballs。ボール群がCubeの中
        { /*ID*/ "filter-clear", /*Param*/{} }, // フィルターをリセットする
        { /*ID*/ "filter-wave", /*Param*/{} }, // 奥行きに対して振幅するフィルター
        { /*ID*/ "filter-flat-wave", /*Param*/{} }, // 奥行きに対して振幅するフィルター。２次元にまるめる
        { /*ID*/ "filter-hsv", /*Param*/{} }, // 奥行きに対して色を変えるフィルター
        { /*ID*/ "filter-skewed", /*Param*/{} }, // y軸を中心に回る
        { /*ID*/ "ctrl-loop", /*Param*/{ { "count", Instruction::Param::Int } } }, // 省略可能。省略時は3と同じ。指定した場合は、ctrl-loopで挟まれた命令を繰り返し実行
        { /*ID*/ "ctrl-overlap", /*Param*/{ { "time", Instruction::Param::Int } } }, // オブジェクト間を重ねる秒数.
        { /*ID*/ "ctrl-inout-effect", /*Param*/{} }, // オブジェクトの切り替わり時のエフェクト。徐々にアルファチャネルの値を変える
    };
    opt.block2inst = std::map<Block, std::string>{
        { { "white", 1}, "object-heart" },
        { { "yellow", 1}, "object-star" },
        { { "red", 1}, "object-fireworks" },
        { { "blue", 1}, "object-mario-run-anime" },
        { { "aqua", 1}, "filter-jump" },
        { { "green", 1}, "filter-jump" },
        { { "yellow", 2}, "filter-bk-snows" },
        { { "blue", 2}, "filter-bk-wave" },
        { { "green", 2}, "filter-bk-mountain" },
        { { "red", 2}, "filter-clear" },
        { { "yellow", 3}, "ctrl-loop" },
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
