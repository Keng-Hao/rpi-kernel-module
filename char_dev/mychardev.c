#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>

#define DEVICE_NAME "mychardev"
#define BUF_LEN 128

MODULE_LICENSE("GPL");
MODULE_AUTHOR("you");
MODULE_DESCRIPTION("My First Char Device Driver");
MODULE_VERSION("0.1");

static int major;
static struct cdev my_cdev;
static char device_buffer[BUF_LEN] = "Hello from kernel!\n";

//open 
static int dev_open(struct inode *inode, struct file * file){
	printk(KERN_INFO "mychardev: device opened\n");
	return 0;
}

//read
static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
	int bytes_read = 0;
	if(*offset >= strlen(device_buffer)) return 0;
	bytes_read = strlen(device_buffer) - *offset;
	if(len < bytes_read) bytes_read = len;
	if(copy_to_user(buf, device_buffer + *offset, bytes_read))
		return -EFAULT;
	*offset += bytes_read;
	printk(KERN_INFO "mychardev: read %d bytes\n", bytes_read);
	return bytes_read;
}

//write
static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
	if( len >= BUF_LEN) len= BUF_LEN -1;
	if( copy_from_user(device_buffer,buf,len))
		return -EFAULT;
	device_buffer[len] = '\0';
	printk(KERN_INFO "mychardev : wrote %zu bytes\n" , len);
	return len;
}

//release
static int dev_release(struct inode *inode, struct file *file) {
	printk(KERN_INFO "mychardev: device closed\n");
	return 0;
}

// build manipuluated function table
static struct file_operations fops = {
	.owner= THIS_MODULE,
	.open = dev_open,
	.read = dev_read,
	.write= dev_write,
	.release = dev_release
};
// module init
static int __init mychardev_init(void) {
	dev_t dev;
	// 1. dynamic allocated major number
	if (alloc_chrdev_region(&dev, 0,1,DEVICE_NAME) < 0) {
		printk(KERN_ALERT "Failed to allocate major number \n");
		return -1 ;
	}

	major = MAJOR(dev);

	// 2. initialize cdev and blind
	cdev_init(&my_cdev, &fops);
	my_cdev.owner = THIS_MODULE;

	//3. register cdev to kernel 
	if(cdev_add(&my_cdev, dev, 1) < 0) {
		unregister_chrdev_region(dev,1);
		printk(KERN_ALERT "Fauled to add cdev\n");
		return -1;
	}

	printk(KERN_INFO "mychardev: registered with major number %d\n", major);
	return 0;
}

// module exit
static void __exit mychardev_exit(void) {
	cdev_del(&my_cdev);
	unregister_chrdev_region(MKDEV(major, 0),1);
	printk(KERN_INFO "mychardev: unregistered\n");
}

module_init(mychardev_init);
module_exit(mychardev_exit);
