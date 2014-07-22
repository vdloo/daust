Vagrant.configure("2") do |config|
	config.vm.box = "precise64"
	config.vm.box_url = 'http://files.vagrantup.com/precise64.box'

	config.vm.define "node1" do |node1|
		node1.vm.hostname = "daust-node1"
		node1.vm.network "private_network", ip: "192.168.50.2"
		node1.vm.provision :shell, :inline => "sudo apt-get -y update && sudo apt-get -y install build-essential autoconf automake"
		node1.vm.provision :shell, :inline => "cd /vagrant; ./bootstrap && ./configure && make && make install && su vagrant -c \"daustd -d\""
	end
	config.vm.define "node2" do |node2|
		node2.vm.hostname = "daust-node2"
		node2.vm.network "private_network", ip: "192.168.50.3"
		node2.vm.provision :shell, :inline => "sudo apt-get -y update && sudo apt-get -y install make"
		node2.vm.provision :shell, :inline => "cd /vagrant && sudo make install && su vagrant -c \"daustd -d; daust remote 192.168.50.2 ping\""
	end
	config.vm.define "node3" do |node3|
		node3.vm.hostname = "daust-node3"
		node3.vm.network "private_network", ip: "192.168.50.4"
		node3.vm.provision :shell, :inline => "sudo apt-get -y update && sudo apt-get -y install make"
		node3.vm.provision :shell, :inline => "cd /vagrant && sudo make install && su vagrant -c \"daustd -d; daust remote 192.168.50.2 ping\""
	end
end
