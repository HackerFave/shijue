#pragma execution_character_set("utf-8")
#include "fileprojectmanager.h"
#include "qfiledialog.h"
#include "qdir.h"
#include "qmessagebox.h"
#include "trainer_common.h"
#include <qapplication.h>
#include "ciwamsgdialog.h"
#include <qtimer.h>
#include <QTime>
#include <qdebug.h>

bool DeleteDirectory(const QString &path)
{
    if (path.isEmpty())
    {
        return false;
    }

    QDir dir(path);
    if (!dir.exists())
    {
        QFile fileTemp(path);
        fileTemp.remove();
        return true;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach(QFileInfo fi, fileList)
    {
        if (fi.isFile())
        {
            fi.dir().remove(fi.fileName());
        }
        else
        {
            DeleteDirectory(fi.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath());
}

FileProjectManager* FileProjectManager::g_FileProjectManager = nullptr;

FileProjectManager * FileProjectManager::Instance()
{
    if (g_FileProjectManager == nullptr)
    {
        g_FileProjectManager = new FileProjectManager();
    }
    return g_FileProjectManager;
}

FileProjectManager::FileProjectManager()
{
    projectisopen_ = false;
    trainimglist_.clear();
    predictresultimagelist_.clear();
}

FileProjectManager::~FileProjectManager()
{
    predictresultimagelist_.clear();
}

bool FileProjectManager::StreamUninit() {
    //TODO 通知所有该成员的使用者释放资源

    m_stream.imagelist.clear();
    return true;
}
//图片名列表
bool FileProjectManager::StreamInit(ainno_stream &ainno_stream) {
    m_stream = ainno_stream;
    trainimglist_.clear();
    for (int j = 0; j < m_stream.imagelist.size(); j++) {
        if (m_stream.imagelist[j].trained == true) {
            trainimglist_.push_back(m_stream.imagelist[j].filename);
        }
    }
    return true;
}

bool FileProjectManager::StreamAddImage(std::vector<QString> imgabspath, bool isOK) {
    for (int i = 0; i < imgabspath.size(); i++){
        ainno_image img;
        QFileInfo fileInfo(imgabspath[i]);
        cv::Mat img_mat = cv::imread(std::string(imgabspath[i].toLocal8Bit()),
                                        cv::IMREAD_GRAYSCALE);
        img.filename = std::string(fileInfo.fileName().toLocal8Bit());
        img.height = img_mat.rows;
        img.width = img_mat.cols;
        img.roi.x = 0;
        img.roi.y = 0;
        img.roi.w = img.width;
        img.roi.h = img.height;
        img.labeled = isOK;
        img.labeltype = AINNO_LABEL_GOOD;
        img.labeldefects.clear();
        img.trained = false;
        img.trainpolicy = AINNO_TRAIN_AUTO;
        img.predictdefects.clear();
        m_stream.imagelist.push_back(img);
    }
    return true;
}

ainno_image FileProjectManager::StreamGetImageInfo(int idx) {
    return m_stream.imagelist[idx];
}

bool FileProjectManager::StreamSetImageLabelInfo(int idx, labelinfo imglabelinfo) {
    auto &curimage = m_stream.imagelist[idx];
    curimage.labeled = imglabelinfo.labeled;
    curimage.labeltype = imglabelinfo.labeltype;
    curimage.labeldefects = imglabelinfo.labeldefects;
    curimage.labeldefectsinfo = imglabelinfo.labeldefectsinfo;
#define SAVE_MASK
#ifdef SAVE_MASK
    //train/mask
    QString dir_mask = QString("%1/mask").arg(trainabspath_);
    QDir dir(dir_mask);
    if (!dir.exists()){
        if (!dir.mkdir(dir_mask)) {
            return false;
        }
    }

    //mask图片生成
    std::string img_path = curimage.filename;
    const size_t last_dot_pos = img_path.find_last_of(".");
    if (std::string::npos != last_dot_pos) {
        img_path.erase(last_dot_pos, std::string::npos);
    }
    std::string mask_path = std::string(dir_mask.toLocal8Bit()) + '/' + img_path + ".png";
    cv::Mat emptyimg = cv::Mat::zeros(curimage.height, curimage.width, CV_8UC1);
    if (curimage.labeltype == AINNO_LABEL_BAD) {
        //cv::drawContours(emptyimg, curimage.labeldefects, -1, cv::Scalar(1), cv::FILLED);
		for (int j = 0; j < curimage.labeldefects.size(); j++) {
			cv::drawContours(emptyimg, curimage.labeldefects, j,
				cv::Scalar(curimage.labeldefectsinfo[j].label), cv::FILLED);
		}
    }
    cv::imwrite(mask_path, emptyimg);
#endif
    return true;
}

bool FileProjectManager::StreamSetImageTrainpolicy(int idx, ainno_train_policy trainpolicy) {
    m_stream.imagelist[idx].trainpolicy = trainpolicy;
    return true;
}


labelinfo FileProjectManager::StreamGetImageLabelInfo(int idx) {
    labelinfo retlabelinfo;

    retlabelinfo.labeled = m_stream.imagelist[idx].labeled;
    retlabelinfo.labeltype = m_stream.imagelist[idx].labeltype;
    retlabelinfo.labeldefects = m_stream.imagelist[idx].labeldefects;
    retlabelinfo.labeldefectsinfo = m_stream.imagelist[idx].labeldefectsinfo;
    return retlabelinfo;
}

bool FileProjectManager::StreamGetTrainedFlag(int idx) {
    return m_stream.imagelist[idx].trained;
}

bool FileProjectManager::StreamGetGoodFlag(int idx) {
    return m_stream.imagelist[idx].labeled 
        && m_stream.imagelist[idx].labeltype == AINNO_LABEL_GOOD;
}

bool FileProjectManager::StreamGetBadFlag(int idx) {
    return m_stream.imagelist[idx].labeled
        && m_stream.imagelist[idx].labeltype == AINNO_LABEL_BAD;
}

bool FileProjectManager::StreamGetLabeledFlag(int idx) {
    return m_stream.imagelist[idx].labeled;
}

int FileProjectManager::StreamGetImageNum() {
    return m_stream.imagelist.size();
}
std::vector<std::string> FileProjectManager::StreamGetImageName()
{
    std::vector<std::string> trainimglist;
    for (int j = 0; j < m_stream.imagelist.size(); j++) {
      // if (m_stream.imagelist[j].trained == true) {
            trainimglist.push_back(m_stream.imagelist[j].filename);
      //  }
    }
    return trainimglist;
}
QString FileProjectManager::StreamGetImageAbsPath(int idx) {
    return QString("%1/%2").arg(imageabspath_).
        arg(QString::fromLocal8Bit(m_stream.imagelist[idx].filename.c_str()));
}

void FileProjectManager::StreamDeleteImage(int idx) {
    auto item = m_stream.imagelist.begin() + idx;
    m_stream.imagelist.erase(item);
}

bool FileProjectManager::ProjectNew() {
    QDir dir;
    QFile file;
    QString ainnoabspath;
    QString imageabspath;
    QString trainabspath;
    QString defaultcfg;
    QString cfgfilename;
    ainno_stream tmpstream;

    //等待输入工程路径名称
    QString projectabspath = QFileDialog::getSaveFileName(
                                        nullptr,QString("New"),"","");
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //点击取消
    if (projectabspath.isEmpty()) {
        return false;
    }

    //创建工程目录
    if (!dir.mkdir(projectabspath)) {
        CiWaMsgDialog::MessageSignal("工程", "创建工程目录失败！");
        return false;
    }
    dir.setPath(projectabspath);

    //创建图像数据目录
    imageabspath = QString("%1/imagedata").arg(projectabspath);
    if (!dir.mkdir(imageabspath)){
        CiWaMsgDialog::MessageSignal("工程", "创建图像数据目录失败！");
        goto err;
    }

    //创建训练目录
    trainabspath = QString("%1/train").arg(projectabspath);
    if (!dir.mkdir(trainabspath)) {
        CiWaMsgDialog::MessageSignal("工程", "创建训练目录失败！");
        goto err;
    }

    //拷贝默认算法配置
    defaultcfg = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg(QString("default_train.yaml"));
    file.setFileName(defaultcfg);
    if (!file.exists()) {
        CiWaMsgDialog::MessageSignal("工程", "默认算法配置文件不存在！");
        goto err;
    }
    cfgfilename = QString("%1/train.yaml").arg(trainabspath);
    if (!QFile::copy(defaultcfg, cfgfilename)) {
        CiWaMsgDialog::MessageSignal("工程", "算法配置文件拷贝失败！");
        goto err;
    }

    //拷贝默认算法配置
    defaultcfg = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg(QString("default_test.yaml"));
    file.setFileName(defaultcfg);
    if (!file.exists()) {
        CiWaMsgDialog::MessageSignal("工程", "默认算法配置文件不存在！");
        goto err;
    }
    cfgfilename = QString("%1/test.yaml").arg(trainabspath);
    if (!QFile::copy(defaultcfg, cfgfilename)) {
        CiWaMsgDialog::MessageSignal("工程", "算法配置文件拷贝失败！");
        goto err;
    }

    //新建ainno文件
    tmpstream.imagelist.clear();
    ainnoabspath = QString("%1/ainno.ainno").arg(projectabspath);
    try {
        FileDataStream::save_bin(tmpstream,
            std::string(ainnoabspath.toLocal8Bit()));
    }
    catch (...) {
        CiWaMsgDialog::MessageSignal("工程", "工程文件保存失败");
        goto err;
    }

    StreamUninit();
    StreamInit(tmpstream);

    projectabspath_ = projectabspath;
    ainnoabspath_ = ainnoabspath;
    imageabspath_ = imageabspath;
    trainabspath_ = trainabspath;
    projectisopen_ = true;
    predictresultimagelist_.clear();
    trainimglist_.clear();
    bsavetestresult_ = false;
    classnum_ = 2;
    return true;

err:
    DeleteDirectory(projectabspath);
    return false;
}

bool FileProjectManager::ProjectDestroy() {
    StreamUninit();
    projectisopen_ = false;
    DeleteDirectory(projectabspath_);
    return true;
}
//打开工程
bool FileProjectManager::ProjectOpen(bool newFile, const QString &filePath) {
    QDir dir;
    QFile file;
    QString ainnoabspath;
    QString imageabspath;
    QString trainabspath;

    QString projectabspath;
    if (newFile) {
        projectabspath = QFileDialog::getExistingDirectory(
            nullptr, QString("Open"), "", QFileDialog::ShowDirsOnly);
    }
    else {
        projectabspath = filePath;
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //点击取消
    if (projectabspath.isEmpty()) {
        return false;
    }

    dir.setPath(projectabspath);
    ainnoabspath = QString("%1/ainno.ainno").arg(projectabspath);
    file.setFileName(ainnoabspath);
    if (!file.exists()) {
        CiWaMsgDialog::MessageSignal("工程", "工程文件不存在！");
        return false;
    }

    //创建图像数据目录
    imageabspath = QString("%1/imagedata").arg(projectabspath);
    if (!dir.exists(imageabspath)) {
        CiWaMsgDialog::MessageSignal("工程", "图像数据目录不存在！");
        return false;
    }

    //创建训练目录
    trainabspath = QString("%1/train").arg(projectabspath);
    if (!dir.exists(trainabspath)) {
        CiWaMsgDialog::MessageSignal("工程", "训练目录不存在！");
        return false;
    }
    QTime time;
    time.start();
    ainno_stream tmpstream;
    try {
        FileDataStream::load_bin(tmpstream,
                                    std::string(ainnoabspath.toLocal8Bit()));
    }
    catch (...) {
        CiWaMsgDialog::MessageSignal("工程", "工程文件加载失败");
        return false;
    }
   
    //清理资源
    StreamUninit();
    //初始化文件名
    StreamInit(tmpstream);

    projectabspath_ = projectabspath;
    ainnoabspath_ = ainnoabspath;
    imageabspath_ = imageabspath;
    trainabspath_ = trainabspath;
    projectisopen_ = true;
    predictresultimagelist_.clear();
    trainimglist_.clear();
    bsavetestresult_ = false;
    classnum_ = 2;
    qDebug() << "加载图片时间:" << time.elapsed() / 1000.0;
    return true;
}
void FileProjectManager::ProjectSetClassNum(int classnum) {
    classnum_ = classnum;
}

bool FileProjectManager::ProjectClose() {
    StreamUninit();
    projectisopen_ = false;
    return true;
}

bool FileProjectManager::ProjectAddImage(bool isOk) {
    std::vector<QString> imgsabspath;

    //当前项目是否打开
    if (!projectisopen_) {
        CiWaMsgDialog::MessageSignal("工程", "工程未打开！");
        return false;
    }
    //弹出文件选择对话框
    QStringList piclist = QFileDialog::getOpenFileNames(nullptr,
                                  QString("添加图像"),"",("*.jpg *png *bmp"));
    //如果未选择则返回
    if (piclist.size() < 1){
        return false;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //将图像复制到工程目录中
    QFileInfo fileInfo;
    QString imageabspath;
    char ch[64];
    time_t t;
    for (int i = 0; i < piclist.size(); i++) {
        t = time(0);
        strftime(ch, sizeof(ch), "%Y%m%d%H%M%S", localtime(&t)); //年月日时分秒
        fileInfo = piclist.at(i);
        imageabspath = QString("%1/%2_%3").arg(imageabspath_)
            .arg(QString(ch)).arg(fileInfo.fileName());
        
        if (QFile::exists(imageabspath)) {
            QFile::remove(imageabspath);
        }

        if (!QFile::copy(fileInfo.absoluteFilePath(), imageabspath)){
            CiWaMsgDialog::MessageSignal("工程", 
                                "图像解析失败:"+ fileInfo.fileName());
            goto err;
        }

        imgsabspath.push_back(imageabspath);
    }

    StreamAddImage(imgsabspath, isOk);

    return true;
err:
    for (int i = 0; i < imgsabspath.size(); i++) {
        if (QFile::exists(imgsabspath[i])) {
            QFile::remove(imgsabspath[i]);
        }
    }
    return false;
}
bool FileProjectManager::ProjectDeleteImage(const QString& fileName)
{
    if (fileName.isEmpty()) {
        CiWaMsgDialog::MessageSignal("工程", "无效的图片索引！");
        return false;
    }

    QString imgabspath = QString("%1/%2").arg(imageabspath_)
        .arg(fileName);
    DeleteDirectory(imgabspath);
    return true;

}
bool FileProjectManager::ProjectDeleteImage(int idx) {
    if (idx >= m_stream.imagelist.size()
        || idx < 0) {
        CiWaMsgDialog::MessageSignal("工程", "无效的图片索引！");
        return false;
    }

    QString imgabspath = QString("%1/%2").arg(imageabspath_)
            .arg(QString::fromLocal8Bit(m_stream.imagelist[idx].filename.c_str()));
    DeleteDirectory(imgabspath);
    m_stream.imagelist.erase(m_stream.imagelist.begin() + idx);
    

    return true;
}

bool FileProjectManager::ProjectSave() {
    if (!projectisopen_) {
        return false;
    }
    FileDataStream::save_bin(m_stream, 
                                std::string(ainnoabspath_.toLocal8Bit()));
    return true;
}

bool FileProjectManager::ProjectLoadLabel() {
    //当前项目是否打开
    if (!projectisopen_) {
        CiWaMsgDialog::MessageSignal("工程", "工程未打开！");
        return false;
    }

    //弹出文件选择对话框
    QStringList piclist = QFileDialog::getOpenFileNames(nullptr,
        QString("添加标注"), "", ("*png *json"));
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //如果未选择则返回
    if (piclist.size() < 1) {
        return false;
    }

    //将标注信息添加到工程文件中
    QFileInfo labelfileInfo;
    QFileInfo imagefileInfo;
    for (int i = 0; i < piclist.size(); i++) {
        labelfileInfo = piclist.at(i);
        for (int j = 0; j < m_stream.imagelist.size(); j++) {
            imagefileInfo.setFile(QString::fromLocal8Bit(m_stream.imagelist[j].filename.c_str()));
            //去掉时间信息
            QString completeBaseName = imagefileInfo.completeBaseName();
            completeBaseName = completeBaseName.mid(completeBaseName.indexOf("_")+1);
            if (labelfileInfo.completeBaseName() == completeBaseName
                || labelfileInfo.completeBaseName() == (completeBaseName +"_mask")) {
                if (labelfileInfo.suffix() == "json") {

                }
                else if (labelfileInfo.suffix() == "png") {
                    cv::Mat mask_img = cv::imread(std::string(labelfileInfo.filePath().toLocal8Bit()), cv::IMREAD_GRAYSCALE);

                    std::vector<cv::Vec4i> hierarchy;
                    std::vector<std::vector<cv::Point>> labeldefects;
                    std::vector<std::vector<cv::Point>> tmpcontours;
                    //cv::findContours(mask_img, labeldefects, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

					//获取当前图中的所有类别
					std::vector<int> class_ids;
					cv::Mat mask_img_small;
					cv::resize(mask_img, mask_img_small, cv::Size(0, 0), 0.5, 0.5, cv::INTER_NEAREST);
					for (int row = 0; row < mask_img_small.rows; row++)
					{
						for (int col = 0; col < mask_img_small.cols; col++)
						{
							int pixel = mask_img_small.ptr<uchar>(row)[col];
							if (pixel != 0)
							{
								if (std::find(class_ids.begin(), class_ids.end(), pixel) == class_ids.end())
									class_ids.push_back(pixel);
							}
						}
					}
					for (int i_classids = 0; i_classids < class_ids.size(); i_classids++)
					{
						//只保留当前类别像素值
						int class_id = class_ids[i_classids];
						cv::Mat mask_img_class;
						cv::threshold(mask_img, mask_img_class, class_id - 1, 0, cv::THRESH_TOZERO);
						cv::threshold(mask_img_class, mask_img_class, class_id, 0, cv::THRESH_TOZERO_INV);
						cv::findContours(mask_img_class, tmpcontours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
						for (int i_tmpcontours = 0; i_tmpcontours < tmpcontours.size(); i_tmpcontours++)
						{
							labeldefects.push_back(tmpcontours[i_tmpcontours]);
						}
					}

                    if (labeldefects.size()) {
                        m_stream.imagelist[j].labeldefects = labeldefects;
                        m_stream.imagelist[j].labeled = true;
                        m_stream.imagelist[j].labeltype = AINNO_LABEL_BAD;

                        cv::Moments moment;
                        cv::Mat defect;
                        cv::RotatedRect minRect;
                        ainno_contour_info datainfo;
                        m_stream.imagelist[j].labeldefectsinfo.clear();
                        for (int k = 0; k < labeldefects.size(); k++) {
                            //类型
                            datainfo.label = mask_img.at<uchar>(labeldefects[k][0]);
                            //面积
                            datainfo.area = cv::contourArea(labeldefects[k]);

                            //中心
                            moment = cv::moments(labeldefects[k]);
                            datainfo.centerx = int(moment.m10 / moment.m00);
                            datainfo.centery = int(moment.m01 / moment.m00);

                            //长短边
                            defect = cv::Mat(labeldefects[k]);
                            minRect = cv::minAreaRect(defect);
                            if (minRect.size.width < minRect.size.height) {
                                datainfo.longsize = minRect.size.height;
                                datainfo.shortsize = minRect.size.width;
                            }
                            else {
                                datainfo.longsize = minRect.size.width;
                                datainfo.shortsize = minRect.size.height;
                            }
                            m_stream.imagelist[j].labeldefectsinfo.push_back(datainfo);
                        }
                    }
                    else {
                        m_stream.imagelist[j].labeled = true;
                        m_stream.imagelist[j].labeltype = AINNO_LABEL_GOOD;
                    }
                }
                else {

                }
                break;
            }
        }
    }
    return true;
}

bool FileProjectManager::FileOperation(FileOperationType type, int idx , const bool newFile, const QString FilePath )
{
    bool retval = true;
    switch (type)
    {
    case FileOperationType::NEW:
        retval = ProjectNew();
        break;

    case FileOperationType::DESTROY:
        retval = ProjectDestroy();
        break;

    case FileOperationType::OPEN:
        retval = ProjectOpen(newFile, FilePath);
        break;

    case FileOperationType::CLOSE:
        retval = ProjectClose();
        break;

    case FileOperationType::ADDIMAGE:
        retval = ProjectAddImage(false);
        if(retval){
            retval = ProjectSave();
        }
        break;

    case FileOperationType::ADDOKIMAGE:
        retval = ProjectAddImage(true);
        if (retval) {
            retval = ProjectSave();
        }
        break;

    case FileOperationType::DELETEIMAGE:
        retval = ProjectDeleteImage(idx);
        if (retval) {
            retval = ProjectSave();
        }
        break;

    case FileOperationType::SAVE:
        retval = ProjectSave();
        break;
        
    case FileOperationType::LOADLABEL:
        retval = ProjectLoadLabel();
        if (retval) {
            retval = ProjectSave();
        }
        break;

    default:
        break;
    }
    return retval;
}

QString FileProjectManager::ProjectGetTestLogAbsPath() {
    return QString("%1/test_log.csv").arg(trainabspath_);
}

QString FileProjectManager::ProjectGetTrainLogAbsPath() {
    return QString("%1/train_log.csv").arg(trainabspath_);
}

QString FileProjectManager::ProjectGetTestCfgAbsPath() {
    return QString("%1/test.yaml").arg(trainabspath_);
}

QString FileProjectManager::ProjectGetTrainCfgAbsPath() {
    return QString("%1/train.yaml").arg(trainabspath_);
}

QString FileProjectManager::ProjectGetTrainAbsPath() {
    return trainabspath_;
}

QString FileProjectManager::ProjectGetProjectAbsPath() {
    return projectabspath_;
}

QString FileProjectManager::ProjectGetImageAbsPath() {
    return imageabspath_;
}

bool FileProjectManager::ResetTrainConfig() {
    //当前项目是否打开
    if (!projectisopen_) {
        CiWaMsgDialog::MessageSignal("工程", "工程未打开！");
        return false;
    }

    QString filename = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg(QString("default_train.yaml"));
    QString newname = QString("%1/%2").arg(trainabspath_).arg(QString("train.yaml"));
    if (QFile::exists(newname)) {
        QFile::remove(newname);
    }
    if (!QFile::copy(filename, newname)) {
        CiWaMsgDialog::MessageSignal("文件", "图像拷贝失败:" + newname);
        return false;
    }
    return true;
}

bool FileProjectManager::PrepareTrainData(float trainratio, bool bkeeptraindata) {
    QDir dir;
    //当前项目是否打开
    if (!projectisopen_) {
        CiWaMsgDialog::MessageSignal("工程", "工程未打开！");
        return false;
    }

    if ((trainimglist_.size() == 0) || !bkeeptraindata) {
        //生成目录
        //train/image
        QString dir_image = QString("%1/image").arg(trainabspath_);
        DeleteDirectory(dir_image);
        if (!dir.mkdir(dir_image)) {
            CiWaMsgDialog::MessageSignal("目录", "创建失败:/image");
            return false;
        }
        //train/mask
        QString dir_mask = QString("%1/mask").arg(trainabspath_);
        DeleteDirectory(dir_mask);
        if (!dir.mkdir(dir_mask)) {
            CiWaMsgDialog::MessageSignal("目录", "创建失败:/mask");
            return false;
        }

        //组织训练所需数据
        trainimglist_.clear();
        std::vector<std::string> valimglist_;
        std::vector<std::string> autoimglist;
        for (int i = 0; i < m_stream.imagelist.size(); i++) {
            ainno_image &tmpimage = m_stream.imagelist[i];
            //没有标注不参与训练
            if (!tmpimage.labeled) {
                continue;
            }
            //训练图片拷贝
            QString filename = QString("%1/%2").arg(imageabspath_)
                .arg(QString::fromLocal8Bit(tmpimage.filename.c_str()));
            QString newname = QString("%1/%2").arg(dir_image)
                .arg(QString::fromLocal8Bit(tmpimage.filename.c_str()));
            if (!QFile::copy(filename, newname)) {
                CiWaMsgDialog::MessageSignal("文件", "图像拷贝失败:" + newname);
                return false;
            }

            //mask图片生成
            std::string img_path = tmpimage.filename;
            const size_t last_dot_pos = img_path.find_last_of(".");
            if (std::string::npos != last_dot_pos) {
                img_path.erase(last_dot_pos, std::string::npos);
            }
            std::string mask_path = std::string(dir_mask.toLocal8Bit()) + '/' + img_path + ".png";
            cv::Mat emptyimg = cv::Mat::zeros(tmpimage.height, tmpimage.width, CV_8UC1);
            if (tmpimage.labeltype == AINNO_LABEL_BAD) {
                for (int j = 0; j < tmpimage.labeldefects.size(); j++) {
                    cv::drawContours(emptyimg, tmpimage.labeldefects, j, 
                        cv::Scalar(tmpimage.labeldefectsinfo[j].label), cv::FILLED);
                }
            }
            cv::imwrite(mask_path, emptyimg);

            if (tmpimage.trainpolicy == AINNO_TRAIN_ALWAYS) {
                trainimglist_.push_back(tmpimage.filename);
            }
            else if (tmpimage.trainpolicy == AINNO_TRAIN_NEVER) {
                valimglist_.push_back(tmpimage.filename);
            }
            else {
                autoimglist.push_back(tmpimage.filename);
            }
        }

        int offset = (trainimglist_.size() + valimglist_.size() + autoimglist.size()) * trainratio;
        if (offset > trainimglist_.size()) {
            offset = offset - trainimglist_.size();
            if (offset >= autoimglist.size()) {
                trainimglist_.insert(trainimglist_.end(), autoimglist.begin(),
                    autoimglist.end());
                autoimglist.clear();
            }
            else {
                std::random_shuffle(autoimglist.begin(), autoimglist.end());
                std::vector<std::string>::const_iterator first = autoimglist.begin();
                std::vector<std::string>::const_iterator last = autoimglist.begin() + offset;
                trainimglist_.insert(trainimglist_.end(), first, last);
                autoimglist.erase(first, last);
            }
        }
        if (autoimglist.size()) {
            valimglist_.insert(valimglist_.end(), autoimglist.begin(),
                autoimglist.end());
            autoimglist.clear();
        }
        else {
            valimglist_.insert(valimglist_.end(), trainimglist_.begin(),
                trainimglist_.end());
        }

        if (trainimglist_.size() <= 2) {
            CiWaMsgDialog::MessageSignal("训练", "图片数量太少！");
            return false;
        }

        //生成train.csv
        std::ofstream outputfile;
        outputfile.open(std::string(trainabspath_.toLocal8Bit()) + "/train.csv");
        for (int i = 0; i < trainimglist_.size(); i++) {
            std::string img_path = trainimglist_[i];
            const size_t last_dot_pos = img_path.find_last_of(".");
            if (std::string::npos != last_dot_pos) {
                img_path.erase(last_dot_pos, std::string::npos);
            }
            outputfile << "image/" << trainimglist_[i] << ",mask/" << img_path << ".png" << std::endl;
        }
        outputfile.close();

        //生成val.csv
        outputfile.open(std::string(trainabspath_.toLocal8Bit()) + "/val.csv");
        for (int i = 0; i < valimglist_.size(); i++) {
            std::string img_path = valimglist_[i];
            const size_t last_dot_pos = img_path.find_last_of(".");
            if (std::string::npos != last_dot_pos) {
                img_path.erase(last_dot_pos, std::string::npos);
            }
            outputfile << "image/" << valimglist_[i] << ",mask/" << img_path << ".png" << std::endl;
        }
        outputfile.close();

    }

    //更新配置文件
    QString filename = QString("%1/%2").arg(trainabspath_).arg(QString("train.yaml"));
    QString newname = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg(QString("ainnovision_train.yaml"));
    if (QFile::exists(newname)) {
        QFile::remove(newname);
    }
    if (!QFile::copy(filename, newname)){
        CiWaMsgDialog::MessageSignal("文件", "图像拷贝失败:" + newname);
        return false;
    }

    //删除之前的训练日志
    QString train_log = QString("%1/%2").arg(trainabspath_).arg(QString("train_log.csv"));
    if (QFile::exists(train_log)) {
        QFile::remove(train_log);
    }
    return true;
}

bool FileProjectManager::UpdateTrainFlag(bool btrained) {
    if (!btrained) {
        for (int j = 0; j < m_stream.imagelist.size(); j++) {
                m_stream.imagelist[j].trained = false;
        }
    }
    else {
        for (int i = 0; i < trainimglist_.size(); i++) {
            for (int j = 0; j < m_stream.imagelist.size(); j++) {
                if (trainimglist_[i] == m_stream.imagelist[j].filename) {
                    m_stream.imagelist[j].trained = true;
                }
            }
        }
    }
    return true;
}

bool FileProjectManager::PrepareTestData(std::vector<int> indexes) {
    QDir dir;

    //当前项目是否打开
    if (!projectisopen_) {
        CiWaMsgDialog::MessageSignal("工程", "工程未打开！");
        return false;
    }

    //判断是否存在模型，不存在则不能测试
    QString checkpoint = QString("%1/models/F1_best_model.pth.tar").arg(trainabspath_);
    if (!QFile::exists(checkpoint)) {
        CiWaMsgDialog::MessageSignal("错误", "不存在已训练模型，请先训练！");
        return false;
    }
    QString dir_test;
    QString dir_mask;
    QString dir_test_predict;
    std::vector<ainno_image> tmpimagelist;
    if (indexes.size()) {
        //生成目录
        //train/test
        dir_test = QString("%1/test").arg(trainabspath_);
        DeleteDirectory(dir_test);
        if (!dir.mkdir(dir_test)) {
            CiWaMsgDialog::MessageSignal("目录", "创建失败：/test");
            return false;
        }
        //train/test_predict
        dir_test_predict = QString("%1/test_predict").arg(trainabspath_);
        if (!dir.exists(dir_test_predict) && !dir.mkdir(dir_test_predict)) {
            CiWaMsgDialog::MessageSignal("目录", "创建失败：train/test_predict");
            return false;
        }

        //train/mask
        dir_mask = QString("%1/mask").arg(trainabspath_);
        if (!dir.exists(dir_mask) && !dir.mkdir(dir_mask)) {
            CiWaMsgDialog::MessageSignal("目录", "创建失败:/mask");
            return false;
        }

        for (int i = 0; i < indexes.size(); i++) {
            int index = indexes[i];
            tmpimagelist.push_back(m_stream.imagelist[index]);
        }
    }
    else {
        //生成目录
        //train/test
        dir_test = QString("%1/test").arg(trainabspath_);
        DeleteDirectory(dir_test);
        if (!dir.mkdir(dir_test)) {
            CiWaMsgDialog::MessageSignal("目录", "创建失败：/test");
            return false;
        }
        //train/test_predict
        dir_test_predict = QString("%1/test_predict").arg(trainabspath_);
        DeleteDirectory(dir_test_predict);
        if (!dir.mkdir(dir_test_predict)) {
            CiWaMsgDialog::MessageSignal("目录", "创建失败：train/test_predict");
            return false;
        }

        //train/mask
        dir_mask = QString("%1/mask").arg(trainabspath_);
        DeleteDirectory(dir_mask);
        if (!dir.mkdir(dir_mask)) {
            CiWaMsgDialog::MessageSignal("目录", "创建失败:/mask");
            return false;
        }

        tmpimagelist = m_stream.imagelist;
    }
    //组织测试所需数据，测试所有图片，包括已用于训练图片
    for (int i = 0; i < tmpimagelist.size(); i++) {
        ainno_image &tmpimage = tmpimagelist[i];
        QString filename = QString("%1/%2").arg(imageabspath_).arg(QString::fromLocal8Bit(tmpimage.filename.c_str()));
        QString testname = QString("%1/%2").arg(dir_test).arg(QString::fromLocal8Bit(tmpimage.filename.c_str()));
        if (!QFile::copy(filename, testname)){
            CiWaMsgDialog::MessageSignal("文件", "图像拷贝失败："+ testname);
            return false;
        }

        if (!tmpimage.labeled) {
            continue;
        }
        //mask图片生成
        std::string img_path = tmpimage.filename;
        const size_t last_dot_pos = img_path.find_last_of(".");
        if (std::string::npos != last_dot_pos) {
            img_path.erase(last_dot_pos, std::string::npos);
        }
        std::string mask_path = std::string(dir_mask.toLocal8Bit()) + '/' + img_path + ".png";
        cv::Mat emptyimg = cv::Mat::zeros(tmpimage.height, tmpimage.width, CV_8UC1);
        if (tmpimage.labeltype == AINNO_LABEL_BAD) {
            for (int j = 0; j < tmpimage.labeldefects.size(); j++) {
                cv::drawContours(emptyimg, tmpimage.labeldefects, j,
                    cv::Scalar(tmpimage.labeldefectsinfo[j].label), cv::FILLED);
            }
        }
        cv::imwrite(mask_path, emptyimg);
    }

    //更新配置文件
    QString filename = QString("%1/%2").arg(trainabspath_).arg(QString("test.yaml"));
    QString newname = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg(QString("ainnovision_test.yaml"));
    if (QFile::exists(newname)) {
        QFile::remove(newname);
    }
    if (!QFile::copy(filename, newname)){
        CiWaMsgDialog::MessageSignal("文件", "图像拷贝失败：" + newname);
        return false;
    }

    //删除之前的测试日志
    QString train_log = QString("%1/%2").arg(trainabspath_).arg(QString("test_log.csv"));
    if (QFile::exists(train_log)) {
        QFile::remove(train_log);
    }
    return true;
}

bool FileProjectManager::LoadPredictImage(std::vector<int> indexes) {
    QString dir_test_predict = QString("%1/test_predict").arg(trainabspath_);
    QFileInfo imagefileInfo;
    std::vector<ainno_image> tmpimagelist;
    if (indexes.size()) {
        for (int i = 0; i < indexes.size(); i++) {
            int index = indexes[i];
            tmpimagelist.push_back(m_stream.imagelist[index]);
        }
    }
    else {
        tmpimagelist = m_stream.imagelist;
        predictresultimagelist_.clear();
    }
    for (int i = 0; i < tmpimagelist.size(); i++) {
        ainno_image &tmpimage = tmpimagelist[i];
        imagefileInfo.setFile(QString::fromLocal8Bit(tmpimage.filename.c_str()));

        for (int j = 1; j < classnum_; j++) {
            std::string mask_path = QString("%1/%2_%3.png").arg(dir_test_predict)
                .arg(imagefileInfo.completeBaseName()).arg(j).toLocal8Bit();

            predictresultimagelist_[tmpimage.filename][j] = cv::imread(mask_path, cv::IMREAD_GRAYSCALE);
        }
    }
    return true;
}

void FileProjectManager::SetTestResultFlag(bool bflag) {
    bsavetestresult_ = bflag;
}
//加载测试结果
bool FileProjectManager::LoadPredictResult(int threshold) {
    QDir dir;
    ainno_contour_info datainfo;
    cv::Moments moment;
    cv::Mat defectmat;
    cv::RotatedRect minRect;
    double minVal;
    double maxVal;
    int minIdx;
    int maxIdx;
    cv::Scalar meanVal;
    cv::Mat mask_bin;
    std::vector<cv::Vec4i> hierarchy;

    if (predictresultimagelist_.size() == 0) {
        std::vector<int> indexes;
        indexes.clear();
        LoadPredictImage(indexes);
    }

    //train/result
    QString dir_test_result = QString("%1/test_result").arg(trainabspath_);
    if (bsavetestresult_) {
        DeleteDirectory(dir_test_result);
        if (!dir.mkdir(dir_test_result)) {
            CiWaMsgDialog::MessageSignal("目录", "创建失败：train/test_result");
            return false;
        }
    }

    QString dir_test_predict = QString("%1/test_predict").arg(trainabspath_);
    for (int i = 0; i < m_stream.imagelist.size(); i++) {
        ainno_image &tmpimage = m_stream.imagelist[i];
        auto& mask_img = predictresultimagelist_[tmpimage.filename];
        tmpimage.predictdefects.clear();
        tmpimage.predictdefectsinfo.clear();
        for (auto it = mask_img.begin(); it != mask_img.end(); ++it) {
            std::vector<std::vector<cv::Point>> defects;
            cv::Mat tmp_mask_img = it->second;
            cv::threshold(tmp_mask_img, mask_bin, threshold, 255, cv::THRESH_BINARY);
            cv::findContours(mask_bin, defects, hierarchy,
                cv::RETR_EXTERNAL, cv::CHAIN_APPROX_TC89_KCOS);

            for (int k = 0; k < defects.size(); k++) {
                auto defect = defects[k];
                //类别
                datainfo.label = it->first;

                //面积
                datainfo.area = cv::contourArea(defect);

                //中心
                moment = cv::moments(defect);
                datainfo.centerx = int(moment.m10 / moment.m00);
                datainfo.centery = int(moment.m01 / moment.m00);

                //最大最小值
                cv::Rect minboundingRect = cv::boundingRect(defect);
                defectmat = cv::Mat(minboundingRect.height, minboundingRect.width, CV_8UC1, cv::Scalar(0));
                cv::drawContours(defectmat, defects, k, cv::Scalar(1), -1, 8,
                    cv::noArray(), INT_MAX, cv::Point(-minboundingRect.x, -minboundingRect.y));
                cv::minMaxIdx(tmp_mask_img(minboundingRect), &minVal, &maxVal, &minIdx, &maxIdx, defectmat);
                meanVal = cv::mean(tmp_mask_img(minboundingRect), defectmat);
                datainfo.scoremin = minVal / 255.;
                datainfo.scoremax = maxVal / 255.;
                datainfo.scoreavg = meanVal[0] / 255.;

                //长短边
                defectmat = cv::Mat(defect);
                minRect = cv::minAreaRect(defectmat);
                if (minRect.size.width < minRect.size.height) {
                    datainfo.longsize = minRect.size.height;
                    datainfo.shortsize = minRect.size.width;
                }
                else {
                    datainfo.longsize = minRect.size.width;
                    datainfo.shortsize = minRect.size.height;
                }
                tmpimage.predictdefects.push_back(defect);
                tmpimage.predictdefectsinfo.push_back(datainfo);
            }
        }
        if (bsavetestresult_) {
            std::string org_img_name = std::string(imageabspath_.toLocal8Bit()) + '/' + tmpimage.filename;
            cv::Mat org_img = cv::imread(org_img_name, cv::IMREAD_COLOR);
            cv::drawContours(org_img, tmpimage.predictdefects, -1, cv::Scalar(0, 0, 255), 2);
            std::string new_img_name = std::string(dir_test_result.toLocal8Bit()) + '/' + tmpimage.filename;
            cv::imwrite(new_img_name, org_img);
        }
    }
   
    return true;
}

bool FileProjectManager::PrepareConvertData() {
    //当前项目是否打开
    if (!projectisopen_) {
        CiWaMsgDialog::MessageSignal("工程", "工程未打开！");
        return false;
    }

    //判断是否存在模型，不存在则不能转换
    QString checkpoint = QString("%1/models/F1_best_model.pth.tar").arg(trainabspath_);
    if (!QFile::exists(checkpoint)) {
        CiWaMsgDialog::MessageSignal("错误", "不存在已训练模型，请先训练！");
        return false;
    }

    //更新配置文件
    QString filename = QString("%1/%2").arg(trainabspath_).arg(QString("test.yaml"));
    QString newname = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg(QString("ainnovision_test.yaml"));
    if (QFile::exists(newname)) {
        QFile::remove(newname);
    }
    if (!QFile::copy(filename, newname)) {
        CiWaMsgDialog::MessageSignal("文件", "图像拷贝失败：" + newname);
        return false;
    }

    return true;
}


std::vector<ainno_contour_info> FileProjectManager::AnalyzeLabeledData(void) {
    std::vector<ainno_contour_info> datainfos;
    cv::Moments moment;
    cv::Mat defect;
    cv::RotatedRect minRect;

#define SAVE_LABELEDDATAINFO
#ifdef SAVE_LABELEDDATAINFO
    std::ofstream outputfile;
    outputfile.open(std::string(trainabspath_.toLocal8Bit()) + "/datainfos.csv");
    outputfile << "filename,area,longsize,shortsize,centerx,centery" << std::endl;
#endif

    for (int i = 0; i < m_stream.imagelist.size(); i++) {
        ainno_image &tmpimage = m_stream.imagelist[i];
        if (tmpimage.labeled 
            && tmpimage.labeltype == AINNO_LABEL_BAD 
            && tmpimage.labeldefects.size()) {
            ainno_contour_info datainfo;
            tmpimage.labeldefectsinfo.clear();
            for (int j = 0; j < tmpimage.labeldefects.size(); j++) {
                //面积
                datainfo.area = cv::contourArea(tmpimage.labeldefects[j]);

                //中心
                moment = cv::moments(tmpimage.labeldefects[j]);
                datainfo.centerx = int(moment.m10 / moment.m00);
                datainfo.centery = int(moment.m01 / moment.m00);

                //长短边
                defect = cv::Mat(tmpimage.labeldefects[j]);
                minRect = cv::minAreaRect(defect);
                if (minRect.size.width < minRect.size.height) {
                    datainfo.longsize = minRect.size.height;
                    datainfo.shortsize = minRect.size.width;
                } else {
                    datainfo.longsize = minRect.size.width;
                    datainfo.shortsize = minRect.size.height;
                }
                tmpimage.labeldefectsinfo.push_back(datainfo);
                datainfos.push_back(datainfo);

#ifdef SAVE_LABELEDDATAINFO
                outputfile << datainfo.area << ',' 
                    << datainfo.longsize << ','
                    << datainfo.shortsize << ','
                    << datainfo.centerx << ','
                    << datainfo.centery
                    << std::endl;
#endif
            }
        }
    }

#ifdef SAVE_LABELEDDATAINFO
    outputfile.close();
#endif


    return datainfos;
}
