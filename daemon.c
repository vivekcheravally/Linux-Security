#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define MIN 100000
#define MAX 1000000
#define R "READ"
#define W "WRITE"
#define X "EXECUTE"

extern int errno;
char path[256],request[256],request1[256];
mode_t mode;
mode_t mod;
int res;
char new[10];
char pathname[256];
char resperm[256];
char mode1[256];
char perm[256];
char getperm[256];
int oflag;
int capid[256];
int flag1,p,s;
int num=0;
int capoid_index;
int i,capguid_index,capid_index,getindex,array2[10],capguid[10];
FILE *fp1;
FILE *fp2;
int nbytes1;
char buffer1[512];
static const char * const SOCKET_PATH = "my_path";

int temp(const char *path);
int remap();

struct GCO
{
	char path1[256];
	char getperm1[256];
	int uuid;
	int count;
}gc[520],*m,*n;

struct GCO gc2;

int getrand(int min,int max)
{
	return(rand()%(max-min)+min);
}

int min(char a, char b)
{
    if(a<b)	
        return a;
    else	
		return b;  	  
}

int getid(int connection_fd,uid_t euid,gid_t gid)
{
    struct ucred cred;
    socklen_t len=sizeof(cred);
    if(getsockopt(connection_fd,SOL_SOCKET,SO_PEERCRED,&cred,&len)<0)
        return(-1);

    euid=cred.uid;
    gid=cred.gid;

    //printf("User id is : %d\n", euid);
    //printf("Group id is : %d\n", gid);

    fp1 = fopen ("mydaemonfile.txt", "a");

    fprintf(fp1,"User id is :%d\n", euid);
    fprintf(fp1,"Group id is :%d\n", gid);

    fclose(fp1);
    return 0;
}

static int create_server()
{
 	int socket_fd = -1;
    struct sockaddr_un address;

    socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        printf("socket() failed\n");
        goto EXIT;
    }

    unlink(SOCKET_PATH);
    /* start with a clean address structure */
    bzero(&address,sizeof(address));
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, SOCKET_PATH);

    if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("bind() failed\n");
        goto ERR_BIND;
    }

    if (listen(socket_fd, 5) < 0)
    {
        printf("listen() failed\n");
        goto ERR_LISTEN;
    }
    
    

    goto EXIT;
	    ERR_LISTEN:
	       ERR_BIND:
            close(socket_fd);
  	      socket_fd = -1;
	  EXIT:
      return socket_fd;
}

int connection_handler(int connection_fd)
{
    int nbytes;
    char buffer[256];
    int i,j;
    int len,len1,random=0;

     /*list of calls */
    const char *call_name[] = {
        "cap_create",
        "open",
        "socket",
        "delete",
        "bind", 
    };
    /*list completion */

    /*recevig the capability request*/
    nbytes = read(connection_fd, buffer, 256);
    buffer[nbytes] = 0;
    strcpy(request,buffer);
    printf("\nRequest received : %s\n", request);
    for(i=0;i<5;i++)
    if(strcmp(request,call_name[i])==0 && strcmp(request,call_name[0])==0)
    {
        //printf("Call is found in the array at position %d %s\n", i,call_name[0]);
        nbytes=snprintf(buffer, 256, "Capd : Request received for '%s' !! Waiting for path and permission", request);
        write(connection_fd, buffer, nbytes);    
    

    nbytes = read(connection_fd, buffer, 256);
    buffer[nbytes] = 0;
    strcpy(pathname,buffer);
    //printf("Resource description : %s\n", pathname);

        nbytes = snprintf(buffer, 256, "CapD : Got the Pathname '%s'....waiting for operation spec", pathname);
        write(connection_fd, buffer, nbytes);

    nbytes = read(connection_fd, buffer, 256);
    buffer[nbytes] = 0;
    strcpy(resperm,buffer);
    printf("Resource description : %s, %s\n", pathname,resperm);

		/* start of mapping*/

        temp(pathname);
        fp1=fopen("mydaemonfile.txt","a");
        fprintf(fp1, "FSTAT: PERMISSION OBTAINED FROM FSTAT: %s\n", getperm);
        fclose(fp1);

        len=strlen(resperm);
        len1=strlen(getperm);
        //printf("%d\t%d\n", len,len1);
        if(len==len1)
        {
            for(j=0;j<len;j++)
            {
                new[j]=min(getperm[j],resperm[j]);
                //printf("%d\n", new[j]);
            }
            //printf("new is %c\n", new[2]);
            /*converting to mode */
            
 
           mode = (((new[1] == 'r') * 4 | (new[2] == 'w') * 2 | (new[3] == 'x')) << 6) |
              (((new[4] == 'r') * 4 | (new[5] == 'w') * 2 | (new[6] == 'x')) << 3) |
              (((new[7] == 'r') * 4 | (new[8] == 'w') * 2 | (new[9] == 'x')));   
            
            //printf("mode accordng to mod is %o\n", mode);
            /*end of code */
            
            printf("Capability Object creation  \n");
            printf("Resource name : %s\n", pathname);
            strcpy(gc[num].getperm1, new);
            printf("Permission : %s\n", gc[num].getperm1);
            fp1=fopen("mydaemonfile.txt","a");
            fprintf(fp1, "Creating Capability permissions for Global Capability Objects: %s\n\n", new);
            fclose(fp1);
        }
        else
        {
            printf("Object Cannot be created\n");
        }
        
        srand(time(NULL));
        random=getrand(MIN,MAX);
        gc[num].uuid=random;
        printf("UUID : %d\n", gc[num].uuid);    
        gc[num].count=1;
        printf("Reference count : %d\n\n", gc[num].count);



        //printf("address is %p\n", &gc[num]);
      	m=&gc[0];
      	n=&gc[num];
     		//printf("address is %p %p\n", n, m);
     		capoid_index=n-m;
     		//printf("capoid is : %d\n", capoid_index);
     	
     		/*mapping starts*/
      	for (i = 0; i <=num; i++ ) {
    		capguid[i] = capoid_index; 
			}
    	
       
				for (i = 0; i <=num; i++){
    			if (capoid_index == capguid[i]){
        		capguid_index = i;
     		}
		}
		//printf("capguid is : %d\n", capguid_index);


		for (i = 0; i <=num; i++ ) {
			capid[0]=capguid_index;
		}

		
		for (i = 0; i <=num; i++){
    		if (capguid_index == capid[i]){
        		capid_index = i;
     		}
     	
		}
		//printf("capid is : %d\n\n", capid_index);
		res=capid_index;
		/* mapping ends */

		/*continuing the request reply*/
        nbytes = snprintf(buffer, 256, "%d", res);
        //printf("Got the Operation spec .. sending back the CAPID %d\n", res);
        write(connection_fd, buffer, nbytes);

    nbytes = read(connection_fd, buffer, 256);
    buffer[nbytes] = 0;
    getindex=atoi(buffer);
    

        nbytes = snprintf(buffer, 256, "CapD : Got the CAPID....waiting for the call to be made");
        write(connection_fd, buffer, nbytes);
    }
    
    nbytes = read(connection_fd, buffer, 256);
    buffer[nbytes] = 0;
    strcpy(request1,buffer);
    printf("Request received : %s\n", request1);
    for(j=0;j<5;j++)
    if(strcmp(request1,call_name[j])==0 && strcmp(request1,call_name[1])==0 && strcmp(request1,call_name[0])!=0)
    {
        //printf("Call is found in the array at position %d %s\n", j,call_name[1]);
        nbytes=snprintf(buffer, 256, "Capd : Request received for '%s' !! Waiting for path and permission", request1);
        write(connection_fd, buffer, nbytes);  

    nbytes = read(connection_fd, buffer, 256);
    buffer[nbytes] = 0;
    strcpy(path,buffer);
    strcpy(gc[num].path1, buffer);
    //printf("FIRST MESSAGE FROM CLIENT: %s\n", path);
    printf("File path : %s\n", gc[num].path1);

        nbytes = snprintf(buffer, 256, "CapD : Got the path....waiting for file flag");
        write(connection_fd, buffer, nbytes);

    nbytes = read(connection_fd, buffer, 256);
    buffer[nbytes] = 0;
    oflag=atoi(buffer);
    printf("File flag : %d\n", oflag);


        nbytes = snprintf(buffer, 256, "CapD : Got the flag....waiting for mode");
        write(connection_fd, buffer, nbytes);

  	nbytes = read(connection_fd, buffer, 256);
    buffer[nbytes] = 0;
    mode=atoi(buffer);
    printf("File mode : %o\n", mode);
    //snprintf(mode1,10,"%d",mode);

        nbytes = snprintf(buffer, 256, "CapD : Got the mode....waiting for operation permissions");
        write(connection_fd, buffer, nbytes);

    nbytes = read(connection_fd, buffer, 256);
    buffer[nbytes] = 0;
    strcpy(perm,buffer);
    //printf("FOURTH MESSAGE FROM CLIENT: %s\n", perm);
    printf("Capid : %d\n\n", getindex);


    	/*new code strats*/

    	 /*new code authorization*/
       
        

        /*calling the remap function */
		remap();
        //printf("%c\n", gc[p].getperm1[2]);
        //printf("%s\n", gc.getperm1);
		/*communicate to user*/
		for(i=0;i<strlen(gc[p].getperm1);i++)
			{
				if(gc[p].getperm1[1]=='r' && gc[p].getperm1[2]=='w')
				flag1=1;
			}
			//printf("flag is %d\n", flag1);
		
		
   			if(flag1==1)
   			{
   					//printf("CapD : User requested file is open with following permissions \t %s\t %s\t, to continue (Y/n)", R,W);
   					 nbytes = snprintf(buffer, 512, "CapD : User program is trying to open the following files with the following permissions : \n\n\t %s\t %s,%s\n\n to continue press 'Y', else 'N'", path,R,W);
   					 write(connection_fd, buffer, nbytes);
   			}
   			else
   			{
   					nbytes = snprintf(buffer, 512, "CapD : User requested file is open with following permissions \t\n %s\t\n, to continue (Y/n)", R);
   					write(connection_fd, buffer, nbytes);
   			}   			

		/*ends*/
    
 	}
     		
    
   	fp1=fopen("mydaemonfile.txt","a");
   	fprintf(fp1,"MESSAGE (PATH) FROM CLIENT: %s\n", path);
 	fprintf(fp1,"MESSAGE (FLAG) FROM CLIENT: %d\n", oflag);
 	fprintf(fp1,"MESSAGE (MODE) FROM CLIENT: %o\n", mode);
    fprintf(fp1,"MESSAGE (PERMISSION) FROM CLIENT: %s\n", perm);
 	fclose(fp1);
	// close(connection_fd);
 	return 0;
}

/* Send File Descriptor */

static int send_file_descriptor(int socket_fd, int fd[1])
{
    struct msghdr message;						/*Structure which holds all the information on what to send and recieve*/
    struct iovec iov;							/*Structure holding the I/O buffers*/
    struct cmsghdr *control_message = NULL;		/*control message header*/

 	char ctrl_buf[CMSG_SPACE(sizeof(int))];
    char data[1];

    memset(&message, 0, sizeof(message));
    memset(&iov, 0, sizeof(iov));
    memset(ctrl_buf, 0, sizeof(ctrl_buf));
    data[0] = '\0';
    iov.iov_base = data;
    iov.iov_len = sizeof(data);

    message.msg_name = NULL;						//optional address
    message.msg_namelen = 0;						//address size in bytes
    message.msg_iov = &iov;							//array of I/O buffers
    message.msg_iovlen = 1;							//number of elements in array
    message.msg_controllen = sizeof(ctrl_buf);		//number of ancillary data
    message.msg_control = ctrl_buf;					//ancillary data

    control_message = CMSG_FIRSTHDR(&message);
    int i = 0;
    //for (i = 0; i < 1; i++)
    //{
    control_message->cmsg_level = SOL_SOCKET;				//orginating protocol
    control_message->cmsg_type = SCM_RIGHTS;				//protocol-specefic type ; SCM-Socket-level control message
    control_message->cmsg_len = CMSG_LEN(sizeof(fd[i]));	//data byte count, including header

    int* dataBuff = ((int*)CMSG_DATA(control_message));		//Macro to obtain the pointer to the integer
    memcpy(dataBuff, fd, sizeof(int));
    control_message = CMSG_NXTHDR(&message, control_message);
 // }

    return sendmsg(socket_fd, &message, 0);
}


/* end code */

/*daemon function */

int daemo()
{
    pid_t pid = 0;
    pid_t sid = 0;

    int i = 0;
    pid = fork();				// fork a new child process

    if (pid < 0)
    {
        printf("fork failed!\n");
        exit(1);
    }

    if (pid > 0)				// its the parent process
    {
        printf("pid of child process %d \n", pid);
        exit(0); 				//terminate the parent process succesfully
    }

    umask(0);					//unmasking the file mode

    sid = setsid();				//set new session
    if(sid < 0)
    {
        exit(1);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    fp1 = fopen ("mydaemonfile.txt", "a");
    fprintf(fp1,"%s\n", "logging details");
    fclose(fp1);
    return 0;
}

/*end */

/*fstat function*/

int temp(const char *path)
{
	struct stat filestat;
	if(stat(path,&filestat)<0)
		return 1;
	strcpy(getperm,"");

	//printf((S_ISDIR(filestat.st_mode)) ? "d" : "-");
	strcat(getperm,(S_ISDIR(filestat.st_mode)) ? "d" : "-");
	//printf((filestat.st_mode & S_IRUSR) ? "r" : "-");
	strcat(getperm,(filestat.st_mode & S_IRUSR) ? "r" : "-");
	//printf((filestat.st_mode & S_IWUSR) ? "w" : "-");
	strcat(getperm,(filestat.st_mode & S_IWUSR) ? "w" : "-");
	//printf((filestat.st_mode & S_IXUSR) ? "x" : "-");
	strcat(getperm,(filestat.st_mode & S_IXUSR) ? "x" : "-");
	//printf((filestat.st_mode & S_IRGRP) ? "r" : "-");
	strcat(getperm,(filestat.st_mode & S_IRGRP) ? "r" : "-");
	//printf((filestat.st_mode & S_IWGRP) ? "w" : "-");
	strcat(getperm,(filestat.st_mode & S_IWGRP) ? "w" : "-");
	//printf((filestat.st_mode & S_IXGRP) ? "x" : "-");
	strcat(getperm,(filestat.st_mode & S_IXGRP) ? "x" : "-");
	//printf((filestat.st_mode & S_IROTH) ? "r" : "-");
	strcat(getperm,(filestat.st_mode & S_IROTH) ? "r" : "-");
	//printf((filestat.st_mode & S_IWOTH) ? "w" : "-");
	strcat(getperm,(filestat.st_mode & S_IWOTH) ? "w" : "-");
	//printf((filestat.st_mode & S_IXOTH) ? "x" : "-");
	strcat(getperm,(filestat.st_mode & S_IXOTH) ? "x" : "-");
	//printf("\n");
	//strcpy(gc.getperm1, getperm);
	printf("fstat output : %s\n\n",getperm);
	//printf("fstat: Permission bits of %s is %s\n", path,gc[p].getperm1);
}

/*end of fstat*/
/*map*/
int remap(){
	/* mapping starts */	
	s= capid[getindex];
	//printf("mapping of capid to capguid is %d\n", s);
	p=capguid[s];
	//printf("mapping capguid to capoid is %d\n", p);
	printf("Capability objects accessed : %s, %s, %d, %d\n", gc[p].path1,gc[p].getperm1,gc[p].uuid,gc[p].count);
	/*end of code*/

	//if(getindex==index2 && array2[getindex]==index1 && array1[index1]==capoid)
	//{
		//printf("Mapping Successful\n\n");

		return 0;
	}

/*map over*/

/*Main Function */

int main()
{
    //daemo();
    int capoid_index;
	
	int flag;
    uid_t euid;
    gid_t gid;
    int capid1;

    FILE *fp = NULL;
    int fd[1] = {-1};
    int server = -1, client = -1,connection_fd;
    server = create_server();
    if (server < 0)
    {
        printf("create_server() failed\n");
        return 0;
        //goto ERR_CREATE_SERVER;
    }

    while(1)
    {
        client = accept(server, NULL, NULL);
        //printf("client() accept %d\n", client);

        fp1=fopen("mydaemonfile.txt","a");
        fprintf(fp1, "client() accept %d\n", client);
        fclose(fp1);

        if (client < 0)
        {
            printf("accept() failed\n");
            goto ERR_ACCEPT;
        }
        
        //printf("give client = %d\n", client);
        getid(client,geteuid(),getgid());
        connection_handler(client);
				
        //printf("value of num is %d\n", num);
       
		/*authorization starts */
		//printf("%c\n", gc[p].getperm1[1]);
		for(i=0;i<strlen(gc[p].getperm1);i++)
		{

			if(gc[p].getperm1[1]=='r' && oflag==0 || gc[p].getperm1[2]== 'w' && oflag==1 || gc[p].getperm1[2]== 'w' && oflag==2 || 
				gc[p].getperm1[2]== 'w' && oflag==1024 || gc[p].getperm1[2]== 'w' && oflag==1025 || gc[p].getperm1[2]== 'w' && oflag==1026)
				flag=1;
		}
        
		if (flag==1)
		{
            printf("Authorization status : Sucess\n");
       		fd[0]=open(path,oflag,mode);
    	}
    	else
    	{
    		printf("Permission denied for a write request\n");
    	}

        printf("File descriptor returned : %d\n\n", fd[0]);
        if (send_file_descriptor(client, fd) < 0)
        {
            printf("send_file_descriptor failed: %d\n", send_file_descriptor(client, fd));
            perror("sendmsg():");
            goto ERR_SEND_FILE_DESCRIPTOR;
        }
    
  	    close(client);
  	    num++;
   	}
	
	ERR_SEND_FILE_DESCRIPTOR:
  	   close(client);
	ERR_ACCEPT:
  	   close(server);
	  
}
