#ifndef _SERVERCLIENTSTRUCT_H_
#define _SERVERCLIENTSTRUCT_H_


#pragma once

#pragma pack(push,1)

struct connect_server
{
	char content[500];
};
#pragma pack(pop)

#pragma pack(push,1)
struct connect_server_return
{
	char content_return[1000];
};
#pragma pack(pop)

#endif