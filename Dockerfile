FROM ubuntu:12.04

copy src/daustd /
copy src/daust /
CMD (sleep 5; ./daust remote 10.0.2.15 ping)& ./daustd -v
