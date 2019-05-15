// =====================================================================================
//
//       Filename:  type_detection.cpp
//
//        Version:  1.0
//        Created:  03/15/2018 03:22:37 PM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  Shoey CHI (Robotics), shoey.chi@iim.ltd
//        Company:  IIM
//
//    Description:  types of detection
//
// =====================================================================================

#include "type_detection.h"
#include <iterator>     // std::ostream_iterator

std::string  perception::detection::AuxiliaryNames[] = {"Spoof_none", "Spoof_ld", "Spoof_kp", "Spoof", "Smoker", "Gender","Emotion", "Age", "Glasses"};


std::string  perception::detection::detectorNames[] = {"rcnn_zf", "rcnn_vgg", "ssd", "ssd_gender", "mtcnn", "openpose", "mtcnn_tf", "mtcnn_tf_rk3399"};


perception::detection::DependAuxDetect::DependAuxDetect()
{
    mType = AUXILIARY_NR_ITEMS;
    mIndependent  = false;
}
perception::detection::DependAuxDetect::DependAuxDetect(AuxiliaryDetectorType t, bool b)
{
    mType = t;
    mIndependent = b;
}

perception::detection::AuxiliaryInfo::AuxiliaryInfo(const AuxiliaryInfo &info)
{
    label=info.label;
    auxiliaryScore = info.auxiliaryScore;
    auxiliaryType = info.auxiliaryType;
}

perception::detection::AuxiliaryInfo::AuxiliaryInfo(const std::string &label_, const float auxiliaryScore_, const AuxiliaryDetectorType auxiliaryType_)
{
    label=label_;
    auxiliaryScore=auxiliaryScore_;
    auxiliaryType=auxiliaryType_;
}


perception::detection::TrackInfo::TrackInfo()
{
    index = -1;
    qualified = false;
    hold = 0.0;
    recognized = 0;
    score = 1.0;
    user_status = detection::INIT;
    user_id = "";
    user_recorded = 0;
    user_image_url = "";
}

perception::detection::TrackInfo::TrackInfo(int idx_)
{
    index = idx_;
    qualified = true;
    hold = 0.0;
    recognized = 0;
    score = 1.0;
    user_status = detection::INIT;
    user_id = "";
    user_recorded = 0;
    user_image_url = "";
}

perception::detection::TrackInfo::TrackInfo(int idx_, bool qualified_)
{
    index = idx_;
    qualified = qualified_;
    hold = 0.0;
    recognized = 0;
    score = 1.0;
    user_status = detection::INIT;
    user_id = "";
    user_recorded = 0;
    user_image_url = "";
}

perception::detection::TrackInfo::TrackInfo(int idx_, double hold_,  cv::Mat facedata_, double stdr_, bool recognized_)
{
    index = idx_;
    qualified = true;
    hold = hold_;
    recognized = recognized_;
    score = stdr_;
    facedata = facedata_;
    user_status = detection::INIT;
    user_id = "";
    user_recorded = 0;
    user_image_url = "";
}

perception::detection::TrackInfo::TrackInfo(const TrackInfo& obj)
{
    index = obj.index;
    qualified = obj.qualified;
    hold = obj.hold;
    recognized = obj.recognized;
    score = obj.score;
    facedata = obj.facedata.clone();
    user_status = obj.user_status;
    user_id = obj.user_id;
    user_recorded = obj.user_recorded;
    user_image_url = obj.user_image_url;
    for(auto &info : obj.mDataAuxiliary)
    {
        mDataAuxiliary.emplace_back(info.label, info.auxiliaryScore,info.auxiliaryType);
    }
}

perception::detection::TrackInfo&
perception::detection::TrackInfo::operator =(int idx_)
{
    this->index = idx_;
    this->qualified = true;
    this->hold = 0.0;
    this->recognized = 0;
    this->score = 1.0;
    this->user_status = detection::INIT;
    this->user_id = "";
    this->user_recorded = 0;
    this->user_image_url = "";
    return *this;
}

std::ostream&
perception::detection::operator << (std::ostream& os,  TrackInfo const& obj)
{
    os << "[" << obj.index << "," << obj.qualified << "]" << std::endl;
    return os;
}


bool
perception::detection::TrackInfoVector::isQualified(int idx) const
{
    if (size() == 0 || idx >= size()) {
        return false;
    }
    return at(idx).qualified;
}

std::ostream&
perception::detection::operator << (std::ostream& os, TrackInfoVector const& obj)
{
    std::ostream_iterator<TrackInfo> out_it(os, "\n");
    std::copy(obj.begin(), obj.end(), out_it);
    return os;
}

std::vector<unsigned>
perception::detection::TrackInfoVector::getIndices() const
{
    size_t nTtl = size();
    std::vector<unsigned> indices;
    for (size_t i = 0; i < nTtl; ++i) {
        if (at(i).qualified) {
            indices.push_back(i);
        }
    }
    return indices;
}
