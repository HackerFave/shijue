#pragma once

#include <QListWidget>
#include "ui_listimagewidget.h"

class ListImageWidget : public QListWidget
{
	Q_OBJECT

public:
	ListImageWidget(QWidget *parent = Q_NULLPTR);
	~ListImageWidget();
	void FileListClear();//����б�
	 //�����б�Ԥ����
	void FileListUpdatePredictdefects();
	//����ѵ����־
	void FileListUpdateTrainFlag();
	//���±�ע��Ϣ
	void FileListUpdateLabeldefects();
private:
	Ui::ListImageWidget ui;
};
