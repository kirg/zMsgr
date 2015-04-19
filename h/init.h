
int initprog()
{
	int i;

	GetModuleFileName(0,o->iniFile,maxPath);
	i=lstrlen(o->iniFile);
	while(o->iniFile[--i]!='\\');
	o->iniFile[++i]=0;//INIFile will end with a '\\'
	lstrcpyn(o->iniFile+i,iniFileName,maxPath-i);

	GetPrivateProfileString("http","host",notFound,o->serverHost,maxServerNameLen,o->iniFile);
	if (*((unsigned long *)o->serverHost)==0xffffff)
		lstrcpy(o->serverHost,_server_host);
	GetPrivateProfileString("http","port",notFound,tStr,msgSize,o->iniFile);
	if (*((unsigned long *)tStr)==0xffffff)
		o->serverPort=_server_port;
	else
		o->serverPort=(unsigned short)datoi(tStr);
	GetPrivateProfileString("http","path",notFound,o->serverPath,maxCgiPathLen,o->iniFile);
	if (*((unsigned long *)o->serverPath)==0xffffff)
		lstrcpy(o->serverPath,_server_path);

	GetPrivateProfileString("http","proxy",notFound,o->proxy,maxServerNameLen,o->iniFile);
	if (*((unsigned long *)o->proxy)==0xffffff)
		o->proxy[0]=0;
	GetPrivateProfileString("http","proxyPort",notFound,tStr,msgSize,o->iniFile);
	if (*((unsigned long *)tStr)==0xffffff)
		o->proxyPort=httpProxyPort;
	else
		o->proxyPort=(unsigned short)datoi(tStr);

	o->X=GetPrivateProfileInt("window","X",32767,o->iniFile);
	o->Y=GetPrivateProfileInt("window","Y",32767,o->iniFile);
	o->dX=GetPrivateProfileInt("window","dX",32767,o->iniFile);
	o->dY=GetPrivateProfileInt("window","dY",32767,o->iniFile);

	GetPrivateProfileString("window","bkColor",notFound,tStr,msgSize,o->iniFile);
	if (*((unsigned long *)tStr)==0xffffff)
		o->bkColor=_window_bkColor;
	else
		o->bkColor=xatoi(tStr);
	GetPrivateProfileString("window","txColor",notFound,tStr,msgSize,o->iniFile);
	if (*((unsigned long *)tStr)==0xffffff)
		o->txColor=_window_txColor;
	else
		o->txColor=xatoi(tStr);

	GetPrivateProfileString("window","bkColorSend",notFound,tStr,msgSize,o->iniFile);
	if (*((unsigned long *)tStr)==0xffffff)
		o->bkColorSend=_window_bkColorSend;
	else
		o->bkColorSend=xatoi(tStr);
	GetPrivateProfileString("window","txColorSend",notFound,tStr,msgSize,o->iniFile);
	if (*((unsigned long *)tStr)==0xffffff)
		o->txColorSend=_window_txColorSend;
	else
		o->txColorSend=xatoi(tStr);

	
	maxNumOfUsers=GetPrivateProfileInt("chat","maxNumOfUsers",~0,o->iniFile);
	if (!~maxNumOfUsers)
		maxNumOfUsers=_chat_maxNumOfUsers;

	GetPrivateProfileString("chat","user",notFound,o->user,1+maxUserLen,o->iniFile);
	if (*((unsigned long *)o->user)!=0xffffff)
	{
		lstrcpyn(user,o->user,1+maxUserLen);
		userLen=lstrlen(user);
	}
//	else
//		user[0]=0;

	hBrBuf=CreateSolidBrush(o->bkColor);
	hBrMsg=CreateSolidBrush(o->bkColorSend);
	return 0;
}

int initwindow(void *V)
{
	int i;
//	for(i=0;i<20;++i)//for 40 empty lines
//		show(~0,"\r\n");
	lstrcpy(tStr,":login ");
	if (user[0])
		lstrcat(tStr,user);
	SendMessage(hWmsg,WM_SETTEXT,0,(LPARAM)tStr);
	SendMessage(hWmsg,EM_SETSEL,i,i=lstrlen(tStr));
	return 0;
}

int exitprog()
{
	return 0;
}
