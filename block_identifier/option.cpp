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
        void serialize(Archive& ar, Instruction & v, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("name", v.name);
            ar & boost::serialization::make_nvp("param", v.param);
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
            ar & boost::serialization::make_nvp("block-instruction-map", v.block2inst);
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
        { "red", { 0x80, 0x80, 0xFF } },
        { "green", { 0x80, 0xFF, 0x80 } },
        { "white", { 0xFC, 0xFC, 0xFC } },
        { "blue", { 0xFF, 0x80, 0x80 } },
        { "aqua", { 0xFF, 0xE0, 0xC0 } },
        { "yellow", { 0x80, 0xFF, 0xFF } },
    };
    opt.block2inst = std::map<Block, Instruction>{
        { Block{ "white", 1}, Instruction{ "object-heart", { { "lifetime", 5 } } } },
        { Block{ "yellow", 1}, Instruction{ "object-star", { { "lifetime", 5 } } } },
        { Block{ "red", 1}, Instruction{ "object-fireworks", { { "lifetime", 5 } } } },
        { Block{ "blue", 1}, Instruction{ "object-mario-run-anime", { { "lifetime", 5 } } } },
        { Block{ "aqua", 1}, Instruction{ "filter-jump", { } } },
        { Block{ "green", 1}, Instruction{ "filter-jump", { } } },
        { Block{ "yellow", 2}, Instruction{ "filter-bk-snows", { } } },
        { Block{ "blue", 2}, Instruction{ "filter-bk-wave", { } } },
        { Block{ "green", 2}, Instruction{ "filter-bk-mountain", { } } },
        { Block{ "red", 2}, Instruction{ "filter-clear", { } } },
        { Block{ "yellow", 3}, Instruction{ "ctrl-loop", { { "count", 10 } } } },
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

