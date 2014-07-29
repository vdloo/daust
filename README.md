daust
=====
*distributed are you still there*

Never lose track of a vm or docker container again. daust is
a decentralized phone home system in the form of a Unix daemon. 

## Quick start

Start a daustd instances on two or more machines
Imagine two machines with eth0 192.168.1.2 and 192.168.1.3, run
the following command on both.

```
$ daustd -d
```

Then on one of the machines connect to the other by sending any command.
For example: on the 192.168.1.2 host connect to the daustd instance on
the machine with 192.168.1.3.

```
$ daust remote 192.168.1.3 ping
```

To display the connected nodes to the local daustd run the list command

```
$ daust list
node1: internal ip 192.168.1.2, public ip N/A, last seen 0 seconds ago
node2: internal ip 192.168.1.3, public ip N/A, last seen 15 seconds ago
```

The commands can also be ran on other nodes by adding the 'remote' option. 
Options can both be prepended and appended to the command. Note the
position of list in the following example:

```
$ daust list remote 192.168.1.3
node2: internal ip 192.168.1.3, public ip N/A, last seen 0 seconds ago
node1: internal ip 192.168.1.2, public ip N/A, last seen 5 seconds ago
```

The 'all' option runs the command on all nodes. 

```
$ daust all list
node1 responded:
node1: internal ip 192.168.1.2, public ip N/A, last seen 0 seconds ago
node2: internal ip 192.168.1.3, public ip N/A, last seen 0 seconds ago

node2 responded:
node2: internal ip 192.168.1.3, public ip N/A, last seen 0 seconds ago
node1: internal ip 192.168.1.2, public ip N/A, last seen 0 seconds ago
```

The 'trace all' command displays a tree of the network.

```
$ daust trace all
|__ node1
|__ node2
```

On a more expansive network this might look something like this.

```
$ daust trace all
|__ hypervisor1
|__ hypervisor2
|   |__ vm 4
|   |__ vm 5
|   |__ vm 6
|__ vm 1
|__ vm 2
|__ vm 3
```

To stop the daemon on all nodes run:

```
$ daust all stop
```

## Test environment

To set up a test environment using the Vagrantfile

```
$ git clone https://github.com/vdloo/daust && cd daust && vagrant up
```

Then ssh into one of the machines

```
$ vagrant ssh node1
```

Inside the vm start a couple of docker containers running the daustd
daemon and connect them to daustd on the vm by running

```
$ . start.sh
```

And that's it, you're good to go. 
To test the cluster you could run:

```
$ daust ping all
```


## Installation

```
$ ./bootstrap && ./configure && make
```
