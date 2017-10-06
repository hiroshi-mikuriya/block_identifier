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
        void serialize(Archive& ar, cv::Rect & v, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("x", v.x);
            ar & boost::serialization::make_nvp("y", v.y);
            ar & boost::serialization::make_nvp("width", v.width);
            ar & boost::serialization::make_nvp("height", v.height);
        }
    }
}

