unsigned int i;
RECT	rMsg;
int		cyMsg;
SOCKET	sNewConn;
unsigned long t_ip;
unsigned short t_pt;

LRESULT CALLBACK dlgproc(HWND hWl,UINT M,WPARAM wp,LPARAM lp)
{
	if (M>=msgRecvMessage && M<(msgRecvMessage+maxNumOfUsers))
	{
		i=M-msgRecvMessage;
		rmsg[recv(conn[i],rmsg,msgSize,0)]=0;
		if (!info[i].ip)//info unavailable (ie this is the first data after connecting ...)
		{
//wsprintf(tmsg,"recv: %s",rmsg);show(~0,tmsg);
			rmsg[12]=rmsg[17]=0;
			info[i].ip=htonl(xatoi(rmsg+4));
			info[i].pt=htons((unsigned short)xatoi(rmsg+13));
			lstrcpyn(info[i].user,rmsg+18,1+maxUserLen);

			wsprintf(rmsg,"iam:%8.8X.%4.4X:",ntohl(ipLocal),port);//reply with "iam:%ip.%pt:%user\0"
			lstrcpyn(rmsg+18,user,1+maxUserLen);//rmsg-> "iam:%ip%.pt:%user\0"
			send(conn[i],rmsg,1+lstrlen(rmsg),0);
//wsprintf(tmsg,"send: %s",rmsg);show(~0,tmsg);

			wsprintf(rmsg,"new user \"%s\" [%s:%u]",info[i].user,ip2a(info[i].ip),ntohs(info[i].pt));
			show(~0,rmsg);
			return 0;
		}
		if (rmsg[0]==':')
			command(i,rmsg+1);
		else
			show(~0,rmsg);
		return 0;
	}

	switch(M)
	{
	case msgChkConnection:
		recvfrom(sChk_udp,rmsg,msgSize,0,0,0);
//wsprintf(tmsg,"udp_recv: %s",rmsg);show(~0,tmsg);
		rmsg[12]=rmsg[17]=0;
		t_ip=htonl(xatoi(rmsg+4));
		t_pt=htons((unsigned short)xatoi(rmsg+13));
		if (*((unsigned long *)rmsg)=='?tya')//=='ayt?'
		{
			sockAddrUdp.sin_family=AF_INET;
			sockAddrUdp.sin_addr.s_addr=t_ip;
			sockAddrUdp.sin_port=t_pt;
			wsprintf(rmsg,"yia:%8.8X.%4.4X",ntohl(ipLocal),port);
			sendto(sUdp,rmsg,1+lstrlen(rmsg),0,(struct sockaddr *)&sockAddrUdp,sizeof(sockAddrUdp));
//wsprintf(tmsg,"udp_send: %s",rmsg);show(~0,tmsg);
		}
		else
			if (*((unsigned long *)rmsg)==':aiy')//=='yia:'
			{
				for(i=0;!(todo[i].ip==t_ip && todo[i].pt==t_pt) && i<maxNumOfUsers;++i);
				if (i==maxNumOfUsers)//not found in the todo list
				{
					for(i=0;i<maxNumOfUsers && todo[i].ip;++i);
					todo[i].ip=t_ip;
					todo[i].pt=t_pt;
					++todoNum;
				}
				thread(newUser,i);
			}
		//else ignore...
		break;
	case msgNewConnection:
//show(~0,"new_user!");
		sNewUser=accept(sListen,0,0);
		addUser(sNewUser,0,0,0);//add socket to list and assign a new FD_READ message
		break;
	case WM_SIZE:
		GetClientRect(hW,&rMsg);
		//rMsg.bottom+=GetSystemMetrics(SM_CYEDGE);
		MoveWindow(hWbuf,0,0,rMsg.right-rMsg.left,rMsg.bottom-cyMsg+GetSystemMetrics(SM_CYEDGE),1);
		MoveWindow(hWmsg,0,rMsg.bottom-cyMsg,rMsg.right-rMsg.left,cyMsg,1);
//MUST thread this to make 'linescroll' message to work
		SendMessage(hWbuf,EM_LINESCROLL,0,32767);
		break;
	case WM_CTLCOLOREDIT:
		SetTextColor((HDC)wp,o->txColorSend);
		SetBkColor((HDC)wp,o->bkColorSend);
		return (long)hBrMsg;
	case WM_CTLCOLORSTATIC:
		SetTextColor((HDC)wp,o->txColor);
		SetBkColor((HDC)wp,o->bkColor);
		return (long)hBrBuf;
	case WM_COMMAND:
		switch(LOWORD(wp))
		{
		case IDC_msg:
			if (HIWORD(wp)==EN_UPDATE)
			{
				SendMessage(hWmsg,WM_GETTEXT,msgSize,(LPARAM)msg);
				lp=lstrlen(msg);
				if (msgState==msgStatePrompt && msg[0]!=':')
				{
					msg[1]=msg[0];msg[0]=':';
					SendMessage(hWmsg,WM_SETTEXT,0,(LPARAM)msg);
					SendMessage(hWmsg,EM_SETSEL,2,2);
				}
			}
			break;
		case IDOK:
			SendMessage(hWmsg,WM_GETTEXT,msgSize,(LPARAM)msg);
			if (msg[0])
			{
				if (msg[0]==':')
					command(~0,msg+1);
				else
				{
					lstrcpy(smsg,user);
					*((unsigned short *)(smsg+userLen))=' :';//lstrcpy(smsg+userLen,": ");
					lstrcpy(smsg+userLen+2,msg);
					SendMessage(hWmsg,WM_SETTEXT,0,0);
					thread(send2all,smsg);
				}
			}
			break;
		case IDCANCEL:
			exitprog();
			EndDialog(hW,0);
			break;
		}
		break;
	case WM_TIMER:
		switch(wp)
		{
		case timerNextAyt:
			thread(doTodo,0);
			break;
		case timerTryConnect:
			while(lastTryConnect--)
			{
				if (todo[lastTryConnect].ip)
				{
					thread(newUser,lastTryConnect);
					goto loopExit;
				}
			}
			KillTimer(hW,timerTryConnect);
			thread(delUsersFromServer,0);
			//break;
		}
loopExit:
		break;
	case WM_INITDIALOG:
		hW=hWl;
		WSAAsyncSelect(sListen,hW,msgNewConnection,FD_ACCEPT);
		WSAAsyncSelect(sChk_udp,hW,msgChkConnection,FD_READ);
		hWbuf=GetDlgItem(hW,IDC_buf);
		hWmsg=GetDlgItem(hW,IDC_msg);
		//GetSystemMetrics;
		GetWindowRect(hWmsg,&rMsg);
		cyMsg=rMsg.bottom-rMsg.top;
		GetClientRect(hW,&rMsg);
		//rMsg.bottom+=GetSystemMetrics(SM_CYEDGE);
		MoveWindow(hWbuf,0,0,rMsg.right-rMsg.left,rMsg.bottom-cyMsg+GetSystemMetrics(SM_CYEDGE),1);
		MoveWindow(hWmsg,0,rMsg.bottom-cyMsg,rMsg.right-rMsg.left,cyMsg,1);
		SendMessage(hWmsg,EM_SETLIMITTEXT,msgSize,0);
		if (o->X!=32767 || o->Y!=32767 || o->dX!=32767 || o->dY!=32767)
			MoveWindow(hW,o->X,o->Y,o->dX,o->dY,1);
		thread(initwindow,0);

			for(i=0;i<maxNumOfUsers;++i)
			{
				if (~conn[i])//!=INVALID_SOCKET
				{
					wsprintf(tmsg,"[%s:%u]:\"%s\"",ip2a(info[i].ip),ntohs(info[i].pt),info[i].user);
					show(~0,tmsg);
				}
			}
			wsprintf(tmsg,"%u user(s) total",numUsers);

		break;
	}
	return 0;
}
