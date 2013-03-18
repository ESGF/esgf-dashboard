export LD_LIBRARY_PATH=$LD_LIBRARY_PATH: 
valgrind --leak-check=full  --show-reachable=yes ./esgf-dashboard-ip
