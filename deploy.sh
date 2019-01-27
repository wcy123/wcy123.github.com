#!/bin/bash
set -e
remote=`git remote -v show | head -n1 | awk '{print $2}'`
cd deploy
[ -d .git ] && rm -fr .git
git init
git remote add origin $remote
git add .
git commit -m 'deploy'
git push --force origin master
