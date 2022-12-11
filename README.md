<img src="CEH-Practical-Logo.jpg">

# CEH-Practical-Notes-and-Tools
Successfully completed the CEH (Practical) exam by EC-Council with a score of 20/20! Took me around 2 hours 20 minutes to complete the 6 hour proctored exam.  Just a typical CTF Player/Hacker going back to basics 💻

# Reconnasiance/Footprinting
<details>
  <summary>Recon</summary>

* -r range , Scan Entire Network for ALive host using ARP
```console
$ netdiscover -r 192.168.29.1/24
```

* -f switch do not fragment, -l buffer size
```console
$ ping <host-ip> -f -l 1300
```
  * __`tracert`__ for windows cmd
```console
$ traceroute <host-ip>
```
* [Path Analyzer Pro](https://www.pathanalyzer.com/download.opp/) in traceroute tools, ensure icmp and smart is selected, stop on control is selected
* Start Metasploit Console
```console
$ msfdb init && msfconsole
$ msfdb status
```
* Nmap Scanning entire Network
* Comprehensive Scan

```console

nmap -Pn -A x.x.x.1/24 -vv --open   
```
  > To scan the live Host
```console

nmap -sP x.x.x.1/24                 // To scan the live Host
nmap -sn x.x.x.1/24                 // To scan the live Host
nmap -p port x.x.x.1/24 --open
```
```console
nmap -Pn -sS -A x.x.x.1/24 -vv --open
```



</details>
