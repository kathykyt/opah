#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

#define BUFLEN  100
#define STDOUT_FILENO 0

/*==========================

  buff length must be more than 12 byte
=============================================
*/
int getMacAddr(char *buff)
{
  struct ifreq s;
  int fd = socket(PF_INET, SOCK_DGRAM, 0);

  strcpy(s.ifr_name, "wlan0");
  if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {
    int i;
    for (i = 0; i < 6; i++) {
      printf(" %02x", (unsigned char) s.ifr_addr.sa_data[i]);
	sprintf((buff + 2*i), "%02x", (unsigned char) s.ifr_addr.sa_data[i]);
    }
   // *(buff+ 2*i) = 0;
    //*(buff+ 2*i + 1) = 0;
    //puts("\n");
    return 0;
  }

  
  return 1;
}


int test_internet_connect(void) {

  struct addrinfo hints, *servinfo, *p;
  int rv;
  struct hostent *he;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET; // use AF_INET6 to force IPv6
  hints.ai_socktype = SOCK_STREAM;

  if ((he = gethostbyname("www.yahoo.com")) == NULL) {
     printf("test_internet_connect error\n");
    return -1;

  }

  return 0;

#if 0
  if ((rv = getaddrinfo("www.yahoo.com", "http", &hints, &servinfo)) != 0) {
    printf("test_internet_connect error\n");
    return -1;
  }
  
   return 0;
#endif

}


int test_internet_ping(void) {

   int pipe_arr[2];
    char buf[BUFLEN];
   int next_line_pos = 0;
   int i;

    //Create pipe - pipe_arr[0] is "reading end", pipe_arr[1] is "writing end"
    pipe(pipe_arr);

    if(fork() == 0) //child
    {
        dup2(pipe_arr[1], STDOUT_FILENO);
        execl("/system/bin/ping", "ping", "-c 1", "8.8.8.8", (char*)NULL);   
	exit(0); 
    }
    else //parent
    {
        wait(NULL);
        read(pipe_arr[0], buf, BUFLEN);
        printf("received data=%s\n", buf);
	//sscanf(buf, "%s", 
	//check the return string
	for (i = 0; i< BUFLEN; i++) {
	  if ( *(buf + i) == 0x0A) {
		printf("next line = %s\n", (buf + i));
	  }
	}
    }

    close(pipe_arr[0]);
    close(pipe_arr[1]);
    return 0;



}



