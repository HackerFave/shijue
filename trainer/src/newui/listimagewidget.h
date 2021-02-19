#pragma once

#include <QListWidget>
#include "ui_listimagewidget.h"

class ListImageWidget : public QListWidget
{
	Q_OBJECT

public:
	ListImageWidget(QWidget *parent = Q_NULLPTR);
	~ListImageWidget();
	void FileListClear();//清空列表
	 //更新列表预测结果
	void FileListUpdatePredictdefects();
	//更新训练标志
	void FileListUpdateTrainFlag();
	//更新标注信息
	void FileListUpdateLabeldefects();
private:
	Ui::ListImageWidget ui;
};
