#pragma once

#include <string>

class NetMgr
{
public:
	NetMgr();
	~NetMgr();

	//初始化网络
	void init_net_work() const;
	
	//接收网络消息
	void net_work_update();

	//发送数据给server
	void send_data_to_server() const;

	//接收server数据
	void receive_data_from_server(const unsigned char* msg_data) const;

private:
	//处理server发送过来的消息
	void _process_server_message() const;

public:
	std::string server_ip;
	unsigned short server_port;
};

