/*
 * Channel retrieval tool
 *
 * (C) 1998 Alan Cox.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h> 
#include <errno.h>
#include <sys/time.h>
#include <sys/select.h>

#include <glib.h>
#include <gnome-xml/parser.h>

static char *top;

static int host_write(int fd, char *ptr)
{
	int err=write(fd, ptr, strlen(ptr));
	return err;
}

static char host_rbuf[4096];
static int host_rptr;
static int host_rlen;

static int host_getblk(int fd)
{
	int rlen;
	fd_set rfd;
	struct timeval tv;
	
	while(1)
	{
		rlen=read(fd, host_rbuf, sizeof(host_rbuf));
		if(rlen>0)
		{
			host_rlen=rlen;
			host_rptr=0;
			return host_rlen;
		}
		if(rlen==0)
		{
			return 0;
		}
		if(rlen==-1 && errno!=EWOULDBLOCK)
		{
			return 0;
		}
		tv.tv_sec=60;
		tv.tv_usec=0;
		FD_ZERO(&rfd);
		FD_SET(fd, &rfd);
		
		if(select(fd+1, &rfd, NULL, NULL, &tv)<1)
			return 0;
	}
}

char *host_readline(int fd)
{
	static char buf[4096];
	char *bp=buf;
	
	while(bp-buf<4095)
	{
		if(host_rptr==host_rlen)
			if(host_getblk(fd)==0)
			{
				if(bp==buf)
					return NULL;
				else
					*bp=0;
				return buf;
			}
		*bp=host_rbuf[host_rptr++];
		if(*bp=='\n')
		{
			*bp=0;
			printf(">%s\n",buf);
			return buf;
		}
		if(*bp!='\r')
			bp++;
	}
}

	

static int host_connect_attempt(struct in_addr ia, int port)
{
	int s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	struct sockaddr_in sin;
	
	fd_set wfd;
	struct timeval tv;
	
	if(s==-1)
	{
		perror("socket");
		return -1;
	}
	
	if(fcntl(s, F_SETFL, FNDELAY)==-1)
	{
		perror("nonblocking");
		close(s);
		return -1;
	}

	sin.sin_family = AF_INET;	
	sin.sin_addr   = ia;
	sin.sin_port   = htons(port);
	
	if(connect(s, (struct sockaddr *)&sin, sizeof(sin))==-1 && errno != EINPROGRESS)
	{
		perror("connect");
		close(s);
		return -1;
	}	
	
	tv.tv_sec = 60;		/* We use 60 second timeouts for now */
	tv.tv_usec = 0;
	
	FD_ZERO(&wfd);
	FD_SET(s, &wfd);
	
	switch(select(s+1, NULL, &wfd, NULL, &tv))
	{
		case 0:
			/* Time out */
			close(s);
			return -1;
		case -1:
			/* Ermm.. ?? */
			perror("select");
			close(s);
			return -1;
	}
	
	return s;
}
 
int host_connect(const char *host, int port)
{
	struct hostent *h;
	int i;
	int s;
	
	h=gethostbyname(host);
	if(h==NULL)
	{
		fprintf(stderr,"unable to resolve '%s'.\n", host);
		return -1;
	}
	
	
	for(i=0; h->h_addr_list[i]; i++)
	{
		struct in_addr ia;
		memcpy(&ia, h->h_addr_list[i],4);
		s=host_connect_attempt(ia, port);
		if(s != -1)
			return s;
	}
	fprintf(stderr, "unable to connect to '%s'.\n", host);
	return -1;
}


static char *layer_find(xmlNodePtr node, char *match, char *fail)
{
	while(node!=NULL)
	{
//		xmlDebugDumpNode(stdout, node, 32);
//		printf("%s.\n", node->name);
		if(strcasecmp(node->name, match)==0)
		{
			if(node->childs != NULL && node->childs->content != NULL)
				return node->childs->content;
			else
				return fail;
		}
		node = node->next;
	}
	return fail;
}

static char *clean(char *x)
{
	char *p=x;
	while(*p)
	{
		if(*p=='/')
			*p=':';
		if(*p<' ' || *p>126)
			*p='?';
		if(*p=='\'')
			*p='"';
		if(*p=='\\')
			*p=':';
		p++;
	}
	return x;
}

static void tree_walk(xmlNodePtr root)
{
	xmlNodePtr walk = root->childs;
	xmlNodePtr channel = NULL;
	xmlNodePtr image = NULL;
	xmlNodePtr item[16];
	int items=0;
	int i;
	char *t;
	char n[512];
	
	while(walk!=NULL)
	{
		/* This is the channel top level */
		printf("Top level '%s'.\n", walk->name);
		if(strcasecmp(walk->name, "channel")==0)
			channel = walk;
		if(strcasecmp(walk->name, "image")==0)
			image = walk;
		if(strcasecmp(walk->name, "item")==0 && items < 16)
			item[items++]=walk;
		walk=walk->next;
	}
	if(channel==NULL)
	{
		fprintf(stderr, "No channel definition.\n");
		return;
	}
	printf("Will open '%s'.\n", 
		t=clean(layer_find(channel->childs, "title", "No title")));
		
	snprintf(n, 512, "%s/Channels", top, t);
	mkdir(n, 0755);
	snprintf(n, 512, "%s/Channels/%s", top, t);
	mkdir(n, 0755);
	
	for(i=0;i<items;i++)
	{
		FILE *fp;
		char *p= layer_find(item[i]->childs, "title", "No information");
		
		snprintf(n, 512, "%s/Channels/%s/channel%d.desktop", top, t, i);
		
		fp=fopen(n, "w");
		if(fp!=NULL)
		{
			fprintf(fp, "[Desktop Entry]\nName=%s\n", p);
                	fprintf(fp, "Comment=Lanch web browser\n");
			fprintf(fp, "Exec=/usr/bin/gnome-moz-remote --newwin '%s'\n", 
				layer_find(item[i]->childs, "link", ""));
                	fprintf(fp, "Icon=/usr/share/pixmaps/netscape.png\n");
			fprintf(fp, "Terminal=0\n");
			fprintf(fp, "Type=Application\n");
                	fclose(fp);
                }
	}
	for(;i<16;i++)
	{
		snprintf(n, sizeof(n), "%s/Channels/%s/channel%d.desktop", top, t, i);
		remove(n);
	}
}

static char document[65536];

int main(int argc, char *argv[])
{
	int s;
	int head=1;
	int docp=0;
	xmlDocPtr doc;
	
	char *p;
		
	if(argc!=3)
	{
		fprintf(stderr, "%s: url path.\n", argv[0]);
		exit(1);
	}
	
	p=getenv("HOME");
	if(!p)
	{
		fprintf(stderr, "%s: $HOME not set.\n", argv[0]);
		exit(1);
	}
	
	top=malloc(strlen(p)+2+strlen(".gnome/apps"));
	if(top==NULL)
	{
		fprintf(stderr, "%s: out of memory.\n", argv[0]);
		exit(1);
	}
	sprintf(top, "%s/.gnome/apps",p);
	
	s=host_connect(argv[1],80);
	if(s==-1)
		return 1;
	
	snprintf(document, sizeof(document),"GET %s HTTP/1.0\r\nhost: %s\r\n\r\n",
		argv[2], argv[1]);

	host_write(s, document);
		
	while((p=host_readline(s))!=NULL)
	{
		if(!head && docp+strlen(p) < 65535)
		{
			memcpy(document+docp, p, strlen(p));
			docp+=strlen(p);
			document[docp++]='\n';
		}
		if(head && *p == 0)
			head = 0;
	}
	close(s);
	document[docp]=0;
	
	doc=xmlParseMemory(document, docp);
	
	if(doc==NULL)
	{
		fprintf(stderr, "Unable to parse document.\n");
		exit(1);
	}
	
	tree_walk(doc->root);
}
	

