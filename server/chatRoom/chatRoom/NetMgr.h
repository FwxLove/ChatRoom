#pragma once
class NetMgr
{
public:
	NetMgr();
	~NetMgr();

	//初始化网络
	void init_net_work() const;

	//接收网络消息
	void net_work_update() const;

	//发送数据给client
	void send_data_to_client() const;

	//接收client的数据
	void receive_data_from_client(const unsigned char* msg_data) const;

private:
	//处理server发送过来的数据
	void _process_client_message() const;

public:
	unsigned int server_port;
	int max_client;

};

