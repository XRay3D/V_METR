#include "qserialport.h"

#pragma comment (lib, "setupapi.lib")

QSerialPort::QSerialPort(QWidget *parent):QWidget(parent)//,ui(new Ui::ComPortSettings)
{
#ifdef QT_5
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
#else
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
#endif
	if (this->objectName().isEmpty())this->setObjectName(QString::fromUtf8("ComPort"));
	this->resize(10, 298);
	this->setWindowTitle("ComPort");

	verticalLayout = new QVBoxLayout(this);
	verticalLayout->setSpacing(6);
	verticalLayout->setContentsMargins(0, 0, 0, 0);
	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

	label = new QLabel(this);
	label->setObjectName(QString::fromUtf8("label"));
	label->setText("Порт:");
	verticalLayout->addWidget(label);

	combo_port = new QComboBox(this);
	combo_port->setObjectName(QString::fromUtf8("combo_port"));
	verticalLayout->addWidget(combo_port);

#ifdef SPEED
	label_2 = new QLabel(this);
	label_2->setObjectName(QString::fromUtf8("label_2"));
	label_2->setText("Скорость:");
	verticalLayout->addWidget(label_2);

	combo_spd = new QComboBox(this);
	combo_spd->insertItems(0, QStringList()
						   << QString::fromUtf8("110")
						   << QString::fromUtf8("300")
						   << QString::fromUtf8("600")
						   << QString::fromUtf8("1200")
						   << QString::fromUtf8("2400")
						   << QString::fromUtf8("4800")
						   << QString::fromUtf8("9600")
						   << QString::fromUtf8("14400")
						   << QString::fromUtf8("19200")
						   << QString::fromUtf8("38400")
						   << QString::fromUtf8("56000")
						   << QString::fromUtf8("57600")
						   << QString::fromUtf8("115200")
						   << QString::fromUtf8("128000")
						   << QString::fromUtf8("256000")
						   );
	combo_spd->setObjectName(QString::fromUtf8("combo_spd"));
	combo_spd->setEditable(false);
	combo_spd->setMaxVisibleItems(15);
	combo_spd->setFrame(true);
	verticalLayout->addWidget(combo_spd);
	combo_spd->setCurrentIndex(0);
#endif
	Play.addFile(":/play.png", QSize(), QIcon::Normal, QIcon::Off);
	Stop.addFile(":/stop.png", QSize(), QIcon::Normal, QIcon::Off);
	button_port_oc = new QPushButton(this);
	button_port_oc->setObjectName(QString::fromUtf8("button_port_oc"));
	button_port_oc->setText("Открыть");
	button_port_oc->setIcon(Play);
	connect(button_port_oc,SIGNAL(clicked()),this,SLOT(QspOpenClose()));
	verticalLayout->addWidget(button_port_oc);
#ifdef DTR_RTS
	check_dtr = new QCheckBox(this);
	check_dtr->setObjectName(QString::fromUtf8("check_dtr"));
	check_dtr->setText("DTR");
	connect(check_dtr,SIGNAL(clicked(bool)),this,SLOT(ComPinDtrRts()));
	verticalLayout->addWidget(check_dtr);

	check_rts = new QCheckBox(this);
	check_rts->setObjectName(QString::fromUtf8("check_rts"));
	check_rts->setText("RTS");
	connect(check_rts,SIGNAL(clicked(bool)),this,SLOT(ComPinDtrRts()));
	verticalLayout->addWidget(check_rts);
#endif
#ifdef EVENT
	check_event = new QCheckBox(this);
	check_event->setObjectName(QString::fromUtf8("check_event"));
	check_event->setText("Event:");
	verticalLayout->addWidget(check_event);

	spin_event = new QSpinBox(this);
	spin_event->setObjectName(QString::fromUtf8("spin_event"));
	spin_event->setMaximum(255);
	spin_event->setValue(13);
	verticalLayout->addWidget(spin_event);
#endif
#ifdef INDICATORS
	label_cts = new QLabel(this);
	label_cts->setObjectName(QString::fromUtf8("label_cts"));
	label_cts->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
	verticalLayout->addWidget(label_cts);
	label_dsr = new QLabel(this);
	label_dsr->setObjectName(QString::fromUtf8("label_dsr"));
	verticalLayout->addWidget(label_dsr);
	label_ring = new QLabel(this);
	label_ring->setObjectName(QString::fromUtf8("label_ring"));
	verticalLayout->addWidget(label_ring);
	label_rlsd = new QLabel(this);
	label_rlsd->setObjectName(QString::fromUtf8("label_rlsd"));
	verticalLayout->addWidget(label_rlsd);
#endif
	verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	verticalLayout->addItem(verticalSpacer);

#ifdef INDICATORS
	label_cts->setText("<html><head/><body><p><img src=\":/off.png\"/> CTS</p></body></html>");
	label_dsr->setText("<html><head/><body><p><img src=\":/off.png\"/> DSR</p></body></html>");
	label_ring->setText("<html><head/><body><p><img src=\":/off.png\"/> RING</p></body></html>");
	label_rlsd->setText("<html><head/><body><p><img src=\":/off.png\"/> RLSD</p></body></html>");
#endif
	//	End setup Ui=============================
	//==============================
	hComPort=0;
	PortList= new SERIAL_PORT;
	doEnumComPort(PortList);
	combo_port->insertItems(0, PortList->ShortName);//->PortFriendlyName);

	//==============================
	Tx = new TxThread;
	Tx->hComPort=&hComPort;
	Tx->bufwr=BufWr;
	Tx->fl=&FlWr;
	Rx = new RxThread;
	Rx->hComPort=&hComPort;
	Rx->COM=this;
	Rx->ComReadS=&QSerialPort::Read_QStr;
	Rx->ComReadC=&QSerialPort::Read_Char;
	Rx->bufrd=BufRd;
	Rx->RdStr=&RdStr;
	Rx->FlRd=&FlRd;
	memset(BufWr,0,BUFSIZE);
	memset(BufRd,0,BUFSIZE);
#ifdef CTS
	Rx->S_ComCTS=&QSerialPort::CTS_Event;
#endif
#ifdef DSR
	Rx->S_ComDSR=&QSerialPort::DSR_Event;
#endif
#ifdef RING
	Rx->S_ComRING=&QSerialPort::RING_Event;
#endif
#ifdef RLSD
	Rx->S_ComRLSD=&QSerialPort::RLSD_Event;
#endif
	Rx->S_ComPinStatus=&QSerialPort::Pin_Status;
	//==============================
#ifdef INDICATORS
	connect(this,SIGNAL(Pin_Status(int)),this,SLOT(SetIndicators(int)));
#endif
	SPD=9600;
	ComEventFlag=false;
	//ReadSettings();
}

QSerialPort::~QSerialPort()
{
	qDebug("~ComPortSettings");
	WriteSettings();
	if(hComPort)ClosePort();
	delete Tx;
	delete Rx;
	delete PortList;
}

//=============================================================
QString QSerialPort::AsciiToQstring(char *ascii)
{
	static QTextCodec *codec=QTextCodec::codecForName("Windows-1251");
	return codec->toUnicode(ascii);
}

QString QSerialPort::AsciiToQstring(char *ascii, int i)
{
	static QTextCodec *codec=QTextCodec::codecForName("Windows-1251");
	return codec->toUnicode(ascii,i);
}

void QSerialPort::AsciiToQstring(QString *str, char *ascii)
{
	static QTextCodec *codec=QTextCodec::codecForName("Windows-1251");
	*str	= codec->toUnicode(ascii);
}

void QSerialPort::QstringToAscii(QString str, char *ascii)
{
	static QTextCodec *codec=QTextCodec::codecForName("Windows-1251");
	QByteArray ba=codec->fromUnicode(str);
	memcpy(ascii,ba.data(),ba.size());
	ascii[ba.size()]=0;
}

bool QSerialPort::isRead()
{
	if(FlRd)
	{
		FlRd=false;
		return true;
	}
	else return false;
}
//=============================================================
void QSerialPort::closeEvent(QCloseEvent *event)
{
	qDebug("closeEvent");
	event->accept();
}

void QSerialPort::hideEvent(QHideEvent *event)
{
	qDebug("hideEvent");
	event->accept();
}

bool QSerialPort::doEnumComPort(LPSERIAL_PORT Port)
{
	GUID GUID_CLASS_COMPORT={0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73};
	QString TmpStr;
	HDEVINFO hDeviceInfoSet = SetupDiCreateDeviceInfoList(NULL, NULL);
	if (hDeviceInfoSet != INVALID_HANDLE_VALUE)
	{
		// Retrieve the device information set for the interface class.
		HDEVINFO hNewDeviceInfoSet = SetupDiGetClassDevsEx(&GUID_CLASS_COMPORT,NULL,NULL,DIGCF_PRESENT | DIGCF_DEVICEINTERFACE,hDeviceInfoSet,NULL,NULL);
		if (hNewDeviceInfoSet != INVALID_HANDLE_VALUE)
		{
			SP_DEVICE_INTERFACE_DATA itf = { sizeof(SP_DEVICE_INTERFACE_DATA) };
			for (DWORD dw=0; SetupDiEnumDeviceInterfaces(hNewDeviceInfoSet, NULL,&GUID_CLASS_COMPORT, dw, &itf); ++dw)
			{
				DWORD dwSize=0;
				DWORD dwType=0;
				std::vector<BYTE> buffer;
				TCHAR szDeviceName[_MAX_PATH];
				TCHAR szUserName[_MAX_PATH];
				SP_DEVINFO_DATA dev = { sizeof(SP_DEVINFO_DATA) };

				SetupDiGetDeviceInterfaceDetail(hNewDeviceInfoSet, &itf, NULL, 0,&dwSize, &dev);
				if (dwSize>0)
				{
					buffer.resize(dwSize);
					SP_DEVICE_INTERFACE_DETAIL_DATA* pDetail =
							reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA*>(&buffer[0]);
					pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

					SetupDiGetDeviceInterfaceDetail(hNewDeviceInfoSet, &itf, pDetail,dwSize, NULL, &dev);
					//qDebug()<<QString::fromWCharArray((const wchar_t*)pDetail->DevicePath);
					Port->PortPath<<QString::fromWCharArray((const wchar_t*)pDetail->DevicePath);
					//wcscpy_s(szDeviceName,_countof(szDeviceName),pDetail->DevicePath);

				}
				else
				{
					szDeviceName[0] = 0;
				}

				SetupDiGetDeviceRegistryProperty(hNewDeviceInfoSet, &dev,/*SPDRP_FRIENDLYNAME*/SPDRP_FRIENDLYNAME, &dwType, NULL, 0, &dwSize);
				if (dwSize>0)
				{
					buffer.resize(dwSize);
					SetupDiGetDeviceRegistryProperty(hNewDeviceInfoSet, &dev,/*SPDRP_FRIENDLYNAME*/SPDRP_FRIENDLYNAME, NULL, &buffer[0], dwSize, NULL);
					//qDebug()<<QString::fromWCharArray(reinterpret_cast<TCHAR*>(&buffer[0]));
					Port->PortFriendlyName<<QString::fromWCharArray(reinterpret_cast<TCHAR*>(&buffer[0]));
					//wcscpy_s(szUserName,_countof(szUserName),reinterpret_cast<TCHAR*>(&buffer[0]));
					TmpStr=QString::fromWCharArray(reinterpret_cast<TCHAR*>(&buffer[0]));
					Port->ShortName<<TmpStr.mid(TmpStr.lastIndexOf("(")+1,TmpStr.mid(TmpStr.lastIndexOf("(")+1).length()-1);               // z == "pineapples"
				}
				else
				{
					szUserName[0] = 0;
				}
			}
		}
		SetupDiDestroyDeviceInfoList(hDeviceInfoSet);
	}
	return TRUE;
}

bool QSerialPort::OpenPort()
{

	COMMTIMEOUTS timeouts;
	wchar_t buf[2048];
	buf[PortList->PortPath.at(combo_port->currentIndex()).toWCharArray((wchar_t*)&buf)]=0;
	hComPort = CreateFile((const wchar_t*)&buf,
						  GENERIC_READ | GENERIC_WRITE,//доступ к порту на чтение/записть
						  0,//порт не может быть общедоступным (shared)
						  NULL,//дескриптор порта не наследуется, используется дескриптор безопасности по умолчанию
						  OPEN_EXISTING,//порт должен открываться как уже существующий файл
						  FILE_FLAG_OVERLAPPED,//этот флаг указывает на использование асинхронных операций
						  NULL//указатель на файл шаблона не используется при работе с портами
						  );

	if (hComPort==INVALID_HANDLE_VALUE){
		qDebug()<< "Не удалось открыть порт";
		hComPort = 0;
		return false;
	}
	//инициализация порта
	dcb.DCBlength = sizeof(DCB);
	if(!GetCommState(hComPort, &dcb))
	{
		CloseHandle(hComPort);
		qDebug()<< "Не удалось считать DCB";
		return false;
	}
	//инициализация структуры DCB
#ifdef SPEED
	dcb.BaudRate = combo_spd->currentText().toULong();             //baudrate
#else
	dcb.BaudRate = SPD;
#endif
	dcb.fBinary = TRUE;                              //включаем двоичный режим обмена
	dcb.fOutxCtsFlow = FALSE;                        //выключаем режим слежения за сигналом CTS
	dcb.fOutxDsrFlow = FALSE;                        //выключаем режим слежения за сигналом DSR
	dcb.fDtrControl = DTR_CONTROL_DISABLE;           //отключаем использование линии DTR
	dcb.fDsrSensitivity = FALSE;                     //отключаем восприимчивость драйвера к состоянию линии DSR
	dcb.fNull = FALSE;                               //разрешить приём нулевых байтов
	dcb.fRtsControl = RTS_CONTROL_DISABLE;           //отключаем использование линии RTS
	dcb.fAbortOnError = FALSE;                       //отключаем остановку всех операций чтения/записи при ошибке
	dcb.ByteSize = 8;                                //задаём 8 бит в байте
	dcb.Parity = 0;                                  //отключаем проверку чётности
	dcb.StopBits = 0;                                //задаём один стоп-бит
#ifdef EVENT
	dcb.EvtChar = spin_event->value();           //сигнал при приходе байта равного 13 (по умолчанию)
#else
	dcb.EvtChar = 13;
#endif


	if(!SetCommState(hComPort, &dcb))
	{
		CloseHandle(hComPort);
		qDebug()<< "Не удалось установить DCB";
		return false;
	}

	timeouts.ReadIntervalTimeout = 0;	 	//таймаут между двумя символами
	timeouts.ReadTotalTimeoutMultiplier = 0;	//общий таймаут операции чтения
	timeouts.ReadTotalTimeoutConstant = 0;       //константа для общего таймаута операции чтения
	timeouts.WriteTotalTimeoutMultiplier = 0;    //общий таймаут операции записи
	timeouts.WriteTotalTimeoutConstant = 0;      //константа для общего таймаута операции записи

	if(!SetCommTimeouts(hComPort, &timeouts))
	{
		CloseHandle(hComPort);
		qDebug()<< "Не удалось установить тайм-ауты";
		return false;
	}
	//установить размеры очередей приёма и передачи
	SetupComm(hComPort,1000,1000);
	PurgeComm(hComPort, PURGE_RXCLEAR);	//очистить принимающий буфер порта
#ifdef EVENT
	Rx->EventFlag=check_event->isChecked();
#else
	Rx->EventFlag=ComEventFlag;
#endif
	Rx->start();//QThread::NormalPriority);
	button_port_oc->setText(/*QApplication::translate("ComPortSettings", */"Закрыть"/*, 0, QApplication::UnicodeUTF8)*/);
	button_port_oc->setIcon(Stop);
#ifdef EVENT
	check_event->setEnabled(false);
	spin_event->setEnabled(false);
#endif
	combo_port->setEnabled(false);
#ifdef SPEED
	combo_spd->setEnabled(false);
#endif
	qDebug()<<"Порт" <<PortList->PortFriendlyName.at(combo_port->currentIndex())<<"открыт";
	return true;
}

bool QSerialPort::ClosePort()
{
	Rx->terminate();
	bool fl=CloseHandle(hComPort);
	CloseHandle(Rx->overlapped.hEvent);
	hComPort=0;
	button_port_oc->setText(/*QApplication::translate("ComPortSettings", */"Открыть"/*, 0, QApplication::UnicodeUTF8)*/);
	button_port_oc->setIcon(Play);
#ifdef EVENT
	check_event->setEnabled(true);
	spin_event->setEnabled(true);
#endif
	combo_port->setEnabled(true);
#ifdef SPEED
	combo_spd->setEnabled(true);
#endif
	qDebug()<<"Порт"<<PortList->PortFriendlyName.at(combo_port->currentIndex())<<"закрыт"<<fl;
	return fl;
}

void QSerialPort::ReadSettings()
{
	QSettings settings("ComPort.ini" ,QSettings::IniFormat);
	settings.beginGroup("window");
	if(settings.value("name",false).toBool())combo_port->insertItems(0, PortList->PortFriendlyName);
	else combo_port->insertItems(0, PortList->ShortName);
#ifdef INDICATORS
	if(settings.value("pins",false).toBool())
	{
		label_cts->setHidden(true);
		label_dsr->setHidden(true);
		label_ring->setHidden(true);
		label_rlsd->setHidden(true);
	}
	else
	{
		label_cts->setHidden(false);
		label_dsr->setHidden(false);
		label_ring->setHidden(false);
		label_rlsd->setHidden(false);
	}
#endif
	settings.endGroup();

	settings.beginGroup("port");
	combo_port->setCurrentIndex(settings.value("prt",0).toInt());
#ifdef SPEED
	combo_spd->setCurrentIndex(settings.value("spd",0).toInt());
#endif
#ifdef DTR_RTS
	check_dtr->setChecked(settings.value("dtr",0).toBool());
	check_rts->setChecked(settings.value("rts",0).toBool());
#endif
#ifdef EVENT
	check_event->setChecked(settings.value("event",true).toBool());
	spin_event->setValue(settings.value("spin_event",13).toInt());
#endif
	settings.endGroup();
}

void QSerialPort::WriteSettings()
{
	QSettings settings("ComPort.ini" ,QSettings::IniFormat);
	settings.beginGroup("window");
	settings.endGroup();
	settings.beginGroup("port");
	settings.setValue("prt", combo_port->currentIndex());
#ifdef SPEED
	settings.setValue("spd", combo_spd->currentIndex());
#endif
#ifdef DTR_RTS
	settings.setValue("dtr",check_dtr->isChecked());
	settings.setValue("rts",check_rts->isChecked());
#endif
#ifdef EVENT
	settings.setValue("event",check_event->isChecked());
	settings.setValue("spin_event",spin_event->value());
#endif
	settings.endGroup();
}
#ifdef INDICATORS
void QSerialPort::SetIndicators(int Pin)
{
	QString s;
	s = (Pin & MS_CTS_ON) ? "<html><head/><body><p><img src=\":/on.png\"> CTS</p></body></html>" : "<html><head/><body><p><img src=\":/off.png\"> CTS</p></body></html>";
	label_cts->setText(s);
	s = Pin & MS_DSR_ON ? "<html><head/><body><p><img src=\":/on.png\"> DSR</p></body></html>" : "<html><head/><body><p><img src=\":/off.png\"> DSR</p></body></html>";
	label_dsr->setText(s);
	s = Pin & MS_RING_ON ? "<html><head/><body><p><img src=\":/on.png\"> RING</p></body></html>" : "<html><head/><body><p><img src=\":/off.png\"> RING</p></body></html>";
	label_ring->setText(s);
	s = Pin & MS_RLSD_ON ? "<html><head/><body><p><img src=\":/on.png\"> RLSD</p></body></html>" : "<html><head/><body><p><img src=\":/off.png\"> RLSD</p></body></html>";
	label_rlsd->setText(s);
}
#endif
HANDLE QSerialPort::QspOpenClose()
{
	button_port_oc->setEnabled(false);
	if (!hComPort/*&&OpenPort()*/)
	{
		if(OpenPort())
		{
#ifdef DTR_RTS
			ComPinDtrRts();
#endif
#ifdef INDICATORS
			DWORD PinStatus;
			GetCommModemStatus(hComPort,&PinStatus);
			SetIndicators(PinStatus);
#endif
		}
	}
	else if(hComPort)
	{
		ClosePort();
	}
	button_port_oc->setEnabled(true);
	return hComPort;
}
#ifdef DTR_RTS
void QSerialPort::ComPinDtrRts()
{
	if(check_dtr->isChecked()) EscapeCommFunction(hComPort,SETDTR);
	else EscapeCommFunction(hComPort,CLRDTR);
	if(check_rts->isChecked()) EscapeCommFunction(hComPort,SETRTS);
	else EscapeCommFunction(hComPort,CLRRTS);
}
#endif
void QSerialPort::Write(char *ch, int lehght)
{
	if(hComPort)
	{
		Tx->lenght=(DWORD)lehght;
		Tx->bufwr=ch;
		Tx->start();
	}
}

void QSerialPort::Write(char *ch)
{
	if(hComPort)
	{
		Tx->lenght=(DWORD)strlen((char*) &ch);
		QstringToAscii(QString::fromUtf8(ch),Tx->bufwr);
		Tx->start();
	}
}

void QSerialPort::Write(QString str)
{
	if(hComPort)
	{
		Tx->lenght=str.length();
		QstringToAscii(str,Tx->bufwr);
		Tx->start(QThread::HighestPriority);
	}
}

void QSerialPort::QspSetPinDTR(bool fl)
{
	if(fl) EscapeCommFunction(hComPort,SETDTR);
	else EscapeCommFunction(hComPort,CLRDTR);
#ifdef DTR_RTS
	check_dtr->setChecked(fl);
#endif
}

void QSerialPort::QspSetPinRTS(bool fl)
{
	if(fl) EscapeCommFunction(hComPort,SETRTS);
	else EscapeCommFunction(hComPort,CLRRTS);
#ifdef DTR_RTS
	check_rts->setChecked(fl);
#endif
}
///////////////////////////////////////////////////////////
/*	╔═══════════════════════════╗
 *	║ Поток записи				║
	╚═══════════════════════════╝*/
RxThread::RxThread()
{
	connect(this,SIGNAL(destroyed()),this,SLOT(terminate()));
}

RxThread::~RxThread()
{
	if(overlapped.hEvent)CloseHandle(overlapped.hEvent);	//перед выходом из потока закрыть объект-событие
}

void RxThread::run()
{
	static QTextCodec *codec=QTextCodec::codecForName("Windows-1251");
	static COMSTAT comstat;//структура текущего состояния порта, в данной программе используется для определения количества принятых в порт байтов
	static DWORD Temp, Mask, Signal,Event,PinStatus;	//переменная temp используется в качестве заглушки
	overlapped.hEvent = CreateEvent(NULL, true, true, NULL);//создать сигнальный объект-событие для асинхронных операций
	overlapped.Internal=0;
	overlapped.InternalHigh=0;
	overlapped.InternalHigh=0;
	overlapped.Offset=0;
	overlapped.OffsetHigh=0;
	// установить маску на срабатывание по событию приёма
#ifdef EVENT
	if(EventFlag) Event=EV_RXFLAG;
	else Event=EV_RXCHAR;
#else
	Event=EV_RXCHAR;
#endif

	SetCommMask(*hComPort, Event| EV_CTS | EV_DSR | EV_RING | EV_RLSD);
	while(*hComPort)//пока поток не будет прерван, выполняем цикл
	{
		WaitCommEvent(*hComPort, &Mask, &overlapped); 	// ожидать события приёма байта
		// (это и есть перекрываемая операция)
		Signal = WaitForSingleObject(overlapped.hEvent, INFINITE);	//приостановить поток до прихода байта
		if(Signal == WAIT_OBJECT_0)				 //если событие прихода байта произошло
		{
			if(GetOverlappedResult(*hComPort, &overlapped, &Temp, true)) // проверяем, успешно ли завершилась
			{
				if(Mask & (EV_CTS | EV_DSR | EV_RING | EV_RLSD))
				{
					GetCommModemStatus(*hComPort,&PinStatus);
					emit (COM->*S_ComPinStatus)(PinStatus);
					bool PinFlag;
#ifdef CTS

					if((Mask & EV_CTS)!=0)					//если произошло именно событие EV_CTS
					{
						PinFlag = PinStatus & MS_CTS_ON ? true : false;
						emit (COM->*S_ComCTS)(PinFlag);
					}
#endif
#ifdef DSR
					if((Mask & EV_DSR)!=0)					//если произошло именно событие EV_DSR
					{
						PinFlag = PinStatus & MS_DSR_ON ? true : false;
						emit (COM->*S_ComDSR)(PinFlag);
					}
#endif
#ifdef RING
					if((Mask & EV_RING)!=0)					//если произошло именно событие EV_RING
					{
						PinFlag = PinStatus & MS_RING_ON ? true : false;
						emit (COM->*S_ComRING)(PinFlag);
					}
#endif
#ifdef RLSD
					if((Mask & EV_RLSD)!=0)					//если произошло именно событие EV_RLSD
					{
						PinFlag = PinStatus & MS_RLSD_ON ? true : false;
						emit (COM->*S_ComRLSD)(PinFlag);
					}
#endif
				}
				if(Mask & Event)//если произошло событие прихода данных.
				{
					memset(bufrd,0,BUFSIZE);//обнулить буфер
					ClearCommError(*hComPort, &Temp, &comstat);	//нужно заполнить структуру COMSTAT для получения количества данных
					if(comstat.cbInQue)//если действительно есть байты для чтения
					{
						ReadFile(*hComPort, bufrd, comstat.cbInQue, &Temp, &overlapped);
						emit (COM->*ComReadC)(bufrd,comstat.cbInQue);
						emit (COM->*ComReadS)(codec->toUnicode(bufrd,comstat.cbInQue));
						*RdStr=codec->toUnicode(bufrd,comstat.cbInQue);
						*FlRd=true;
					}
				}
			}
		}
	}
	CloseHandle(overlapped.hEvent);	//перед выходом из потока закрыть объект-событие
	overlapped.hEvent=0;
	exit();
}

void RxThread::is_terminated()
{
	if(overlapped.hEvent)
	{
		qDebug()<<"is_terminated";
		CloseHandle(overlapped.hEvent);	//перед выходом из потока закрыть объект-событие
		overlapped.hEvent=0;
	}
}

void TxThread::run()
{
	DWORD temp, signal;	//temp - переменная-заглушка
	qDebug()<<bufwr[0]<<"WR";
	//создать событие
	for(lenght=BUFSIZE;!bufwr[lenght-1];lenght--);
	if (lenght)
	{
		overlapped.hEvent = CreateEvent(NULL, true, true, NULL);
		WriteFile(*hComPort, bufwr, lenght, &temp, &overlapped);//записать байты в порт (перекрываемая операция!)
		signal = WaitForSingleObject(overlapped.hEvent, INFINITE);//приостановить поток, пока не завершится перекрываемая операция WriteFile
		if((signal == WAIT_OBJECT_0) && (GetOverlappedResult(*hComPort, &overlapped, &temp, true))) *fl = true;//если операция завершилась успешно, установить соответствующий флажок
		else *fl = false;
		CloseHandle(overlapped.hEvent);	//перед выходом из потока закрыть объект-событие
		overlapped.hEvent=0;
	}
	memset(bufwr,0,BUFSIZE);
	exit();
}
