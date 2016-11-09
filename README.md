Warning This Page/Project is evolving, things can break/change/damage without notice!

#dvswitch
* live PAL/NTSC VideoMixer (network orientated) for DV file(s)/stream(s) originating from local FireWire/iLink devices like camera's, recorders or even remote ones on the network/internet.

* fork of commit 1d73d4a which compiles and runs on ubuntu 16.04 i686/amd64(both tested)
* reason 2015 mailinglist activity shows the dvswitch project is dead and removed from current ubuntu/debian repositories!
* attempt to continue/add missing "half-assed" features to this beautifull robust software package that honours field interlacing!

##Compile and Run on ubuntu 16.04

Based on ubuntu 16.04 mini.iso install with tasksel "standard system utilities + Ubuntu MATE minimal installation + OpenSSH-server":

```
sudo apt-get install dvgrab ffmpeg cmake build-essential libboost-thread-dev libgtkmm-2.4-dev libavcodec-dev libavutil-dev libasound2-dev libxv-dev libjack-jackd2-dev liblo-dev git
mkdir mygitprojects
cd mygitprojects
git clone https://github.com/walterav1984/dvswitch.git
mkdir -p mybuilds/dvswitch/build1
cd mybuilds/dvswitch/build1
cmake ../../../dvswitch
make
cd src
./dvswitch -p 1234 -h 127.0.0.1 #see error missing icons!
sudo mkdir /usr/local/share/dvswitch
sudo cp ~/mygitprojects/dvswitch/data/*.png /usr/local/share/dvswitch/
./dvswitch -p 1234 -h 127.0.0.1 #Main Process

./dvsource-dvgrab --firewire -p 1234 -h 127.0.0.1 #Sub Process connects 'some/single' DV iLink/FireWire to dvswitch
./dvsource-dvgrab --firewire -g typeGUIDcodehere -p 1234 -h 127.0.0.1 #Sub Process connects specific DV device to dvswitch

#What is typeGUIDcodehere? DV devices have unique codes that help identify same type/models so you can choose a specific
#camera to be the first or the third device in your video mixer. To find GUID codes, use terminal and type:dmesg | grep "GUID"

./dvsource-file -p 1234 -h 127.0.0.1 -l  ~/mygitprojects/dvswitch/tests/test1.dv #Sub Process -l loops file for continues playback in dvswitch 

./dvsink-files -p 1234 -h 127.0.0.1 ~/Desktop/recordmixeroutput.dv #Sub Process records the output of dvswitch(when record is pressed)!

#when all parts behave fine do 
sudo make install #dvswitch gets installed with icons and related sub process tools and can be run global!
```
###Fix ubuntu 16.04 firewire sudo permissions /dev/fw* for "ancient" DV devices

Grabbing DV video signals with "dvgrab" or using "dvswitch" normally works for modern DV devices which get added to the video/audio USER GROUP automatically by udev without sudo permissions, but some "ancient" DV camera's are not recognized by the udev rules since 16.04!

```
#quick&dirty autopatch
sudo sed -i 's/0x010001/0x01000/g' /lib/udev/rules.d/50-udev-default.rules
sudo sed -i 's/0x010001/0x01000/g' /lib/udev/rules.d/70-uaccess.rules

#manual readon compare permisions of working "modern" dv devices with "ancient" dv devices
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

```
Ubuntu 16.04 tweaks hdd/cpu
sudo nano /etc/hdparm.conf #uncomment apm=255 which disables hdd powermanagement
sudo nano /etc/init.d/ondemand #replace GOVERNOR="ondemand/performance/interactive/etc" all with "performance"
```

##TODO
- [x] ~~https://github.com/yoe/dvswitch/commit/79b63feef6f7a27c2f962d5a7efffa4f9d657f57       #fix memleak dvsink-file~~
- [x] ~~https://github.com/jnweiger/dvswitch/commit/82355bfd0861dd4553899a460b8b415ec86b2e3b  #mouse clickable preview thumbs~~
- [x] ~~https://github.com/jnweiger/dvswitch/commit/95838c05470adf363fee02af7fb791df075255f1  #title safe default off~~
- [x] ~~myself dif.c                                                                        #show full D1 720 pixels vs 702/704 "active"~~
- [x] ~~http://lists.alioth.debian.org/pipermail/dvswitch-devel/2011-August/000480.html       #save independent source streams~~
- [x] ~~https://github.com/jnweiger/dvswitch/commit/b0eff96d68fafaa3704426c5d4ff3c0391b921ef  #icon for lost sources~~
- [x] ~~myself ?                                                                              #crop titlesafe 16/9 for 4/3 camera~~
- [x] ~~https://github.com/yoe/dvswitch/commit/26d1c37a90a64362fe4afa08057331ba10c06eb0	    #highres color thumbnails~~
- [x] ~~myself remove sources from settings menu~~
- [x] ~~myself change GUI naming to match keyboard shortcut keys!~~
- [ ] https://github.com/jnweiger/dvswitch/commit/0d6e549b26f81096038f703000316605f51e5b67  #title safe area activated by flag
- [x ] ~~myself ?                                                                              #limit warning/error message flood console~~
- [ ] myself complete dvsource-file patch with manual/doc by CarlFK. 
- [ ] Why dvsource-file '-n' parameter has worse performance than dirty tee pipe script contact CarlFK.?
- [ ] Write user friendly instruction/step/debug plan instead of minimal steps for developers.

##TODO hard?
- [ ] automate/link "dvpause project" start/play button with source selection (keyboard press) in dvswitch?
- [ ] integrate watermark/tga/png overlay (multiple slots) or use MLT at the end? 

* example scripts of all great dv/firewire related tools / pipe-constructions
```
#mixer output of 1st dvswitch as a source to 2nd dvswitch mixer input
dvsink-command -p 1234 -h 127.0.0.1 'dvsource-file - -p remoteport -h remotehost' 

#record individual sources instead of dvswitch single mix output!
dvgrab -noavc -g typeGUIDcamera1 - | tee >(dvsource-dvgrab --firewire - -p 1234 -h 127.0.0.1) >(~/Desktop/camera1fileoutput.dv) > /dev/null
dvgrab -noavc -g typeGUIDcamera2 - | tee >(dvsource-dvgrab --firewire - -p 1234 -h 127.0.0.1) >(~/Desktop/camera2fileoutput.dv) > /dev/null
```

* some firewire debug hints "lspci / kernel modules / dmesg / IRQ interrupts sharing / etc"
```
#lists pci(-e) FireWire adapter(s)
lspci | egrep "IEEE|Ieee|ieee|1394|Fire|FIRE|fire|OHCI|Ohci|ohci"

#lists kernelmodule(s) loaded
lsmod | egrep "IEEE|Ieee|ieee|1394|Fire|FIRE|fire|OHCI|Ohci|ohci|RAW|Raw|raw"

#list GUID of any FireWire device including the pci(-e) adapter(s) itself
dmesg | grep GUID #s400 are probably the pci(-e) chipsets itself, dv/hdv devices are probably s100

#list GUID of connected FireWire devices excluding the adapter(s) itself
dmesg | grep GUID | egrep -v "puts400GUIDhere|puts400GUIDhere|etc" #  

#show IRQ / interrupts of FireWire pci(-e) adapters or shared devices using the same interupt
cat /proc/interrupts | egrep "firewire|ohci"
```

##Other important DV related tools
* dvgrab (easy cli capture util for dv/hdv firewire sources)
  see repository
```
#examples capture live dv in camera mode 
dvgrab -noavc -f raw ~/Desktop/testcapturecamerastream.dv #captures dv files usable by dvswitch/dvpause Final Cut X/Pro/Express
dvgrab -noavc -f raw -s 0 ~/Desktop/testcapturemaerastream.dv #captures dv 'unlimmited filesize' bigger than 1GB 
dvgrab -noavc -g typeGUIDcodehere -f raw -s 0 ~/Desktop/testcapturecamerestream.dv #captures unlimited dv from specific camera

#examples capture tape dv/hdv in recorder mode
dvgrab -rewind #rewinds tape in 1st/all recorders 
dvgrab -f raw -s 0 ~/Destkop/tape1-shot- #captures dv from tape from 1st recorder
dvgrab -f raw -s 0 -a ~/Destkop/tape1-shot- #captures dv from tape with 'shot/scene detection' from 1st recorder
dvgrab -f hdv -s 0 ~/Destkop/tape1-shot- #captures 'hdv' from tape from 1st recorder
dvgrab -g typecameraGUIDhere -f hdv -s 0 ~/Destkop/tape1-shot- #captures hdv from tape from specific recorder

#example convert movie file container type
cat rawdvexamplefile.dv | dvgrab --stdin -noavc -s 0 -f dv2 rawdvexamplefiletype2.avi #converts raw dv to type2 avi container
cat rawdvexamplefile.dv | dvgrab --stdin -noavc -f dv1 rawdvexamplefiletype1.avi #why??? limited compatibility/filesize

#play dv stream in mediaplayer
dvgrab -noavc - | vlc - #plays dv stream instantly in vlc
dvgrab -noavc - | mplayer -nocache - #less delay than vlc?
```

* dvpause (interactive playback tool with frame accurate seek to feed offline raw.dv files into dvswitch mixer) 
  https://github.com/Jeija/dvpause
```
#to compile on ubuntu 16.04
sudo apt-get install libgtk-3-dev libcairo2-dev #ontop of packages to compile dvswitch!!!

cd mygitprojects
git clone https://github.com/walterav1984/dvpause
cd dvpause

make

./main | dvsource-file - -p 1234 -h 127.0.0.1
#only works if dvswitch sub process 'dvsource-file' with "stdin option" was installed see dvswitch!

chmod +x dvpause-full-playlist.sh
./dvpause-full-playlist.sh #edit this file for playlist item / dvswitch location!
```
  
* test-dv (sent raw dv/hdv video from PC back to FireWire/iLink recorder/camcorder for tape or instance S-video/SDI/HDMI monitor)  
  https://www.kernel.org/pub/linux/libs/ieee1394/libiec61883-1.2.0.tar.gz
```
#to compile on ubuntu 16.04
sudo apt-get install libraw1394-dev libiec61883-dev libdv-bin libiec61883-0
./configure
make
cd examples
./test-dv filename-raw-video.dv #outputs dv file/stream back to iLink/FireWire recorder tape/monitor
./test-mpeg2 filename-mpeg2hdv.mts #untested

#install if all is working right!
sudo cp .libs/test-dv /usr/local/bin/ #installs test-dv command to make it global available (needs libiec61883-0 installed)

dvsink-command -p 1234 -h 127.0.0.1 test-dv - #dvswitch live mixer outputs stream back to iLink/FireWire recorder tape/monitor!
```

* ffmpeg
  see repository
```
TODO add examples howto:
*trim video in/out start stop mlt/kino?
#http://superuser.com/questions/459313/how-to-cut-at-exact-frames-using-ffmpeg

*still repeats to pal-dv?
ffmpeg -loop 1 -i still.jpg -shortest -target pal-dv still.dv #?

*still sequence to pal-dv?

*convert any videotype to a raw DV 
ffmpeg -i dvd_vts_example.vob -vf "fieldorder=bff" -target pal-dv -aspect 16:9 output.dv #upperfield source to lowerfield dv (48Khz only)!

*stream to youtube
dvsink-command -p 1234 -h 127.0.0.1 'ffmpeg -re -i pipe:0  -acodec libmp3lame  -ar 44100 -b:a 128k -pix_fmt yuv420p -profile:v baseline -s 854x480 -bufsize 2048k -vb 1300k -maxrate 2000k -deinterlace -vcodec libx264 -preset medium -g 25 -r 25 -f flv "rtmp://a.rtmp.youtube.com/live2/yourcodehere"

*speedup 23,976p / 24p to 25 pal (eliminate judder) and letterbox non standard aspect ratio to max width
#https://ffmpeg.org/pipermail/ffmpeg-user/2013-February/013406.html (inspired by Bernd Butscheidt)
ffmpeg -i tears_of_steel_720p.mov -vf 'scale=1024:-1,pad=1024:576:(ow-iw)/2:(oh-ih)/2',setpts='24000/25000*(PTS-STARTPTS)' -sws_flags lanczos -af asetpts='24000/25000*(PTS-STARTPTS)',aresample=48000:async=1:min_comp=0.01:comp_duration=1:max_soft_comp=100000000:min_hard_comp=0.3 -r 25 -target pal-dv -aspect 16:9 dvtearsasetptswide.dv
#quick/dirty speedup audio out of sync possible depending on source?
ffmpeg -r 25 -i sintel-1280-surround.mp4 -vf 'scale=1024:-1,pad=1024:576:(ow-iw)/2:(oh-ih)/2' -sws_flags lanczos -af atempo=1.04166666667 -r 25 -target pal-dv -aspect 16:9 dvsint.dv 

*720p50/1920p50 to pal-dv
#https://forum.kde.org/viewtopic.php?f=272&t=129152
ffmpeg -i source -flags +ildct+ilme -vf interlace=lowpass=1:scan=bff #test (Inapickle post#3)
ffmpeg -i source -vf "tinterlace=5:flags=vlpf" #test (Inapickle post#3)
#http://forum.videohelp.com/threads/357744-Encoding-interlaced-with-Mencoder-or-ffmpeg 
ffmpeg -i 50p_input.avi -aspect 16:9 -vf scale=720:576,colormatrix=bt709:bt601,interlace #test ( poisondeathray post#6)
#https://ffmpeg.org/pipermail/ffmpeg-user/2014-March/020589.html #see next mpv
#https://ffmpeg.org/pipermail/ffmpeg-user/2012-January/004485.html

*1920p25 to pal-dv

*1920i50/1920i25 to pal-dv
ffmpeg -i input.mts -aspect 16:9 -vf scale=720:576:interl=1,colormatrix=bt709:bt601 #test ( poisondeathray post#6)

*other 29.976/30 59/60 ntsc based material to pal-dv

```

* mlt/melt
  see repository
  or "shotcut" for newest included melt builds https://www.shotcut.org/
  https://www.mltframework.org/
```
TODO add examples howto
**dvsink-command pipe Decklink SDI outpu add overlay png/tga watermark
dvsink-command -p 1234 -h 127.0.0.1 dv2melt-logo-sdi-script.sh 
dv2melt-logo-sdi-script.sh #contents below
melt -profile dv_pal_wide pipe: -consumer decklink -filter watermark:~/logo-anamorph.png distort=1

*fix chroma upsameling error interlaced artifacts yuv420p > yuv422p files only...
ffmpeg -i example420interlaced.dv -vf scale=interl=1 -vcodec prores -profile:v 2 output.mov #correclty upscales mpeg2/dv to 10bit 422 yuv interlaced proress for playback in melt to declink sd
melt -profile dv_pal_wide examplevideo.dv -filter swscale:interl=1 #melt equevalent of chroma upscaling error??? error Cannot allocate memory.

*melt firewire input specify dvguid option?
melt iec61883:auto #works only with single firewire device per firewire card auto is firewire card number
```
