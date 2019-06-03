#include "NetMgr.h"
#include "RakPeerInterface.h"						//链接管理接口
#include "RakNetTypes.h"							//定义了在RakNet中使用的结构体
#include "MessageIdentifiers.h"						//包含了一个巨大的枚举数据，表示了RakNet用于发送消息的标识符，例如断开连接通知。
#include "BitStream.h"								//定义了一个可写入、读取比特流的类。

#include <iostream>
#include "ServerClientMessage.h"
#include "ServerClientStruct.h"

RakNet::RakPeerInterface* net_peer;					//网络消息
RakNet::Packet* net_packet;							//网络消息包

NetMgr::NetMgr():server_port(10001),max_client(1000)
{

}


NetMgr::~NetMgr()
{

}


//初始化网络
void NetMgr::init_net_work() const
{
	//初始化网络消息
	net_peer = RakNet::RakPeerInterface::GetInstance();
	if (NULL == net_peer)
	{
		std::cout << "GetInstance failed" << std::endl;
		return;
	}

	std::cout << "Start Game Server ......." << server_port << std::endl;

	//开启网络线程来监听相应的端口
	//startup函数的第一个参数是接收的最大连接数，客户端一般设为1
	//第二个参数就是要监听的端口，SocketDescriptor监听制定的端口，他是一种套接字，服务器一般设置为SocketDescriptor（server_prot,0）
	//第三个参数是SocketDescriptor数组的大小，传1表明不明确
	int start_up = net_peer->Startup(1, &RakNet::SocketDescriptor(server_port, 0), 1);
	if (start_up > 0)
	{
		std::cout << "Startup failed" << std::endl;
	}

	//SetMaxiumIncomingConnections容许最多的链接数量
	net_peer->SetMaximumIncomingConnections(max_client);
}

//接收网络消息
void NetMgr::net_work_update() const
{
	//Receive从消息队列中获取消息
	for (net_packet = net_peer->Receive(); net_packet; net_peer->DeallocatePacket(net_packet), net_packet = net_peer->Receive())
	{
		switch (net_packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			std::cout << "Another client has disconnected" << std::endl;
			break;
		case ID_REMOTE_CONNECTION_LOST:
			std::cout << "Another client has lost the connection" << std::endl;
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			std::cout << "Another client has connected" << std::endl;
			break;
			//client连接上server后会触发
		case ID_CONNECTION_REQUEST_ACCEPTED:
			std::cout << "Our connection request has been accepted for server" << std::endl;
			break;
		case ID_NEW_INCOMING_CONNECTION:
			std::cout << "A connection is incoming" << std::endl;
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			std::cout << "The server is full" << std::endl;
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			std::cout << "A client has disconnected" << std::endl;
			break;
		case ID_CONNECTION_LOST:
			std::cout << "A client lost the connection" << std::endl;
			break;
		case ID_USER_PACKET_ENUM:
			_process_client_message();
			break;
		default:
			std::cout << "Message with identifier %d has arrived" << net_packet->data[0] << std::endl;
			break;
		}
	}
}

//发送数据给client
void NetMgr::send_data_to_client() const
{

}

//接收client的数据
void NetMgr::receive_data_from_client(const unsigned char* msg_data) const
{
	connect_server* receive_data = (connect_server*)msg_data;
	std::cout << "receive from client data is:" << receive_data->content << std::endl;

	connect_server_return send_data;
	memset(&send_data, 0, sizeof(connect_server_return));

	std::string content = receive_data->content;
	content += ". server already receive client data, this is server data";

	sprintf_s(send_data.content_return, "%s", content.c_str());

	std::cout << "server data is:" << send_data.content_return << std::endl;

	//使用Bitstream对象来封装数据
	RakNet::BitStream str;
	//先写入raknet的消息类型（raknet中自定义类型（ID_USER_PACKET_ENUM）)
	str.Write((RakNet::MessageID)ID_USER_PACKET_ENUM);
	//再写入我们自定义的消息类型
	str.Write(msg_connect_server_return);
	//将数据写到str中，第一个参数是要写入的数据字节数组，第二个是数据的位数
	str.WriteBits((unsigned char*)&send_data, sizeof(connect_server_return) * 8);
	//将数据发送到指定的地方
	//第一个参数是要发送的bitstream对象
	//第二个参数是发送的优先级
	//第三个参数是发送的可靠性，这里使用RELIABLE_ORDERED，具体在PacketPriority.h枚举的PacketReliability中有具体说明
	//自己看api，这里设置成0
	//接收方地址
	//是否广播，注意下，要是用那个true，上一个参数就是不需要接受数据的地址
	net_peer->Send(&str, HIGH_PRIORITY, RELIABLE_ORDERED, 0, net_packet->systemAddress, false);
}

//处理server发送过来的数据
void NetMgr::_process_client_message() const
{
	int message_id = 0;
	//将消息解析出来，使用bitstream
	//IgnoreBytes忽略掉最外层的raknet的消息类型
	RakNet::BitStream bs_in(net_packet->data, net_packet->length, false);
	bs_in.IgnoreBytes(sizeof(RakNet::MessageID));
	bs_in.Read(message_id);

	//偏移掉自定义的消息，获取实际数据，raknet的消息类型大小是一个字节，自定义的结构体是4个字节
	unsigned char* msg_data = net_packet->data;
	msg_data += sizeof(unsigned long) + sizeof(unsigned char);

	switch (message_id)
	{
	case msg_connect_server:
		this->receive_data_from_client(msg_data);
		break;


	default:
		break;
	}
}