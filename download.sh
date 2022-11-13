#!/bin/bash

############################
##### CONFIGURATION ########
############################
VER=3.4.0            #VERSION
DIR=./cache          #CACHE DIRECTORY
INSTALL=./src/Eigen  #EIGEN INSTALL DIRECTION (DON'T TOUCH)

#####################
##### SCRIPT ########
#####################

##### INSTALL EIGEN 3 ########
mkdir $DIR
# LINUX
wget -O $DIR/eigen.tar.gz  https://gitlab.com/libeigen/eigen/-/archive/$VER/eigen-$VER.tar.gz

# MACOSX
# curl https://gitlab.com/libeigen/eigen/-/archive/$VER/eigen-$VER.tar.gz -o $DIR/eigen.tar.gz

tar xvf $DIR/eigen.tar.gz -C $DIR
cp -R $DIR/eigen-$VER/Eigen/* $INSTALL/


##### UNPACK DOCUMENTATION ########
unzip doc.zip
