#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/capability.h>
#include "ck.h"
#include "chck.h"

extern int errno;
int daemonfd,i;
FILE *fp1;
char *perm;
char *pathname;
int capid;
char ch;
static int (*real_open) (const char *path, int oflag, mode_t mode)=NULL;
static const char * const SOCKET_PATH = "my_path";
static const char * const HELLO_WORLD = "Hello, world!\n";

/*Function to recieve file Descriptor */

static void recv_file_descriptor(int socket_fd, int fd[1])
{
    struct msghdr message;            /*Structure which holds all the information on what to send and recieve*/
    struct iovec iov;             /*Structure holding the I/O buffers*/
    struct cmsghdr *control_message = NULL;   /*control message header*/
    char ctrl_buf[CMSG_SPACE(sizeof(int) * 1)]; //sending fd in control msg
    char data[1];
    int res = -1;

    memset(&message, 0, sizeof(message));
    memset(&iov, 0, sizeof(iov));
    memset(ctrl_buf, 0, sizeof(ctrl_buf));

    data[0] = '\0';
    iov.iov_base = data;
    iov.iov_len = sizeof(data);

    message.msg_name = NULL;            //optional address
    message.msg_namelen = 0;            //address size in bytes
    message.msg_control = ctrl_buf;         //ancillary data
    message.msg_controllen = sizeof(ctrl_buf);    //number of ancillary data
    message.msg_iov = &iov;             //array of I/O buffers
    message.msg_iovlen = 1;             //number of elements in array

    res = recvmsg(socket_fd, &message, 0);

    if (res < 0)
    {
        fprintf(stderr, "recvmsg failed: %s\n", strerror(errno));
        goto ERR_RECVMSG;
    }

    for (control_message = CMSG_FIRSTHDR(&message);control_message != NULL;control_message = CMSG_NXTHDR(&message, control_message))
    {
        //printf("LibC : Got control_message from Capd...!!\n");
        int i = 0;
        int q = (control_message->cmsg_len - CMSG_ALIGN(sizeof(struct cmsghdr))) / sizeof(int);
        //printf("fd count = %d\n", q);

        for (i = 0; i < q; i++)
        {
             if ((control_message->cmsg_level == SOL_SOCKET)&& (control_message->cmsg_type == SCM_RIGHTS))
             {
                  fd[i] = *((int *)CMSG_DATA(control_message) + i);
                  printf("LibC : File Descriptor recieved :  %d\n\n",fd[i]);
            }
        }
    }

    ERR_RECVMSG:
    	EXIT:
       		return;
}

/*end of function recieve file descriptor */


/*Client function to establish connection with server*/

void client(const char *path,int oflag, mode_t mode, char *perm)
{
    int fd = -1, socket_fd = -1,nbytes;
    char buffer[256];
    struct sockaddr_un address;

    socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        fprintf(stderr, "socket failed: %s\n", strerror(errno));
        goto ERR_SOCKET;
    }

    bzero(&address,sizeof(address));
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, SOCKET_PATH);

    if (connect(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        fprintf(stderr, "connect failed: %s\n", strerror(errno));
        goto ERR_CONNECT;
    }

    /*cap_create request*/
    printf("Capability Object Creation\n");
    nbytes=snprintf(buffer,256,"%s","cap_create");
    printf("LibC : Sending capability create request : %s\n", buffer);
    write(socket_fd,buffer,nbytes);

    	nbytes = read(socket_fd, buffer, 256);
        buffer[nbytes] = 0;
        //printf("%s\n", buffer);

    nbytes=snprintf(buffer,256,"%s",pathname);
    printf("LibC : Sending resource specification from xml file : %s\n", buffer);
    write(socket_fd,buffer,nbytes);

    	nbytes = read(socket_fd, buffer, 256);
        buffer[nbytes] = 0;
        //printf("%s\n", buffer);

    nbytes=snprintf(buffer,256,"%s",perm);
    printf("LibC : Sending operation specification from xml file : %s\n\n", buffer);
    write(socket_fd,buffer,nbytes);



    	nbytes = read(socket_fd, buffer, 256);
        //printf("%s\n", buffer);
        buffer[nbytes] = 0;
    	capid=atoi(buffer);
    	//printf("%d\n", capid);
        printf("LibC : Recieved CAPID from daemon : %d\n\n", capid);

       
    nbytes=snprintf(buffer,20,"%d",capid);
    //printf("LibC : Sending CAPID back to daemon %d\n\n", capid);
    write(socket_fd, buffer, nbytes);

    	nbytes = read(socket_fd, buffer, 256);
        buffer[nbytes] = 0;
        //printf("%s\n", buffer);
    
    nbytes=snprintf(buffer,256,"%s","open");
    printf("Invocation of open\n");
    printf("LibC : Sending the function name : %s\n", buffer);
    write(socket_fd,buffer,nbytes);

        nbytes = read(socket_fd, buffer, 256);
        buffer[nbytes] = 0;
        //printf("%s\n", buffer);

    nbytes=snprintf(buffer,256,"%s",path);
    printf("LibC : Sending file path : %s\n", buffer);
    write(socket_fd,buffer,nbytes);

        nbytes = read(socket_fd, buffer, 256);
        buffer[nbytes] = 0;
        //printf("%s\n", buffer);


    nbytes=snprintf(buffer,10,"%d",oflag);
    printf("LibC : Sending file flag : %s\n", buffer);
    write(socket_fd, buffer, nbytes);

        nbytes = read(socket_fd, buffer, 256);
        buffer[nbytes] = 0;
        //printf("%s\n", buffer);

    nbytes=snprintf(buffer,10,"%d",mode);
    printf("LibC : Sending file mode : %o\n", mode);
    write(socket_fd, buffer, nbytes);

    	nbytes = read(socket_fd, buffer, 256);
        buffer[nbytes] = 0;
        //printf("%s\n", buffer);

    nbytes=snprintf(buffer,20,"%s",perm);
    //printf("LibC : Sending file permission bits : %s\n\n", perm);
    write(socket_fd, buffer, nbytes);

    /*new insertion for review*/
    printf("LibC : Sending capid : %d\n\n", capid);
    /*eoc*/

    	nbytes = read(socket_fd, buffer, 256);
        buffer[nbytes] = 0;
        printf("%s\n", buffer);
        //getchar();
        ch=getc(stdin);
        //printf("%c\n", ch);
    	//nbytes=snprintf(buffer,20,"%c", ch);
    	//write(socket_fd,buffer,nbytes);
    	/*continue from here */
    
    if(ch=='y'){
    int fdrecv[2];
    recv_file_descriptor(socket_fd, fdrecv);

    //printf("fd recieved is %d\n", fdrecv[0]);

    if (fdrecv[0] < 0)
    {
        fprintf(stderr, "recv_file_descriptor failed\n");
        goto ERR_RECV_FILE_DESCRIPTOR;
    }

    daemonfd=fdrecv[0];     //copying fd to a global variable
		//printf("fd recieved is %d\n", daemonfd);
    if (write(fdrecv[0], HELLO_WORLD, strlen(HELLO_WORLD)) < 0)
    {
        fprintf(stderr, "write failed: %s\n", strerror(errno));
        goto ERR_WRITE;
    }}
else
{
    printf("request failed!!\n");
    exit;
}
  ERR_WRITE:
      close(fd);
  ERR_RECV_FILE_DESCRIPTOR:
      if (shutdown(socket_fd, SHUT_RDWR) < 0)
      {
           fprintf(stderr, "shutdown failed: %s\n", strerror(errno));
      }
  ERR_CONNECT:
      close(socket_fd);
  ERR_SOCKET:
      return;
}

/*end of client function code*/

int open(const char *path, int oflag, mode_t mode)
{
    char *msg;
    int fd,k,i;
    int capmode=1;
    int j;
    cap_t cap;
    cap_value_t cap_list;
    cap_value_t cap_list1;
    cap_value_t cap_new_list;
    cap_flag_value_t cap_valu;
    k = 0;

    struct 
    {
      const char *str;
      cap_flag_t flag;
    }
    flags[3] = {
      {"EFFECTIVE", CAP_EFFECTIVE},
      {"PERMITTED", CAP_PERMITTED},
      {"INHERITABLE", CAP_INHERITABLE}
    };

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char get_file[256];
    char *extract;
    char *word= "permission";
    char *word1="title";
    char *ext= ".xml";
    char *fname = "open.xml";

    snprintf(get_file,256,"%s",path);
    extract = strtok (get_file, ".");
    //printf("extracted name is %s\n", extract);
    strcat(extract,ext);
    //printf("concated name is %s\n", extract);

    fp = fopen(fname , "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

   while ((read = getline(&line, &len, fp)) != -1) {
        if (strstr(line, word) != NULL){
            removeSpaces(line);
            perm = (char *) malloc (sizeof (char)*20);
   			/* get the second token */
  			strtok(line, "></");
   			strcpy(perm, strtok(NULL , "<"));
  			//printf("permission required is %s\n", perm);
            }

    
        if (strstr(line, word1) != NULL){
            removeSpaces(line);
            pathname = (char *) malloc (sizeof (char)*20);
   			/* get the second token */
  			strtok(line, "></");
   			strcpy(pathname, strtok(NULL , "<"));
  			//printf("path required is %s\n", pathname);
            }
    }

    if(real_open==NULL)
    {
    	printf("\nSwitching the mode\n");
        printf("LibC : Wrapping open()..");
        //printf("File path : %s\n", path);
        //printf("File flag : %d\n", oflag);
        //printf("File mode : %o\n", mode);
        //printf("Permission required by the process : %s\n", perm);
        int *list;
        list = pidof ("open");
        for (i=0; list[i] != -1; i++)
        {
          //printf ("pid of open is %d ", list[i]);
        }
        free (list);
        if (list[0] != -1)
        {
          printf ("\n");
        }
        cap = cap_get_pid(list[0]);

        cap_from_name("cap_audit_control", &cap_list);
        //printf("value is %-20s %d\t\t\n", "cap_audit_control", cap_list);
        cap_from_name("cap_audit_control", &cap_list1);


        /* effetive cap */
        cap_new_list = CAP_AUDIT_CONTROL;
        if (cap_set_flag(cap, CAP_EFFECTIVE, 1, &cap_new_list, CAP_SET) == -1) {
          perror("cap_set_flag cap_audit_control");
          cap_free(cap);
          exit(-1);
        }
  
        /* permitted cap */
        if (cap_set_flag(cap, CAP_PERMITTED, 1, &cap_new_list, CAP_SET) == -1) {
          perror("cap_set_flag cap_audit_control");
          cap_free(cap);
          exit(-1);
        }

        /* inherit cap */
        if (cap_set_flag(cap, CAP_INHERITABLE, 1, &cap_new_list, CAP_SET) == -1) {
          perror("cap_set_flag cap_audit_control");
          cap_free(cap);
          exit(-1);
        }

        cap_from_name("cap_audit_control", &cap_list);
        printf("LibC : POSIX Capability flag set : %-20s %d\n", "cap_audit_control", cap_list);
      	printf("flags: ");

        for (j=0; j < 3; j++)
        {
            cap_get_flag(cap, cap_list, flags[j].flag, &cap_valu);
            printf("%s %-4s \n", flags[j].str, (cap_valu == CAP_SET) ? "OK" : "NOK");
            //printf("\n");
            if(cap_valu == CAP_SET)
            {
                k = 1;
                break;
            }
        }
        //printf("the valie of k is %d\n",k);

        if(k)
        {
            printf("LibC : Capability mode... Diverting the request to CapD via UDS...\n\n");
            client(path,oflag,mode,perm);
            return daemonfd;
        }
        else
        {
            printf("LibC : Normal mode..Calling the real open\n");
            real_open=dlsym(RTLD_NEXT, "open");

            if((msg=dlerror())!=NULL)
            {
                fprintf(stderr, "open : dlopen failed\n");
            }
            else
                fd=real_open(path,oflag,mode);
        }
    }
    return daemonfd;
}
