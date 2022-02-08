#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/signal.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/pci.h>
#include <linux/interrupt.h> 
#include <asm/uaccess.h> 

#define DEBUG

#ifdef DEBUG
	#define DEBUG_ERR(format,args...) \
	do{  \
		printk("[anonynouse] - [%s:%d] ",__FUNCTION__,__LINE__); \
		printk(format,##args); \
	}while(0)
#else
	#define DEBUG_PRINT(format,args...) 
#endif

#define CHR_DEVICE_NAME "anonynouse_driver"
#define PCI_DEVICE_NAME "anonynouse_device"

#define XBAR_NUM    3

#define XBUF_NUM    8
#define XBUF_SIZE   1 << 23

#define virt_addr_t u64

struct XBar {
    phys_addr_t phy_addr;
    virt_addr_t vir_addr;
    unsigned long size;
};

struct XBuf {
    phys_addr_t phy_addr;
    virt_addr_t vir_addr;
    unsigned long size;
    unsigned long type;
};

static struct XBar xbar[XBAR_NUM];
static struct XBuf xbuf[XBUF_NUM];


//自定义设备结构体 
struct lspci_dev { 
    struct cdev cdev; // 字符设备 
    struct device *device;
    struct class *class;
    dev_t dev_id;
    int major;
    int minor;

    struct pci_dev *pci_dev;
}; 

struct lspci_dev *lspci_devp; 

/* 字符设备file_operations open 成员函数 */ 
static int xxx_open(struct inode *inode, struct file *filp) { 

    DEBUG_ERR("xxx_open execution");

    return 0; 
}

/* 字符设备file_operations ioctl 成员函数 */ 
static long xxx_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) { 

    DEBUG_ERR("xxx_oictl execution");

    return 0; 
}

/* 字符设备file_operations read 成员函数 */ 
static ssize_t xxx_read(struct file *filp, char __user * buf, size_t size, 
    loff_t * ppos) { 

    DEBUG_ERR("xxx_read execution");

    return 0; 
}

/* 字符设备file_operations write成员函数 */ 
static ssize_t xxx_write(struct file *filp, const char __user *buf,size_t size, 
    loff_t *ppos) { 
    
    DEBUG_ERR("xxx_write execution");

    return 0; 
}

/* 字符设备file_operations release成员函数 */ 
static int xxx_release(struct inode *inode, struct file *filp) { 

    DEBUG_ERR("xxx_release execution");

    return 0; 
} 

/* 设备文件操作接口 */ 
static const struct file_operations xxx_fops = { 
    .owner = THIS_MODULE, /* xxx_fops 所属的设备模块 */ 
    .read = xxx_read, /* 读设备操作*/ 
    .write = xxx_write, /* 写设备操作*/ 
    .unlocked_ioctl = xxx_ioctl, /* 控制设备操作*/ 
    .open = xxx_open, /* 打开设备操作*/ 
    .release = xxx_release, /* 释放设备操作*/ 
}; 

/*pci_device_id 用于标识一个PCI 设备。 
它的成员包括：厂商ID、设备ID、子厂商ID、子设备ID、 类别、类别掩码（类可分为基类、子类）和私有数据。*/ 
static struct pci_device_id xxx_pci_tbl [] = { 
    {0x10ee, 0x0050,PCI_ANY_ID, PCI_ANY_ID, }, 
    {0,} 
}; 
MODULE_DEVICE_TABLE(pci, xxx_pci_tbl); 

static int chr_dev_init(void) {
    int ret = 0;

    // create device id
    if (lspci_devp->major) {
        lspci_devp->dev_id = MKDEV(lspci_devp->major, 0);
        ret = register_chrdev_region(lspci_devp->dev_id, 1, CHR_DEVICE_NAME);
        if (ret) {
            goto err_handle_1;
        }
    }
    else {
        ret = alloc_chrdev_region(&lspci_devp->dev_id, 0, 1, CHR_DEVICE_NAME);
        if (ret) {
            goto err_handle_2;
        }
        lspci_devp->major = MAJOR(lspci_devp->dev_id);
        lspci_devp->minor = MINOR(lspci_devp->dev_id);
    }
    DEBUG_ERR("major.minor: %d.%d", lspci_devp->major, lspci_devp->minor);

    // init char device struct
    lspci_devp->cdev.owner = THIS_MODULE;
    cdev_init(&lspci_devp->cdev, &xxx_fops);
    // add a char device
    ret = cdev_add(&lspci_devp->cdev, lspci_devp->dev_id, 1);
    if (ret) {
        goto err_handle_3;
    }

    // create class
    lspci_devp->class = class_create(THIS_MODULE, CHR_DEVICE_NAME);
    if (IS_ERR_VALUE(lspci_devp->class)) {
        ret = PTR_RET(lspci_devp->class);
        goto err_handle_4;
    }

    // create the class device
    lspci_devp->device = device_create(lspci_devp->class, NULL, lspci_devp->dev_id, 
        NULL, CHR_DEVICE_NAME);
    if (IS_ERR_VALUE(lspci_devp->device)) {
        ret = PTR_RET(lspci_devp->device);
        goto err_handle_5;
    }

    return 0;

err_handle_5:
    device_destroy(lspci_devp->class, lspci_devp->dev_id);

err_handle_4:
    class_destroy(lspci_devp->class);

err_handle_3:
    cdev_del(&lspci_devp->cdev);

err_handle_2:
    unregister_chrdev_region(lspci_devp->dev_id, 1);

err_handle_1:
    ;
    
    return ret;
}

static void chr_dev_exit(void) {
    device_destroy(lspci_devp->class, lspci_devp->dev_id);

    class_destroy(lspci_devp->class);

    cdev_del(&lspci_devp->cdev);

    unregister_chrdev_region(lspci_devp->dev_id, 1);
}

static int xbar_map(struct pci_dev *p_dev) {
    int i = 0;
    int ret = 0;

    for (i = 0; i < XBAR_NUM; i++) {
        xbar[i].phy_addr = pci_resource_start(p_dev, i);
        if (xbar[i].phy_addr < 0) {
            DEBUG_ERR("pci_resource_start execution failed");
            ret =-EIO;
            goto err_handle;
        }

        xbar[i].size = pci_resource_len(p_dev, i);
        if (xbar[i].size < 0) {
            goto err_handle;
        }
        xbar[i].vir_addr = (unsigned long)ioremap(xbar[i].phy_addr, xbar[i].size);
        if (xbar[i].vir_addr == 0) {
            goto err_handle;
        }
    }
    return 0;

err_handle:
    ;

    return -1;
}

static irqreturn_t irq_interrupt(int irq, void * dev) {  
    DEBUG_ERR("irq_interrupt execution");
	return 0;
}

/* pci_driver 的 probe 成员函数 */ 
static int xxx_probe(struct pci_dev *pci_dev, const struct pci_device_id *pci_id) {
    int ret = 0;

    //为自定义设备结构体申请内存 
    lspci_devp = kzalloc(sizeof(struct lspci_dev), GFP_KERNEL);

    //使能pci设备
    ret = pci_enable_device(pci_dev);
	if (ret) {
        DEBUG_ERR("pci_enable_device execution failed");
		goto err_handle_end;
	}
    lspci_devp->pci_dev = pci_dev;

    ret = chr_dev_init();
    if (ret) {
        DEBUG_ERR("chr_dev_init execution failed");
        goto err_handle_1;
    }

    // 申请I/O 资源 return 0; 
    ret = pci_request_regions(pci_dev, PCI_DEVICE_NAME);
    if (ret) {
        DEBUG_ERR("pci_request_regions execution failed");
        goto err_handle_1;
    }

    //使能msi，然后才能得到pdev->irq
	 ret = pci_enable_msi(pci_dev);
	 if (ret){
		DEBUG_ERR("pci_enable_msi execution failed");
		goto err_handle_2;
    }
	
	ret = request_irq(pci_dev->irq, irq_interrupt, 0, PCI_DEVICE_NAME, pci_dev);
    if (ret) {
       DEBUG_ERR("request_irq execution failed");
	   goto err_handle_3;
    }

    DEBUG_ERR("xxx_probe execution");

    return 0;

err_handle_3:
    free_irq(pci_dev->irq, lspci_devp->pci_dev);
	pci_disable_msi(pci_dev);

err_handle_2:
    pci_release_regions(pci_dev);

err_handle_1:
    pci_disable_device(pci_dev);

err_handle_end:
    kfree(lspci_devp);

    return ret;
} 

/* pci_driver 的remove 成员函数 */ 
static void xxx_remove(struct pci_dev *p_dev) { 
    /* 注销字符设备 */ 
    chr_dev_exit();

    free_irq(p_dev->irq, lspci_devp->pci_dev);
	pci_disable_msi(p_dev);

    pci_release_regions(p_dev);

    kfree(lspci_devp); 
} 

/* PCI设备模块信息 */ 
static struct pci_driver xxx_pci_driver = { 
    .name = PCI_DEVICE_NAME, /* 设备模块名称 */ 
    .id_table = xxx_pci_tbl, /* 能够驱动的设备列表 */
    .probe = xxx_probe, /* 查找并初始化设备 */ 
    .remove = xxx_remove, /* 卸载设备模块 */ 
}; 

static int __init xxx_init_module (void) { 
    //注册pci驱动,进入probe函数 
    int ret = pci_register_driver(&xxx_pci_driver);

    DEBUG_ERR("xxx_init_module execution"); 

    return ret; 
} 

static void __exit xxx_exit_module (void) { 
    //注销pci驱动 
    pci_unregister_driver(&xxx_pci_driver); 

    DEBUG_ERR("xxx_exit_module execution");
}

/* 驱动模块加载函数 */ 
module_init(xxx_init_module); 
/* 驱动模块卸载函数 */ 
module_exit(xxx_exit_module); 

MODULE_AUTHOR("George");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("pcie device driver for anonynouse"); 