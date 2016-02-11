echo "LD_LIBRARY_PATH must be properly set... please check"
echo "LD_LIBRARY_PATH currently set to $LD_LIBRARY_PATH:/usr/lib64/"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64/
./esgf-dashboard-ip 
