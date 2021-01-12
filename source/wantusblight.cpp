#include <iostream>
#include <thread>
#include <io.h> //access
#include <direct.h> //mkdir
#include "windows.h"
#include <tchar.h>
#include <opencv2/opencv.hpp>
#include "common.h"
#include "log.h"
#include "alg.h"
#include "solution.h"
#include "baseparam.h"
#include "debugdata.h"
#include "uicommon.h"
#include "qsettings.h"
#include "qcoreapplication.h"
#include "qfileinfo.h"
#include "qdir.h"
#include "wantusblight.h"

namespace light {
	class LightService {
	public:
		static LightService& instance() {
			static LightService the_instance;
			return the_instance;
		}

	private:
		LightService() = default;

	public:
		void deinitialize() {
			log_info("Enter\n");
			running_ = false;
			condvar_.notify_one();
			thread_.join();

			QByteArray tmpcmd;
			switch (last_cmd_)
			{
			case CMD_RED_OPEN:
				tmpcmd = QByteArray::fromHex(QString::number(0x21, 16).toLatin1());
				break;
			case CMD_YELLOW_OPEN:
				tmpcmd = QByteArray::fromHex(QString::number(0x22, 16).toLatin1());
				break;
			case CMD_GREEN_OPEN:
				tmpcmd = QByteArray::fromHex(QString::number(0x24, 16).toLatin1());
				break;
			default:
				return;
			}
			m_serialport->write(tmpcmd);
			m_serialport->waitForBytesWritten(200);

			CloseSerialPort();
			log_info("Exit\n");
		}
		bool initialize() {
			log_info("Enter\n");
			//FindPort();
			//OpenSerialPort();
			running_ = true;
			thread_ = std::thread(&LightService::run, this);
			log_info("Exit\n");
			return true;
		};
		bool push(Light_cmd cmd) {
			log_info("Enter\n");
			cmd_mutex_.lock();
			cmd_.push_back(cmd);
			cmd_mutex_.unlock();
			condvar_.notify_one();
			log_info("Exit\n");
			return true;
		}
	private:
		void FindPort(){
			//通过QSerialPortInfo查找可用串口
			foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
			{
				QSerialPort serialport;

				serialport.setPort(info);
				if (serialport.open(QIODevice::ReadWrite))
				{
					serialport.setBaudRate(300);
					char data[11] = { "$CH341Ser?" };
					serialport.write(data, 10);
					while (serialport.waitForReadyRead(200))
					{
						QByteArray rdata = serialport.readAll(); //读取串口数据
						if (!rdata.isEmpty())
						{
							if (rdata.at(0) == 'W');
							{
								serialport.close();
								m_serialport = new QSerialPort(info);
								break;
							}
						}
					}
				}
			}
		}

		bool OpenSerialPort(){
			if (m_serialport == NULL)
				return false;

			if (!m_serialport->open(QIODevice::WriteOnly))
				return false;

			// 设置波特率
			m_serialport->setBaudRate(9600);
			//设置数据位数
			m_serialport->setDataBits(QSerialPort::Data8);
			// 设置校验位
			m_serialport->setParity(QSerialPort::NoParity);
			//设置停止位
			m_serialport->setStopBits(QSerialPort::OneStop);
			QByteArray size = QByteArray::fromHex(QString::number(0x21, 16).toLatin1());
			//设置缓存
			//m_serialport->setReadBufferSize(0);
			return true;
		}

		void CloseSerialPort()
		{
			if (m_serialport != NULL)
			{
				m_serialport->close();
				delete m_serialport;
				m_serialport = NULL;
			}
		}

		void run() {
			std::unique_lock<std::mutex> lock(mutex_);

			//FindPort();
			m_serialport = new QSerialPort("COM33");
			OpenSerialPort();

			QByteArray tmpcmd;
			while (running_) {
				//查询是否有数据要处理，没有则休眠
				if (cmd_.empty()) {
					condvar_.wait_for(lock, std::chrono::milliseconds(10));
					continue;
				}

				cmd_mutex_.lock();
				auto cur_cmd = cmd_.front();
				cmd_.pop_front();
				cmd_mutex_.unlock();

				if (!m_serialport) {
					continue;
				}

				switch (last_cmd_)
				{
				case CMD_RED_OPEN:
					tmpcmd = QByteArray::fromHex(QString::number(0x21, 16).toLatin1());
					break;
				case CMD_YELLOW_OPEN:
					tmpcmd = QByteArray::fromHex(QString::number(0x22, 16).toLatin1());
					break;
				case CMD_GREEN_OPEN:
					tmpcmd = QByteArray::fromHex(QString::number(0x24, 16).toLatin1());
					break;
				default:
					return;
				}
				m_serialport->write(tmpcmd);
				m_serialport->waitForBytesWritten(200);

				switch (cur_cmd)
				{
				case CMD_RED_OPEN:
					tmpcmd = QByteArray::fromHex(QString::number(0x11, 16).toLatin1());
					break;
				case CMD_YELLOW_OPEN:
					tmpcmd = QByteArray::fromHex(QString::number(0x12, 16).toLatin1());
					break;
				case CMD_GREEN_OPEN:
					tmpcmd = QByteArray::fromHex(QString::number(0x14, 16).toLatin1());
					break;
				default:
					return;
				}
				m_serialport->write(tmpcmd);
				m_serialport->waitForBytesWritten(200);
				last_cmd_ = cur_cmd;

			}
		}

	private:
		std::mutex mutex_;
		std::condition_variable condvar_;
		std::thread thread_;
		bool running_ = true;
		std::deque<Light_cmd> cmd_;
		std::mutex cmd_mutex_;
		Light_cmd last_cmd_;

		QSerialPort *m_serialport;
	};

	bool deinitialize() {
		LightService::instance().deinitialize();
		return true;
	};

	bool initialize() {
		return LightService::instance().initialize();
	};

	void push(Light_cmd cmd) {
		LightService::instance().push(cmd);
	}
}
