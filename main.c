#define zMsgrVersion	"zMsgr_v0.4.1b[02jan2k1]"
#define zMsgrVersionNum	0x001228

#include <windows.h>
#include <winsock.h>
#include "resource.h"

#include "h\defines.h"
#include "h\defaults.h"
#include "h\globals.h"

#include "h\misc.h"
#include "h\server.h"
#include "h\command.h"
#include "h\init.h"
#include "h\user.h"
#include "h\wndproc.h"

/*
working_rule
1. start listening at a (random) port
2. connect to add.cgi and register your listening ip:port [add.cgi?7f0000012857]
3. add.cgi returns an array of ip:pt of current users
4. connect to each of the ip:pt
5. if connecting fails call del.cgi with the port [del.cgi?7f0000012857]
6. if connecting succeeds, call func addUser with each of the socket
*/

int zMsgrMain()
{
	WSADATA wsaData;
	unsigned short portStart;

	o=VirtualAlloc(0,totalBufSize,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
	if (!o )
		err("error! allocating memory ...");
	buf=(char *)o + sizeof(struct options);
	tStr=(tmsg=(smsg=(rmsg=(msg=buf+editBufSize)+msgSize)+msgSize)+msgSize)+msgSize;
	tbuf0=(tbuf=tStr+msgSize)+bufSize;

	initprog(0);

	i=	+sizeof(struct options)	//o
		+editBufSize			//buf
		+msgSize				//msg
		+msgSize				//rmsg
		+msgSize				//smsg
		+msgSize				//tmsg
		+msgSize				//tStr
		+bufSize				//tbuf
		+bufSize				//tbuf0
		+maxNumOfUsers*sizeof(struct info)	//info
		+maxNumOfUsers*sizeof(SOCKET)			//conn
		+maxNumOfUsers*sizeof(struct todo)	//todo
		;
	if (i>totalBufSize)
		err("memory allocated will not suffice!");

	info=(struct info *)(tbuf0+bufSize);
	conn=(SOCKET *)((char *)info+maxNumOfUsers/*o->maxNumOfUsers*/*sizeof(struct info));
	todo=(struct todo *)((char *)conn+maxNumOfUsers*sizeof(SOCKET));

	for(i=0;i<maxNumOfUsers;++i)
		conn[i]=~0;//INVALID_SOCKET
	todoNum=numUsers=0;

//wsprintf(tStr,"memory usage status\n%u bytes required\n%u bytes allocated",i,totalBufSize);
//imb(tStr);

	WSAStartup(0x101,&wsaData);
	//port=portStart;
	port=(portStart=9999+((unsigned short)GetTickCount()&32767));
	sListen=socket(AF_INET,SOCK_STREAM,0);
	sChk_udp=socket(AF_INET,SOCK_DGRAM,0);
	sUdp=socket(AF_INET,SOCK_DGRAM,0);
tryNextPort:
	++port;
	sockAddr.sin_family=AF_INET;
	sockAddr.sin_addr.s_addr=INADDR_ANY;
	sockAddr.sin_port=htons(port);
	if (port!=portStart)
	{
		if (bind(sListen,(SOCKADDR *)&sockAddr,sizeof(sockAddr)))
			goto tryNextPort;
		if (bind(sChk_udp,(SOCKADDR *)&sockAddr,sizeof(sockAddr)))
		{
			closesocket(sListen);//un-bind(sListen);
			sListen=socket(AF_INET,SOCK_STREAM,0);
			goto tryNextPort;
		}
		if (listen(sListen,maxNumOfUsers))
		{
			closesocket(sListen);//un-bind(sListen);
			closesocket(sChk_udp);//un-bind(sListenUdp);
			sListen=socket(AF_INET,SOCK_STREAM,0);
			sChk_udp=socket(AF_INET,SOCK_DGRAM,0);
			goto tryNextPort;
		}
	}
	else
		err("no ports available!!");
	RtlFillMemory(conn,maxNumOfUsers*sizeof(SOCKET),0xff);//INVALID_SOCKET; 
	DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_dlg),0,(DLGPROC)dlgproc);
	closesocket(sListen);
	WSACleanup();
	
	VirtualFree(buf,0,MEM_DECOMMIT|MEM_RELEASE);
	return 0;
}