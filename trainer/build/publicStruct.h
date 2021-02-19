#pragma once
#include <string>
#include <qobject.h>
using namespace std;
typedef struct
{
    std::string onnxmodel;
    std::string trtengine;
    int batchSize{ 1 };                     //!< Number of inputs in a batch
    int dlaCore{ -1 };                   //!< Specify the DLA core to run network on.
    bool int8{ false };                  //!< Allow runnning the network in Int8 mode.
    bool fp16{ false };                  //!< Allow running the network in FP16 mode.
    std::size_t workspaceSize;
    bool addSoftmax{ false };
}Sserializeparam; //常用：1 -1 0 1 4 1

typedef struct
{
    std::string funname;
    void *private_data;
}SRunInfo;
enum MenuOperateT
{
    O_NOLABEL_T,
    O_ALWAYS_T,
    O_NEVER_T,
    O_AUTO_T,
    O_CLOSE_T,
    O_OPEN_T,
    O_TEST_T,
};
enum trained_flag {
    all = 0,
    trained = 1,
    not_trained = 2,
    good = 3,
    bad = 4,
    labeled = 5,
    not_labeled = 6,
};
const int SHOWLISTIMAGESNUM = 4;    //add 页面显示缩略图的数量 [WangPing 2020/3/30]
const int LISTIMAGEWIDTH = 235;     //add 缩略图的宽度 [WangPing 2020/3/30]
const int LISTIMAGEHEIGHT = 150;    //add 缩略图的高度 [WangPing 2020/3/30] 

class ImageLabel;
