#pragma once

namespace threshold {
    typedef struct {
        double ok;
        double ng;
    } thresholdval;

    int deinitialize();

    int initialize();

    int readini();

    int writeini();

    int set(int stationid, std::string defectid, std::string type, thresholdval* pval);

    int get(int stationid, int defectid, int type, thresholdval* pval);

    int get(int stationid, int defectid, std::string type, thresholdval* pval);

    std::string getdefectname(int stationid, int defectid);

    std::string gettypename(int stationid, int defectid, int type);

    int getstationnum();

    int getdefectnum(int stationid);

    int gettypenum(int stationid, int defectid);

    int gettotalnum();
}