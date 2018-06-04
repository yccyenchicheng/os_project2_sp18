#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/net.h>
#include <net/sock.h>
#include <asm/processor.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
// #include <linux/uaccess.h>    for the copy_to_user function
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <linux/mm.h>
#include <asm/page.h>


#ifndef VM_RESERVED
#define VM_RESERVED   (VM_DONTEXPAND | VM_DONTDUMP)
#endif

#define slave_IOCTL_CREATESOCK 0x12345677
#define slave_IOCTL_MMAP 0x12345678
#define slave_IOCTL_EXIT 0x12345679

#define DEVICE_NAME "slavechar"
#define CLASS_NAME "slave"

#ifndef BUF_SIZE
#define BUF_SIZE 512
#endif

MODULE_LICENSE("GPL");

static int majorNumber;
//static char message[BUF_SIZE] = {0};
//static int short size_of_message;
static int numberOpens = 0;
static struct class* slavecharClass = NULL;
static struct device* slavecharDevice = NULL;


struct dentry  *file1;//debug file

typedef struct socket * ksocket_t;

//functions about kscoket are exported,and thus we use extern here
extern ksocket_t ksocket(int domain, int type, int protocol);
extern int kconnect(ksocket_t socket, struct sockaddr *address, int address_len);
extern ssize_t krecv(ksocket_t socket, void *buffer, size_t length, int flags);
extern int kclose(ksocket_t socket);
extern unsigned int inet_addr(char* ip);
extern char *inet_ntoa(struct in_addr *in); //DO NOT forget to kfree the return pointer

static int __init slave_init(void);
static void __exit slave_exit(void);

static int slave_open(struct inode *inode, struct file *filp);
static int slave_release(struct inode *inode, struct file *filp);
//int receive_msg(struct file *filp, char *buf, size_t count, loff_t *offp );
static ssize_t slave_read(struct file *filp, char *buf, size_t count, loff_t *offp );
static long slave_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);

static mm_segment_t old_fs;
static ksocket_t sockfd_cli;//socket to the master server
static struct sockaddr_in addr_srv; //address of the master server

//file operations
static struct file_operations slave_fops = 
{
	.owner = THIS_MODULE,
	.unlocked_ioctl = slave_ioctl,
	.open = slave_open,
	.read = slave_read,
	.release = slave_release
};

//device info
//static struct miscdevice slave_dev = {
//	.minor = MISC_DYNAMIC_MINOR,
//	.name = "slave_device",
//	.fops = &slave_fops
//};

static int __init slave_init(void)
{
    printk(KERN_INFO "slave device: Initializing the slave char device\n");

    // Try to dynamically allocate a major number for the device -- more difficult but worth it
    //majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    majorNumber = register_chrdev(0, DEVICE_NAME, &slave_fops);
    if (majorNumber<0) {
        printk(KERN_ALERT "slave device: failed to register a major number\n");
        return majorNumber;
    }
    
    printk(KERN_INFO "slave device: registered correctly with major number %d\n", majorNumber);

    // Register the device class
    slavecharClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(slavecharClass)) {                // Check for error and clean up if there is
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(slavecharClass);          // Correct way to return an error on a pointer
    }
    printk(KERN_INFO "slave device: device class registered correctly\n");

    // Register the device driver
    slavecharDevice = device_create(slavecharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(slavecharDevice)) {               // Clean up if there is an error
        class_destroy(slavecharClass);           // Repeated code but the alternative is goto statements
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(slavecharDevice);
    }
    printk(KERN_INFO "slave device: device class created correctly\n"); // Made it! device was initialized
    return 0;
}

static void __exit slave_exit(void)
{
    device_destroy(slavecharClass, MKDEV(majorNumber, 0));     // remove the device
    class_unregister(slavecharClass);                          // unregister the device class
    class_destroy(slavecharClass);                             // remove the device class
    unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
    printk(KERN_INFO "slave device exited!\n");
	//misc_deregister(&slave_dev);
	//printk(KERN_INFO "slave exited!\n");
	//debugfs_remove(file1);
}


//int slave_open(struct inode *inode, struct file *filp)
static int slave_open(struct inode *inode, struct file *filp)
{
    numberOpens++;
    printk(KERN_INFO "slave device: Device has been opened %d time(s)\n", numberOpens);
    return 0;
}

//int slave_read(struct file *filp, char *buf, size_t count, loff_t *offp )
static ssize_t slave_read(struct file *filp, char *buf, size_t count, loff_t *offp )
{
    //call when user is reading from this device
    int error_count = 0;
    //len = krecv(sockfd_cli, msg, sizeof(msg), 0);
    char message[BUF_SIZE];
    size_t len;
    len = krecv(sockfd_cli, message, sizeof(message), 0);
    
    error_count = copy_to_user(buf, message, len);
    if (error_count == 0) {
        printk(KERN_INFO "slave device: Sent %d characters to the user\n", len);
        return len;  // clear the position to the start and return 0
    } else {
        printk(KERN_INFO "slave device: Failed to sent %d characters to the user\n", error_count);
        return -EFAULT;
    }
}


//long slave_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
static long slave_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
	long ret = -EINVAL;

	int addr_len ;
	unsigned int i;
	size_t len, data_size = 0;
	char *tmp, ip[20], buf[BUF_SIZE];
	struct page *p_print;
	unsigned char *px;

        pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
        pte_t *ptep, pte;
	old_fs = get_fs();
	set_fs(KERNEL_DS);


	switch(ioctl_num){
		case slave_IOCTL_CREATESOCK:// create socket and connect to master

			if(copy_from_user(ip, (char*)ioctl_param, sizeof(ip)))
				return -ENOMEM;

			sprintf(current->comm, "ksktcli");

			memset(&addr_srv, 0, sizeof(addr_srv));
			addr_srv.sin_family = AF_INET;
			addr_srv.sin_port = htons(2325);
			addr_srv.sin_addr.s_addr = inet_addr(ip);
			addr_len = sizeof(struct sockaddr_in);

			sockfd_cli = ksocket(AF_INET, SOCK_STREAM, 0);
			printk("sockfd_cli = 0x%p  socket is created\n", sockfd_cli);
			if (sockfd_cli == NULL)
			{
				printk("socket failed\n");
				return -1;
			}
			if (kconnect(sockfd_cli, (struct sockaddr*)&addr_srv, addr_len) < 0)
			{
				printk("connect failed\n");
				return -1;
			}
			tmp = inet_ntoa(&addr_srv.sin_addr);
			printk("connected to : %s %d\n", tmp, ntohs(addr_srv.sin_port));
			kfree(tmp);
			ret = 0;
			break;
		case slave_IOCTL_MMAP:

			break;

		case slave_IOCTL_EXIT:
			if(kclose(sockfd_cli) == -1)
			{
				printk("kclose cli error\n");
				return -1;
			}
			set_fs(old_fs);
			ret = 0;
			break;
		default:
                        pgd = pgd_offset(current->mm, ioctl_param);
			pud = pud_offset(pgd, ioctl_param);
			pmd = pmd_offset(pud, ioctl_param);
			ptep = pte_offset_kernel(pmd , ioctl_param);
			pte = *ptep;
			printk("slave: %lX\n", pte);
			ret = 0;
			break;
	}

	return ret;
}

int slave_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "slave device: Device closed\n");
    return 0;
}

module_init(slave_init);
module_exit(slave_exit);
