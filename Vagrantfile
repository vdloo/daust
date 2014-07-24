Vagrant.configure("2") do |config| 
	config.vm.box = "phusion-open-ubuntu-12.04-amd64"
	config.vm.box_url = "https://oss-binaries.phusionpassenger.com/vagrant/boxes/latest/ubuntu-12.04-amd64-vbox.box"
	config.vm.hostname = "daust-testenv"
	config.vm.network "private_network", ip: "192.168.50.2"
	config.vm.provision :shell, :inline => "sudo apt-get -y update && sudo apt-get -y install build-essential autoconf automake"
	config.vm.provision :shell, :inline => "cd /vagrant; ./bootstrap && ./configure && make && make install"
	config.vm.provision :shell, :inline => "curl -s https://get.docker.io/ubuntu/ | sudo sh"
	config.vm.provision :shell, :inline => "cd /vagrant; docker build -t daust ."
	
	# run the daust daemon on the vagrant vm
	config.vm.provision :shell, :inline => "su vagrant -c \"daustd -d\""
	# run the daust daemon in n docker containers
	config.vm.provision :shell, :inline => "for ((i=0; i < 10; i++)); do docker run -d daust; done"
end
