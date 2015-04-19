unsigned int maxNumOfUsers;

int msgState;

HWND	hW;
HWND	hWmsg;
HWND	hWbuf;
HBRUSH	hBrMsg,hBrBuf;

char	*buf;
char	*msg;
char	*smsg;
char	*rmsg;
char	*tmsg;
char	*tStr;
char	*tbuf;
char	*tbuf0;

struct hostent	*pHost;
unsigned long	ipServer;
unsigned short	ptServer;
unsigned long	ipLocal;
unsigned short	port;

int				numUsers;
unsigned int	openConn;


//char	fake_olduser[maxUserLen+1];
char	user[maxUserLen+1];
char	userLen;

struct options	*o;
struct info		*info;//[maxNumOfUsers];
struct todo		*todo;//[maxNumOfUsers];

unsigned int	lastTryConnect;
unsigned int	attemptNum;
unsigned int	todoNum;


SOCKET	*conn;//[maxNumOfUsers];

SOCKET	sListen,sNewUser;
SOCKET	sChk_udp,sUdp;
SOCKADDR_IN sockAddr;
SOCKADDR_IN sockAddrUdp;

DWORD	threadID;

int show_(const char *m)
{
	SendMessage(hWbuf,WM_GETTEXT,bufSize,(LPARAM)buf);
	lstrcat(buf,m);
	SendMessage(hWbuf,WM_SETTEXT,0,(LPARAM)buf);
	SendMessage(hWbuf,EM_LINESCROLL,0,32767);
	return 0;
}

int show(unsigned int connNum,const char *m)
{
	int l;
	if (!~connNum)
	{
		SendMessage(hWbuf,WM_GETTEXT,bufSize,(LPARAM)buf);
		l=lstrlen(buf);
		if (l)
		{
			lstrcpyn(buf+l,"\r\n",bufSize-l);
			l+=2;
		}
		lstrcpyn(buf+l,m,bufSize-l);
		SendMessage(hWbuf,WM_SETTEXT,0,(LPARAM)buf);
		SendMessage(hWbuf,EM_LINESCROLL,0,32767);
	}
	else
		send(conn[connNum],m,lstrlen(m),0);
	return 0;
}