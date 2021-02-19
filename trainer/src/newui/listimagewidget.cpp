#include "listimagewidget.h"
#include "fileprojectmanager.h"
#include "qtmat.h"
#include <iostream> 
#include "qdir.h"
ListImageWidget::ListImageWidget(QWidget *parent)
	: QListWidget(parent)
{
	ui.setupUi(this);
}

ListImageWidget::~ListImageWidget()
{
}
//清空列表
void ListImageWidget::FileListClear()
{
	this->clear();
 /*	for (int it = 0; it < imagelist_.size(); it++)
	{
		ImageLabel* temp = imagelist_[it];
		if (temp != NULL)
		{
			//单击
			disconnect(temp, SIGNAL(signalSendId(bool, void *)), this, SLOT(OnSignalSendId(bool, void *)));
			//右键删除
			disconnect(temp, SIGNAL(signalDelImage(bool)), this, SLOT(OnSignalDelImage(bool)));
			//右键无缺陷
			disconnect(temp, SIGNAL(signalMenuOperate(bool, int, void*)), this, SLOT(OnSignalMenuOperate(bool, int, void*)));
			//右键总是

			//右键
			disconnect(temp, SIGNAL(signalInitMenu(bool, void*, int)), this, SLOT(OnSignalInitMenu(bool, void*, int)));

			delete temp;
			temp = NULL;
		}
	}
	imagelist_.clear();
	trainedlist_.clear();
	labeledlist_.clear();
	labelbadlist_.clear();
	labelgoodlist_.clear();
	curitemlist_.clear();
	selectitemlist_.clear();
	ui.lcdFileNumber->display(0);
	curitemidx_ = -1;
	curitem_ = NULL;

	*/
}
//更新列表预测结果
void ListImageWidget::FileListUpdatePredictdefects()
{
	/*ainno_image curimg;
	int w = this->width() - 20;
	for (int i = 0; i < imagelist_.size(); i++) {
		curimg = FileProjectManager::Instance()->StreamGetImageInfo(i);
		futurevec_.at(i).waitForFinished();
		imagelist_[i]->UpdateImageInfo(curimg);
		//imagelist_[i]->DrawPredict();
		imagelist_[i]->ResizePredictdefects(w, w);
	}
	*/
}
//更新训练标志
void ListImageWidget::FileListUpdateTrainFlag()
{
	/*ainno_image curimg;
	int w = this->width() - 20;
	for (int i = 0; i < imagelist_.size(); i++) {
		curimg = FileProjectManager::Instance()->StreamGetImageInfo(i);
		imagelist_[i]->UpdateImageInfo(curimg);
		imagelist_[i]->ShowInfo();
		FileListFilterCount(i);
	}
	*/
}
//更新标注信息
void ListImageWidget::FileListUpdateLabeldefects()
{
	/*ainno_image curimg;
	int w = this->width() - 20;
	for (int i = 0; i < imagelist_.size(); i++) {
		curimg = FileProjectManager::Instance()->StreamGetImageInfo(i);
		imagelist_[i]->UpdateImageInfo(curimg);
		imagelist_[i]->setLineSizeWidth(_penSizeValue);//设置线宽
		imagelist_[i]->isFill(_isFillChecked);
		imagelist_[i]->set_keepaspectratio(_isKeepaspectratio);
		imagelist_[i]->DrawLabel();
		imagelist_[i]->ResizeWidetSize(w, w);
		FileListFilterCount(i);
	}
	*/
}
