cd ./ksocket
sudo make
sudo insmod ksocket.ko
cd ../master_device
sudo make
sudo insmod master_device.ko
cd ../slave_device
sudo make
sudo insmod slave_device.ko
cd ../user_program
sudo make


