#include "sys.h"

MY_STATIC int rwProcMem_open(struct inode *inode, struct file *filp)
{
	return 0;
}

MY_STATIC ssize_t rwProcMem_read(struct file *filp, char __user *userbuf, size_t size, loff_t *ppos)
{

	//struct rwProcMemDev* devp = filp->private_data; //获得设备结构体指针

	char data[16] = {0};

	char buf[64] = {0};

	if (copy_from_user(data, userbuf, 16) == 0)
	{
		int proc_pid = *(size_t *)&data;
		uint64_t proc_virt_addr = *(uint64_t *)&data[8];

		printk(KERN_EMERG "READ proc_pid:0x%d,size:%ld\n", proc_pid, sizeof(proc_pid));

		printk(KERN_EMERG "proc_virt_addr:0x%x,size:%ld\n", proc_virt_addr, sizeof(proc_virt_addr));

		read_phy_addr(proc_pid, (uint64_t)proc_virt_addr, size, buf);

		printk(KERN_EMERG "rwProcMemDemo_read:%x\n", *(uint64_t*)buf);
	}
	copy_to_user(userbuf, buf, size);

	return 0;
}

MY_STATIC ssize_t rwProcMem_write(struct file* filp, const char __user* userbuf, size_t size, loff_t *ppos)
{

	//struct rwProcMemDev* devp = filp->private_data; //获得设备结构体指针

	char data[16] = {0};

	char buf[64] = {0x11,0x11,0x11,0x11};

	if (copy_from_user(data, userbuf, 16) == 0)
	{
		int proc_pid = *(size_t *)&data;
		uint64_t proc_virt_addr = *(uint64_t *)&data[8];

		printk(KERN_EMERG "READ proc_pid:0x%d,size:%ld\n", proc_pid, sizeof(proc_pid));

		printk(KERN_EMERG "proc_virt_addr:0x%x,size:%ld\n", proc_virt_addr, sizeof(proc_virt_addr));

		write_phy_addr(proc_pid, (uint64_t)proc_virt_addr, size, buf);

		printk(KERN_EMERG "rwProcMemDemo_write:%x\n", *(uint64_t*)buf);
	}

	return 0;
}

MY_STATIC int rwProcMem_release(struct inode *inode, struct file *filp)
{
	return 0;
}

#ifndef CONFIG_MODULE_GUIDE_ENTRY
static
#endif
	int __init
	rwProcMem_dev_init(void)
{
	int result;
	int err;
	//动态申请设备号
	result = alloc_chrdev_region(&g_rwProcMem_devno, 0, 1, DEV_FILENAME);
	g_rwProcMem_major = MAJOR(g_rwProcMem_devno);

	if (result < 0)
	{
		printk(KERN_EMERG "rwProcMem alloc_chrdev_region failed %d\n", result);
		return result;
	}

	// 2.动态申请设备结构体的内存
	g_rwProcMem_devp = kmalloc(sizeof(struct rwProcMemDev), GFP_KERNEL);
	if (!g_rwProcMem_devp)
	{
		//申请失败
		result = -ENOMEM;
		goto _fail;
	}
	memset(g_rwProcMem_devp, 0, sizeof(struct rwProcMemDev));

	//3.初始化并且添加cdev结构体
	cdev_init(&g_rwProcMem_devp->cdev, &rwProcMem_fops); //初始化cdev设备
	g_rwProcMem_devp->cdev.owner = THIS_MODULE;			 //使驱动程序属于该模块
	g_rwProcMem_devp->cdev.ops = &rwProcMem_fops;		 //cdev连接file_operations指针
	//将cdev注册到系统中
	err = cdev_add(&g_rwProcMem_devp->cdev, g_rwProcMem_devno, 1);
	if (err)
	{
		printk(KERN_NOTICE "Error in cdev_add()\n");
		result = -EFAULT;
		goto _fail;
	}

	//4.创建设备文件
	g_Class_devp = class_create(THIS_MODULE, DEV_FILENAME);							//创建设备类（位置在/sys/class/xxxxx）
	device_create(g_Class_devp, NULL, g_rwProcMem_devno, NULL, "%s", DEV_FILENAME); //创建设备文件（位置在/dev/xxxxx）

	printk(KERN_EMERG "Hello, %s\n", DEV_FILENAME);
	return 0;
_fail:
	unregister_chrdev_region(g_rwProcMem_devno, 1);
	return result;
}

#ifndef CONFIG_MODULE_GUIDE_ENTRY
static
#endif
	void __exit
	rwProcMem_dev_exit(void)
{
	device_destroy(g_Class_devp, g_rwProcMem_devno); //删除设备文件（位置在/dev/xxxxx）
	class_destroy(g_Class_devp);					 //删除设备类

	cdev_del(&g_rwProcMem_devp->cdev);				//注销cdev
	kfree(g_rwProcMem_devp);						// 释放设备结构体内存
	unregister_chrdev_region(g_rwProcMem_devno, 1); //释放设备号

	printk(KERN_EMERG "Goodbye, %s\n", DEV_FILENAME);
}

#ifdef CONFIG_MODULE_GUIDE_ENTRY
module_init(rwProcMem_dev_init);
module_exit(rwProcMem_dev_exit);
#endif

MODULE_AUTHOR("Linux");
MODULE_DESCRIPTION("Linux default module");
MODULE_LICENSE("GPL");