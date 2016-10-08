#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "pthread.h"
#include "msgmanage.h"
#include "log.h"


/* 
============================================

   File: wifi_manager.h

   Description: This file define the class for wifi connection

============================================
*/

//#define BUFF_LEN  1024
//#define SCAN_RESULT_REPLY_LEN 1023

#define BUFF_LEN  2048
#define SCAN_RESULT_REPLY_LEN 2047

#define BSSID_NAME_LEN  80
#define WIFI_CMD_BUF_LEN 80

#define WIFI_IFACE_NAME  "wlan0"
#define wifiEventStateChange "CTRL-EVENT-STATE-CHANGE"

//#define wifiMgrLog  printf
#define wifiMgrLog(fmt...)  nativeLog(fmt) 


#define BSS_FIELD_LEN 		60
#define MAX_NUM_OF_LIST_BSS  	30
#define NUM_BSS_PROPERTY  	15
#define PROPERTY_NAME_LEN	30
#define PSK_FIELD_LEN		20
#define IPDATALEN 		8
#define IPDATALEN_INCHAR	32
#define MACADDRLEN 		20

enum wifiStates {

WIFI_IDLE = 0,
WIFI_INITIATING,
WIFI_CONNECTED,
WIFI_UNDER_CONNECT,
WIFI_DISCONNECTED,
WIFI_SCANNING,
WIFI_OFF,
WIFI_AP_MODE,


WIFI_STATE_MAX

};

//THis is the event IDs used for Connection Manager to send
//message back to UI object via system msg parser.
//Reply IDs 
enum ConMgrMsgReplyIDs {
	WIFI_IND_NULL = 0,
	WIFI_SEARCH_REPLY_IND,
	WIFI_SCAN_RESULT_ACK_IND,
	WIFI_CONNECT_ACTION_ACK_IND,
	WIFI_DISCONNECT_ACTION_ACK_IND,
	WIFI_SCAN_RESULT_READY_IND,
	WIFI_EVENT_DISCONNECTED_IND,
        WIFI_CONNECT_FAIL_NEED_PSK_IND,
	WIFI_SET_PSK_ACK_IND,   
        WIFI_SET_PSK_FAIL_IND,
	WIFI_EVENT_CONNECTED_IND,
	WIFI_EVENT_SSID_TEMP_DISABLED_IND,
	WIFI_STATUS_CMD_FAIL_IND,
	WIFI_STATUS_CMD_OK_IND,
	WIFI_SCAN_CMD_OK_IND,
	WIFI_SCAN_CMD_FAIL_IND,
	WIFI_SET_MGMT_ACK_IND,
	WIFI_SET_MGMT_FAIL_IND,
	WIFI_SAVE_CONFIG_ACK_IND,
	WIFI_TERMINATE_ACK_IND,
	

	WIFI_IND_MAX,


	GMMP_REPLY_IND_START = 0x200,
	GMMP_OMP_REPLY_IND,

	GMMP_REPLY_IND_MAX

};


//This is the event ID defined inside WiFi manager
enum wifiEvents {
	WIFI_NULL = 0,
	WIFI_ENABLE,
	WIFI_DISABLE,
	WIFI_STATUS,
	WIFI_SEARCH,
	WIFI_SCAN_RESULT,
	WIFI_CONNECT_SSID,
	WIFI_CONNECT_WITH_PSK,
	WIFI_LIST_NETWORK,
        WIFI_WPA_EVENT_IND_REQ,
	WIFI_SET_SSID_PSK,
	WIFI_SET_MGMT_PROTO,

	WIFI_FORGET_BSSID,
	WIFI_REMOVE_NETWORK_REQ,
	WIFI_RECONNECT_REQ,
	WIFI_DISCONNECT_REQ,
	WIFI_SAVE_CONFIG_REQ,
	WIFI_TERMINATE_WPA_REQ,

	WIFI_EVENT_MAX

};

typedef struct BSSID_FIELD_T {
    char name[PROPERTY_NAME_LEN];
    char data[BSS_FIELD_LEN];
} bssidFieldType;

struct wifiStatusType {

   int connectState;
   int bssid;




};

struct wifiCallBackMsgType {

   int WiFiReplyInd;
   unsigned char *Mgs;
};


typedef struct  BSSID_INFO_T {
  int networkId;  
  bssidFieldType bssInfoData[NUM_BSS_PROPERTY];
  int frequency;
  int signalLevel;
  char flags[BSS_FIELD_LEN];
  char ssidName[BSSID_NAME_LEN];
  char bssid[BSSID_NAME_LEN];
  char passkey[PSK_FIELD_LEN];
  char setMgmt[PSK_FIELD_LEN];
  char setProto[PSK_FIELD_LEN];
  int wpaSupplicantNetworkNum;
   
}  bssidInfoType;



class wifiMgr 
{

    private:


	int state;
	int connectionStatus;
	int targetToConnectBss;	//define the bss that we are now going to connect
	

    public:
	pthread_cond_t *loopThreadCond;
	pthread_mutex_t *loopMutex;
	pthread_mutex_t *MgrProcessMutex;
	int wifiThreadLoopReady;
	bssidInfoType BssData[MAX_NUM_OF_LIST_BSS];

	char wifiReplyBuffer[BUFF_LEN];
	char wpaIndBuffer[BUFF_LEN];

	char wpa_state[PROPERTY_NAME_LEN];  //contains the wpa_state after STATUS COMMAND.
	bssidFieldType CurrrentConnectedBssInfoData[NUM_BSS_PROPERTY];

	char CurrentBssid[BSSID_NAME_LEN];
	char CurrentIpaddr[IPDATALEN_INCHAR];
	int CurrentGateway[IPDATALEN];
    	int CurrentMask[IPDATALEN];
	int CurrentDns1[IPDATALEN];
	int CurrentDns2[IPDATALEN];
	int CurrentServerIP[IPDATALEN];
	int CurrentLease;
	char MACAddr[MACADDRLEN];

	int wpaIndEventLen;  //the buffer size of the received WPA event indication,
	int replyMsgLen;
	int eventNo;

	int ssidEntryReqConnect;
	int wpaLastNumOfBssFind;   //This is the number of ssid searched by wpa from last scan result.
	int ssidEntryReqRemove;   //This is the ssid entry that UI requested to remove from wpa network list.

	int hostapd_pid;  //This is used for softAP mode, get the hostapd Pid number.
	int numOfStatusCheckDuringConnect;  // This is the number of status check under connecting state.

	void *wifiGmmpMgr;   //This holds the GMMP manager object pointer.

	wifiMgr();

	~wifiMgr(void);

   	int wifiEnable(void);
	int wifiDisable(void);
        int postEvents(int event);

	int handle_callback(void);
	int parseReplyBuffer(int event, char *ReplyBuff);
	int statusReq(void);
	int searchReq(void);
	int scanResultReq(void);
	int extractStrings(char *match_string, char *input, char *outBuf);
	int extractScanResult(char *input, char *outBuf);
	int fillBssInfoData(bssidInfoType *bssInfoBuff, char *bssid, char *ssid, char *flags, 
								    int freq, int signalLevel);

	int connectReq(int ssidEntryReqToConnect);
	int extractListNetworkResult(char *input);

	int ListNetworkReq(void);

	int SetWiFiState(int Setstate);
        int getWiFiState(void);

	int SetPskReq(int ssidEntryReq, char *passkey);

	int ForgetBssReq(int ssidEntryForgetReq);
	//int wifiScanStart(void);
        
	//int wifiStartSupplicant(int p2pSupported);
	//int wifiConnectSupplicant(const char *ifname);

  	int wifiConnectWpa(void);
	int SetMgmtReq(int ssidEntryReq, char *mgmt, char *proto);
	int doDHCPReq(void);
	int disConnectReq(int ssidEntryReqToConnect);
	int saveConfigReq(void);
	void ClearData(void);
	int switchToSoftAP(void);
	int resumeWiFiMode(int hostapdPid);


};












#endif

