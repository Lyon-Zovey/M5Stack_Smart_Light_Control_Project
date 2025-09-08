# M5Stack Smart Light Control Project

<img src="https://m5stack-doc.oss-cn-shenzhen.aliyuncs.com/471/Build_a_Smart_Light_Control.jpg" width="20%"/> <img src="https://static-cdn.m5stack.com/resource/docs/products/unit/finger/finger_07.webp" width="20%"/>

## Description

Ever come to the office in the morning… only to find the lights have been on all night?
Our lights aren’t smart bulbs. No smart switch. No remote control.
If you want them off – you literally have to push the wall switch by hand.

So – we built our own Smart Light Control ⚡

Powered by AtomS3-Lite core controller
Paired with Atomic ABC Base to drive 5 servos
Unit TimerPWR for timed power cycles & long battery life
Custom 3D-printed mount, designed to fit multi-gang switches & still allow manual control
Connected to EzData Cloud Service for weekly remote on/off scheduling
Every day, it wakes → syncs schedule → flips the switch → reports battery → sleeps again.
Just a few parts, some code, and now… any regular light can be “smart”.

## Project Structure 

```
├── lib
│ ├── Config_parse -> The configuration analysis of the switch on and off light schedule
│ ├── M5Unit-TimerPWR -> Driver library for TimerPWR
│ ├── NTP_server -> Used to configure the NTP service
│ ├── NVS_save -> Store the configuration in NVS
│ ├── Network_manager -> Network related configuration
│ └── Servo -> Servo drive library
├── src
│ └── main.cpp -> Main function logic
```

