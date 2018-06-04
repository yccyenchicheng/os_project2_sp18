cd ./ksocket
make clean
sudo rmmod ksocket.ko
cd ../master_device
make clean
sudo rmmod master_device.ko
cd ../slave_device
make clean
sudo rmmod slave_device.ko
cd ../user_program
make clean
cd ..


