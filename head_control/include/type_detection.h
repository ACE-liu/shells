/**
 *          @file  type_detection.h
 *
 *         @brief  type-definitions for detection
 *
 *       @version  1.0
 *          @date  10/26/2017 09:34:27 PM
 *        @author  Shoey CHI (Robotics), shoey.chi@iim.ltd
 *
 * @section Description
 *
 *       Revision:  none
 *       Compiler:  g++
 *        Company:  IIM
 *
 * @section Description
 *
 * -->describe more here<--
 *
 */

#ifndef _TYPE_DEFINITION_H_
#define _TYPE_DEFINITION_H_
#include <string>
#include <opencv2/core/core.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <unordered_map>

namespace perception
{

namespace nonfacial
{

enum NonfacialType {
    falldown
};

struct NonfacialResult {
    cv::Point pos;
    int state;
};

typedef std::vector<NonfacialResult> NonfacialVector;
typedef boost::shared_ptr<NonfacialVector> NonfacialVectorPtr;
typedef boost::shared_ptr<NonfacialVector const> NonfacialVectorConstPtr;

}

namespace detection
{

enum DetectorType {
    rcnn_zf = 0,
    rcnn_vgg,
    ssd,
    ssd_gender,
    mtcnn,
    openpose,
    mtcnn_tf,
    mtcnn_tf_rk3399,
    DETECTOR_NR_ITEMS
};


extern std::string detectorNames[];

enum AuxiliaryDetectorType {
    spoof_none = 0,
    spoof_ld,
    spoof_kp,
    spoof,
    smoker,
    gender,
    emotion,
    age,
    glasses,
    AUXILIARY_NR_ITEMS
};

enum TrackType {
    INIT = -1,
    STRANGER = 0,
    RECOGNIZER = 1,
    DESTROY = 2
};

struct DependAuxDetect
{
    DependAuxDetect();
    DependAuxDetect(AuxiliaryDetectorType t, bool b);
    AuxiliaryDetectorType mType;
    bool mIndependent;
};

extern std::string  AuxiliaryNames[];

typedef std::vector<cv::Rect> RectVector;
typedef boost::shared_ptr<RectVector> RectVectorPtr;
typedef boost::shared_ptr<RectVector const> RectVectorConstPtr;

typedef std::vector<RectVector> RectVectorVector;
typedef boost::shared_ptr<RectVectorVector> RectVectorVectorPtr;
typedef boost::shared_ptr<RectVectorVector const> RectVectorVectorConstPtr;

struct AuxiliaryInfo
{
    std::string label;
    float auxiliaryScore;
    AuxiliaryDetectorType auxiliaryType;
    AuxiliaryInfo(const std::string& label_, const float auxiliaryScore_,const AuxiliaryDetectorType auxiliaryType_);
    AuxiliaryInfo(const AuxiliaryInfo& info);
};

struct TrackInfo {
    int index;
    bool qualified;
    int recognized;
    double hold;
    double score;
    cv::Mat facedata;
    detection::TrackType user_status;
    std::string user_id;
    int user_recorded;
    std::string user_image_url;
    std::vector<AuxiliaryInfo> mDataAuxiliary;
    explicit TrackInfo();
    explicit TrackInfo(int idx_);
    explicit TrackInfo(int idx_, bool qualified_);
    explicit TrackInfo(int idx_, double hold_, cv::Mat facedata_, double stdr_, bool recognized_);
    TrackInfo(TrackInfo const& obj);
    TrackInfo& operator = (int idx_);
    friend std::ostream& operator << (std::ostream& os, TrackInfo const& obj);
};

std::ostream& operator << (std::ostream& os, TrackInfo const& obj);

class TrackInfoVector : public std::vector<TrackInfo, typename std::allocator<void>::template rebind<TrackInfo>::other>
{
public:
    bool isQualified(int idx) const;
    friend std::ostream& operator << (std::ostream& os, TrackInfoVector const& obj);
    std::vector<unsigned> getIndices() const;
};

std::ostream& operator << (std::ostream& os, TrackInfoVector const& obj);

typedef boost::shared_ptr<TrackInfoVector> TrackInfoVectorPtr;
typedef boost::shared_ptr<TrackInfoVector const> TrackInfoVectorConstPtr;

}
}
#endif //!_TYPE_DEFINITION_H_
