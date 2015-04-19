#define _DoLogin	0
#define _GetList	1
#define _AddUser	2
#define _DelUser	3

int login(int reason)
{
	SOCKET s;
	struct sockaddr_in sa;
	unsigned int i0,iT;

	gethostname(tmsg,bufSize);
	pHost=gethostbyname(tmsg);
	ipLocal=*((unsigned long *)pHost->h_addr);
	wsprintf(tStr,"%8.8X.%4.4X",ntohl(ipLocal),port);
	wsprintf(tmsg,"finding server \"%s\" ...",o->serverHost);show(~0,tmsg);
	if (!(pHost=(struct hostent *)gethostbyname(o->serverHost)))
	{
		beep;
		show(~0,"server not found!");
		goto loginFailed;
	}
	ipServer=*((unsigned long *)pHost->h_addr);
	wsprintf(tmsg,"\"%s\"->%s",o->serverHost,ip2a(ipServer));
	show(~0,tmsg);
	sa.sin_family=AF_INET;
	sa.sin_addr.s_addr=ipServer;
	sa.sin_port=htons(o->serverPort);
	wsprintf(tmsg,"connecting to %s:%u ...",ip2a(ipServer),o->serverPort);
	show(~0,tmsg);
	s=socket(AF_INET,SOCK_STREAM,0);
	if (connect(s,(struct sockaddr *)&sa,sizeof(sa))<0)
	{
		beep;
		show(~0,"could not connect to server!");
		closesocket(s);
		goto loginFailed;
	}
	//show(~0,"connected to server");
	wsprintf(tmsg,	"ippt:%s\n"
					"user:%s\n",
					tStr,
					user
			);
	wsprintf(tbuf,	"POST %s?list&add HTTP/1.0\r\n"
					"Host: %s\r\n"
					"Accept: */*\r\n"
					"User-Agent: %s/%.6x\r\n"
					"Content-length: %u\r\n"
					"Connection: close\r\n"
					"\r\n"
					"%s"
				,	o->serverPath,
					o->serverHost,
					zMsgrVersion,
					zMsgrVersionNum,
					lstrlen(tmsg),
					tmsg
			);
	show(~0,"registering ip and port ...");
	send(s,tbuf,lstrlen(tbuf),0);
	show(~0,"receiving list of users ...");
	for(iT=0;i0=recv(s,tbuf+iT,bufSize-iT,0);iT+=i0);//recv until close
	closesocket(s);
	for(i0=0;i0<iT && tbuf[i0]!=' ';++i0);//search for httpRetCode "HTTP/1.x <???> ...."
	if (*((unsigned long *)(tbuf+i0))!=0x30303220)//==' 200' (http->OK)
	{
		beep;
		for(iT=i0;(*((unsigned long *)(tbuf+iT)))!=0x0a0d0a0d;++iT);tbuf[iT]=0;
		show(~0,"error! http reply header:");
		show(~0,tbuf);
		goto loginFailed;
	}
	while(i0<iT && (*((unsigned long *)(tbuf+(i0++))))!=0x0a0d0a0d);
	i0+=3;tbuf[iT]=0;//i0 & iT are now pointers to the beginning and the end of data
	numUsers=0;
	todoNum=0;
	while(i0<iT)
	{
		tbuf[i0+8]=tbuf[i0+13]=0;
		todo[todoNum].ip=htonl(xatoi(tbuf+i0));
		todo[todoNum].pt=htons((unsigned short)xatoi(tbuf+i0+9));
		wsprintf(tmsg,"[%s:%u] \"%s\"",ip2a(todo[todoNum].ip),ntohs(todo[todoNum].pt),tbuf+(i0+=14));
		i0+=1+lstrlen(tbuf+i0);
		show(~0,tmsg);
		++todoNum;
	}

	wsprintf(tmsg,"%u user(s) found ...",todoNum);show(~0,tmsg);
	if (todoNum)
	{
		show(~0,"checking if user(s) are alive ...");
		attemptNum=totalNumOfAyts;
		doTodo(0);
		SetTimer(hW,timerNextAyt,delayNextAyt,0);
		show(~0,"waiting for reply ...");
		lastTryConnect=maxNumOfUsers;
		SendMessage(hW,WM_TIMER,timerTryConnect,0);
		SetTimer(hW,timerTryConnect,delayTryConnect,0);
	}
	else
		show(~0,"waiting for connections ...");
//	show(~0,"login complete!");
	return 0;
loginFailed:
	beep;
	return ~0;
}

int delUsersFromServer(void *V)
{
	SOCKET s;
	struct sockaddr_in sa;
	unsigned int i,l;

	tbuf[l=0]=0;
	for(i=0;i<maxNumOfUsers;++i)
		if (todo[i].ip)
		{
			wsprintf(tbuf+l,"%8.8X.%4.4X\n",ntohl(todo[i].ip),ntohs(todo[i].pt));
			l+=14;//8(ip)++1(.)+4(pt)+1(\n)
		}

	wsprintf(tmsg,"no response received from %u user(s) ...",todoNum);show(~0,tmsg);
	show(~0,"requesting server to delete the user(s) ...");
	sa.sin_family=AF_INET;
	sa.sin_addr.s_addr=ipServer;
	sa.sin_port=htons(o->serverPort);
	wsprintf(tmsg,"connecting to %s:%u ...",ip2a(ipServer),o->serverPort);
	show(~0,tmsg);
	s=socket(AF_INET,SOCK_STREAM,0);
	if (connect(s,(struct sockaddr *)&sa,sizeof(sa))<0)
	{
		beep;
		show(~0,"could not connect to server!");
		closesocket(s);
	}
	else
	{
		lstrcpyn(tmsg,tbuf,bufSize);
		wsprintf(tbuf,	"POST %s?del HTTP/1.0\r\n"
						"Host: %s\r\n"
						"Accept: */*\r\n"
						"User-Agent: %s/%.6x\r\n"
						"Content-length: %u\r\n"
						"Connection: close\r\n"
						"\r\n"
						"%s"
					,	o->serverPath,
						o->serverHost,
						zMsgrVersion,
						zMsgrVersionNum,
						lstrlen(tmsg),
						tmsg
				);
		show(~0,"sending list of user(s) to remove ...");
		send(s,tbuf,lstrlen(tbuf),0);
		recv(s,tbuf,bufSize,0);//no useful info is sent back
		closesocket(s);
		show(~0,"deleted invalid user(s)");
	}
	return 0;
}
