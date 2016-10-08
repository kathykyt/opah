
#include "wifi_manager.h"


#include "stdlib.h"
#include "stdio.h"
#include "hardware_legacy/wifi.h"
#include "log.h"
#include "wifi_process.h"
#include "memorymanage.h"

#define MAX_NUM_WPA_CONNECT_RETRY 10


#if __cplusplus
extern "C" {
#endif

/**
 * Load the Wi-Fi driver.
 *
 * @return 0 on success, < 0 on failure.
 */
extern int wifi_load_driver();
extern int wifi_start_supplicant(int p2pSupported);
extern int wifi_connect_to_supplicant(const char *ifname);
extern int wifi_wait_for_event(const char *iface, char *buf, size_t len);
extern int wifi_command(const char *iface, const char *command, char *reply, size_t *reply_len);
extern void InitWiFiRxThread(wifiMgr* wmgr);


extern int getMacAddr(char *buff);

void test(void) {}

#if __cplusplus
};
#endif

int dhcpcd_flag = 0;

char bssPropertyName[][PROPERTY_NAME_LEN] = {
	{"bssid="},
	{"\nssid="},
	{"\nid="},
	{"\nmode="},
	{"\npairwise_cipher="},
	{"\ngroup_cipher="},
	{"\nkey_mgmt="},
	{"\nwpa_state="},
	{"\nip_address="},
	{"\np2p_device_address="},
	{"\naddress="}
  };



wifiMgr::wifiMgr(void) 
{
    int i;

     replyMsgLen = 0;
     state = 0;
     wifiThreadLoopReady = 0;
     

     wpaLastNumOfBssFind = 0;

     //If thread is not init, init a thread to handle all events.
     InitWiFiMgrThread(this);
     

     if ((loopThreadCond == NULL) || (loopMutex == NULL)) {
	wifiMgrLog("error: wifi thread error\n");
	
     }

     //wait for wifi Mgr loop start
     if (wifiThreadLoopReady == false) {
		usleep(100000);
	}

      //Init the BSS data structures
      memset(BssData, 0, sizeof(BssData));	

     //init the wpa netork id for all entry to < 0
      for (i=0; i< MAX_NUM_OF_LIST_BSS; i++) {
	 BssData[i].wpaSupplicantNetworkNum = -1;
     }

    InitWiFiRxThread(this);

    state = WIFI_INITIATING;
    //state = WIFI_OFF;

    //get the wifi mac address
    
    numOfStatusCheckDuringConnect = 0;

    wifiGmmpMgr = NULL;

    memset(MACAddr, 0, MACADDRLEN);

    getMacAddr(MACAddr);
    wifiMgrLog("get mac address =%s\n", MACAddr);

}


void wifiMgr::ClearData(void) {

 int i;

      //Init the BSS data structures
      memset(BssData, 0, sizeof(BssData));	

     //init the wpa netork id for all entry to < 0
      for (i=0; i< MAX_NUM_OF_LIST_BSS; i++) {
	 BssData[i].wpaSupplicantNetworkNum = -1;
     }

     numOfStatusCheckDuringConnect = 0;

}


wifiMgr::~wifiMgr(void) 
{

   //Destroy thread.


}


int wifiMgr::wifiConnectWpa(void) 
{
   int i;

        i = 0;

    //connect to supplicant
   while ( ( wifi_connect_to_supplicant(WIFI_IFACE_NAME) < 0) && (i < MAX_NUM_WPA_CONNECT_RETRY)) {
	i++;
	wifi_stop_supplicant(0);
	usleep(1000000);
	wifi_start_supplicant(0);	
	wifiMgrLog("retry supplicant connect\n");
	usleep(500000);
    }

   if (i >= MAX_NUM_WPA_CONNECT_RETRY) {
        wifiMgrLog("error: wifi connect wpa supplicant error\n");
	return -1;
   }

}

/*================================================

    Assume we don't need to load the driver
==================================================
*/
int wifiMgr::wifiEnable(void) 
{
    replyMsgLen = 0;
    int i, j;

     //load the wifi driver
     

     wifi_stop_supplicant(0);

     //start wpa supplicant, with psp supported.
     if (wifi_start_supplicant(0) < 0) {
         wifiMgrLog("error: wifi start wpa supplicant error, retry one more time\n");
	  return -1;	
    }



    return 0;
}

int wifiMgr::wifiDisable(void) 
{

    //call wpa_cli terminate can close wpa_supplicant.
    postEvents(WIFI_TERMINATE_WPA_REQ);
    //wifi_stop_supplicant(0);

    return 0;	

}

int wifiMgr::handle_callback(void) {



}

int wifiMgr::parseReplyBuffer(int event, char *ReplyBuff) {

	int bssID, propertyID = 0;  	      
	char networkID[4];
	int num_of_property = (sizeof(bssPropertyName) / PROPERTY_NAME_LEN);
	int i;	
	int bssEntry = 0;
	int status = 0;

	switch (event) {
		
		case WIFI_SEARCH:
			
			break;


		case WIFI_STATUS:
			bssID = 0; 
			propertyID = 3;
			extractStrings("wpa_state=", ReplyBuff, (char *)wpa_state );			
			//refer to the network ID, this should match to the BssData[id] 
			//Check if we have the same bssid name first, 
			//Search for the propertyNames from the replybuffer, 
			//extract the data and copy to the bssidInfoType fields
			//strcat(tempReference, "id=");
			status = extractStrings("\nid=", ReplyBuff, (char *)networkID );
			//wifiMgrLog("id = %s, strlen=%d\n", (char *)networkID, strlen(networkID));

			//extract the ipaddress
			extractStrings("ip_address=", ReplyBuff, (char *)CurrentIpaddr );
			extractStrings("bssid=", ReplyBuff, (char *)CurrentBssid);
			
			 
			if (status <= 0) {
			   //no id found
			   wifiMgrLog("no id string found\n");
			   break;
			}

			bssID = atoi(networkID);

			//The bssID here in fact is the network ID in wpa, it must be >0
			wifiMgrLog("bssid no = %d\n", bssID);
			
			if (bssID < 0) {
			 //no valid id found			  
			 break;
			}
			
			bssEntry = -1;
			//search for the WpaNetworkID to see if we found the match ID
			for (i =0; i < MAX_NUM_OF_LIST_BSS; i++) {
				if (BssData[i].wpaSupplicantNetworkNum == bssID) {
				   //we found the match ID
				   bssEntry = i;
				   wifiMgrLog("match network ID in status check, entry = %d\n", bssEntry);				   
				}
			}

		
			if (bssEntry < 0) {
				// no match ID found
			       break;
			}

			for (i = 0; i< num_of_property; i++) {
			   extractStrings((char *)(&bssPropertyName[i][0]), 
				ReplyBuff, (char *)(&BssData[bssEntry].bssInfoData[i].data[0]) );
			   
			    extractStrings((char *)(&bssPropertyName[i][0]), 
				ReplyBuff, (char *)(CurrrentConnectedBssInfoData[i].data) );

			    strcpy((char *)(&BssData[bssEntry].bssInfoData[i].name[0]), bssPropertyName[i]); 
			   //update the current bssidproperty.
			   strcpy((char *)(CurrrentConnectedBssInfoData[i].name), bssPropertyName[i]); 
			   wifiMgrLog("%s%s", (char *)(&BssData[bssEntry].bssInfoData[i].name[0]), (char *)(&BssData[bssEntry].bssInfoData[i].data[0]));
			}

		 	break;

		case WIFI_SCAN_RESULT:
			//Scan result will have all the bssid info that can be received.
			wpaLastNumOfBssFind = extractScanResult(ReplyBuff, NULL);
			

			//we should sent a message to the UI message Queue and raise UI callback func.
			SendConMgrWiFiMsg(WIFI_SCAN_RESULT_ACK_IND, this);
			//wifiMgrLog("send conMgrWiFi msg to UI, ctrlroot\n");
			break;

		case WIFI_LIST_NETWORK:
			//wifiMgrLog("parseReplyBuffer WIFI_LIST_NETWORK\n");
			extractListNetworkResult(ReplyBuff);
			break;
			

		default:
			break;


	}


}

/* ==============================================================================
   This function extract the string after the match string and until the next 
   carriage return
==============================================================================
*/
int wifiMgr::extractStrings(char *match_string, char *input, char *outBuf) {

    char *start, *end;
    int j, stringFind = 0;
    volatile int i;

    char *tmp_buff;
   
   //wifiMgrLog("length of input = %d\n", strlen(input));

   if (strlen(match_string) > strlen(input)) {
     return -2;
   }

    tmp_buff = (char *)uimalloc(strlen(match_string) + 1);

    if (tmp_buff == NULL) {
	wifiMgrLog("error: extract strings malloc error\n");
	return -2;
    }

    i = 0;
    stringFind = 0;
    while (i <= (strlen(input) - strlen(match_string)) ) {

        for (j=0; j< strlen(match_string); j++) {
		tmp_buff[j] = *(input + i + j);
 	}
	tmp_buff[strlen(match_string)] = 0;    

	//wifiMgrLog("check %s, %s\n", match_string, tmp_buff);
	if (strcmp(match_string, tmp_buff)  == 0) {
	    stringFind = 1;
	    break; //we find the match string position at i
        }

	i++;

    } // end of while

    if (stringFind == 1 ) {

	//wifiMgrLog("extract string find!\n");
	i += strlen(match_string);
	j = 0;
	// if we find the match string, copy the rest of string until "newline"
	while ( (*(input + i + j) != 0x0A ) && (*(input + i + j) != 0) ) {/* newline char */

		*(outBuf + j) = *(input + i + j);
		j++;
        }

	if ( (*(input + i) == 0x0A ) || (*(input + i) == 0) ) {
	  //we have null string match
	  wifiMgrLog("extractStrings, null string found!\n");
	  return -2;
	}

	*(outBuf + j) = 0;
	uifree(tmp_buff);	
	return i;
    }
    else {
	//no match
	uifree(tmp_buff);	
	return -1;
   }

}


/* 
=======================================================================================

    Extract the result from scan result, the data will be saved to bssInfoData[] inside 
    class object.

    input: reply buffer pointer from scan result.

    return: it return the number of bssid information get and filled.
=======================================================================================
*/
int wifiMgr::extractScanResult(char *input, char *outBuf) {

   int i, numVars, numBssScanned, ret;
   char *tmpBuff;
   char bssid[300];
   char ssid[300];
   char tmpssid[300];
   char tmpssid2[300];	
   int frequency;
   int signalLevel;
   char flag[50];
   int loopFinish = 0;

   i=0;

   wifiMgrLog("extractScanResult\n");

   numBssScanned = 0;
   while (loopFinish == 0) {
      while (*(input + i) != '\n') {
	   if (*(input + i) == 0) {
	      loopFinish = 1;
	      break;
           }
	   i++; //find the first line feed.
      }
  
      i++;
      if (loopFinish == 0) {
           tmpBuff = input + i;

	   //Parse the data out by newline, and tab as well
           numVars = sscanf(tmpBuff, "%s	%d	%d	%s	%s" , bssid, &frequency, &signalLevel, flag, ssid); 

	     //we need to cross check if the ssid contains space.
	     strcpy(tmpssid, ssid);
	     tmpssid[strlen(ssid)] = 0;
	     //search for the tmpssid as start
	      if ( extractStrings(tmpssid, tmpBuff, tmpssid2) > 0) {
		//we have ssid contains space
	        strcpy(&ssid[strlen(tmpssid)], tmpssid2);
	         wifiMgrLog("new ssid = %s\n", ssid);
	      }

	    if (numVars == 5) {
	          //5 data input get successfully. Otherwise, don't trust the data.
		  numBssScanned++;
           	  //wifiMgrLog("freq = %d, ssid = %s, bssid = %s\n", frequency, ssid, bssid); 
		  ret = fillBssInfoData(BssData, bssid, ssid, flag, frequency, signalLevel);
		  //wifiMgrLog("fillBssInfoData, ret = %d, ssid = %s\n", ret, BssData[ret].ssidName);
	    }
	    else {
			wifiMgrLog("something wrong...\n");
		        wifiMgrLog("bssid = %s, freq=%d, signalLevel = %d, flag =%s, ssid = %s\n",
				bssid, frequency, signalLevel, flag, ssid);
						
		}
	}
  }  //end of while  loop_finish

  //Look if we already have any entry (same bssid) in the BssInfo table.
  return numBssScanned;

}

// Parse the result from list_network

int wifiMgr::extractListNetworkResult(char *input) {

   int i, numVars, j;
   int loopFinish = 0;
   char *tmpBuff;
   int networkID;
   char ssidName[30];
   char bssid[30];
   char flags[30];

   int IDfind = 0;


   i=0;
   IDfind = 0;
   while (loopFinish == 0) {
      while (*(input + i) != '\n') {
	   if (*(input + i) == 0) {
	      loopFinish = 1;
	      break;
           }
	   i++; //find the first line feed.
      }
  
      i++;
      if (loopFinish == 0) {
           tmpBuff = input + i;

	   //Parse the data out by newline, and tab as well
           numVars = sscanf(tmpBuff, "%d	%s	%s	%s" , &networkID, ssidName, bssid, flags); 
	 //  wifiMgrLog("numvars = %d\n", numVars);
	   if (numVars == 4) {
		//search the ssidName from bssInfo.
		for (j = 0; j< MAX_NUM_OF_LIST_BSS; j++) {
		    if (strcmp(ssidName, BssData[j].ssidName) == 0) {
			 //wifiMgrLog("find networkID match ssid name\n");
		         BssData[j].wpaSupplicantNetworkNum = networkID;
			 IDfind++;
			 break;
		    }

		} //end of for loop 
	   }
      }
   } //end of loopFinsh

   return IDfind;
}


int wifiMgr::fillBssInfoData(bssidInfoType *bssInfoBuff, char *bssid, char *ssid, char *flags, 
									int freq, int signalLevel)
{
   int i, numEntry;
   bssidInfoType *bssInfoBuffp = bssInfoBuff;
   int propertyNum = 0;
   int findMatchBss;

   //check if there is an existing record in the table.
   numEntry = MAX_NUM_OF_LIST_BSS;
   findMatchBss = 0;

   for (i = 0; i <  numEntry; i++) {
       if ( strcmp( ((bssInfoBuffp + i)->bssid), bssid) == 0) {
	   findMatchBss = 1;
	   break;
       }
   }

   if (findMatchBss == 1) {
	//i--;
	//we find matched entry	
 	strcpy( ((bssInfoBuffp + i)->flags), flags ); 
	strcpy( ((bssInfoBuffp + i)->bssInfoData[propertyNum].data), bssid);
	strcpy( ((bssInfoBuffp + i)->ssidName), ssid ); 
	(bssInfoBuffp + i)->frequency = freq;
 	(bssInfoBuffp + i)->signalLevel = signalLevel;
	return i;   //The position of the entry.
    }
    else {

	//find a new empty entry
	for (i= 0; i< numEntry; i++) {

	   if ((bssInfoBuffp + i)->frequency == 0 )  {
		strcpy( ((bssInfoBuffp + i)->bssid), bssid );
	        strcpy( ((bssInfoBuffp + i)->ssidName), ssid );
		strcpy( ((bssInfoBuffp + i)->flags), flags ); 
		strcpy( ((bssInfoBuffp + i)->bssInfoData[propertyNum].data), bssid);
		(bssInfoBuffp + i)->frequency = freq;
 		(bssInfoBuffp + i)->signalLevel = signalLevel;
		return i;
	   } 	

        } //end of for	  
     
    } //end of else

    return -2; //No any entry found and write with the info.

}


int wifiMgr:: ListNetworkReq(void) {
  
     postEvents(WIFI_LIST_NETWORK);

}


int wifiMgr:: scanResultReq(void) {
  
     postEvents(WIFI_SCAN_RESULT);

}


int wifiMgr:: statusReq(void) {

	postEvents(WIFI_STATUS);

}


int wifiMgr:: searchReq(void) {

	postEvents(WIFI_SEARCH);
	//usleep(500000);
	//scanResultReq();

}


int wifiMgr::disConnectReq(int ssidEntryReqToConnect) {


    ssidEntryReqConnect = ssidEntryReqToConnect;
    postEvents(WIFI_DISCONNECT_REQ);

}

int wifiMgr::connectReq(int ssidEntryReqToConnect) {


    ssidEntryReqConnect = ssidEntryReqToConnect;
    postEvents(WIFI_CONNECT_SSID);

}

int wifiMgr::saveConfigReq(void) {


    postEvents(WIFI_SAVE_CONFIG_REQ);

}


//Please note that, before setting the passkey, user should
//have called the scan and scan result to update the bssInfo.

int wifiMgr::SetPskReq(int ssidEntryReq, char *passkey) {

    if (ssidEntryReq >= 0) {
	 if (BssData[ssidEntryReq].wpaSupplicantNetworkNum < 0) {
	     return -1; // This bss has not been added to the wpa netowrk yet. 
         }
	ssidEntryReqConnect = ssidEntryReq;
	strcpy(BssData[ssidEntryReqConnect].passkey, passkey);
	BssData[ssidEntryReqConnect].passkey[strlen(passkey)] = 0;
	wifiMgrLog("-----SetPskReq = %s\n", BssData[ssidEntryReqConnect].passkey);
    }

     postEvents(WIFI_SET_SSID_PSK);
     return 0;

}

//Please note that, before setting the mgmt and proto, user should
//have called the scan and scan result to update the bssInfo.
// This function will set the mgmt and proto together with a valid bssInfo.

int wifiMgr::SetMgmtReq(int ssidEntryReq, char *mgmt, char *proto) {

    if (ssidEntryReq >= 0) {
	 if (BssData[ssidEntryReq].wpaSupplicantNetworkNum < 0) {
	     return -1; // This bss has not been added to the wpa netowrk yet. 
         }
	ssidEntryReqConnect = ssidEntryReq;
	strcpy(BssData[ssidEntryReqConnect].setMgmt, mgmt);
	BssData[ssidEntryReqConnect].setMgmt[strlen(mgmt)] = 0;
	strcpy(BssData[ssidEntryReqConnect].setProto, proto);
	BssData[ssidEntryReqConnect].setProto[strlen(proto)] = 0;

	wifiMgrLog("-----SetMgmt = %s, proto = %s\n", BssData[ssidEntryReqConnect].setMgmt, BssData[ssidEntryReqConnect].setProto);


    }

     postEvents(WIFI_SET_MGMT_PROTO);
     return 0;

}

// This method delete the entry of network info inside wpa_supplicant. 
int wifiMgr::ForgetBssReq(int ssidEntryForgetReq) {

    if (ssidEntryForgetReq >= 0) {
	 if (BssData[ssidEntryForgetReq].wpaSupplicantNetworkNum < 0) {
		wifiMgrLog("wrong wpa network ID\n");
	     return -1; // This bss has not been added to the wpa netowrk yet. 
         }
	
	ssidEntryReqRemove = ssidEntryForgetReq;

    }

     postEvents(WIFI_REMOVE_NETWORK_REQ);
     return 0;

   

}


int wifiMgr::doDHCPReq(void) {

    if (dhcpcd_flag == 0) {
 
        system("/data/misc/wifi/start_dhcp.sh");
        dhcpcd_flag = 1;
    }
 
#if 0
    char *argv[2] = {"wlan0", NULL};
    int pid = fork();

	if ( pid == 0 ) {
		execvp( "/system/bin/dhcpcd", argv );
	}

	/* Put the parent to sleep for 2 seconds--let the child finished executing */
	usleep( 2000000 );


	return 0;

     return (do_dhcp_request((int *)CurrentIpaddr, (int *)CurrentGateway, CurrentMask,
                   CurrentDns1, CurrentDns2, CurrentServerIP, &CurrentLease));
#endif

}

int wifiMgr::SetWiFiState(int Setstate) {

      state = Setstate;

}

int wifiMgr::getWiFiState(void) {

      return state;

}

int wifiMgr::switchToSoftAP(void) {

    FILE *fp, *pid_tmp;
    char path[100];
    char tempdata[40];
    char tempdata1[40];
    char tempdata2[40];
    int hostapd_pid = 0;

    /* Open the command for reading. */
    fp = popen("/system/bin/start_hostapd.sh", "r");
    if (fp == NULL) {
      wifiMgrLog("Failed to run command /system/bin/start_hostapd.sh\n" );
      exit(1);
    }

    /* Read the output a line at a time - output it. */
    while (fgets(path, sizeof(path), fp) != NULL) {
      wifiMgrLog("%s", path);
    }

    /* close */
    pclose(fp);

   //It happens that under android native, fgets is not working,
   // So we need to output the ps data to /data/misc/wifi/temp.txt,
   // The format is "hostapd star wifi <pid> ....." so pid is the fourth data.
   // make sure that temp.txt is with mode 777.

   sleep(1);

   pid_tmp = fopen("/data/misc/wifi/temp.txt" , "r");

    if (pid_tmp == NULL) {
      wifiMgrLog("Failed to run command /data/misc/wifi/temp.txt\n" );
      exit(1);
    }
   
  //  if( fgets (path, 10, pid_tmp)!=NULL ) 
    if (fscanf(pid_tmp, "%s %s %s %d", tempdata, tempdata1, tempdata2, &hostapd_pid) > 0) 
   {
      /* writing content to stdout */
      wifiMgrLog("reading temp.txt and got pid = %d \n", hostapd_pid);
   }
   fclose(pid_tmp);


  return hostapd_pid;


}

int wifiMgr::resumeWiFiMode(int hostapdPid) {

    char buff[100];

    sprintf(buff, "/system/bin/resume_wifi_from_ap.sh %d", hostapdPid);

    wifiMgrLog("%s\n", buff);

    system(buff);


}
/*
int wifiMgr::getMacAddress(char *buff) {

  

}
*/

/* ===================================================================

   wifiMgr::postEvents(int event) 
   
   description: This function post an event to Wifi Manager then return.
                Since the Manager process takes time and we don't have
 		event queue to queue up events, any new events will be 
		wait until the  MgrProcessMutex is released. Therefore,
		simultaneous events may be blocked for some time. 
		The app object can register the call back function to WiFi manager object.
		The call back function will be called at the end of wifiMgrLoop if the
		callback flag is set and call back func is not NULL

   limitation: We should not post the event inside the wifiMgrLoop Process thread, eg., 
		 parseReplyBuffer(), otherwise, it will enter a deadlock.   
===================================================================
*/
int wifiMgr::postEvents(int event) 
{
   //loopMutex, loopThreadCond are pointers, not the structure itself.
   //This is needed to make sure loopMgr finish task before next event come.
   // wifiMgrLog("enter process lock\n");
    pthread_mutex_lock(MgrProcessMutex); 
    pthread_mutex_lock(loopMutex);
    
   // wifiMgrLog("enter start event pos\n");
    eventNo = event;
    pthread_cond_signal(loopThreadCond);
    pthread_mutex_unlock(loopMutex);
    usleep(100);

}


/*
int wifiMgr::GetStatus(void) {

     


}

*/

