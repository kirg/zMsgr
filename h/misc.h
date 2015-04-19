__inline unsigned int datoi(const char *a)
{
	__asm
	{
		xor eax,eax
		mov ecx,eax
		mov ebx,a
next:
		movzx ecx,[byte ptr ebx]
		and ecx,ecx
		jz finished
		mov edx,0Ah
		mul edx
		sub ecx,30h
		add eax,ecx
		inc ebx
		jmp next
finished:
	};
}

__inline unsigned int xatoi(const char *a)
{
	__asm
	{
		xor eax,eax
		mov ebx,a
next:
		movzx ecx,[byte ptr ebx]
		and ecx,ecx
		jz finished
		mov edx,10h
		mul edx
		sub ecx,30h
		cmp ecx,0Ah
		jl skip
		sub ecx,07h
skip:
		add eax,ecx
		inc ebx
		jmp next
finished:
	};
}

int crack_http_url(const char *url,char *host,unsigned short *port,char *path)
{
	int i,j,k;
	char ptStr[6];
	i=7*(*((unsigned long *)url)=='ptth' && *((unsigned long *)(url+3))=='//:p');
	for(j=i;url[i]!='/' && url[i]!=':' && url[i];++i);
	if (i<=j)
		return ~0;//no server name
	lstrcpyn(host,url+j,i-j+1);//i-j+1(for '\0')
	if (port)	*port=80;//default port
	if (path)	*((unsigned short *)path)='\0/';//default path="/"
	switch(url[i])
	{
	case ':':
		//i->':'
		for(j=++i;url[i]!='/' && url[i];++i);
		k=i+!url[i]-j+1;
		if (k>6) k=6;
		lstrcpyn(ptStr,url+j,k);
		if (port)
			*port=(unsigned short)datoi(ptStr);
	case '/':
		//i->'/'
		if (path)
			lstrcpy(path+1,url+i+1);
	}
	return 0;
}