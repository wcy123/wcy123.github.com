#!/bin/bash
set -e
export GIT_DIR=$PWD/.git
tmp_deploy=/tmp/deply_$USER
mkdir -p $tmp_deploy
cp -a deploy $tmp_deploy
cd $tmp_deploy/deploy
# git pull
git reset --soft v1.0
git add .
git commit -m 'deploy'
git push --force -u origin master
