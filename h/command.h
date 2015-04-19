int command(unsigned int connNum,char *cmdStr)//connNum=~0 for local command
{
	unsigned int i,j;
	char	*param;
	
	for(i=0;cmdStr[i]!=' ' && cmdStr[i]!='=' && cmdStr[i] && i<4;++i);//find first space within the first four bytes
	for(j=i;cmdStr[j]!=' ' && cmdStr[j]!='=' && cmdStr[j];++j);
	param+=!!(*(param=cmdStr+j));cmdStr[j]=0;
	if (!i && !~connNum)
	{
		msgState=msgStatePrompt;
		SendMessage(hWmsg,WM_SETTEXT,0,(LPARAM)":");
		SendMessage(hWmsg,EM_SETSEL,1,1);
	}
	else
	{
		switch(i=((*((unsigned int *)cmdStr)))&(0xffffffff>>(32-8*i)))
		{
		case ':':
			if (!~connNum)
				msgState=msgStateChat;
			break;
		case 'tixe'://exit
			//EndDialog(hW,0);
			PostMessage(hW,WM_CLOSE,0,0);
			break;
		case 'tsil'://list
			gethostname(tStr,msgSize);
			pHost=gethostbyname(tStr);
			ipLocal=*((unsigned long *)pHost->h_addr);
			wsprintf(tmsg,"[%s:%u] (%s) \"%s\" user list:",ip2a(ipLocal),port,tStr,user);
			show(connNum,tmsg);
			for(i=0;i<maxNumOfUsers;++i)
			{
				if (~conn[i])//!=INVALID_SOCKET
				{
					wsprintf(tmsg,"[%s:%u]:\"%s\"",ip2a(info[i].ip),ntohs(info[i].pt),info[i].user);
					show(connNum,tmsg);
				}
			}
			wsprintf(tmsg,"%u user(s) total",numUsers);
			show(connNum,tmsg);
			break;
		case 'tset'://test
			show(connNum,"test!");
			break;
		case 'igol'://login
			if (param[0])
			{
				if (!user[0])
					msgState=msgStateChat;
				lstrcpyn(user,param,1+maxUserLen);
				userLen=lstrlen(user);
				SendMessage(hWmsg,EM_SETLIMITTEXT,msgSize,0);
				thread(login,0);
				//login(0);
			}
			else
			{
				beep;
				show(connNum,"user name missing!");
			}
			break;
		default:
			beep;
			wsprintf(tmsg,"[%8.8X] unknown command \"%s\"",i,cmdStr);
			show(connNum,tmsg);
		}
		if (!~connNum)
		{
			if (msgState==msgStatePrompt)
			{
				SendMessage(hWmsg,WM_SETTEXT,0,(LPARAM)":");
				SendMessage(hWmsg,EM_SETSEL,1,1);
			}
			else
				SendMessage(hWmsg,WM_SETTEXT,0,0);
		}
	}
	return 0;
}
