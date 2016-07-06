Warning This Page/Project is evolving, things can break/change/damage without notice!

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
mkdir -p mybuilds/dvswitch/build
cd mybuilds/dvswitch/build
cmake ../../../dvswitch
make
sudo make install 
dvswitch -p 1234 -h 127.0.0.1 #run global command available after install
#or copy *.png  icons from dvswitch/data/ folder to /usr/local/share/dvswitch/ 
#and run binaries from compiled src/ ./dv*
```
###Fix ubuntu 16.04 firewire sudo permissions /dev/fw* for "ancient" DV devices

Grabbing DV video signals with "dvgrab" or using "dvswitch" normally works for modern DV devices which get added to the video/audio USER GROUP automatically by udev without sudo permissions, but some "ancient" DV camera's are not recognized by the udev rules since 16.04!

```
#compare permisions of working "modern" dv devices with "ancient" dv devices
ls -la /dev/fw*
getfacl /dev/fw*
cat /sys/bus/firewire/devices/fw1/* #shows more firewire info like ATTR

#wrong group root/video, no rw rights, missing +attribute, ATTR different!

#duplicate the lines in the following udev rules containing:
#*0x00a02d:0x010001* alter to the ATTR of the old sony camera *0x00a02d:0x010000*

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
- [x] ~~https://github.com/jnweiger/dvswitch/commit/82355bfd0861dd4553899a460b8b415ec86b2e3b  #mouse clickable preview thumbs~~
- [x] ~~https://github.com/jnweiger/dvswitch/commit/95838c05470adf363fee02af7fb791df075255f1  #title safe default off~~
- [x] ~~myself dif.c                                                                        #show full D1 720 pixels vs 702/704 "active"~~
- [x] ~~http://lists.alioth.debian.org/pipermail/dvswitch-devel/2011-August/000480.html       #save independent source streams~~
- [x] ~~https://github.com/jnweiger/dvswitch/commit/b0eff96d68fafaa3704426c5d4ff3c0391b921ef  #icon for lost sources~~
- [ ] https://github.com/jnweiger/dvswitch/commit/0d6e549b26f81096038f703000316605f51e5b67  #title safe area activated by flag
- [x] ~~myself ?                                                                              #crop titlesafe 16/9 for 4/3 camera~~
- [ ] myself ?                                                                              #limit warning/error message flood console
- [ ] Carl K.complete dvsource-file patch with manual/doc 
- [ ] myself remove sources from settings menu
- [ ] myself change GUI naming to match keyboard shortcut keys!
  
* check all these instructions I've written down!

* example scripts of all great dv/firewire related tools

* some firewire debug hints "kernel modules / lspci / dmesg / IRQ interupts sharing / etc"

##Other Usefull DV related tools
* dvgrab (simple cli capture util for dv/hdv firewire sources)
  see repository

* dvpause (interactive playback tool with frame accurate seek to feed offline raw.dv files into dvswitch mixer) 
  https://github.com/Jeija/dvpause
```
To compile on ubuntu 16.04
sudo apt-get install libgtk-3-dev libcairo2-dev #ontop of packages to compile dvswitch!!!

change "Makefile" line 7 order by putting $(FILES) before $(LIBS):
	$(CC) $(FILES) $(LIBS) $(CFLAGS) -Wall -g -rdynamic -o main

#only works if dvswitch dvsource-file was installed!
make run #Change -p port -h host in the Makefile

TODO commit patches to dvpause for autogenerating playlist based on folder content!
```
  
* test-dv (sent raw dv video from PC back to FireWire recorder/monitor with for instance SDI/HDMI interfaces)  
  https://www.kernel.org/pub/linux/libs/ieee1394/libiec61883-1.2.0.tar.gz
```
To compile on ubuntu 16.04
sudo apt-get install libraw1394-dev libiec61883-dev libdv-bin
./configure
make
cd examples
./test-dv filename-raw-video.dv
```

* ffmpeg
  see repository

* mlt/melt
  see repository
  or "shotcut" for newest included melt builds https://www.shotcut.org/
  https://www.mltframework.org/
