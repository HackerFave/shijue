#pragma once
#include <map>
#include <vector>
#include "qstring.h"
#include "filedatastream.h"

enum FileOperationType
{
    NEW,
    DESTROY,
    OPEN,
    CLOSE,
    ADDIMAGE,
    DELETEIMAGE,
    SAVE,
    LOADLABEL,
    ADDOKIMAGE,
    FOCOUNT
};

class FileProjectManager
{
public:
    ~FileProjectManager();

private:
    ainno_stream m_stream;
    static FileProjectManager* g_FileProjectManager;
    QString projectabspath_; //当前项目的绝对路径
    QString ainnoabspath_;   //当前项目工程文件的绝对路径
    QString imageabspath_;   //当前项目图片目录的绝对路径
    QString trainabspath_;   //当前项目训练目录的绝对路径
    bool projectisopen_;

    bool bsavetestresult_;

    std::vector<std::string> trainimglist_;//训练图片名
    std::map<std::string, std::map<int,cv::Mat>> predictresultimagelist_;//预测结果图片列表

    int classnum_;          //类别数
private:
    FileProjectManager();

public:
    static FileProjectManager* Instance();

    //通知所有m_stream的使用者释放资源，然后清空m_stream
    bool StreamUninit();
    //初始化m_stream
    bool StreamInit(ainno_stream & ainno_stream);
    //往m_stream中增加图片
    bool StreamAddImage(std::vector<QString> imgabspath, bool isOk);
    //获取图片信息
    ainno_image StreamGetImageInfo(int idx);
    //设置标注信息
    bool StreamSetImageLabelInfo(int idx, labelinfo retlabelinfo);
    //设置训练属性
    bool StreamSetImageTrainpolicy(int idx, ainno_train_policy trainpolicy);
    //获取标注信息
    labelinfo StreamGetImageLabelInfo(int idx);
    //获取训练标志
    bool StreamGetTrainedFlag(int idx);
    //获取good标志
    bool StreamGetGoodFlag(int idx);
    //获取bad标志
    bool StreamGetBadFlag(int idx);
    //获取labeled标志
    bool StreamGetLabeledFlag(int idx);

    //获取工程中图片总数
    int StreamGetImageNum();
    //获取对应索引的图片的绝对路径
    QString StreamGetImageAbsPath(int idx);
    //删除图片
    void StreamDeleteImage(int idx);

    //新建工程
    bool ProjectNew();
    //删除工程
    bool ProjectDestroy();
    //打开工程
    bool ProjectOpen(bool newFile,const QString &filePath);
    //设置类别
    void ProjectSetClassNum(int classnum);
    //关闭工程
    bool ProjectClose();
    //添加图像
    bool ProjectAddImage(bool isOk);
    //删除图像
    bool ProjectDeleteImage(int idx);
    bool ProjectDeleteImage(const QString& fileName);
    //保存工程文件
    bool ProjectSave();
    //加载标注文件
    bool ProjectLoadLabel();
    //工具栏(打开\创建工作目录)
    bool FileOperation(FileOperationType type, int idx = -1,const bool newFile = true,const QString FilePath = "");
    //获取测试log文件绝对路径
    QString ProjectGetTestLogAbsPath();
    //获取训练log文件绝对路径
    QString ProjectGetTrainLogAbsPath();
    //获取测试配置文件绝对路径
    QString ProjectGetTestCfgAbsPath();
    //获取训练配置文件绝对路径
    QString ProjectGetTrainCfgAbsPath();
    //获取训练目录绝对路径
    QString ProjectGetTrainAbsPath();
    //获取项目绝对路径
    QString ProjectGetProjectAbsPath();
    //获取项目图片目录绝对路径
    QString ProjectGetImageAbsPath();
    //恢复默认配置
    bool ResetTrainConfig();
    
    //准备训练数据
	bool PrepareTrainData(float trainratio, bool bkeep);
    //更新训练标志
    bool UpdateTrainFlag(bool btrained);
    //准备测试数据
    bool PrepareTestData(std::vector<int> indexes);
    //加载测试结果图片
    bool LoadPredictImage(std::vector<int> indexes);
    //是否保存测试结果
    void SetTestResultFlag(bool bflag);
    //加载测试结果
    bool LoadPredictResult(int threshold);
    //准备模型转换所需数据
    bool PrepareConvertData();
    //分析标记数据
    std::vector<ainno_contour_info> AnalyzeLabeledData(void);
    //获取文件名列表
    std::vector<std::string>  StreamGetImageName();
};

