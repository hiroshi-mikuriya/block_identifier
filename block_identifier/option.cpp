#pragma once

#include "option.h"
#include <boost/serialization/string.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
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
        void serialize(Archive& ar, Option & v, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("color", v.colors);
            ar & boost::serialization::make_nvp("instruction", v.clr2inst);
        }
    }
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
    opt.clr2inst = std::map<std::string, std::string>{
        { "red", "object-ripple" },
        { "green", "object-mario-run-anime" },
        { "white", "object-mario-runandjump-anime" },
        { "blue", "object-cube" },
        { "aqua", "object-repbang" },
        { "yellow", "object-fireworks" },
    };
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
