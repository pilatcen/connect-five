# Simple C++ Qt4.8 Connect Five Game

![Connect five](/doc/connect-five.png)

### Features
* 2 players
* Network game
* simple Qt gui

## Dependencies

### Run Dependencies for Qt4.8
* libqtgui4
* libqtcore4
* libqt4-network

### Compilation Dependencies on Debian, Ubuntu (for Qt4, for Qt5 is it similar)
* qt4-qmake
* libqt4-dev
* build-essential

## Compilation
* make

## Run
* 2 players on one PC:
```
./piskvorky
```
* Network game:
   1. Start server - server-port is number of free TCP port
```
./piskvorky server server-port
```
   2. And then connect your client to it - server-ip is ip of your server and server-port is port number which your server is listening on:
```
./piskvorky client server-ip server-port 
```
