

#include "wifi_manager.h"
#include "stdlib.h"


#include "stdio.h"
#include "hardware_legacy/wifi.h"
#include "log.h"

#include "pthread.h"
#include "wakelock.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int processWpaEventInd(wifiMgr *wmgr);

static pthread_t wifiThread;
static pthread_mutex_t wifiLoopMutex;
static pthread_cond_t wifiThreadCond;
static pthread_mutex_t wifiProcessMutex;  // For lock when the WiFi Loop is on-going.

static pthread_t wifiRxThread;
static pthread_mutex_t wifiRxMutex;
static pthread_cond_t wifiRxThreadCond;


//extern int wifi_wait_for_event(char *buf, size_t len);

#if 0
int os_snprintf(char *str, size_t size, const char *format, ...)
{
	va_list ap;
	int ret;

	/* See http://www.ijs.si/software/snprintf/ for portable
	 * implementation of snprintf.
	 */

	va_start(ap, format);
	ret = vsnprintf(str, size, format, ap);
	va_end(ap);
	if (size > 0)
		str[size - 1] = '\0';
	return ret;
}

#endif

static void  wifiMgrLoop(void *wmgr) 
{
       int eventGet =0;
       wifiMgr *wiMgr = (wifiMgr *)wmgr;
       char cmdBuffer[WIFI_CMD_BUF_LEN];  	
       int addNetworkID = -1;	
	
       int ssidEntryReqToConnect;
       int WpaNetworkidNeedToRemove = 0xff;
       int numOfVars = 0;

       wifiMgrLog("enter wifiMgrLoop\n");
       pthread_mutex_lock(&wifiProcessMutex);
       pthread_mutex_lock(&wifiLoopMutex);
       wiMgr->wifiThreadLoopReady = true;   //tells app object that wifi loop thread is ready.
       while(1) {
	   pthread_mutex_unlock(&wifiProcessMutex);
	   pthread_cond_wait(&wifiThreadCond, &wifiLoopMutex);
	   
	   pm_wake_lock(WIFI_MGR_FLAG, 1);

	   wifiMgrLog("wifiMgrLoop process events, eventNo = %d\n", wiMgr->eventNo);

	   eventGet = wiMgr->eventNo;
	   //Process the event here
	   switch (eventGet) {

		case WIFI_NULL:
			break;

		case WIFI_ENABLE:

			break;

		case WIFI_STATUS:

			//send a scan command to wpa supplicant, command must be Capital letters
    			wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
    			if ( wifi_command(WIFI_IFACE_NAME, "STATUS", wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			{
         			if (wiMgr->replyMsgLen > 0) {
				    wifiMgrLog("msglen = %d, wifiReplyBuffer = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				    wiMgr->parseReplyBuffer(eventGet, wiMgr->wifiReplyBuffer);
				    SendConMgrWiFiMsg(WIFI_STATUS_CMD_OK_IND, wiMgr);
				}
    			} 
			else {
			     //status cmd failed
			     SendConMgrWiFiMsg(WIFI_STATUS_CMD_FAIL_IND, wiMgr);
			}
			break;

		case WIFI_SEARCH:
			 wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
    			if ( wifi_command(WIFI_IFACE_NAME, "SCAN", wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			{
         			if (wiMgr->replyMsgLen > 0) {
				    wifiMgrLog("msglen = %d, wifiReplyBuffer = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				   SendConMgrWiFiMsg(WIFI_SCAN_CMD_OK_IND, wiMgr);
				}
				
    			} 
			else {
				SendConMgrWiFiMsg(WIFI_SCAN_CMD_FAIL_IND, wiMgr);

			}
			break;

		case WIFI_SCAN_RESULT:
			wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
    			if ( wifi_command(WIFI_IFACE_NAME, "SCAN_RESULTS", wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			{
         			if (wiMgr->replyMsgLen > 0) {
				    wifiMgrLog("msglen = %d, wifiReplyBuffer = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				    wiMgr->parseReplyBuffer(eventGet, wiMgr->wifiReplyBuffer);
				}
    			} 

		       break;


		case WIFI_CONNECT_SSID:
			wiMgr->SetWiFiState(WIFI_UNDER_CONNECT);	//update the wifi state
			wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			memset(cmdBuffer, 0, sizeof(cmdBuffer));
			memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
			ssidEntryReqToConnect = wiMgr->ssidEntryReqConnect;

			//0. check if we already establish a network id with wpa.
			if (wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum < 0) 
			{

			    //if wpaSupplicantNetworkNum <0 (=-1), means no network id.
			

			   //1. add_network, and get the return network id, save it to bssInfo if it is correct value.
			
			   snprintf(cmdBuffer, sizeof(cmdBuffer), "ADD_NETWORK");
			   wifiMgrLog("%s\n", cmdBuffer);
			   if (wifi_command(WIFI_IFACE_NAME, cmdBuffer, wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			   {
         			if (wiMgr->replyMsgLen > 0) {
				    wifiMgrLog("msglen = %d, wpaSupplicantID = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				    //save the wpaSupplicantID into the corresponsing bssInfo entry.
				   numOfVars = sscanf(wiMgr->wifiReplyBuffer, "%d", 
					&(wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum));				   
					
					if ( (wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum < 0) ||
					     (wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum > 20) ) {
						//something wrong with the  wpaSupplicantNetworkNum
						//wifiMgrLog("something wrong with the  wpaSupplicantNetworkNum\n");
						wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum = -1;
				        }
				}
			   }

			   //2. set_network (network id ) ssid <ssid name>
			   wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			   memset(cmdBuffer, 0, sizeof(cmdBuffer));
			   memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);			

			   snprintf(cmdBuffer, WIFI_CMD_BUF_LEN, "SET_NETWORK %d ssid \"%s\"",
					wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum,
					wiMgr->BssData[ssidEntryReqToConnect].ssidName);
			   wifiMgrLog("%s\n", cmdBuffer);

			   if (wifi_command(WIFI_IFACE_NAME, cmdBuffer, wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			   {

				if (wiMgr->replyMsgLen > 0) {
				    wifiMgrLog("msglen = %d, wpaSupplicantID = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);

				}
			   }
			   else { 
				//some error happen
				//wifiMgrLog("set_network error\n");
				wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum = -1;
  			   }

			} //end of if network id			

			//3. Check if the password is already in wpa, if yes, continue , if not return failure.
			wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
			memset(cmdBuffer, 0, sizeof(cmdBuffer));
			snprintf(cmdBuffer, WIFI_CMD_BUF_LEN, "GET_NETWORK %d psk",
					wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum);
			    wifiMgrLog("%s\n", cmdBuffer);
			    if (wifi_command(WIFI_IFACE_NAME, cmdBuffer, wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) < 0)
    			   {
				//No passkey, send fail message to UI to ask user to input password first.
				SendConMgrWiFiMsg(WIFI_CONNECT_FAIL_NEED_PSK_IND, wmgr);
				break;
			   }
			   else {
				if (wiMgr->replyMsgLen > 0) {
				    wifiMgrLog("msglen = %d, wpaSupplicantID = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);

				}
   			   }
			
			
			
			//passkey OK
			//4. Send the scan_ssid = 1
	/*	
			wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
			memset(cmdBuffer, 0, sizeof(cmdBuffer));
			snprintf(cmdBuffer, WIFI_CMD_BUF_LEN, "SET_NETWORK %d scan_ssid 1",
					wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum);
			if (wifi_command(WIFI_IFACE_NAME, cmdBuffer, wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			{
				//cmd success
				if (wiMgr->replyMsgLen > 0) {
				    wifiMgrLog("msglen = %d, wpaSupplicantID = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);

				}
				
			}
		*/	


			//6. start re-scan
			wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
			memset(cmdBuffer, 0, sizeof(cmdBuffer));
			snprintf(cmdBuffer, WIFI_CMD_BUF_LEN, "SELECT_NETWORK %d",
					wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum);
			wifiMgrLog("%s\n", cmdBuffer);
			if (wifi_command(WIFI_IFACE_NAME, cmdBuffer, wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			{
				//cmd success
				if (wiMgr->replyMsgLen > 0) {
				    wifiMgrLog("msglen = %d, wpaSupplicantID = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				    
				}
				
			}

			//5. enable network <id>
#if 1
			wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
			memset(cmdBuffer, 0, sizeof(cmdBuffer));
			snprintf(cmdBuffer, WIFI_CMD_BUF_LEN, "ENABLE_NETWORK %d",
					wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum);
			wifiMgrLog("%s\n", cmdBuffer);
			if (wifi_command(WIFI_IFACE_NAME, cmdBuffer, wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			{
				//cmd success
				if (wiMgr->replyMsgLen > 0) {
				    wifiMgrLog("msglen = %d, wpaSupplicantID = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);

				}
				
			}	

#endif

	/*			
			 wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
    			if ( wifi_command(WIFI_IFACE_NAME, "RECONNECT", wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			{
         			if (wiMgr->replyMsgLen > 0) {
				    wifiMgrLog("msglen = %d, wifiReplyBuffer = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				   //SendConMgrWiFiMsg(WIFI_SCAN_CMD_OK_IND, wiMgr);
				}
				
    			}
   */ 
			
			SendConMgrWiFiMsg(WIFI_CONNECT_ACTION_ACK_IND, wiMgr);
		        break;

			case WIFI_SET_SSID_PSK:
				wiMgr->SetWiFiState(WIFI_UNDER_CONNECT);	//update the wifi state
				wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
				memset(cmdBuffer, 0, sizeof(cmdBuffer));
				memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
				ssidEntryReqToConnect = wiMgr->ssidEntryReqConnect;

				//0. check if we already establish a network id with wpa.
				if (wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum < 0) 
				{
					//return with SET_PSK_FAIL_IND
					//send PSK set failure
				        SendConMgrWiFiMsg(WIFI_SET_PSK_FAIL_IND, wiMgr);
					break;
				}

#if 0 //move to set mgmt
				snprintf(cmdBuffer, WIFI_CMD_BUF_LEN, "SET_NETWORK %d key_mgmt WPA-PSK",
					wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum);
				if (wifi_command(WIFI_IFACE_NAME, cmdBuffer, wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    				{
					//cmd success
					if (wiMgr->replyMsgLen > 0) {
				    	wifiMgrLog("msglen = %d, wpaSupplicantID = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				    
					}
				
				} else {
				   //send PSK set failure
				   SendConMgrWiFiMsg(WIFI_SET_PSK_FAIL_IND, wiMgr);
				   break;
                                }

				//set proto
				snprintf(cmdBuffer, WIFI_CMD_BUF_LEN, "SET_NETWORK %d proto WPA2",
					wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum);
				if (wifi_command(WIFI_IFACE_NAME, cmdBuffer, wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    				{
					//cmd success
					if (wiMgr->replyMsgLen > 0) {
				    	wifiMgrLog("msglen = %d, reply = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				    
					}
				
				} else {
				   //send PSK set failure
				   SendConMgrWiFiMsg(WIFI_SET_PSK_FAIL_IND, wiMgr);
				   break;
                                }

#endif

				wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
				memset(cmdBuffer, 0, sizeof(cmdBuffer));
				memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);

				snprintf(cmdBuffer, WIFI_CMD_BUF_LEN, "SET_NETWORK %d psk \"%s\"",
					wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum, 
					wiMgr->BssData[ssidEntryReqToConnect].passkey);

				wifiMgrLog("============ SET_NETWORK psk = %s ====\n", cmdBuffer);
				if (wifi_command(WIFI_IFACE_NAME, cmdBuffer, wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    				{
					//cmd success
					if (wiMgr->replyMsgLen > 0) {
				    	wifiMgrLog("msglen = %d, wpaSupplicantID = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				    
					}
				
				} else {
				   //send PSK set failure
				   SendConMgrWiFiMsg(WIFI_SET_PSK_FAIL_IND, wiMgr);
				   break;
                                }
				
				SendConMgrWiFiMsg(WIFI_SET_PSK_ACK_IND, wiMgr);
			break;

			case WIFI_LIST_NETWORK:
				
			     wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			     memset(cmdBuffer, 0, sizeof(cmdBuffer));
			     memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
			     if (wifi_command(WIFI_IFACE_NAME, "LIST_NETWORKS", wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			     {
				//cmd success
					if (wiMgr->replyMsgLen > 0) {
				    		wifiMgrLog("msglen = %d, wpaSupplicantID = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
						wiMgr->parseReplyBuffer(eventGet, wiMgr->wifiReplyBuffer);

					}
				
			     }

			break;


		case WIFI_REMOVE_NETWORK_REQ:			     
			     wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;			
			     memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
			     WpaNetworkidNeedToRemove = wiMgr->BssData[(wiMgr->ssidEntryReqRemove)].wpaSupplicantNetworkNum;
			     snprintf(cmdBuffer, WIFI_CMD_BUF_LEN, "REMOVE_NETWORK %d", WpaNetworkidNeedToRemove);

			    // wifiMgrLog("============== WIFI_REMOVE_NETWORK_REQ === cmdbuffer = %s =========\n", cmdBuffer);

			     if (wifi_command(WIFI_IFACE_NAME, cmdBuffer, wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			     {
				//cmd success
				if (wiMgr->replyMsgLen > 0) {
					wifiMgrLog("msglen = %d, reply = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);

					//reset the wpa network ID to be <0
					wiMgr->BssData[(wiMgr->ssidEntryReqRemove)].wpaSupplicantNetworkNum = -1;
				}
			      else {
				       //fail to remove network
					wifiMgrLog("error, fail to remove network\n");
				}
				
			     }

			break;

		case WIFI_SET_MGMT_PROTO:
				wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
				memset(cmdBuffer, 0, sizeof(cmdBuffer));
				memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);

				//0. check if we already establish a network id with wpa.
				if (wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum < 0) 
				{
					//return with SET_PSK_FAIL_IND
					//send PSK set failure
				        SendConMgrWiFiMsg(WIFI_SET_MGMT_FAIL_IND, wiMgr);
					break;
				}

				snprintf(cmdBuffer, WIFI_CMD_BUF_LEN, "SET_NETWORK %d key_mgmt %s",
					wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum, 
					wiMgr->BssData[ssidEntryReqToConnect].setMgmt);
				wifiMgrLog("%s\n", cmdBuffer);
				if (wifi_command(WIFI_IFACE_NAME, cmdBuffer, wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    				{
					//cmd success
					if (wiMgr->replyMsgLen > 0) {
				    	wifiMgrLog("msglen = %d, wpaSupplicantID = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				    	//SendConMgrWiFiMsg(WIFI_SET_MGMT_ACK_IND, wiMgr);
					}
				
				} else {
				   //send PSK set failure
				   SendConMgrWiFiMsg(WIFI_SET_MGMT_FAIL_IND, wiMgr);
				   break;
                                }

				//set proto
#if 0
				snprintf(cmdBuffer, WIFI_CMD_BUF_LEN, "SET_NETWORK %d proto %s",
					wiMgr->BssData[ssidEntryReqToConnect].wpaSupplicantNetworkNum,
					wiMgr->BssData[ssidEntryReqToConnect].setProto);
				if (wifi_command(WIFI_IFACE_NAME, cmdBuffer, wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    				{
					//cmd success
					if (wiMgr->replyMsgLen > 0) {
				    	   wifiMgrLog("msglen = %d, reply = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				           
					}
				
				} else {
				   //send PSK set failure
				   SendConMgrWiFiMsg(WIFI_SET_MGMT_FAIL_IND, wiMgr);
				   break;
                                }

				SendConMgrWiFiMsg(WIFI_SET_MGMT_ACK_IND, wiMgr);
#endif

			break;


		case WIFI_RECONNECT_REQ:
			 wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
    			if ( wifi_command(WIFI_IFACE_NAME, "RECONNECT", wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			{
         			if (wiMgr->replyMsgLen > 0) {
				    wifiMgrLog("msglen = %d, wifiReplyBuffer = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				   //SendConMgrWiFiMsg(WIFI_SCAN_CMD_OK_IND, wiMgr);
				}
				
    			} 
				
			break;

		
		case WIFI_DISCONNECT_REQ:
			wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
    			if ( wifi_command(WIFI_IFACE_NAME, "DISCONNECT", wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			{
         			if (wiMgr->replyMsgLen > 0) {
				    wifiMgrLog("msglen = %d, wifiReplyBuffer = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				   SendConMgrWiFiMsg(WIFI_DISCONNECT_ACTION_ACK_IND, wiMgr);
				}
				
    			} 

			break;


		case WIFI_SAVE_CONFIG_REQ:
			wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
    			if ( wifi_command(WIFI_IFACE_NAME, "SAVE_CONFIG", wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			{
         			if (wiMgr->replyMsgLen > 0) {
				    wifiMgrLog("msglen = %d, wifiReplyBuffer = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				   SendConMgrWiFiMsg(WIFI_SAVE_CONFIG_ACK_IND, wiMgr);
				}
				
    			} 

			break;


		//This case handles all event coming from WPA lower layer.
		case WIFI_WPA_EVENT_IND_REQ:				
				processWpaEventInd(wiMgr);				

			break;

		case WIFI_TERMINATE_WPA_REQ:
			wiMgr->replyMsgLen = SCAN_RESULT_REPLY_LEN;
			memset(wiMgr->wifiReplyBuffer, 0, BUFF_LEN);
    			if ( wifi_command(WIFI_IFACE_NAME, "TERMINATE", wiMgr->wifiReplyBuffer, (size_t *)&(wiMgr->replyMsgLen)) >= 0)
    			{
         			if (wiMgr->replyMsgLen > 0) {
				    wifiMgrLog("msglen = %d, wifiReplyBuffer = %s\n", wiMgr->replyMsgLen, wiMgr->wifiReplyBuffer);
				   SendConMgrWiFiMsg(WIFI_TERMINATE_ACK_IND, wiMgr);
				}
				
    			} 
			break;
			

		default:
			break;


	   }
	   
	  pm_wake_lock(WIFI_MGR_FLAG, 0);  //allow to system sleep

       }
      

       pthread_exit(NULL);
}



int processWpaEventInd(wifiMgr *wmgr) {

     char eventIndName[30];
     int numVars = 0; 

     numVars = sscanf(wmgr->wpaIndBuffer, "%s" , eventIndName);
     
     if (numVars <= 0) {
         return -1;
     }  

     if (eventIndName == NULL) {
        return 0;
     }
 
     wifiMgrLog("processWpaEventInd eventIndName=%s\n", eventIndName);   

     if (strcmp(eventIndName, "CTRL-EVENT-BSS-REMOVED") == 0) {


     }
     else if ( strcmp(eventIndName, "CTRL-EVENT-CONNECTED") == 0) {
	SendConMgrWiFiMsg(WIFI_EVENT_CONNECTED_IND, wmgr);
     }
     
     else if ( strcmp(eventIndName, "CTRL-EVENT-SCAN-RESULTS") == 0) {
	
	SendConMgrWiFiMsg(WIFI_SCAN_RESULT_READY_IND, wmgr);	
     }

     else if (  strcmp(eventIndName, "CTRL-EVENT-DISCONNECTED") == 0) {
	SendConMgrWiFiMsg(WIFI_EVENT_DISCONNECTED_IND, wmgr);
     }


     else if ( strcmp(eventIndName, "CTRL-EVENT-SSID-TEMP-DISABLED") == 0) {
	SendConMgrWiFiMsg(WIFI_EVENT_SSID_TEMP_DISABLED_IND, wmgr);
     }

//CTRL-EVENT-SSID-TEMP-DISABLED id=2 ssid="OliveOil" auth_failures=1 duration=10

//CTRL-EVENT-DISCONNECTED bssid=22:19:be:81:2b:48 reason=3 , this is when switch to another bss

     return 0;

}



static void  wifiRxLoop(void *wmgr) 
{
   wifiMgr *wiMgr = (wifiMgr *)wmgr;

   wifiMgrLog("enter wifiRxLoop\n");

   


   while(1) {

	usleep(10000);
 	//wiMgr->rxWiFiDeviceMsg();
	memset(wiMgr->wpaIndBuffer, 0, BUFF_LEN);
	 wiMgr->wpaIndEventLen = wifi_wait_for_event(WIFI_IFACE_NAME, (char *)wiMgr->wpaIndBuffer, BUFF_LEN);

	wifiMgrLog("WPA event Ind received, %s\n", wiMgr->wpaIndBuffer); 
	wiMgr->postEvents(WIFI_WPA_EVENT_IND_REQ);

   }
    

}

void InitWiFiMgrThread(wifiMgr* wmgr)
{
	
	pthread_mutex_init(&wifiLoopMutex, NULL);
	pthread_mutex_init(&wifiProcessMutex, NULL);
	pthread_cond_init(&wifiThreadCond, NULL);

	if(pthread_create(&wifiThread,NULL,(void*(*)(void*))(wifiMgrLoop),(void *)wmgr))
	{
		ERROR_X("InitWiFiMgrThread fail\n");
		pthread_mutex_destroy(&wifiLoopMutex);
		pthread_cond_destroy(&wifiThreadCond);
		exit(0);
	}

	wmgr->MgrProcessMutex = &wifiProcessMutex;
        wmgr->loopThreadCond = &wifiThreadCond;
	wmgr->loopMutex = &wifiLoopMutex;
	
	
}

/*
==============================

   This is for the Rx path thread
======================================
*/
void InitWiFiRxThread(wifiMgr* wmgr)
{
	
	pthread_mutex_init(&wifiRxMutex, NULL);
	pthread_cond_init(&wifiRxThreadCond, NULL);

	if(pthread_create(&wifiRxThread,NULL,(void*(*)(void*))(wifiRxLoop),(void *)wmgr))
	{
		ERROR_X("InitWiFiRxThread fail\n");
		pthread_mutex_destroy(&wifiRxMutex);
		pthread_cond_destroy(&wifiRxThreadCond);
		exit(0);
	}

       // wmgr->loopThreadCond = &wifiThreadCond;
	//wmgr->loopMutex = &wifiLoopMutex;
	
}



#ifdef __cplusplus
}   /* end of extern C */
#endif





