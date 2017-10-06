#pragma once

#include <opencv2/opencv.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/nvp.hpp>

/*!
 色情報
 */
struct Color
{
    std::string name; ///< 色名
    cv::Vec3b bgr; ///< RGB値
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

namespace boost {
    namespace serialization {
        template <class Archive>
        void serialize(Archive& ar, cv::Vec3b & v, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_NVP(v[0]);
            ar & BOOST_SERIALIZATION_NVP(v[1]);
            ar & BOOST_SERIALIZATION_NVP(v[2]);
        }
        
        template <class Archive>
        void serialize(Archive& ar, Color & v, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_NVP(v.name);
            ar & BOOST_SERIALIZATION_NVP(v.bgr);
        }
        
        template <class Archive>
        void serialize(Archive& ar, cv::Rect & v, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_NVP(v.x);
            ar & BOOST_SERIALIZATION_NVP(v.y);
            ar & BOOST_SERIALIZATION_NVP(v.width);
            ar & BOOST_SERIALIZATION_NVP(v.height);
        }

        template <class Archive>
        void serialize(Archive& ar, BlockInfo & v, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_NVP(v.color);
            ar & BOOST_SERIALIZATION_NVP(v.rc);
            ar & BOOST_SERIALIZATION_NVP(v.color_area);
            ar & BOOST_SERIALIZATION_NVP(v.ave);
            ar & BOOST_SERIALIZATION_NVP(v.type);
        }
    }
}

