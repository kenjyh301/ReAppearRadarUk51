/*
 * main.cpp
 *
 *  Created on: Jan 11, 2021
 *      Author: Minh
 */

#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <process.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/dcmd_chr.h>
#include <stdint.h>

#define SLEEP_TIME 16000

int nodeid = 0;
int pid = -1;
int chid = 1;
int coid = -1;

int n=255;
int sign=1;

void get_uk51_nodeid() {
	nodeid = netmgr_strtond("uk51.", NULL);
	printf("uk51 nodeid: %d\n", nodeid);
}

void get_ppi51_pid() {
	char tmp[512];
	FILE* fp = popen("pidin -n uk51. -p ppi51 -F '%a %J %B' | grep RECEIVE",
			"r");
	fscanf(fp, "%d %s %d", &pid, tmp, &chid);
	printf("ppi51 pid %d, chid %d\n", pid, chid);
	pclose(fp);
}

void connect_ppi51() {
	coid = ConnectAttach(nodeid, pid, chid, _NTO_SIDE_CHANNEL, 0);
	printf("coid: %d\n", coid);
}

FILE* f;
uint8_t pre0[]={0,0};
uint8_t pre1[]={0,0};
void send_to_ppi51(int az, int azd, int num_of_line) {

	unsigned char buf[1370];


	fread(buf,2,1,f);
	fread(buf,1370,1,f);

	buf[4]*=2;
	buf[5]*=2;
	uint16_t* tmp=(uint16_t*)&buf[0];
	if (pre0[0] != buf[0] || pre0[1] != buf[1]) {
		int r = MsgSend(coid, buf, 1370, 0, 0);
		if (r == -1) {
			printf("send fail\n");
			exit(EXIT_FAILURE);
		}
//		printf("%d\n", *tmp);
	}

//	pre0[0]=buf[0];
//	pre0[1]=buf[1];
//	usleep(SLEEP_TIME);
//
//	fread(buf,2,1,f);
//	fread(buf,1370,1,f);
//	if (pre1[0] != buf[0] || pre1[1] != buf[1]) {
//			int r = MsgSend(coid, buf, 1370, 0, 0);
//			if (r == -1) {
//				printf("send fail\n");
//				exit(EXIT_FAILURE);
//			}
//	//		printf("%d\n", *tmp);
//	}
//	pre1[0]=buf[0];
//	pre1[1]=buf[1];


	if(feof(f)){
		fseek(f,0,SEEK_SET);
		printf("rewind success\n");
	}

//	*tmp+=37;
//	*tmp+=sign;
//	if(sign)sign=0;
//	else sign=1;
//	printf("%d\n\n",*tmp);
//	r = MsgSend(coid, buf, 1370, 0, 0);
//	if (r == -1) {
//		printf("send fail\n");
//		exit(EXIT_FAILURE);
//	}


//	printf("send success\n");

}

int my_chid = 1;
int my_node_id = -1;
int my_pid = 454691;
int my_coid = 0;
int swap=0;
void send_to_my_self() {
	//	my_chid=ChannelCreate(0x0A);

	char* msg;
	if(swap){
		msg="aaaaa";
		swap=0;
	}else{
		msg="bbbbb";
		swap=1;
	}
	int msg_len = 5;
	int r = MsgSend(my_coid, msg, msg_len, 0, 0);
	if (r == -1) {
		printf("send fail\n");
//		exit(EXIT_FAILURE);
	}
	else printf("send ok \n");
}

void print_state_dev19() {
	char receive_str[100];
	int a = 0;
	printf("hello\n");
	FILE* fp = popen("pidin -n uk51. -p dev19 -F '%B' | grep RECEIVE", "r");
	while (1) {
		fscanf(fp, "%d", &a);
		printf("%s\n", a);
	}
	printf("hello1\n");
}

int main(int argc, char *argv[]) {

//	f= fopen(argv[1],"r");
//	printf("%s",argv[0]);
		f=fopen("//minh_data//log1","r");
		get_uk51_nodeid();
		if (nodeid <= 0) {
			printf("get node id error\n");
			return 1;
		}

		get_ppi51_pid();
		if (pid <= 0) {
			printf("get ppi51 pid error\n");
			return 1;
		}
		if (chid <= 0) {
			printf("get ppi51 chid error\n");
			return 1;
		}

		connect_ppi51();
		if (coid <= 0) {
			printf("connect to ppi51 error\n");
			return 1;
		}

		int az=0;
		int azd=0;
		int num_of_line=9;
		int cnt=0;
		while (1) {
			send_to_ppi51(az, azd, num_of_line);
//			printf("%d  %d   %d\n",az+azd,cnt,n);
			cnt++;
			if(cnt<300)n=254;
			else if(cnt==600)cnt=0;
			else n=255;

//			n=rand()%256;

			az = (az + azd) % 6000;
			usleep(SLEEP_TIME);
			 // 10s/radar circle
		}


	return EXIT_SUCCESS;
	//	printf("hello");
}
