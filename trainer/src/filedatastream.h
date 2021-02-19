#pragma once
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp> 
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <iostream> 
#include <fstream> 
#include <sstream> 
#include <string>  
#include <vector>
#include "opencv2/opencv.hpp"

namespace boost {
    namespace serialization {
        template<typename Archive>
        void serialize(Archive & ar, cv::Point & p, const unsigned int version)
        {
            ar & p.x;
            ar & p.y;
        }
    }
}

enum ainno_train_policy {
    AINNO_TRAIN_AUTO = 0,
    AINNO_TRAIN_ALWAYS = 1,
    AINNO_TRAIN_NEVER = 2,
};

enum ainno_label_type {
    AINNO_LABEL_GOOD = 0,
    AINNO_LABEL_BAD = 1,
};

class ainno_rect {
public:
    float x;
    float y;
    float w;
    float h;
private:
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & x;
        ar & y;
        ar & w;
        ar & h;
    }
};

class ainno_defect {
public:
    std::vector<std::vector<cv::Point> > contours;
private:
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & contours;
    }
};

class ainno_contour_info {
public:
    int label;
    int area;
    int longsize;
    int shortsize;
    int centerx;
    int centery;
    double scoremin;
    double scoremax;
    double scoreavg;

private:
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & label;
        ar & area;
        ar & longsize;
        ar & shortsize;
        ar & centerx;
        ar & centery;
        ar & scoremin;
        ar & scoremax;
        ar & scoreavg;
    }
};

class ainno_image {
public:
    ainno_image(void) {
        trained = false;
        labeled = false;
        trainpolicy = AINNO_TRAIN_AUTO;
        labeltype = AINNO_LABEL_GOOD;
        labeldefects.clear();
        predictdefects.clear();
        labeldefectsinfo.clear();
        predictdefectsinfo.clear();
    };

    std::string filename;
    int height;
    int width;
    ainno_rect roi;
    bool trained;
    bool labeled;
    ainno_train_policy trainpolicy;
    ainno_label_type labeltype;
    std::vector<std::vector<cv::Point>> labeldefects;
    std::vector<std::vector<cv::Point>> predictdefects;
    std::vector<ainno_contour_info> labeldefectsinfo;
    std::vector<ainno_contour_info> predictdefectsinfo;
private:

    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & filename;
        ar & height;
        ar & width;
        ar & roi;
        ar & trained;
        ar & labeled;
        ar & trainpolicy;
        ar & labeltype;
        ar & labeldefects;
        ar & predictdefects;
        ar & labeldefectsinfo;
        ar & predictdefectsinfo;
    }
};

class ainno_stream
{
public:
    std::vector<ainno_image> imagelist;

private:
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & imagelist;
    }

};

class FileDataStream
{
public:
    static void save_txt(ainno_stream& stream, std::string filename)
    {
        std::ofstream file(filename);
        boost::archive::text_oarchive oa(file);
        oa << stream;
    }
    static void load_txt(ainno_stream& stream, std::string filename)
    {
        std::ifstream file(filename);
        boost::archive::text_iarchive ia(file);
        ia >> stream;
    }

    static void save_bin(ainno_stream& stream, std::string filename)
    {
        std::ofstream file(filename, std::ios::binary);
        boost::archive::binary_oarchive oa(file);
        oa << stream;
    }
    static void load_bin(ainno_stream& stream, std::string filename)
    {
        std::ifstream file(filename, std::ios::binary);
        boost::archive::binary_iarchive ia(file);
        ia >> stream;
    }

};

typedef struct
{
    bool labeled;
    ainno_train_policy trainpolicy;
    ainno_label_type labeltype;
    std::vector<std::vector<cv::Point>> labeldefects;
    std::vector<ainno_contour_info> labeldefectsinfo;
}labelinfo;











