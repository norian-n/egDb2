#!/bin/bash
# rebuild
rm *.exe *.gdn *.tdf *.dnl *.fng *.ind all_tests.log
make

# exec all *.exe to log file
for i in *.exe
do 
  ./$i 2>&1 | tee -a all_tests.log 
done
echo

PASSED_COUNT=`cat all_tests.log | grep PASS | wc -w`	
echo PASS total: $PASSED_COUNT

FAILED_COUNT=`cat all_tests.log | grep FAIL | wc -w`
echo FAIL total: $FAILED_COUNT

