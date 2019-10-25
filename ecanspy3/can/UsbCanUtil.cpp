#include "UsbCanUtil.h"

#include <QMessageBox>
#include <QDebug>
#include <QString>


UsbCanUtil::UsbCanUtil() : QObject(NULL), opened_(FALSE), dev_ind_(0), dev_type_(USBCAN1)
{
    INIT_CONFIG init_config = DEFAULT_VCI_INIT_CONFIG;
    init_config_ = init_config;

    baurate = USB_CAN_BAURATE_500KBPS;
    initBaurateMap();
}

void UsbCanUtil::initBaurateMap()
{
    AddBaurate(USB_CAN_BAURATE_5KBPS, BAURATE_5KBPS);
    AddBaurate(USB_CAN_BAURATE_10KBPS, BAURATE_10KBPS);
    AddBaurate(USB_CAN_BAURATE_20KBPS, BAURATE_20KBPS);
    AddBaurate(USB_CAN_BAURATE_40KBPS, BAURATE_40KBPS);
    AddBaurate(USB_CAN_BAURATE_80KBPS, BAURATE_80KBPS);
    AddBaurate(USB_CAN_BAURATE_100KBPS, BAURATE_100KBSP);
    AddBaurate(USB_CAN_BAURATE_125KBPS, BAURATE_125KBPS);
    AddBaurate(USB_CAN_BAURATE_200KBPS, BAURATE_200KBPS);
    AddBaurate(USB_CAN_BAURATE_250KBPS, BAURATE_250KBPS);
    AddBaurate(USB_CAN_BAURATE_400KBPS, BAURATE_400KBPS);
    AddBaurate(USB_CAN_BAURATE_500KBPS, BAURATE_500KBPS);
    AddBaurate(USB_CAN_BAURATE_666KBPS, BAURATE_666KBPS);
    AddBaurate(USB_CAN_BAURATE_800KBPS, BAURATE_800KBPS);
    AddBaurate(USB_CAN_BAURATE_1000KBPS, BAURATE_1000KBPS);
}

UsbCanUtil::~UsbCanUtil( void )
{
	Close();
}

UsbCanUtil& UsbCanUtil::GetInstance()
{
	static UsbCanUtil instance;
    return instance;
}

QString UsbCanUtil::GetBaurate() const
{
    return baurate;
}

void UsbCanUtil::AddBaurate(QString name, UsbCanUtil::Baurate baurate)
{
    baurateMap.insert(name, baurate);
    supportedBaurateList.append(name);
}

QList<QString> UsbCanUtil::GetSupportedBaurateList()
{
    return this->supportedBaurateList;
}

void UsbCanUtil::SetBaurate(QString baurate)
{
    if (this->baurate == baurate) return;

    auto iter = this->baurateMap.find(baurate);
    if (iter != baurateMap.end()) {
        this->baurate = baurate;
    } else {
        this->baurate = USB_CAN_BAURATE_DEFAULT;
    }

    USHORT uBaurate = this->baurateMap.value(this->baurate);
    init_config_.Timing0 = (uBaurate>>8);
    init_config_.Timing1 = (uBaurate&0xff);

    if (opened_) {
        Open(dev_type_);
    }
}

bool UsbCanUtil::ReadCANStatus(P_CAN_STATUS can_status)
{
    if (opened_ && can_status) {
        return (1 == ::ReadCANStatus(dev_type_, dev_ind_, 0, can_status));
    } else {
        return false;
    }
}

BOOL UsbCanUtil::Open( DWORD device_type, INIT_CONFIG *init_config )
{
	if (opened_) {
		Close();
	}

	dev_type_ = device_type;

    if (init_config) {
        init_config_ = *init_config;
	}

	if (FALSE == OpenDevice(MAX_DEV_IND)) {
        qDebug() << QObject::tr("Can设备打开失败");
        QMessageBox::critical(NULL, QObject::tr("CAN错误"), tr("CAN设备打开失败"));
		return FALSE;
	}

    if (::InitCAN(dev_type_, dev_ind_, 0, &init_config_) != STATUS_OK) {
        qDebug() << QObject::tr("CAN初始化失败");
        QMessageBox::critical(NULL, QObject::tr("CAN错误"), tr("CAN设备初始化失败"));
		return FALSE;
	}

	if (::ClearBuffer(dev_type_, dev_ind_, 0) != STATUS_OK) {
        qDebug() << QObject::tr("清除CAN缓冲区失败");
        QMessageBox::critical(NULL, QObject::tr("CAN错误"), tr("CAN设备清除缓冲区失败"));
		return FALSE;
	}

	if (::StartCAN(dev_type_, dev_ind_, 0)) {
		opened_ = TRUE;
        qDebug() << QObject::tr("USB CAN open successfully!\n");
	} else {
        qDebug() << QObject::tr("CAN启动失败");
        QMessageBox::critical(NULL, QObject::tr("CAN错误"), tr("CAN设备启动失败"));
		return FALSE;
	}

    if (opened_) {
        emit openCan();
    }

	return TRUE;
}

ULONG UsbCanUtil::Close()
{
	if (opened_) {
		opened_ = FALSE;
        ULONG ret = CloseDevice(dev_ind_, dev_ind_);
        emit closeCan();
        return ret;
	}
	return 0;
}

BOOL UsbCanUtil::IsOpened() const
{
	return opened_;
}

#if 0
static void logErr(const ERR_INFO err)
{
    QString errStr(_T("["));
    static USHORT errCodeTable[] = {0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080};
    static QString errDescTable[] = {_T("设备已经打开"), _T("设备打开错误"), _T("设备没有打开"), _T("缓冲区溢出"), _T("此设备不存在"),
		_T("装载动态库失败"), _T("表示为执行命令失败错误"), _T("内存不足"), _T("CAN控制器内部FIFO溢出"),
		_T("CAN控制器错误报警"), _T("CAN控制器消极错误"), _T("CAN控制器仲裁丢失"), _T("CAN控制器总线错误"), _T("CAN接收寄存器满"),
		_T("CAN接收寄存器溢出"), _T("CAN控制器主动错误")};
    for (int i = 0; i < (int)(sizeof(errCodeTable) / sizeof(USHORT)); i++) {
		if (errCodeTable[i] & err.ErrCode) {
			errStr += errDescTable[i];
			errStr += _T(",");
		}
	}
	errStr += _T("\n");
    qDebug() << errStr;
}
#endif

ULONG UsbCanUtil::Send(CAN_OBJ & can_obj )
{
	USB_CAN_NOT_OPENED_RETURN_VALUE(ERR_CAN_NOT_OPENED);
	ULONG ret = ::Transmit(dev_ind_, dev_ind_, 0, &can_obj, 1);

	if (ret != 1) {
		ERR_INFO err;
		if (ReadErrInfo(dev_ind_, dev_ind_, 0, &err)) {
			//logErr(err);
		}
	}

	return ret;
}

ULONG UsbCanUtil::Send( CAN_OBJ *can_objs, UINT obj_len )
{
	USB_CAN_NOT_OPENED_RETURN_VALUE(ERR_CAN_NOT_OPENED);
	return ::Transmit(dev_type_, dev_ind_, 0, can_objs, obj_len);
}

DWORD UsbCanUtil::Receive( CAN_OBJ *can_obj, UINT obj_len /*= 1*/, INT wait_time)
{
	USB_CAN_NOT_OPENED_RETURN_VALUE(ERR_CAN_NOT_OPENED);
    return ::Receive(dev_type_, dev_ind_, 0, can_obj, obj_len, wait_time);
}

DWORD UsbCanUtil::ReadErrorInfo( ERR_INFO &error_info )
{
    //USB_CAN_NOT_OPENED_RETURN_VALUE(ERR_CAN_NOT_OPENED);
    if (!opened_) return 0;
	return ReadErrInfo(dev_type_, dev_ind_, 0, &error_info);
}

BOOL UsbCanUtil::OpenDevice( UINT max_dev_ind )
{
	for (UINT i = 0; i < max_dev_ind; i++) {
		if (STATUS_OK == ::OpenDevice(dev_type_, i, 0)) {
			dev_ind_ = i;
			return TRUE;
		}
	}

	return FALSE;
}
