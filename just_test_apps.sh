#!/bin/bash

./clean.sh

# rebuild
echo ============ build test apps only ============

make -f Makefile.tests

echo ============ run test apps ============

cd tests
# exec all *.exe to log file
for i in *.exe
do 
  LD_LIBRARY_PATH=~/egdb/egDb2 ./$i 2>&1 | tee -a all_tests.log 
done
echo

cd ..
mv tests/all_tests.log ./

PASSED_COUNT=`cat all_tests.log | grep PASS | wc -w`	
echo PASS total: $PASSED_COUNT

FAILED_COUNT=`cat all_tests.log | grep FAIL | wc -w`
echo FAIL total: $FAILED_COUNT

