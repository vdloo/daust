FROM ubuntu:12.04

copy src/daustd /
copy src/daust /
CMD ./daustd -d && sleep 1 && ./daust remote 10.0.2.15 ping && while :; do echo "fake test process"; sleep 5; done
