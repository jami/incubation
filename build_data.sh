#!/bin/sh

cd data;
#zip -v ../data.zip `find . -type f -name "*.png" -or -name "*.cnf" | sed 's/\n/ /'`
tar -cvvzf ../data.tgz `find . -type f -name "*.png" -or -name "*.cnf" | sed 's/\n/ /'` 
