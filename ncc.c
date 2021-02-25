#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <winsock.h>
#include <conio.h>

/* res_init
   winsock needs to be initialized. Might as well do it as the res_init
   call for Win32 */
void res_init()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(1, 1);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
        /* Tell the user that we couldn't find a useable */
        /* winsock.dll.     */
        return;

    /* Confirm that the Windows Sockets DLL supports 1.1.*/
    /* Note that if the DLL supports versions greater */
    /* than 1.1 in addition to 1.1, it will still return */
    /* 1.1 in wVersion since that is the version we */
    /* requested. */

    if (LOBYTE(wsaData.wVersion) != 1 ||
        HIBYTE(wsaData.wVersion) != 1)
    {
        /* Tell the user that we couldn't find a useable */
        /* winsock.dll. */
        WSACleanup();
        return;
    }
}

int is_ip_format(const char *str){
    int ip[4];
    int ret = sscanf(str, "%u.%u.%u.%u", ip, ip + 1, ip + 2, ip + 3);
    if(ret != 4 || ip[0] > 255 || ip[1] > 255 || ip[2] > 255 || ip[3] > 255){
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[]){
    const char *ip = NULL;
    int port = 0;
    uint8_t crlf_enable = 0;
    for(int i = 1; i < argc; i++){
        if(is_ip_format(argv[i])){
            ip = argv[i];
        }
        else if(atoi(argv[i]) > 0){
            port = atoi(argv[i]);
        }
        else if(strcmp(argv[i], "-c") == 0){
            crlf_enable = 1;
        }
        else{
            fprintf(stderr, "invalid option %s\r\n", argv[i]);
        }
    }

    if(port == 0 || ip == NULL){
        fprintf(stderr, "invalid ip or port\r\n");
        return 0;
    }

    res_init();

    struct sockaddr_in addr_server = {0};
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(port);
    addr_server.sin_addr.s_addr = inet_addr(ip);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        fprintf(stderr, "can't create socket\r\n");
        return 0;
    }

    int ret = connect(sockfd, (struct sockaddr*)&addr_server, sizeof(addr_server));
    if(ret < 0){
        fprintf(stderr, "can't connect\r\n");
        return 0;
    }

    struct fd_set fds;
    struct timeval timeout={0, 50};
    #define BUF_SIZE 256
    char recv_buf[BUF_SIZE];

    while(1){
        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);
        ret = select(sockfd + 1, &fds, NULL, NULL, &timeout);
        if(ret == -1){
            fprintf(stderr, "select error\r\n");
            break;
        }
        else if(ret == 0){
            if(kbhit()){
                fgets(recv_buf, BUF_SIZE - 2, stdin);
                ret = strlen(recv_buf);
                if(ret){
                    if(crlf_enable){
                        recv_buf[ret - 1] = '\0';
                        strcat(recv_buf, "\r\n");
                        ret++;
                    }
                    send(sockfd, recv_buf, ret, 0);
                }
            }
        }
        else{
            if(FD_ISSET(sockfd, &fds)){
                ret = recv(sockfd, recv_buf, BUF_SIZE - 1, 0);
                if(ret <= 0){
                    break;
                }
                recv_buf[ret] = '\0';
                fputs(recv_buf, stdout);
            }
        }
    }

    shutdown(sockfd, 0x02);
    closesocket(sockfd);

    WSACleanup();

    fprintf(stderr, "exit\r\n");

    return 0;
}
