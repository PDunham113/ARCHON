# -*- mode: ruby -*-
# vi: set ft=ruby :


################################################################################
# Custom configurations
################################################################################

# Define Vagrantfile locations
ex_vagrantfile = File.expand_path("./Vagrantfile.local.example", __dir__)
local_vagrantfile = File.expand_path("./Vagrantfile.local", __dir__)
project_vagrantfile = File.expand_path("./Vagrantfile.project", __dir__)

# Load project configurations.
load project_vagrantfile if File.exists?(project_vagrantfile)

# Load local configurations. If they don't exist, copy the example Vagrantfile.
if !File.exists?(local_vagrantfile)
  print "Creating local Vagrantfile...\n"
  FileUtils.cp(ex_vagrantfile, local_vagrantfile)
end
load local_vagrantfile


################################################################################
# Provisioning Scripts
################################################################################

# Use the current Git SHA1 as the VM version number.
$vm_version = `git rev-parse HEAD`

# Configure the VM to use the proxy used by the host machine. Configuration is
# done at the system level (vs per-user).
$configure_proxy = <<EOT
if [ "#{$http_proxy}" == "" ]; then
    exit 0
fi

echo "Configuring proxy as '#{$http_proxy}'."

export http_proxy="#{$http_proxy}"
export https_proxy="$http_proxy"

sed -i -e '/https?_proxy *=/d' /etc/environment
echo "http_proxy=$http_proxy" >> /etc/environment
echo "https_proxy=$https_proxy" >> /etc/environment

sed -i -e '/env_keep =/d' /etc/sudoers
echo "Defaults env_keep = \\"http_proxy https_proxy ftp_proxy\\"" >> \
/etc/sudoers

if [ -f /etc/apt/apt.conf ]; then
    sed -i -e '/Acquire::http::Proxy/d' /etc/apt/apt.conf
fi
echo "Acquire::http::Proxy \\"$http_proxy\\";" >> /etc/apt/apt.conf
EOT

# Install required tools.
$install_apt_packages = <<EOT
echo "Installing required apt packages."

apt-get update
apt-get install -y git linux-image-extra-virtual \
                   #{$project_apt_packages} #{$local_apt_packages}

if [ "#{$enable_gui_mode}" = "true" ]; then
    # Note that the following lines are necessary to resolve errors that come
    # up when installing ubuntu-desktop below (in Vagrant only).
    /usr/share/debconf/fix_db.pl
    dpkg-reconfigure dictionaries-common

    apt-get install -y ubuntu-desktop
fi
EOT

$install_pip_packages = <<EOT
echo "Installing required pip packages."

yes | pip3 install #{$project_pip_packages} #{$local_pip_packages}
EOT


################################################################################
# VM Configuration
################################################################################

# All Vagrant configuration is done below. The "2" in Vagrant.configure
# configures the configuration version (we support older styles for
# backwards compatibility). Please don't change it unless you know what
# you're doing.
Vagrant.configure("2") do |config|
  # Every Vagrant development environment requires a box. You can search for
  # boxes at https://vagrantcloud.com/search.
  config.vm.box = "bento/ubuntu-18.04"

  # Use local setting for insecure box downloads.
  config.vm.box_download_insecure = $download_insecure

  # Configure the VM network settings.
  if $network_bridged
    if $network_ip == ""
      config.vm.network "public_network"
    else
      config.vm.network "public_network", ip: "#{$network_ip}"
    end
  else
    # NAT settings (e.g., port forwarding) would go here.
  end

  # Share an additional folder to the guest VM. The first argument is
  # the path on the host to the actual folder. The second argument is
  # the path on the guest to mount the folder. And the optional third
  # argument is a set of non-required options.
  config.vm.synced_folder "./", "/home/vagrant/#{$project_name}/"

  # VirtualBox Config
  config.vm.provider "virtualbox" do |vb|
    # Display the VM name in the VirtualBox GUI
    vb.name = $vm_name

    # Display the VirtualBox GUI when booting the machine.
    vb.gui = $enable_gui_mode

    # Specify number of cores for VM.
    vb.cpus = $num_cpus

    # Specify amount of memory for VM [MB].
    vb.memory = $memory_size

    # Configure USB settings.
    vb.customize ["modifyvm", :id, "--usb", "on"]
    vb.customize ["modifyvm", :id, "--usbehci", "on"]

    # Add filter for AVR Dragon.
    vb.customize ["usbfilter", "add", "0", "--target", :id,
                 "--name", "ATMEL AVRDRAGON",
                 "--manufacturer", "ATMEL", "--product", "AVRDRAGON"]

    # Add filter for USB to RS232 converters.
    vb.customize ["usbfilter", "add", "0", "--target", :id,
                 "--name", "FTDI TTL232R-3V3",
                 "--manufacturer", "FTDI", "--product", "TTL232R-3V3"]
  end

  # Configure machine hostname.
  config.vm.hostname = $hostname

  # Enable X11 forwarding.
  config.ssh.forward_x11 = true
  config.ssh.forward_agent = true

  # Provision the VM.
  if $ssh_public_key != ""
    config.vm.provision "file",
                        source: "#{$ssh_key_location}/#{$ssh_public_key}",
                        destination: "~/.ssh/#{$ssh_public_key}"
  end

  for file in $files
    if File.exists?(file)
      config.vm.provision "file", source: file, destination: file
    end
  end

  if $http_proxy != ""
    config.vm.provision "shell", inline: $configure_proxy
  end

  # Configure custom system-level settings.
  config.vm.provision "shell", inline: $configure_project_settings
  config.vm.provision "shell", inline: $configure_local_settings

  # Install dependencies.
  config.vm.provision "shell", inline: $install_apt_packages
  config.vm.provision "shell", inline: $install_pip_packages

  # Configure installed tools.
  config.vm.provision "shell", inline: $configure_project_tools
  config.vm.provision "shell", inline: $configure_local_tools,
                      privileged: false

  # Store the version of the Vagrant configuration used to provision the VM.
  config.vm.provision "shell",
                      inline: "echo -n \"#{$vm_version}\" > \
                               /home/.vm_version"

end
