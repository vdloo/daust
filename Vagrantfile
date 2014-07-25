Vagrant.configure("2") do |config| 
	config.vm.box = "phusion-open-ubuntu-12.04-amd64"
	config.vm.box_url = "https://oss-binaries.phusionpassenger.com/vagrant/boxes/latest/ubuntu-12.04-amd64-vbox.box"
	config.vm.hostname = "daust-testenv"
	config.vm.network "private_network", ip: "192.168.50.2"
	config.vm.provision :shell, :inline => "sudo apt-get -y update && sudo apt-get -y install build-essential autoconf automake"
	config.vm.provision :shell, :inline => "sudo apt-get -y install gdb valgrind"
	config.vm.provision :shell, :inline => "curl -s https://get.docker.io/ubuntu/ | sudo sh"
	
	# define scripts for building from shared folder
	config.vm.provision :shell, :inline => "echo -e '#!/bin/bash\ncd /vagrant; ./bootstrap && ./configure; sudo make uninstall; make clean; make && sudo make install' > build_bin.sh; chmod u+x build_bin.sh; chmod 777 build_bin.sh;"
	config.vm.provision :shell, :inline => "echo -e '#!/bin/bash\ncd /vagrant; sudo docker build -t daust .' > build_container.sh; chmod u+x build_container.sh; chmod 777 build_container.sh;"
	config.vm.provision :shell, :inline => "echo -e '#!/bin/bash\n(echo r; cat) | gdb --args daustd -v' > run_bin_gdb.sh; chmod u+x run_bin_gdb.sh; chmod 777 run_bin_gdb.sh;"
	config.vm.provision :shell, :inline => "echo -e '#!/bin/bash\ndaustd -v' > run_bin.sh; chmod u+x run_bin.sh; chmod 777 run_bin.sh;"
	config.vm.provision :shell, :inline => "echo -e '#!/bin/bash\nvalgrind --leak-check=full --track-origins=yes daustd -v' > run_bin_valgrind.sh; chmod u+x run_bin_valgrind.sh; chmod 777 run_bin_valgrind.sh;"
	config.vm.provision :shell, :inline => "echo -e '#!/bin/bash\nfor ((i=0; i < 10; i++)); do sudo docker run -d daust; done' > run_container.sh; chmod u+x run_container.sh; chmod 777 run_container.sh;"

	# scripts for starting the program normally, with gdb or with vagrand. don't run as shell script, instead source the files (. start) so that job control is available
	config.vm.provision :shell, :inline => "echo -e '#!/bin/bash\nsudo killall daustd; ./build_bin.sh; ./build_container.sh; ./run_bin.sh& ./run_container.sh; fg' > start.sh"
	config.vm.provision :shell, :inline => "echo -e '#!/bin/bash\nsudo killall daustd; ./build_bin.sh; ./build_container.sh; ./run_bin_gdb.sh& ./run_container.sh; fg' > start_gdb.sh"
	config.vm.provision :shell, :inline => "echo -e '#!/bin/bash\nsudo killall daustd; ./build_bin.sh; ./build_container.sh; ./run_bin_valgrind.sh& ./run_container.sh; fg' > start_valgrind.sh"
	config.vm.provision :shell, :inline => "echo -e 'start script to start the program on the vm and in docker containers,\nstart_gdb does the same but runs the non-docker instance in gdb etc.\ndo not run as shell scripts, instead source the files (. start)' > README"

end
