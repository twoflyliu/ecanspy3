#include "Core.h"
#include "UsbCanUtil.h"

#include <QDebug>

#define FFX_LOG(x) qDebug() << x;
#define ASSERT Q_ASSERT

class MessageTxGroup
{
public:

	static MessageTxGroup& GetInstance()
	{
		static MessageTxGroup instance;
		return instance;
	}

	void AddCanObj(const CAN_OBJ &can_obj)
	{
		can_objs_.push_back(can_obj);
	}

	void Transmit()
	{
		if (can_objs_.size() > 0) {
			usb_can_.Send(&can_objs_[0], can_objs_.size());
			can_objs_.clear();
		}
	}

protected:
	std::vector<CAN_OBJ> can_objs_;
	UsbCanUtil &usb_can_;

private:
	MessageTxGroup() : usb_can_(UsbCanUtil::GetInstance()) {}
};

MessageTx::MessageTx(UINT id, UINT len) :  cur_can_obj_(NULL), mode_(TEST_MODE_NORMAL)
{
	memset(&can_obj_, 0, sizeof(can_obj_));
	can_obj_.ID = id;
	can_obj_.DataLen = len;
    can_obj_.SendType = 2;

	memcpy(&press_can_obj_, &can_obj_, sizeof(can_obj_));

	cur_can_obj_ = &can_obj_;
}

void MessageTx::SetNormalTestMode()
{
	mode_ = TEST_MODE_NORMAL;
	cur_can_obj_ = &can_obj_;
}

void MessageTx::SetPressTestMode()
{
	mode_ = TEST_MODE_PRESS;
	cur_can_obj_ = &press_can_obj_;
}

TestMode MessageTx::GetTestMode() const
{
	return mode_;
}

void MessageTx::PreTransmit()
{
	if (TEST_MODE_PRESS == mode_) {
		for (UINT i = 0; i < can_obj_.DataLen; i++) {
			press_can_obj_.Data[i] = rand();
		}
		cur_can_obj_ = &press_can_obj_;
	} else {
		cur_can_obj_ = &can_obj_;
	}
}

void MessageTx::Transmit()
{
	PreTransmit();
	MessageTxGroup::GetInstance().AddCanObj(*cur_can_obj_);

	for (auto iter = callbacks_.begin(); iter != callbacks_.end(); ++iter) {
		auto callback = *iter;
		if (callback != NULL) {
			callback->NeedCalled();
		}
	}
}

MessagePeriodTx::MessagePeriodTx( UINT id, UINT len, UINT period, UINT offset )
    : MessageTx(id, len), period_(period),
    tick_count_(0), offset_(offset), started_(TRUE)
{

}

void MessagePeriodTx::OnElapsed( UINT elapsed )
{
	if (started_) {
		if (offset_ > 0) {
			offset_ -= elapsed;
			return;
		}

		tick_count_ += elapsed;
		if (tick_count_ >= period_) {
			tick_count_ = 0;
			Transmit();
		}
	}
}

void MessagePeriodTx::Start()
{
	started_ = TRUE;
}

void MessagePeriodTx::Stop()
{
	started_ = FALSE;
}

MessagePeriodTxMgr& MessagePeriodTxMgr::GetInstance()
{
	static MessagePeriodTxMgr instance;
	return instance;
}

void MessagePeriodTxMgr::AddMessagePeriodTx( UINT id, UINT len, UINT period, UINT offset )
{
	message_period_txs_.insert(std::make_pair(id, MessagePeriodTx(id, len, period, offset)));
}

void MessagePeriodTxMgr::OnElapsed( UINT elapsed )
{
	if (started_)
	{
		for (auto iter = message_period_txs_.begin(); iter != message_period_txs_.end(); ++iter) {
			iter->second.OnElapsed(elapsed);
		}
		MessageTxGroup::GetInstance().Transmit();
		MessageTxCallbackManager::GetInstance().Call();
	}
}

void MessagePeriodTxMgr::Start()
{
	started_ = TRUE;
}

void MessagePeriodTxMgr::Start( UINT can_id )
{
	auto iter = message_period_txs_.find(can_id);
	if (iter != message_period_txs_.end()) {
		iter->second.Start();
	}
}

void MessagePeriodTxMgr::Stop()
{
	started_ = FALSE;
}

void MessagePeriodTxMgr::Stop( UINT can_id )
{
	auto iter = message_period_txs_.find(can_id);
	if (iter != message_period_txs_.end()) {
		iter->second.Stop();
	}
}

void MessagePeriodTxMgr::AddCallback( UINT can_id, MessageTxCallback *callback )
{
	auto iter = message_period_txs_.find(can_id);
	if (iter != message_period_txs_.end()) {
		iter->second.SetCallback(callback);
	}
}

void MessagePeriodTxMgr::SetNormalTestMode()
{
	for (auto iter = message_period_txs_.begin(); iter != message_period_txs_.end(); ++iter) {
		iter->second.SetNormalTestMode();
	}
	mode_ = TEST_MODE_NORMAL;
}

void MessagePeriodTxMgr::SetPressTestMode()
{
	for (auto iter = message_period_txs_.begin(); iter != message_period_txs_.end(); ++iter) {
		iter->second.SetPressTestMode();
	}
	mode_ = TEST_MODE_PRESS;
}

BOOL MessagePeriodTxMgr::SetCanPeriod(UINT id, UINT period)
{
	auto iter = message_period_txs_.find(id);
	BOOL ret = FALSE;

	if (iter != message_period_txs_.end()) {
		iter->second.SetPeriod(period);
		ret = TRUE;	
	}

	return ret;
}

BOOL MessagePeriodTxMgr::GetCanPeriod(UINT id, UINT & period)
{
	auto iter = message_period_txs_.find(id);
	BOOL ret = FALSE;

	if (iter != message_period_txs_.end()) {
		period = iter->second.GetPeriod();
		ret = TRUE;	
	}

	return ret;	
}


void QTimerMessagePeriodTxMgr::Init(UINT period, UINT res)
{
    Q_UNUSED(res);
	already_destroyed_ = destroyed_ = FALSE;
    timer_ = new QTimer();
    connect(timer_, SIGNAL(timeout()), this, SLOT(OnTimer()));
	period_ = period;
    timer_->start(period_);
}

void QTimerMessagePeriodTxMgr::OnTimer()
{
	if (destroyed_) {
		FFX_LOG(_T("Starting destroy MultiMeidaTimer.\r\n"));
        timer_->stop();
		already_destroyed_ = TRUE;
		FFX_LOG(_T("MultiMeidaTimer Destroyed.\r\n"));
		return;
	}

	if (message_period_tx_mgr_.IsStarted()) {
		message_period_tx_mgr_.OnElapsed(period_);	
	}
}

RxMessageDispatcher::~RxMessageDispatcher()
{
	for (auto iter = items_.begin(); iter != items_.end(); ++iter) {
		if (iter->second) {
			delete iter->first;
		}
	}
}

void RxMessageDispatcher::AddRxMessageListener(RxMessageListener *listener, BOOL auto_delete)
{
    ASSERT(listener != NULL);
	items_[listener] = auto_delete;
}

void RxMessageDispatcher::RemoveRxMessageListener(RxMessageListener *listener)
{
	ASSERT(listener != NULL);
	if (items_.find(listener) != items_.end()) {
		if (items_[listener]) {
			delete listener;
		}
		items_.erase(listener);
	}
}

void RxMessageDispatcher::Dispatch(const CAN_OBJ &message)
{
	for (auto iter = items_.begin(); iter != items_.end(); ++iter) {
		if (iter->first != NULL) {
			iter->first->OnMessageChanged(message);
		}
	}
}

RxMessageFilterManager::~RxMessageFilterManager()
{
	for (auto iter = filters_.begin(); iter != filters_.end(); ++iter) {
		delete iter->second;
	}
}

void RxMessageFilterManager::AddRxMessageFilter(UINT can_id, RxMessageFilter *filter)
{
	auto iter = filters_.find(can_id);
	if (iter != filters_.end()) {
		if (filter != iter->second) {
			delete iter->second;
			iter->second = filter;
		}
	} else {
		filters_[can_id] = filter;
	}
}

void RxMessageFilterManager::RemoveRxMessageFilter(UINT can_id)
{
	auto iter = filters_.find(can_id);
	if (iter != filters_.end()) {
		if (iter->second) {
			delete iter->second;
		}
		filters_.erase(iter);
	}
}

void RxMessageFilterManager::Filter(CAN_OBJ &obj)
{
	auto iter = filters_.find(obj.ID);
	if (iter != filters_.end()) {
		iter->second->Filter(obj);
	}
}

void RxMessageFilter::AddFilterRange(UINT start_pos, UINT len, EndianKind kind)
{
	FilterRange filter;

	filter.start_pos = start_pos;
	filter.len = len;
	filter.kind = kind;

	filter_ranges_.push_back(filter);
}

void RxMessageFilter::Filter(CAN_OBJ &obj)
{
	for (auto iter = filter_ranges_.begin(); iter != filter_ranges_.end(); ++iter) {
		doFilter(obj, iter->start_pos, iter->len, iter->kind);
	}
}

void RxMessageFilter::doFilter(CAN_OBJ &obj, UINT start_pos, UINT len, EndianKind kind)
{
	switch (kind) {
	case INTEL_LSB:
		doIntelLsbFilter(obj, start_pos, len);
		break;
	case INTEL_MSB:
		break;
		doIntelMsbFilter(obj, start_pos, len);
	case MOTOROLA_LSB:
		doMotorolaLsbFilter(obj, start_pos, len);
		break;
	case MOTOROLA_MSB:
		doMotorolaMsbFilter(obj, start_pos, len);
		break;
	}
}

void RxMessageFilter::doIntelLsbFilter(CAN_OBJ &obj, UINT start_pos, UINT len)
{
    Q_UNUSED(obj);
    Q_UNUSED(start_pos);
    Q_UNUSED(len);
	ASSERT(FALSE && "Not Implemeted");
}

void RxMessageFilter::doIntelMsbFilter(CAN_OBJ &obj, UINT start_pos, UINT len)
{
    Q_UNUSED(obj);
    Q_UNUSED(start_pos);
    Q_UNUSED(len);
    ASSERT(FALSE && "Not Implemeted");
}

static inline void __SetBit(BYTE *data, UINT bit_pos, UINT value)
{
	ASSERT(0 == value || 1 == value);
	if (value) {
		data[bit_pos / 8] |= (1 << (bit_pos % 8));
	} else {
		data[bit_pos / 8] &= ~(1 << (bit_pos % 8));
	}
}

void RxMessageFilter::doMotorolaLsbFilter(CAN_OBJ &obj, UINT start_pos, UINT len)
{
	UINT first_byte_last_bit = start_pos / 8 * 8 + 7;
	UINT first_byte_max_len = first_byte_last_bit - start_pos + 1;

	UINT byte_index = start_pos / 8;
	UINT bit_index = start_pos % 8;
    Q_UNUSED(bit_index);

	UINT first_byte_len = (len < first_byte_max_len) ? len : first_byte_max_len;

	//第一个字节
	for (UINT i = 0; i < first_byte_len; i++) {
		__SetBit(obj.Data, start_pos + i, FILTER_BIT_VALUE);
	}

	UINT left_bit_len = len - first_byte_len;
	UINT mid_byte_len = left_bit_len / 8;

	// 中间字节
	for (UINT i = 0; i < mid_byte_len; i++) {
		obj.Data[++byte_index] = FILTER_BIT_VALUE;
	}

	// 最后一个字节
	UINT last_byte_bit_len = left_bit_len % 8;
	++byte_index;
	for (UINT i = 0; i < last_byte_bit_len; i++) {
		obj.Data[byte_index] &= ~(1 << i);
	}
}

void RxMessageFilter::doMotorolaMsbFilter(CAN_OBJ &obj, UINT start_pos, UINT len)
{
    Q_UNUSED(obj);
    Q_UNUSED(start_pos);
    Q_UNUSED(len);
	ASSERT(FALSE && "Not Implemeted");
}

RxMessageMonitor::RxMessageMonitor() 
	: dispatcher_(RxMessageDispatcher::GetInstance()),
	filter_(RxMessageFilterManager::GetInstance())
{

}

void RxMessageMonitor::Receive(CAN_OBJ &obj)
{
	filter_.Filter(obj);

	if (objs_.end() == objs_.find(obj.ID)) { //如果是新的报文则执行更新
		DispatchObj(obj);
	} else {
		auto old_obj = objs_[obj.ID];
		if (memcmp(old_obj.Data, obj.Data, old_obj.DataLen) != 0) {
			DispatchObj(obj);
		}
	}
}

void RxMessageMonitor::DispatchObj(const CAN_OBJ &obj)
{
	objs_[obj.ID] = obj;
	dispatcher_.Dispatch(obj);
}

void RxMessageMonitor::Reset()
{
	objs_.clear();
}
