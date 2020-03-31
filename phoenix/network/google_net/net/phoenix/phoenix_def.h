#ifndef _NET_PHOENIX_PHOENIX_DEF_H_
#define _NET_PHOENIX_PHOENIX_DEF_H_

NET_BEGIN_DECLS

enum PhoenixNetErrorCode
{
	kPhoenixNetErrorCodeNormal			= 0, // 无特殊错误
	kPhoenixNetErrorCodeConnectError,		 // 连接失败
	kPhoenixNetErrorCodeTimeout,			 // 链接超时
	kPhoenixNetErrorCodeInnerActiveClose,	 // 主动发起断开, 内部使用，不会上报给调用方
};

enum PhoenixLinkEvent
{
	kPhoenixLinkEventDefault         = 0, // 默认
	kPhoenixLinkEventConnected,           // 链接成功
	kPhoenixLinkEventDisconnected,        // 链接断开
};

// linksocket数据收发事件的回调接口
struct IPhoenixLinkSocketCallback
{
	// 连接成功事件通知
	virtual void OnConnected(PhoenixNetErrorCode error) = 0;
	// 没有出错处理，出错后直接断开连接并调用OnDisConnected
	virtual void OnDisConnected(PhoenixNetErrorCode error) = 0;
};

NET_END_DECLS

#endif//_NET_PHOENIX_PHOENIX_DEF_H_
