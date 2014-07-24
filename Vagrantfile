Vagrant.configure("2") do |config| 
	config.vm.box = "phusion-open-ubuntu-12.04-amd64"
	config.vm.box_url = "https://oss-binaries.phusionpassenger.com/vagrant/boxes/latest/ubuntu-12.04-amd64-vbox.box"
	config.vm.hostname = "daust-testenv"
	config.vm.network "private_network", ip: "192.168.50.2"
	config.vm.provision :shell, :inline => "sudo apt-get -y update && sudo apt-get -y install build-essential autoconf automake"
	config.vm.provision :shell, :inline => "curl -s https://get.docker.io/ubuntu/ | sudo sh"
	
	# run the daust daemon on the vagrant vm
	config.vm.provision :shell, :inline => "echo -e '#!/bin/bash\ncd /vagrant; ./bootstrap && ./configure; make uninstall; make clean; make && make install\ndocker build -t daust .\nsu vagrant -c \"daustd -d\"\nfor ((i=0; i < 10; i++)); do docker run -d daust; done' > rundaust.sh; chmod u+x rundaust.sh; ./rundaust.sh"
	# run the daust daemon in n docker containers
end
