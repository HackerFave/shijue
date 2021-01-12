#include "solutionicon.h"

SolutionIcon::SolutionIcon(QWidget *parent)
{
	m_issettled = false;
	m_pgridlayout = new QGridLayout();
	m_pgridlayout->setContentsMargins(0, 0, 0, 0);
	

	m_widget = new QWidget();
	m_widget->setMaximumSize(42, 42);
	m_widget->setMinimumSize(42, 42);

	m_widget->setLayout(m_pgridlayout);
	m_widget->setStyleSheet(QString("border: 0px solid; background-image: url(:/resources/icons/ic_count_2.png); "));

	m_lab = new QLabel();

	m_lab->setMaximumSize(32, 32);
	m_lab->setMinimumSize(32, 32);
	m_lab->setAlignment(Qt::AlignCenter);
	m_lab->setStyleSheet(QString("border: 0px solid;border-radius: 16px;background: rgba(255,255,255,0.20);border-color: rgba(255,255,255,0.20);"));
	m_pgridlayout->addWidget(m_lab);
}

SolutionIcon::~SolutionIcon()
{
	if (m_lab != NULL)
	{
		delete m_lab;
		m_lab = NULL;
	}
	if (m_pgridlayout != NULL)
	{
		delete m_pgridlayout;
		m_pgridlayout = NULL;
	}
	if (m_widget != NULL)
	{
		delete m_widget;
		m_widget = NULL;
	}
}


void SolutionIcon::SetFristIcon()
{
	if(m_widget)
		m_widget->setStyleSheet(QString("border: 0px solid; background-image: url(:/resources/icons/ic_count_1.png); "));
}

void SolutionIcon::SetLastIcon()
{
	if (m_widget)
		m_widget->setStyleSheet(QString("border: 0px solid; background-image: url(:/resources/icons/ic_count_3.png); "));
}
void SolutionIcon::SetNormalIcon()
{
	if (m_widget)
		m_widget->setStyleSheet(QString("border: 0px solid; background-image: url(:/resources/icons/ic_count_2.png); "));
}


void SolutionIcon::Settled()
{
	if (m_widget)
	{
		m_lab->setStyleSheet(QString("border: 0px solid;border-radius: 16px;background: #F44336;border-color: rgba(255,255,255,0.20);"));
		m_issettled = true;
	}
}

void SolutionIcon::UnSettled()
{
	if (m_widget)
		m_lab->setStyleSheet(QString("border: 0px solid;border-radius: 16px;background: rgba(255,255,255,0.20);border-color: rgba(255,255,255,0.20);"));
}

void SolutionIcon::Setting()
{
	if (m_widget)
		m_lab->setStyleSheet(QString("border: 0px solid;border-radius: 16px;background: #4BAF4F;border-color: rgba(255,255,255,0.20);"));

}
