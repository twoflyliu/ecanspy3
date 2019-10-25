#pragma once

#include "ECanVci.H"
#include <QObject>

/*!
*	\brief 设备类型，对于测试软件就是使用USBCAN了
*/
#define USB_CAN_DEVICE_TYPE (USBCAN1)
#define USB_CAN_DEVICE_INDEX (0)

/*!
*	\brief 默认的VCI_CAN初始化设置
*/
#define DEFAULT_VCI_INIT_CONFIG {0x00000000, 0xffffffff, 0, 0x00, 0, 0x1c, 0x00}

/*!
*	\brief 辅助宏，当USB CAN没有被打开的时候，直接返回
*/
#define USB_CAN_NOT_OPENED_RETURN() \
	if(!UsbCanUtil::GetInstance().IsOpened()) { \
		return; \
	}

/*!
*	\brief 辅助宏，当USB CAN没有被打开的时候，直接返回某个值
*/
#define USB_CAN_NOT_OPENED_RETURN_VALUE(val) \
	if(!UsbCanUtil::GetInstance().IsOpened()) { \
		return val; \
	}

/*!
*	\brief 常量，当返回ERR_CAN_NOT_OPENED，就表示USB CAN没有被打开
*/
#define ERR_CAN_NOT_OPENED (-1)

#include <QMap>
#include <QString>

#define USB_CAN_BAURATE_5KBPS            QObject::tr("5KBPS")
#define USB_CAN_BAURATE_10KBPS           QObject::tr("10KBPS")
#define USB_CAN_BAURATE_20KBPS           QObject::tr("20KBPS")
#define USB_CAN_BAURATE_40KBPS           QObject::tr("40KBPS")
#define USB_CAN_BAURATE_80KBPS           QObject::tr("80KBPS")
#define USB_CAN_BAURATE_100KBPS          QObject::tr("100KBPS")
#define USB_CAN_BAURATE_125KBPS          QObject::tr("125KBPS")
#define USB_CAN_BAURATE_200KBPS          QObject::tr("200KBPS")
#define USB_CAN_BAURATE_250KBPS          QObject::tr("250KBPS")
#define USB_CAN_BAURATE_400KBPS          QObject::tr("400KBPS")
#define USB_CAN_BAURATE_500KBPS          QObject::tr("500KBPS")
#define USB_CAN_BAURATE_666KBPS          QObject::tr("666KBPS")
#define USB_CAN_BAURATE_800KBPS          QObject::tr("800KBPS")
#define USB_CAN_BAURATE_1000KBPS         QObject::tr("1000KBPS")
#define USB_CAN_BAURATE_DEFAULT          USB_CAN_BAURATE_500KBPS


/*!
*	\brief VCI_CAN的一个辅助封装类
*/
class UsbCanUtil : public QObject
{
    Q_OBJECT

public:
	enum {
		MAX_DEV_IND = 10,
    };

    /*!
     * \brief The Baurate enum 波特率
     */
    enum Baurate{
        BAURATE_5KBPS    = 0xBFFF,
        BAURATE_10KBPS   = 0x311C,
        BAURATE_20KBPS   = 0x181C,
        BAURATE_40KBPS   = 0x87FF,
        BAURATE_80KBPS   = 0x83FF,
        BAURATE_100KBSP  = 0x041C,
        BAURATE_125KBPS  = 0x031C,
        BAURATE_200KBPS  = 0x81FA,
        BAURATE_250KBPS  = 0x011C,
        BAURATE_400KBPS  = 0x80FA,
        BAURATE_500KBPS  = 0x001C,
        BAURATE_666KBPS  = 0x80B6,
        BAURATE_800KBPS  = 0x0016,
        BAURATE_1000KBPS = 0x0014,
        BAURATE_DEFAULT  = BAURATE_500KBPS,
    };

	/*!
	*	\brief 析构器
	*/
	~UsbCanUtil(void);

	/*!
	*	\brief 单利模式在c/c++中就相当于使用全局变量
	*
	*	总之要想获取UsbCanUtil就直接使用UsbCanUtil::GetInstance()就行
	*/
	static UsbCanUtil& GetInstance();

    /*!
     * \brief getBaurate 获取波特率
     * \retval 波特率
     */
    QString GetBaurate() const;

    /*!
     * \brief addBaurate 添加波特率
     * \param name       波特率名称
     * \param baurate    波特率值
     */
    void AddBaurate(QString name, Baurate baurate);

    /*!
     * \brief getSupportedBaurate 获取支持的波特率
     * \retval 支持的波特率列表
     */
    QList<QString> GetSupportedBaurateList();

    /*!
     * \brief setBaurate 设置波特率
     * \param baurate 波特率，可以是：
     *         + USB_CAN_BAURATE_5KBPS
     *         + USB_CAN_BAURATE_10KBPS
     *         + USB_CAN_BAURATE_20KBPS
     *         + USB_CAN_BAURATE_40KBPS
     *         + USB_CAN_BAURATE_80KBPS
     *         + USB_CAN_BAURATE_100KBPS
     *         + USB_CAN_BAURATE_125KBPS
     *         + USB_CAN_BAURATE_200KBPS
     *         + USB_CAN_BAURATE_250KBPS
     *         + USB_CAN_BAURATE_400KBPS
     *         + USB_CAN_BAURATE_500KBPS
     *         + USB_CAN_BAURATE_666KBPS
     *         + USB_CAN_BAURATE_800KBPS
     *         + USB_CAN_BAURATE_1000KBPS
     */
    void SetBaurate(QString baurate);

    /*!
     * \brief ReadCANStatus 读取CAN状态
     * \param can_status 接收CAN状态
     * \retval 成功与否
     */
    bool ReadCANStatus(P_CAN_STATUS can_status);


	/*!
	*	\brief 打开指定的VCI_CAN设备
	*
    *	\param [in] device_type 设备类型，对于我们测试项目直接使用DEVICE_TYPE宏就可以，使用的是USB CAN
    *	\param [in] init 打开VCI_CAN后要进行的配置，如果为NULL，那么默认的值就是使用DEFAULT_VCI_INIT_CONFIG
	*	\retval 返回成功与否
	*/
    BOOL Open(DWORD device_type, INIT_CONFIG *init = NULL);

	/*!
	*	\brief 关闭已经打开的CAN设备
	*/
	ULONG Close();

	/*!
	*	\brief 获取CAN设备是否打开
	*/
	BOOL IsOpened() const;

	/*!
	*	\brief 使用CAN设备来发送数据
	*	\param [in] can_obj 表示要发送的对象
	*	\retval 返回STATUS_OK表示发送成功，否则表示发送失败
	*/
	ULONG Send(CAN_OBJ & can_obj);

	/*!
	*	\brief 使用CAN设备来发送多组报文
	*	\param [in] can_objs 要发送can报文的首地址
	*	\param [in] obj_len CAN报文的数目
	*/
	ULONG Send(CAN_OBJ *can_objs, UINT obj_len);

	/*!
	*	\brief 接收数据
	*	\param [in] can_obj 表示要发送的对象指针
	*	\param [in] obj_len 表示can_obj指针具有包含几个VCI_CAN_OBJ
	*	\param [in] wait_time
	*	\retval 返回值<=0表示发送失败，否则发送成功
	*/
    DWORD Receive(CAN_OBJ *can_obj, UINT obj_len = 1, INT wait_time = INT(0));

	// 档接收失败的时候，使用此函数可以获取错误消息

	/*!
	*	\brief 读取错误信息
	*	\param [out] err_info 用来读取具体的错误信息
	*	\retval 
	*/
	DWORD ReadErrorInfo(ERR_INFO &error_info);

signals:
    void openCan();
    void closeCan();

private:
    UsbCanUtil();                           //!< 单利模式的固定模式，禁止其他类直接构造这个对象

    void initBaurateMap();

    BOOL opened_;                           //!< 保存CAN设备是否被打开的状态

    DWORD dev_ind_;                         //!< CAN设备索引，对于新版ECAN DLL非常重要
    DWORD dev_type_;                        //!< 设备类型

    INIT_CONFIG init_config_;               //!< 初始化配置

    QMap<QString, Baurate> baurateMap;      //!< 管理波特率
    QList<QString> supportedBaurateList;    //!< 支持的波特率列表

    QString baurate;

	BOOL OpenDevice(UINT max_dev_ind); //!< 用来打开可以可选的CAN设备，最多尝试max_dev_ind此
};

