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

#define BUF_SIZE 512
#define MAP_SIZE PAGE_SIZE * 100

typedef struct socket * ksocket_t;

// According to the sample code, functions about kscoket are exported, so we should use extern here
extern ksocket_t ksocket(int domain, int type, int protocol);
extern int kconnect(ksocket_t socket, struct sockaddr *address, int address_len);
extern ssize_t krecv(ksocket_t socket, void *buffer, size_t length, int flags);
extern int kclose(ksocket_t socket);
extern unsigned int inet_addr(char* ip);
extern char *inet_ntoa(struct in_addr *in); // According to the sample code, remember to kfree the return pointer

static int __init slave_init(void);
static void __exit slave_exit(void);

// for slave_device's file operations
int slave_open(struct inode *inode, struct file *file_p);
int slave_close(struct inode *inode, struct file *file_p);
static long slave_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);
int slave_read(struct file *file_p, char *buf, size_t count, loff_t *offp );

static mm_segment_t old_fs;
static ksocket_t sockfd_cli; // socket to the master
static struct sockaddr_in addr_srv; //address of the master

// the following part of mmap implementation is based on the sample code
void mmap_open(struct vm_area_struct *vma) {
	/* Do nothing */
}

void mmap_close(struct vm_area_struct *vma) {
	/* Do nothing */
}

static int mmap_fault(struct vm_area_struct *vma, struct vm_fault *vmf) {
	vmf->page = virt_to_page(vma->vm_private_data);
	get_page(vmf->page);
	return 0;
}

static const struct vm_operations_struct my_vm_ops = {
	.open = mmap_open,
	.close = mmap_close,
	.fault = mmap_fault
};

static int my_mmap(struct file *file, struct vm_area_struct *vma) {
	io_remap_pfn_range(vma,
		vma->vm_start,
		virt_to_phys(file->private_data) >> PAGE_SHIFT,
                vma->vm_end - vma->vm_start,
		vma->vm_page_prot);
	vma->vm_ops = &my_vm_ops;
	vma->vm_flags |= VM_RESERVED;
	vma->vm_private_data = file->private_data;
	mmap_open(vma);
	return 0;
}

// file operations for slave_device
static struct file_operations slave_fops = {
	.owner = THIS_MODULE,
	.open = slave_open,
	.release = slave_close,
	.unlocked_ioctl = slave_ioctl,
	.read = slave_read,
	.mmap = my_mmap
};

// the information for slave_device
static struct miscdevice slave_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "slave_device",
	.fops = &slave_fops
};

static int __init slave_init(void) {
	int ret;

	// register the slave_device
	if( (ret = misc_register(&slave_dev)) < 0){
		printk(KERN_ERR "misc_register failed!\n");
		return ret;
	}

	printk(KERN_INFO "slave_device has been registered!\n");

	return 0;
}

static void __exit slave_exit(void) {
	misc_deregister(&slave_dev);
	printk(KERN_INFO "slave_device exited!\n");
}

int slave_open(struct inode *inode, struct file *file_p) {
	file_p->private_data = kmalloc(MAP_SIZE, GFP_KERNEL);
	return 0;
}

int slave_close(struct inode *inode, struct file *file_p) {
	kfree(file_p->private_data);
	return 0;
}

static long slave_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param) {
	long ret = -EINVAL;

	int addr_len;
	size_t offset = 0, rec_n;
	char *tmp, ip[20], buf[BUF_SIZE];

	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *ptep, pte;
	old_fs = get_fs();
	set_fs(KERNEL_DS);

	switch(ioctl_num){
		case slave_IOCTL_CREATESOCK: // create the socket and connect to master_device

			if(copy_from_user(ip, (char*)ioctl_param, sizeof(ip)))
				return -ENOMEM;

			sprintf(current->comm, "ksktcli");

			memset(&addr_srv, 0, sizeof(addr_srv));
			addr_srv.sin_family = AF_INET;
			addr_srv.sin_port = htons(2325);
			addr_srv.sin_addr.s_addr = inet_addr(ip);
			addr_len = sizeof(struct sockaddr_in);

			sockfd_cli = ksocket(AF_INET, SOCK_STREAM, 0);
			printk("sockfd_cli = 0x%p, socket has been created\n", sockfd_cli);

			if (sockfd_cli == NULL) {
				printk("socket failed\n");
				return -1;
			}

			if (kconnect(sockfd_cli, (struct sockaddr*)&addr_srv, addr_len) < 0) {
				printk("connect failed\n");
				return -1;
			}

			tmp = inet_ntoa(&addr_srv.sin_addr);
			printk("connected to : %s %d\n", tmp, ntohs(addr_srv.sin_port));
			kfree(tmp);
			ret = 0;
			
			break;

		case slave_IOCTL_MMAP:
			while (1) {
				rec_n = krecv(sockfd_cli, buf, sizeof(buf), 0);
				if (rec_n == 0) {
					break;
				}

				memcpy(file->private_data + offset, buf, rec_n);
				offset += rec_n;
			}

			ret = offset;
			break;

		case slave_IOCTL_EXIT:
			if (kclose(sockfd_cli) == -1) {
				printk("error: kclose cli failed.\n");
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
			// show the page descriptors of the mapped memory region
			//printk("page descriptors of slave_device: %lX\n", pte);
			printk("%lX\n", pte);
			ret = 0;
			break;
	}

	return ret;
}

//to read from device, user may call slave_read
int slave_read(struct file *file_p, char *buf, size_t count, loff_t *offp ) {
	char msg[BUF_SIZE];
	size_t len;
	len = krecv(sockfd_cli, msg, sizeof(msg), 0);
	if(copy_to_user(buf, msg, len))
		return -ENOMEM;
	return len;
}

module_init(slave_init);
module_exit(slave_exit);
MODULE_LICENSE("GPL");
