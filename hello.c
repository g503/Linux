#include<linux/module.h>
#include<linux/device.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
//#include<asm/errno.h>

#define DEVICE_NAME "helloDevName"
#define CLASS_NAME "helloClassName"

volatile static int is_open = 0;

static char message[1024];
static char number[10];
static int num_bytes = 0;
static int majorNumber = 0;
static int readCount = 0;
static struct class* helloCharClass = NULL;
static struct device* helloCharDevice = NULL;

ssize_t hello_read(struct file *filep, char __user *outb, size_t nbytes, loff_t *offset)
{
    int bytes_read = 0;
/*
    unsigned long len;
    unsigned long ret;
*/
    if (outb == NULL) {
        printk(KERN_INFO "hello_read: outb is NULL\n");
        return -EINVAL;
    }

    if (offset == NULL) {
        printk(KERN_INFO "hello_read: offset is NULL\n");
        return -EINVAL;
    }

    if (*offset >= num_bytes) {
        printk(KERN_INFO "hello_read: offset >= num_bytes\n");
        return 0;
    }

    printk(KERN_INFO "readCount = %02d, message %s\n", readCount, message);
    sprintf(number, "%02d\n", readCount++);
    strcpy(&message[strlen(message) - strlen(number)], number);

    while ((bytes_read < nbytes) && (*offset< num_bytes)) {
        put_user(message[*offset], &outb[bytes_read]);
        (*offset)++;
        bytes_read++;
    }

    return bytes_read;
    
/*
    if (bytes_read + 2 < nbytes) {
        put_user(' ', &outb[bytes_read]);
        bytes_read++;
        put_user(30 + (readCount % 10), &outb[bytes_read]);
        bytes_read++;
        }

    readCount++;

    return bytes_read;

    len = (nbytes > (num_bytes - *offset)) ? num_bytes - *offset : nbytes;
    printk(KERN_INFO "hello_read: len is %lu offset is %llu\n", len, *offset);

    ret = copy_to_user(outb, &message[*offset], len);
    if (ret == 0) {
        printk(KERN_INFO "hello_read: copy_to_user success\n");
        num_bytes = 0;  // clear the position in message buffer
        return 0;
    }

    printk(KERN_INFO "hello_read: failed to send %lu charakters to user\n", len);

    return -EFAULT;
*/    
}

ssize_t hello_write(struct file *filep, const char __user *extb, size_t nbytes, loff_t *offset)
{
    printk(KERN_INFO "hello_write nbytes=%zu \n", nbytes);

    if ((extb == NULL) || (nbytes < 5) || (nbytes > (sizeof(message) - 20))){
        printk(KERN_INFO "Error - cannot handle write, nbytes=%zu\n", nbytes);
        return -EINVAL;
        }

    sprintf(message, "%s (%zu letters)  ", extb, nbytes);
    num_bytes = strlen(message);
    printk(KERN_INFO "hello_write received %zu characters from the user\n", nbytes);

    return nbytes;
}

int hello_open(struct inode *inodep, struct file *filep)
{
    if (is_open == 1) {
        printk(KERN_INFO "Error - hello device already open\n");
        return -EBUSY;
    }

    is_open = 1;
    try_module_get(THIS_MODULE);

    return 0;
}


int hello_release(struct inode *inodep, struct file *filep)
{
    if (is_open == 0) {
        printk(KERN_INFO "Error - device wasn't opened\n");
        return -EBUSY;
    }

    is_open = 0;
    module_put(THIS_MODULE);

    return 0;
}

static struct file_operations fops = {
    owner: THIS_MODULE,
    read: hello_read,
    write: hello_write,
    open: hello_open,
    release: hello_release
};

static int __init hello_start(void)
{
    printk(KERN_INFO "Hello, I'm here to help\n");

    strncpy(message, "Hello world - hi hi hi nr: ", 1023);
    sprintf(number, "%02d\n", readCount++);
    strcat(message, number);
    num_bytes = strlen(message);

    // Try to dynamically allocate a major number for a device
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_INFO "Registering the char device faild with %d\n", majorNumber);
        return majorNumber;
    }

    printk(KERN_INFO "The hello device is major: %d\n", majorNumber);

    // Register the device class
    helloCharClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(helloCharClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Fail to register device class\n");
        return PTR_ERR(helloCharClass);
    }
    printk(KERN_INFO "hello: device class registered correctly\n");

    // Register the device driver
    helloCharDevice = device_create(helloCharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(helloCharDevice)) {
        class_destroy(helloCharClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Fail to create the device\n");
        return PTR_ERR(helloCharDevice);
    }
    printk(KERN_INFO "hello: device class created correctly\n");

    return 0;
}

static void __exit hello_end(void)
{
    device_destroy(helloCharClass, MKDEV(majorNumber, 0));
    class_unregister(helloCharClass);
    class_destroy(helloCharClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "Inregister. Goodbay, I hope I was helpful\n");
}

module_init(hello_start);
module_exit(hello_end);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GS");
MODULE_DESCRIPTION("A simple Linux char driver for learning");
MODULE_VERSION("0.1");
