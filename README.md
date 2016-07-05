#dvswitch
* fork of commit 1d73d4a which compiles and runs on ubuntu 16.04 i686/amd64(tested)
* reason 2015 mailinglist activity shows the dvswitch project is dead and removed from current ubuntu/debian repositories!
* attempt to continue/add missing "half-assed" features to this beautifull robust software package that honours field interlacing!

##Compile and Run on ubuntu 16.04

Based on ubuntu 16.04 mini.iso install with tasksel "Ubuntu MATE minimal installation + openssh-server + other system defaults tasksel package (don't know name anymore)"

```
sudo apt-get install dvgrab ffmpeg cmake build-essential libboost-thread-dev libgtkmm-2.4-dev libavcodec-dev libavutil-dev libasound2-dev libxv-dev libjack-jackd2-dev liblo-dev git
mkdir mygitprojects
cd mygitprojects
git clone https://github.com/walterav1984/dvswitch.git
cd dvswitch
#exclude build dir from git, but don't know yet howto do it...
mkdir build
cd build
cmake ..
make
sudo make install #or copy *.png ../data/ icons  to /usr/local/share/dvswitch/ yourself and run from ./dvswitch from src/
```
###Fix ubuntu 16.04 firewire sudo permissions /dev/fw* for "ancient" DV devices

Grabbing DV video signals with "dvgrab" or using "dvswitch" normally works for modern DV devices which get added to the video/audio USER GROUP automatically by udev without sudo permissions, but some "ancient" DV camera's are not recognized by the udev rules since 16.04!

```
#compare permisions of working "modern" dv devices with "ancient" dv devices
ls -la /dev/fw*
getfacl /dev/fw*

#wrong group root/video, no rw rights, missing + attribute!

cat /sys/bus/firewire/devices/fw1/* #shows more firewire info like ATTR

#duplicate the lines in the following udev rules containing:
#"*0x00a02d:0x010001*" alter to the ATTR of the old sony camera "*0x00a02d:0x010000*"

sudo nano /lib/udev/rules.d/50-udev-default.rules
#SUBSYSTEM=="firewire", ATTR{units}=="*0x00a02d:0x010001*", GROUP="video"
+SUBSYSTEM=="firewire", ATTR{units}=="*0x00a02d:0x010000*", GROUP="video"

sudo nano /lib/udev/rules.d/70-uaccess.rules
#SUBSYSTEM=="firewire", ATTR{units}=="*0x00a02d:0x010001*", TAG+="uaccess"
+SUBSYSTEM=="firewire", ATTR{units}=="*0x00a02d:0x010000*", TAG+="uaccess"
```

##TODO
* create list of usefull/stable patches and commit them accordingly by git(hub) rules:

- [x] ~~https://github.com/yoe/dvswitch/commit/79b63feef6f7a27c2f962d5a7efffa4f9d657f57       #fix memleak dvsink-file~~
- [ ] https://github.com/jnweiger/dvswitch/commit/b0eff96d68fafaa3704426c5d4ff3c0391b921ef  #icon for lost sources
- [ ] https://github.com/jnweiger/dvswitch/commit/82355bfd0861dd4553899a460b8b415ec86b2e3b  #mouse clickable preview thumbs
- [ ] https://github.com/jnweiger/dvswitch/commit/0d6e549b26f81096038f703000316605f51e5b67  #title safe area activated by flag
- [ ] https://github.com/jnweiger/dvswitch/commit/95838c05470adf363fee02af7fb791df075255f1  #title safe default off
- [ ] http://lists.alioth.debian.org/pipermail/dvswitch-devel/2011-August/000480.html       #save independent source streams
- [ ] myself dif.c                                                                          #show full D1 720 pixels vs 702/704 "active"
- [ ] myself ?                                                                              #adjustable titlesafe 16/9 crop from 4/3
- [ ] myself ?                                                                              #limit warning/error message flood console
  
* little firewire debug hinst "kernel modules / lspci / dmesg / IRQ interupts sharing / etc"

##Other Usefull DV related tools
* dvpause (interactive playback tool with frame accurate seek to feed offline raw.dv files into dvswitch mixer) 
  https://github.com/Jeija/dvpause
  
* test-dv (sent raw dv video from PC to FireWire recorder/monitor)  
  https://www.kernel.org/pub/linux/libs/ieee1394/libiec61883-1.2.0.tar.gz 

* ffmpeg

* mlt/melt
