#ifndef COMPORTSETTINGS_H
#define COMPORTSETTINGS_H

#include <QCloseEvent>
#include <QDebug>
#include <QHideEvent>
#include <QSettings>
#include <QStringList>
#include <QtCore/QVariant>
#include <QTextCodec>
#ifdef QT_5
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>
#else
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#endif
#include <QThread>
//==============================
#include <windows.h>
#include <SetupAPI.h>
//==============================

#define BUFSIZE 500

//#define CTS
//#define DSR
//#define RING
//#define RLSD
//#define INDICATORS
//#define DTR_RTS
//#define EVENT
//#define SPEED

typedef struct
{
	QStringList PortPath;
	QStringList PortFriendlyName;
	QStringList ShortName;
} SERIAL_PORT,*LPSERIAL_PORT;

class TxThread;
class RxThread;

class QSerialPort : public QWidget
{
	Q_OBJECT
public:
	explicit QSerialPort(QWidget *parent = 0);
	~QSerialPort();
	//======================================
	QVBoxLayout *verticalLayout;
	QLabel *label;
	QComboBox *combo_port;
#ifdef SPEED
	QLabel *label_2;
	QComboBox *combo_spd;
#endif
	QPushButton *button_port_oc;
	QIcon Stop,Play;
#ifdef DTR_RTS
	QCheckBox *check_dtr;
	QCheckBox *check_rts;
#endif
#ifdef EVENT
	QCheckBox *check_event;
	QSpinBox *spin_event;
#endif
#ifdef INDICATORS
	QLabel *label_cts;
	QLabel *label_dsr;
	QLabel *label_ring;
	QLabel *label_rlsd;
#endif
	QSpacerItem *verticalSpacer;
	//=======================================
	//Конвертеры для ascii <=> unicode
	QString AsciiToQstring(char *ascii);
	QString AsciiToQstring(char *ascii,int i);
	void AsciiToQstring(QString *str,char *ascii);
	void QstringToAscii(QString str,char *ascii);
	char BufRd[BUFSIZE];
	QString RdStr;
	char BufWr[BUFSIZE];
	bool ComEventFlag;
	bool FlWr;	//	Флаг успешности записи.
	bool FlRd;	//	Флаг успешности чтения.
	bool isRead();

private:
//	Ui::ComPortSettings *ui;
	HANDLE hComPort;
	DCB dcb;
	long SPD;
	LPSERIAL_PORT PortList;
	bool doEnumComPort(LPSERIAL_PORT Port);
	//==============================
	TxThread *Tx;
	RxThread *Rx;
	//==============================
	void ReadSettings();
	void WriteSettings();
	bool OpenPort();
	bool ClosePort();

protected:
	void closeEvent(QCloseEvent *event);
	void hideEvent(QHideEvent *event);

private slots:
#ifdef DTR_RTS
	void ComPinDtrRts();
#endif
#ifdef INDICATORS
	void SetIndicators(int Pin);
#endif

public slots:
	void QspSetSpeed(long Spd){SPD=Spd;}
	long QspGetSpeed(){return SPD;}
	HANDLE QspOpenClose();
	void Write(char *ch,int lehght);
	void Write(char *ch);
	void Write(QString str);
	void QspSetPinDTR(bool fl);
	void QspSetPinRTS(bool fl);

signals:
	void Read_QStr(QString Str);
	void Read_Char(char *Ch,int Lenght);
	void Pin_Status (int PinStatus);
#ifdef CTS
	void CTS_Event (bool Pin);//С
#endif
#ifdef DSR
	void DSR_Event (bool Pin);
#endif
#ifdef RING
	void RING_Event (bool Pin);
#endif
#ifdef RLSD
	void RLSD_Event (bool Pin);
#endif

};
/*	╔═══════════════════════════╗
 *	║	Поток чтения			║
	╚═══════════════════════════╝*/
class RxThread : public QThread
{
	Q_OBJECT
public:
	explicit RxThread();
	~RxThread();
	HANDLE *hComPort;		//	Указатель на хэндл файла.
	char *bufrd;			//	Указатель на буфер передачи.
	QString *RdStr;
	bool EventFlag;			//  Флаг для приёма всего подряд или по событию эвент-чар
	bool *FlRd;
	OVERLAPPED overlapped;

	/*Очень хитрожопый вызов сигналов в другом (основном) классе, Для упрощения
	  последуюшего исползования и профита в быстродействии)))*/
	QSerialPort *COM;
	void (QSerialPort::*ComReadS)(QString);
	void (QSerialPort::*ComReadC)(char*,int);
	void (QSerialPort::*S_ComCTS)(bool);
	void (QSerialPort::*S_ComDSR)(bool);
	void (QSerialPort::*S_ComRING)(bool);
	void (QSerialPort::*S_ComRLSD)(bool);
	void (QSerialPort::*S_ComPinStatus)(int);
protected:
	void run();			//	Поток чтения всегда работает (или спит в основном),
						//	При закрытии порта Must be ilemeneted :)
private slots:
	void is_terminated();
};
/*	╔═══════════════════════════╗
	║	Поток записи			║
	╚═══════════════════════════╝*/
class TxThread : public QThread
{
	Q_OBJECT
public:
	OVERLAPPED overlapped;
	HANDLE *hComPort;	//	Указатель на хэндл файла.
	char *bufwr;		//	Указатель на буфер передачи.
	DWORD lenght;		//	Количество записываемых байтов.
	bool *fl;			//	Флаг успешности записи.
protected:
	void run();			//	Поток записи (чтобы не подвешивать основную программу),
						//	по окончании записи самоликвидируетя :)
};
#endif // COMPORTSETTINGS_H
