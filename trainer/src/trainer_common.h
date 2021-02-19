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
//labelinfo : �ָ��ע��Ϣ
//train : yes / no
//predict : none / good / bad
//predictinfo : Ԥ������Ϣ
struct ImageJsonInfo
{
    LabelState label;
    LabelState labelinfo;
    LabelState train;
    LabelState predict;
    LabelState predictinfo;
};