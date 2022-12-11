<img src="CEH-Practical-Logo.jpg">

# CEH-Practical-Notes-and-Tools
Successfully completed the CEH (Practical) exam by EC-Council with a score of 20/20! Took me around 2 hours 20 minutes to complete the 6 hour proctored exam.  Just a typical CTF Player/Hacker going back to basics 💻

# Reconnasiance/Footprinting
<details>
  <summary>Network</summary>
  
## Netdiscover
  
* Scan Entire Network for ALive host using ARP
```console
netdiscover -r x.x.x.1/24
```
  
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
* Comprehensive Scan
```console
nmap -Pn -A x.x.x.1/24 -vv --open   
```

### Common Port  

* 22        - SSH
* 21        - FTP
* 389,3389  - RDP
* 3306      - MYSQL
</details>

# Reference
<details>
  <summary>TryHackMe</summary>

## TryHackMe
### Learning Path
* [Pre-Security](https://tryhackme.com/paths) 
* [Jr Penetration Tester](https://tryhackme.com/paths)
* [Complete Beginner](https://tryhackme.com/paths) 
### Rooms
* [Nmap](https://tryhackme.com/room/furthernmap)
* [SQLMAP]https://tryhackme.com/room/sqlmap

  

## Useful Links
[hash.com](https://hashes.com/en/decrypt/hash) is a online hash Identifier and Cracker 
  

</details>
