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
        { /*ID*/ "object-clear", /*Param*/{} }, // �����\�����Ȃ���Ԃɂ���
        { /*ID*/ "object-fill", /*Param*/{} }, // �Ԃœh��Ԃ�
        { /*ID*/ "object-ripple", /*Param*/{} }, // �����_���Ȉʒu�ɔg��
        { /*ID*/ "object-mario", /*Param*/{} }, // �}������\��
        { /*ID*/ "object-mario-run1", /*Param*/{} }, // �����Ă����Ԃ̃}����1
        { /*ID*/ "object-mario-run2", /*Param*/{} }, // �����Ă����Ԃ̃}����2
        { /*ID*/ "object-mario-run-anime", /*Param*/{} }, // ����}���I(�A�j��)
        { /*ID*/ "object-mario-jump-anime", /*Param*/{} }, // �W�����v�}���I(�A�j��)
        { /*ID*/ "object-mario-runandjump-anime", /*Param*/{} }, // �����ăW�����v����}���I(�A�j��)
        { /*ID*/ "object-drop-mushroom", /*Param*/{} }, // �Ȃ񂩋��剻�ł������ȃL�m�R�������Ă���
        { /*ID*/ "object-bitmap", /*Param*/{ { "bitmap", Instruction::Param::Bitmap } } }, // bitmap
        { /*ID*/ "object-cube", /*Param*/{} }, // �X����R���e���c��CUBE�B�l�p���O�ɍL���葱����
        { /*ID*/ "object-sphere", /*Param*/{} }, // �X����R���e���c��SPHERE�B�ۂ��O�ɍL���葱����
        { /*ID*/ "object-skewed-cube", /*Param*/{} }, // �X����R���e���c��SKEWED
        { /*ID*/ "object-skewed-sphere", /*Param*/{} }, // �X����R���e���c��SKEWED
        { /*ID*/ "object-repbang", /*Param*/{} }, // �X����R���e���c��RepBang�B�U���ďW�܂��Ă܂��U����
        { /*ID*/ "object-fireworks", /*Param*/{} }, // �X����R���e���c��fireworks�B�ԉ�
        { /*ID*/ "object-balls", /*Param*/{} }, // �X����R���e���c��balls�B�{�[���Q��Cube�̒�
        { /*ID*/ "filter-clear", /*Param*/{} }, // �t�B���^�[�����Z�b�g����
        { /*ID*/ "filter-wave", /*Param*/{} }, // ���s���ɑ΂��ĐU������t�B���^�[
        { /*ID*/ "filter-flat-wave", /*Param*/{} }, // ���s���ɑ΂��ĐU������t�B���^�[�B�Q�����ɂ܂�߂�
        { /*ID*/ "filter-hsv", /*Param*/{} }, // ���s���ɑ΂��ĐF��ς���t�B���^�[
        { /*ID*/ "filter-skewed", /*Param*/{} }, // y���𒆐S�ɉ��
        { /*ID*/ "ctrl-loop", /*Param*/{ { "count", Instruction::Param::Int } } }, // �ȗ��\�B�ȗ�����3�Ɠ����B�w�肵���ꍇ�́Actrl-loop�ŋ��܂ꂽ���߂��J��Ԃ����s
        { /*ID*/ "ctrl-overlap", /*Param*/{ { "time", Instruction::Param::Int } } }, // �I�u�W�F�N�g�Ԃ��d�˂�b��.
        { /*ID*/ "ctrl-inout-effect", /*Param*/{} }, // �I�u�W�F�N�g�̐؂�ւ�莞�̃G�t�F�N�g�B���X�ɃA���t�@�`���l���̒l��ς���
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
