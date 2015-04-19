//addUser called on accept-ing connection
int addUser(SOCKET s,unsigned long ip,unsigned short pt,char *user)
{
	unsigned int i;

	//newUser checks if already connected before connecting...
	//it is done again here in case two threads complete connecting at the same time
	if (ip && pt)//not called as addUser(s,0,0,0); on accepting connection
	{
		for(i=0;!(info[i].ip==ip && info[i].pt==pt) && i<maxNumOfUsers;++i);
		if (i<maxNumOfUsers)//already connected (found in the connected list)
		{
			closesocket(s);
			return i;
		}
	}

	for(i=0;~conn[i] && info[i].ip && i<maxNumOfUsers;++i);//while ip!=0 && conn[i]!=INVALID_SOCKET
	if (i==maxNumOfUsers)
	{
		beep;
		return ~0;
	}
	conn[i]=s;
	if (user)
		lstrcpyn(info[i].user,user,1+maxUserLen);
	else
		info[i].user[0]=0;
	info[i].ip=ip;
	info[i].pt=pt;
	++numUsers;
	if (~s)//!=INVALID_SOCKET
		WSAAsyncSelect(s,hW,msgRecvMessage+i,FD_READ);
	return i;
}

//delUser called if send fails
int delUser(int i)
{
	if (~conn[i])
	{
		closesocket(conn[i]);
		conn[i]=~0;//INVALID_SOCKET;
	}
	info[i].ip=0;
	--numUsers;
	return i;
}


int doTodo(void *V)
{
	unsigned int i;
	char lStr[4+1+12+1+maxUserLen+1];
	struct sockaddr_in sa;
	if (todoNum && --attemptNum)
	{
		wsprintf(lStr,"ayt?%8.8X.%4.4X:",ntohl(ipLocal),port);
		lstrcpyn(lStr+18,user,1+maxUserLen);//lStr-> ip,pt,user\0
		for(i=0;i<maxNumOfUsers;++i)
		{
			if (todo[i].ip)
			{
				sa.sin_family=AF_INET;
				sa.sin_addr.s_addr=todo[i].ip;
				sa.sin_port=todo[i].pt;
				sendto(sUdp,lStr,lstrlen(lStr),0,(struct sockaddr *)&sa,sizeof(sa));
//wsprintf(tmsg,"udp_send:[%s:%u]: %s",ip2a(todo[i].ip),ntohs(todo[i].pt),lStr);show(~0,tmsg);
			}
		}
	}
	else
		KillTimer(hW,timerNextAyt);
	return 0;
}

int send2all(char *m)
{
	unsigned int i;
	for(i=0;i<maxNumOfUsers;++i)
	{
tryAgain:
		if (~conn[i])//!=INVALID_SOCKET
		{

			if (send(conn[i],m,lstrlen(m),0)==SOCKET_ERROR)
			{
				if (WSAGetLastError()==WSAEWOULDBLOCK)
				{
					Sleep(100);
					goto tryAgain;
				}
				else
				{
wsprintf(tStr,"\"%s\" disconnected!",info[i].user);show(~0,tStr);
					delUser(i);
					beep;
				}
			}
		}
	}
	show(~0,m);
	return 0;
}

int newUser(int N)
{
	SOCKET sNewConn;
	struct sockaddr_in sa;
	char lStr[64];//9+5+1+maxUserLen];
	unsigned int i;

	for(i=0;!(info[i].ip==todo[N].ip && info[i].pt==todo[N].pt) && i<maxNumOfUsers;++i);
	if (i<maxNumOfUsers)//already connected (found in the connected list)
	{
		--todoNum;todo[N].ip=0;
		wsprintf(lStr,"already connected to \"%s\" [%s:%u]",i,info[i].user,ip2a(info[i].ip),ntohs(info[i].pt));
		show(~0,lStr);
		return 0;
	}

	sNewConn=socket(AF_INET,SOCK_STREAM,0);
	sa.sin_family=AF_INET;
	sa.sin_addr.s_addr=todo[N].ip;
	sa.sin_port=todo[N].pt;
wsprintf(tmsg,"connecting to (%d) [%s:%u]",N,ip2a(todo[N].ip),ntohs(todo[N].pt));show(~0,tmsg);
	if (!connect(sNewConn,(struct sockaddr *)&sa,sizeof(sockAddrUdp)))
	{
		--todoNum;todo[N].ip=0;
		wsprintf(lStr,"iam:%8.8X.%4.4X:",ntohl(ipLocal),port);
		lstrcpyn(lStr+18,user,1+maxUserLen);
		send(sNewConn,lStr,1+lstrlen(lStr),0);//wsprintf(tmsg,"send: %s",lStr);show(~0,tmsg);
		recv(sNewConn,lStr,64,0);//wsprintf(tmsg,"recv: %s",lStr);show(~0,tmsg);
		lStr[12]=lStr[17]=0;
		i=addUser(sNewConn,htonl(xatoi(lStr+4)),htons((unsigned short)xatoi(lStr+13)),lStr+18);
		wsprintf(lStr,"connected to \"%s\" [%s:%u]",info[i].user,ip2a(info[i].ip),ntohs(info[i].pt));
		show(~0,lStr);
		return ~0;
	}
	else
	{
		beep;
		show_("...failed");
	}
	return 0;
}
