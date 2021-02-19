#pragma once
#pragma execution_character_set("utf-8")
enum LabelState
{
    NONE,
    GOOD,
    BAD,
    YES,
    NO,
    COUNT
};

//label:none / good / bad
//labelinfo : 分割标注信息
//train : yes / no
//predict : none / good / bad
//predictinfo : 预测结果信息
struct ImageJsonInfo
{
    LabelState label;
    LabelState labelinfo;
    LabelState train;
    LabelState predict;
    LabelState predictinfo;
};