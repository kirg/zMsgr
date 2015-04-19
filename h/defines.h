#define iniFileName			"zMsgr.ini"

#define delayTryConnect		10000
#define delayNextAyt		5000//
#define totalNumOfAyts		10

#define	notFound			"\xff\xff\xff"

#define timerNextAyt		142857
#define timerTryConnect		857142

#pragma comment(linker,"/align:4096")
#pragma comment(linker,"/nodefaultlib")
#pragma comment(linker,"/entry:zMsgrMain")

#define msgNewConnection	0x8000
#define msgChkConnection	0x8001
#define msgRecvMessage		0x8002

#define maxUserLen			16
#define	totalBufSize		128000//1048576
#define editBufSize			65535
#define msgSize				512
#define bufSize				16384
#define maxServerNameLen	100
#define maxCgiPathLen		100
#define maxPath				300

#define err(M)		{MessageBeep(~0);MessageBox(GetForegroundWindow(),M,"error!",MB_ICONERROR);return ~0;}
#define beep		MessageBeep(~0)
#define imb(M)		MessageBox(0,M,"hi!",MB_ICONINFORMATION)
#define emb(M)		{MessageBeep(~0);MessageBox(0,M,"error!",MB_ICONERROR);}
#define thread(func,param)	CreateThread(0,0,(LPTHREAD_START_ROUTINE)&func,(void *)param,0,&threadID)
#define ip2a(ip)	inet_ntoa(*((struct in_addr *)&(ip)))

#define msgStateChat		0
#define msgStateUser		1
#define msgStatePrompt		2

#pragma pack(1)
struct options
{
	char	iniFile[MAX_PATH];
	unsigned short	startPort;
	char	user[1+maxUserLen];
	int		X;
	int		Y;
	int		dX;
	int		dY;
	int		isMinMaxRes;//minimized,maximized or restored
	int		systray;
	int		bkColor;
	int		txColor;
	int		bkColorSend;
	int		txColorSend;
	int		modeAuto;
	int		maxNumOfUsers;
	char			serverHost[maxServerNameLen];
	unsigned short	serverPort;
	char			serverPath[maxCgiPathLen];
	char			proxy[maxServerNameLen];
	unsigned short	proxyPort;
};

struct info
{
	unsigned long	ip;
	unsigned short	pt;
	char			user[maxUserLen+1];//+1 for '\0'
};

struct todo
{
	unsigned long	ip;
	unsigned short	pt;
};
#pragma pack()