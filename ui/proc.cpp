#include "proc.h"
#include "uicommon.h"
#include "solution.h"
#include "qdebug.h"
DebugPROC* g_proc = NULL;


DebugPROC::DebugPROC()
{
	time = new QTime();

	for (int i = 0; i < TIMEID::_MAX;i++)
	{
		m_debugtime[i] = -1;
	}
}
DebugPROC::~DebugPROC()
{
	if (time != NULL)
	{
		delete time;
		time = NULL;
	}
}


void DebugPROC::SetRunTime(TIMEID id, int data)
{
	if (g_proc == NULL)
		return ;

	g_proc->m_debugtime.at(id) = data;
}


int DebugPROC::GetRunTime(TIMEID id)
{
	if (g_proc == NULL)
		return -1;

	return g_proc->m_debugtime.at(id);
}



QString DebugPROC::GetTextBUff()
{
	if (g_proc == NULL)
		return QString();

	QString strini = QString("��ʼ����ʱ:\n�����ļ� =%1\n�㷨 =%2\nUI =%3\n")
		.arg(g_proc->m_debugtime.at(TIMEID::T_INICONFIG))
		.arg(g_proc->m_debugtime.at(TIMEID::T_ALGINIT)) 
		.arg(g_proc->m_debugtime.at(TIMEID::T_UI));
	//T_SAVEIMG
	QString strrun = QString("����״̬��ʱ:\n�ص���ʱ =%1\nBigAndSmall��Fragment =%2\nSeg��PingMianBuPing =%3\nPingMianBuPing =%4\nSaveImg =%5\nAutoSave =%6\nGradeFilter =%7\n\nInsertDatabase =%8\n")
		.arg(g_proc->m_debugtime.at(TIMEID::T_CALLBACK))
		.arg(g_proc->m_debugtime.at(TIMEID::T_ALG_F_B))
		.arg(g_proc->m_debugtime.at(TIMEID::T_ALG_S_P))
		.arg(g_proc->m_debugtime.at(TIMEID::T_ALG_P))
		.arg(g_proc->m_debugtime.at(TIMEID::T_SAVEIMG))
		.arg(g_proc->m_debugtime.at(TIMEID::T_AUTOSAVE))
		.arg(g_proc->m_debugtime.at(TIMEID::T_GRADEFILTER))
		.arg(g_proc->m_debugtime.at(TIMEID::T_ADDDATABASE));

	QString strbutton = QString("������ʱ:\n��ⷽ�� =%1\n�л���ⷽ�� =%2\nϵͳ���� =%3\n")
		.arg(g_proc->m_debugtime.at(TIMEID::T_BTNJIANCHE))
		.arg(g_proc->m_debugtime.at(TIMEID::T_CHANGE_SOLUTION))
		.arg(g_proc->m_debugtime.at(TIMEID::T_BTNSYSTEM));

	QString stralg="";
	for (size_t i = 0; i < Solution::GetCurrentSolution()->get_station(); i++)
	{
		stralg.append(QString("��λ[%7]:\nalgtime_avg: =%1\nalgtime_max =%2\nalgtime_min =%3\nlifetime_avg =%4\nlifetime_max =%5\nlifetime_min =%6\n\n")
		.arg(sln::get_algtime_avg(i))
		.arg(sln::get_algtime_max(i))
		.arg(sln::get_algtime_min(i))
		.arg(sln::get_lifetime_avg(i))
		.arg(sln::get_lifetime_max(i))
		.arg(sln::get_lifetime_min(i))
		.arg(i));
	}
	return QString(strini+ "\n"+ strrun + "\n" +strbutton + "\n" + stralg);
}

void DebugPROC::StartTime()
{
	if (g_proc == NULL)
	{
		g_proc = new DebugPROC();
		g_proc->time->start();
	}
	g_proc->time->restart();
}

void DebugPROC::EndTime(TIMEID id)
{
	if (g_proc == NULL)
		return;
	g_proc->m_debugtime.at(id) = g_proc->time->elapsed();
}

void DebugPROC::EndTime(QTime & dtime, TIMEID id)
{
	if (g_proc == NULL)
		return;

	g_proc->m_debugtime.at(id) = dtime.elapsed();
}

