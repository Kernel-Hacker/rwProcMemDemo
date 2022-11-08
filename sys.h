#ifndef SYS_H_
#define SYS_H_
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <asm/io.h>
//#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/version.h>
///////////////////////////////////////////////////////////////////
#include <linux/slab.h> //kmalloc与kfree
#include <linux/cdev.h>
#include <linux/device.h> //device_create创建设备文件（/dev/xxxxxxxx）
////////////////////////////////////////////////////////////////

#include "proc_mem.h"
#include "ver_control.h"


MY_STATIC int g_rwProcMem_major = 0; //记录动态申请的主设备号
MY_STATIC dev_t g_rwProcMem_devno;

//rwProcMemDev设备结构体
struct rwProcMemDev {
	struct cdev cdev; //cdev结构体
};
MY_STATIC struct rwProcMemDev *g_rwProcMem_devp; //创建的cdev设备结构
MY_STATIC struct class *g_Class_devp; //创建的设备类

MY_STATIC int rwProcMem_open(struct inode *inode, struct file *filp);
MY_STATIC int rwProcMem_release(struct inode *inode, struct file *filp);
MY_STATIC ssize_t rwProcMem_read(struct file* filp, char __user* buf, size_t size, loff_t* ppos);
MY_STATIC ssize_t rwProcMem_write(struct file* filp, const char __user* buf, size_t size, loff_t *ppos);

MY_STATIC const struct file_operations rwProcMem_fops =
{
  .owner = THIS_MODULE,
  .open = rwProcMem_open, //打开设备函数
  .release = rwProcMem_release, //释放设备函数
  .read = rwProcMem_read, //读设备函数
  .write = rwProcMem_write, //读设备函数
};


#endif /* SYS_H_ */