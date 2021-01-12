#include <map>
#include <vector>
#include "qsettings.h"
#include "qcoreapplication"
#include "uicommon.h"
#include "solution.h"
#include "threshold.h"

namespace threshold {
    class ThresholdService {
    public:
        static ThresholdService& instance() {
            static ThresholdService the_instance;
            return the_instance;
        }

    private:
        ThresholdService() = default;

    public:
        int deinitialize() {
            return 0;
        };

        int initialize() {
            
            return 0;
        };

        int readini() {
            pixelpermm_ = 1;
            totalnum_ = 0;
            thresholds_.clear();
            defectnamemap_.clear();
            typenamemap_.clear();
            QString inifile = QCoreApplication::applicationDirPath() + QString(SOLUTION) 
                + Solution::GetCurrentSolution()->get_solutionname() + '/' + QString(THRESHOLDNAME);
            QSettings configini(inifile, QSettings::IniFormat);
            configini.setIniCodec("UTF-8");
            QStringList keylist = configini.childGroups();
            for (size_t i = 0; i < keylist.length(); i++)
            {
                QString key = keylist[i];
                if ("global" == key) {
                    configini.beginGroup(key);
                    pixelpermm_ = configini.value("pixelpermm").toInt();
                    configini.endGroup();
                } else {
                    QStringList tmplist = key.split("_");
                    if (2 == tmplist.size()) {
                        int stationid = tmplist[0].toInt();
                        int defectid = tmplist[1].toInt();
                        configini.beginGroup(key);
                        QStringList childkeylist = configini.childKeys();
                        int childkeyidx = 0;
                        for (size_t j = 0; j < childkeylist.length(); j++)
                        {
                            QString childkey = childkeylist[j];
                            if ("name" == childkey) {
                                std::string defectname = std::string(configini.value(childkey).toString().toLocal8Bit());
                                defectnamemap_[stationid][defectid] = defectname;
                            } else {
                                QStringList childkeyval = configini.value(childkey).toString().split(",");
                                if (2 == childkeyval.size()) {
                                    thresholdval val;
                                    val.ok = childkeyval[0].toDouble();
                                    val.ng = childkeyval[1].toDouble();
                                    thresholds_[stationid][defectid][childkeyidx] = val;
                                    typenamemap_[stationid][defectid][childkeyidx] = std::string(childkey.toLocal8Bit());
                                    totalnum_++;
                                    childkeyidx++;
                                }
                            }
                        }
                        configini.endGroup();
                    }
                }
            }

            return 0;
        };

        int writeini() {
            QString inifile = QCoreApplication::applicationDirPath() + QString(SOLUTION)
                + Solution::GetCurrentSolution()->get_solutionname() + '/' + QString(THRESHOLDNAME);
            QSettings configini(inifile, QSettings::IniFormat);
            configini.setIniCodec("UTF-8");
            QStringList keylist = configini.childGroups();
            for (size_t i = 0; i < keylist.length(); i++)
            {
                QString key = keylist[i];
                if ("global" == key) {
                    configini.beginGroup(key);
                    configini.setValue("pixelpermm", QString::number(pixelpermm_));
                    configini.endGroup();
                }
                else {
                    QStringList tmplist = key.split("_");
                    if (2 == tmplist.size()) {
                        int stationid = tmplist[0].toInt();
                        int defectid = tmplist[1].toInt();
                        configini.beginGroup(key);
                        QStringList childkeylist = configini.childKeys();
                        int childkeyidx = 0;
                        for (size_t j = 0; j < childkeylist.length(); j++)
                        {
                            QString childkey = childkeylist[j];
                            if ("name" == childkey) {
                                configini.setValue(childkey, QString::fromLocal8Bit(defectnamemap_[stationid][defectid].c_str()));
                            }
                            else {
                                for (int k = 0; k < thresholds_[stationid][defectid].size(); k++) {
                                    if (typenamemap_[stationid][defectid][k] == std::string(childkey.toLocal8Bit())) {
                                        thresholdval *pval = &(thresholds_[stationid][defectid][k]);
                                        configini.setValue(childkey, QString::number(pval->ok, 'f', 2) + "," + QString::number(pval->ng, 'f', 2));
                                        break;
                                    }
                                }
                            }
                        }
                        configini.endGroup();
                    }
                }
            }
            return 0;
        }

        int set(int stationid, std::string defect, std::string type, thresholdval *pval) {
            //根据缺陷名称，查找对应的索引
            int defect_id = -1;
            for (int i = 0; i < thresholds_[stationid].size(); i++) {
                if (defectnamemap_[stationid][i] == defect) {
                    defect_id = i;
                    break;
                }
            }
            if (-1 == defect_id) {
                return -1;
            }

            //根据类型名称，查找对应的索引
            int type_id = -1;
            for (int i = 0; i < thresholds_[stationid][defect_id].size(); i++) {
                if (typenamemap_[stationid][defect_id][i] == type) {
                    type_id = i;
                    break;
                }
            }
            if (-1 == type_id) {
                return -1;
            }

            thresholds_[stationid][defect_id][type_id] = *pval;
            return 0;
        };

        int get(int stationid, int defectid, int type, thresholdval *pval) {
            *pval = thresholds_[stationid][defectid][type];
            return 0;
        };

        int get(int stationid, int defectid, std::string type, thresholdval *pval) {
            for (int i = 0; i < thresholds_[stationid][defectid].size(); i++) {
                if (typenamemap_[stationid][defectid][i] == type) {
                    *pval = thresholds_[stationid][defectid][i];
                    return 0;
                }
            }
            return 1;
        };

        std::string getdefectname(int stationid, int defectid) {
            return defectnamemap_[stationid][defectid];
        };

        std::string gettypename(int stationid, int defectid, int type) {
            return typenamemap_[stationid][defectid][type];
        };

        int getstationnum() {
            return thresholds_.size();
        };

        int getdefectnum(int stationid) {
            return thresholds_[stationid].size();
        };

        int gettypenum(int stationid, int defectid) {
            return thresholds_[stationid][defectid].size();
        };

        int gettotalnum() {
            return totalnum_;
        };
    private:
        //工位， 缺陷类型， 阈值类型，<OK阈值， NG阈值>
        std::map<int, std::map<int, std::map<int, thresholdval > > > thresholds_;
        std::map<int, std::map<int, std::string> > defectnamemap_;
        std::map<int, std::map<int, std::map<int, std::string> > > typenamemap_;
        int pixelpermm_;
        int totalnum_;
    };

    int deinitialize() {
        return ThresholdService::instance().deinitialize();
    };

    int initialize() {
        return ThresholdService::instance().initialize();
    };

    int readini() {
        return ThresholdService::instance().readini();
    };

    int writeini() {
        return ThresholdService::instance().writeini();
    };

    int set(int stationid, std::string defect, std::string type, thresholdval* pval) {
        return ThresholdService::instance().set(stationid, defect, type, pval);
    };

    int get(int stationid, int defectid, int type, thresholdval* pval) {
        return ThresholdService::instance().get(stationid, defectid, type, pval);
    };

    int get(int stationid, int defectid, std::string type, thresholdval* pval) {
        return ThresholdService::instance().get(stationid, defectid, type, pval);
    };

    std::string getdefectname(int stationid, int defectid) {
        return ThresholdService::instance().getdefectname(stationid, defectid);
    };

    std::string gettypename(int stationid, int defectid, int type) {
        return ThresholdService::instance().gettypename(stationid, defectid, type);
    };

    int getstationnum() {
        return ThresholdService::instance().getstationnum();
    };
    
    int getdefectnum(int stationid) {
        return ThresholdService::instance().getdefectnum(stationid);
    };

    int gettypenum(int stationid, int defectid) {
        return ThresholdService::instance().gettypenum(stationid, defectid);
    };

    int gettotalnum() {
        return ThresholdService::instance().gettotalnum();
    };
}