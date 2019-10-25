#pragma once

#include <vector>
#include <map>
#include "ECanVci.H"

#include <QObject>
#include <QTimer>

/////////////////////////////////////////////////////////////////////////////////////////
// 周期性发送报文
////////////////////////////////////////////////////////////////////////////////////////

// 这个类当需要调用的时候调用, 
// 当前用于，当报文发送完毕的时候，调用
class MessageTxCallback
{
public:
	MessageTxCallback(BOOL need_called = FALSE) : need_called_(need_called) {}

	void NeedCalled()
	{
		need_called_ = TRUE;
	}

	void Call()
	{
		if (need_called_) {
			need_called_ = FALSE;
			OnCall();
		}
	}
	virtual void OnCall() = 0;
protected:
	BOOL need_called_;
};

//MessageTxCallback管理类
class MessageTxCallbackManager
{
public:
	void AddMessageTxCallback(MessageTxCallback *callback)
	{
		callbacks_.push_back(callback);
	}

	void Call()
	{
		for (auto iter = callbacks_.begin(); iter != callbacks_.end(); ++iter) {
			if ((*iter) != NULL) {
				(*iter)->Call();
			}
		}
	}

	static MessageTxCallbackManager& GetInstance()
	{
		static MessageTxCallbackManager inst;
		return inst;
	}

protected:
	std::vector<MessageTxCallback*> callbacks_;
};

enum TestMode
{
	TEST_MODE_NORMAL = 0, /*普通测试模式*/
	TEST_MODE_PRESS		  /*压力测试模式*/
};

// 发送报文基类
class MessageTx
{
public:
	MessageTx(UINT id, UINT len);

	void SetCallback(MessageTxCallback *callback)
	{
		callbacks_.push_back(callback);	
		MessageTxCallbackManager::GetInstance().AddMessageTxCallback(callback);
	}

	void SetNormalTestMode();
	void SetPressTestMode();
	TestMode GetTestMode() const;

	void PreTransmit();

protected:
	void Transmit();

public:
	CAN_OBJ can_obj_;
	CAN_OBJ press_can_obj_;
	CAN_OBJ *cur_can_obj_;

	std::vector<MessageTxCallback *>callbacks_;
	TestMode mode_;
};

class MessagePeriodTx : public MessageTx
{
public:
	MessagePeriodTx(UINT id, UINT len, UINT period, UINT offset);

	void OnElapsed(UINT elapsed);
	void Start();
	void Stop();

	UINT GetPeriod() const
	{
		return period_;
	}

	void SetPeriod(UINT period)
	{
		period_ = period;
	}

protected:
	UINT period_;
	UINT tick_count_;
	int offset_;
	BOOL started_;
};

class MessagePeriodTxMgr
{
public:
	static MessagePeriodTxMgr& GetInstance();
	void AddMessagePeriodTx(UINT id, UINT len, UINT period, UINT offset);
	void OnElapsed(UINT elapsed);

	void Start();
	void Start(UINT can_id);
	void Stop();
	void Stop(UINT can_id);

	void AddCallback(UINT can_id, MessageTxCallback *callback);

	BYTE* GetCanData(UINT can_id)
	{
		auto iter = message_period_txs_.find(can_id);
		if (iter != message_period_txs_.end()) {
			return iter->second.can_obj_.Data;
		}
		return NULL;
	}

    CAN_OBJ* GetCanObj(UINT can_id)
    {
        auto iter = message_period_txs_.find(can_id);
        if (iter != message_period_txs_.end()) {
            return &iter->second.can_obj_;
        }
        return NULL;
    }

	BOOL IsStarted() const
	{
		return started_;
	}

    void clear()
    {
        message_period_txs_.clear();
    }

	void SetNormalTestMode();
	void SetPressTestMode();

	TestMode GetTestMode() const 
	{
		return mode_;
	}

	std::vector<UINT> GetAllCanIds() const
	{
		std::vector<UINT> ids;
		for (auto iter = message_period_txs_.begin(); iter != message_period_txs_.end(); ++iter) {
			ids.push_back(iter->first);
		}
		return ids;
	}

protected:
	std::map<UINT, MessagePeriodTx> message_period_txs_;

	BOOL started_;
private:
	MessagePeriodTxMgr() : started_(FALSE), mode_(TEST_MODE_NORMAL) {}
	TestMode mode_;
public:
	BOOL SetCanPeriod(UINT id, UINT period);
	BOOL GetCanPeriod(UINT id, UINT & period);
};

class QTimerMessagePeriodTxMgr : public QObject
{
    Q_OBJECT
public:
	enum {
		MULTI_MEDIA_MESSAGE_PERIOD = 10,
	};

    static QTimerMessagePeriodTxMgr &GetInstance()
	{
        static QTimerMessagePeriodTxMgr instance(MULTI_MEDIA_MESSAGE_PERIOD);
		return instance;
	}

    ~QTimerMessagePeriodTxMgr()
	{
        //Q_ASSERT(already_destroyed_ && "MultiMedia Must Destroyed");
        if (timer_) {
            delete timer_;
            timer_ = NULL;
        }
	}

	operator bool() const
	{
		return (TRUE != already_destroyed_);
	}

	void Start()
	{
		message_period_tx_mgr_.Start();
	}

	void Start(UINT can_id)
	{
		message_period_tx_mgr_.Start(can_id);
	}

	void Stop()
	{
		message_period_tx_mgr_.Stop();
	}

	void Stop(UINT can_id)
	{
		message_period_tx_mgr_.Stop(can_id);
	}

	void Close()
	{
		Stop();
		destroyed_ = TRUE;
	}

protected:
	void Init(UINT period, UINT res);

protected slots:
	void OnTimer();

private:
	UINT timer_res_;
	UINT timer_id_;
	BOOL destroyed_;
	BOOL already_destroyed_;
	UINT period_;
    QTimer *timer_;

	MessagePeriodTxMgr &message_period_tx_mgr_;

private:
    QTimerMessagePeriodTxMgr(UINT period, UINT timer_res=1) : message_period_tx_mgr_(MessagePeriodTxMgr::GetInstance())
	{
		Init(period, timer_res);
	}
};

struct RxMessageListener
{
	virtual void OnMessageChanged(const CAN_OBJ &message) = 0;
	virtual ~RxMessageListener() {}
};

class RxMessageDispatcher
{
public:
	~RxMessageDispatcher();

	void AddRxMessageListener(RxMessageListener *listener, BOOL auto_delete);
	void RemoveRxMessageListener(RxMessageListener *listener);
	void Dispatch(const CAN_OBJ &message);

	static RxMessageDispatcher& GetInstance()
	{
		static RxMessageDispatcher instance;
		return instance;
	}

protected:
	std::map<RxMessageListener*, BOOL> items_;
};

class RxMessageFilter
{
public:
	enum {
		FILTER_BIT_VALUE = 0,
	};

	enum  EndianKind{
		INTEL_LSB = 0,
		INTEL_MSB,
		MOTOROLA_LSB,
		MOTOROLA_MSB
	};

	struct FilterRange {
		UINT start_pos;
		UINT len;
		EndianKind kind;
	};

	void AddFilterRange(UINT start_pos, UINT len, EndianKind kind);
	void Filter(CAN_OBJ &obj);

protected:
	void doFilter(CAN_OBJ &obj, UINT start_pos, UINT len, EndianKind kind);
	void doIntelLsbFilter(CAN_OBJ &obj, UINT start_pos, UINT len);
	void doIntelMsbFilter(CAN_OBJ &obj, UINT start_pos, UINT len);
	void doMotorolaLsbFilter(CAN_OBJ &obj, UINT start_pos, UINT len);
	void doMotorolaMsbFilter(CAN_OBJ &obj, UINT start_pos, UINT len);

protected:
	std::vector<FilterRange> filter_ranges_;
};

class RxMessageFilterManager
{
public:
	~RxMessageFilterManager();
	void AddRxMessageFilter(UINT can_id, RxMessageFilter *filter);
	void RemoveRxMessageFilter(UINT can_id);
	void Filter(CAN_OBJ &obj);

	static RxMessageFilterManager& GetInstance()
	{
		static RxMessageFilterManager instance;
		return instance;
	}

protected:
	std::map<UINT, RxMessageFilter*> filters_;
};

#define BEGIN_RX_MESSAGE_FILTER(__can_id) {\
	auto can_id = __can_id;\
	auto *filter = new RxMessageFilter;\
	RxMessageFilterManager::GetInstance().AddRxMessageFilter(can_id, filter);

#define RX_MESSAGE_FILTER_ITEM(start_pos, len, kind)\
	filter->AddFilterRange(start_pos, len, RxMessageFilter::kind);

#define END_RX_MESSAGE_FILTER() }

class RxMessageMonitor
{
public:
	RxMessageMonitor();

	static RxMessageMonitor& GetInstance()
	{
		static RxMessageMonitor instance;
		return instance;
	}

	void Receive(CAN_OBJ &obj);
	void DispatchObj(const CAN_OBJ &obj);
	void Reset();

protected:
	RxMessageDispatcher &dispatcher_;
	RxMessageFilterManager &filter_;
	std::map<UINT, CAN_OBJ> objs_;
};
