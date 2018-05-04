# QTGCS

My previous repository pyGCS was temporarily paused for technical difficulties. It may crash or get stuck when control a quadcopter fly, which is very dangerous in real flight. There might be a lot of bugs in the scripts. I will try to figure it out in the future when I get time.

This repository QTGCS is written with C++ and QT. Currently it is quite stable and fast compared to pyGCS. It is still under construction and I will continue focusing on this repository and try to finish it.

# Introduction:

QTGCS is an open source project for multiple quadcopter ground control station mainly written in C++, which will communicate with multiple quadcopters to exchange real time data or mission or other data fields.

On the quadcopters, flight controllers with iNav firmware are used, while XBee is the only communication way for the whole flight process. By now, the connection structure is star structure, but XBees have the capability to form mesh or other structures.

Currently, this project is still under construction and the focus point will be successfully running on macOS, iOS systems. But it has the possibility to run on Linux or Windows systems in the future. 

This work is part of my research project. And this ground control station is meant to be working with my own AutonomousFlight firmware for the best performance. Most of the features are still pending to repeatable verification.

# Features already implemented:
    1. Upload missions
    2. Check multiple quadcopter status
    3. Load Google Static Maps
    4. Mouse click on map to create waypoints
    5. Edit waypoints by dragging points on the map
    6. Load missions from file
    7. Save missions to file
    8. iPhone Joystick App for single quadcopter control
    9. iPad GCS App mainly for quadcopter position monitoring

# Features to be implemented:
    1. More features

# Sample:
Screenshot: 
![alt text](https://github.com/liutairan/QTGCS/blob/dev/Pics/Snip20171011_13.png "Sample image")

![alt text](https://github.com/liutairan/QTGCS/blob/dev/Pics/Snip20170426_3.png "Sample image")

# Related applications:
    1. QTGCS_Configurator: configure environment for QTGCS.
    2. QTGCS_MapManager: local map image management, pre-download map images, update new map images, etc.
    3. AutonomousFlight: dialected flight control firmware works with QTGCS.
