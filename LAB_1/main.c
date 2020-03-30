	#include<linux/init.h>
	#include<linux/module.h>
	#include<linux/moduleparam.h>
	#include<linux/version.h>
	#include<linux/types.h>
	#include<linux/kdev_t.h>
	#include<linux/fs.h>
	#include<linux/device.h>
	#include<linux/cdev.h>
	#include<linux/kernel.h>
	#include <linux/random.h>
	#include <linux/uaccess.h>
	#include <asm/io.h>
	
	#define PERF_BASE 0xFE000000
	#define GPIO_BASE (PERF_BASE + 0x200000)


	static dev_t set_gpio;

	static struct cdev c_set_gpio;


	static struct class *cls_set;


	static char *mystr = "assign1";
	static int myint = 666;
	static uint32_t* sel_reg;
	static uint32_t* set_reg;
	static uint32_t* clr_reg;

	//STEP 4.... callback functions

	static int my_open(struct inode *i, struct file *f)
		{
			printk(KERN_INFO "mychar: open()\n");
			return 0;
		}

	static int my_close(struct inode *i, struct file *f)
		{
			printk(KERN_INFO "mychar: close()\n");
			return 0;
		}

	static ssize_t set_pin(struct file *f, char __user *buf, size_t len, loff_t *offp)
		{
			printk(KERN_INFO "Setting GPIO 27 to HIGH");	
			writel(0x08000000,set_reg);
			
			return 0;
		}

	static ssize_t clr_pin(struct file *f, const char __user *buf, size_t len, loff_t *offp)
		{
			printk(KERN_INFO "Setting GPIO 27 to LOW");	
			writel(0x08000000,clr_reg);
			return 0;
		}




	static struct file_operations fops_set =
					{
						.owner = THIS_MODULE,
						.open = my_open,
						.release = my_close,
						.read = set_pin,
						.write = clr_pin,
												
					};





	module_param(mystr, charp, S_IRUGO);
	module_param(myint, int, S_IRUGO);

	static int __init mychar_init(void)
	
	{	
	
		uint32_t read_reg_val,write_reg_val;
		// MAP HARDWARE ADDRESSES TO THE VIRTUAL PAGE TABLE
		
		if ((sel_reg = ioremap(GPIO_BASE+0x08,0X04))==NULL)
			{
				printk(KERN_INFO "IOREMAP FAIL");
				return -1;
			}
		if ((set_reg = ioremap(GPIO_BASE+0x1C,0X04))==NULL)
			{
				printk(KERN_INFO "IOREMAP FAIL");
				return -1;
			}
		if ((clr_reg = ioremap(GPIO_BASE+0x28,0X04))==NULL)
			{
				printk(KERN_INFO "IOREMAP FAIL");
				return -1;
			}
		read_reg_val=readl(sel_reg);
		printk(KERN_INFO "PINSEL DATA: %#010x",read_reg_val);
		write_reg_val= (0xFF3FFFFF&read_reg_val)|0x00200000;
		printk(KERN_INFO "ATTEMPTING TO WRITE: %#010x",write_reg_val);
		writel(write_reg_val,sel_reg);
		printk(KERN_INFO "IOREMAP READBACK: %#010x",readl(sel_reg));	
		
		printk(KERN_INFO "SET_GPIO \n\n");

		if((alloc_chrdev_region(&set_gpio, 0, 1, "SET_GPIO")) <0){
					printk(KERN_INFO "Cannot allocate major number for device SET_GPIO\n");
					return -1;
			}

		printk(KERN_INFO "<Major, Minor>: <%d, %d>\n", MAJOR(set_gpio), MINOR(set_gpio));

		//----------------------------------------------------


		//Step 2 creation of device fileS
		if((cls_set=class_create(THIS_MODULE, "CLASS_SET"))==NULL)
		{
			unregister_chrdev_region(set_gpio,1);
			return -1;
		}

		if(device_create(cls_set, NULL, set_gpio, NULL, "DEV_SET_GPIO")==NULL)
		{
			class_destroy(cls_set);
			unregister_chrdev_region(set_gpio,1);
			return -1;
		}

	
		//-------------------------------------------------------------
		//-------------------------------------------------------------

		//STEP 3 Link fops and cdev to the device node
		cdev_init(&c_set_gpio, &fops_set);
		if(cdev_add(&c_set_gpio, set_gpio,1)==-1)
		{
			printk(KERN_INFO "Linking fops_set and cdev_set\n");		
			device_destroy(cls_set,set_gpio);
			class_destroy(cls_set);
			unregister_chrdev_region(set_gpio,1);
			return -1;
		}

		


		//----------------------------------------------------------------
			
		return 0;
	}

	static void __exit mychar_exit(void)
	{
		iounmap(set_reg);
		iounmap(sel_reg);
		iounmap(clr_reg);
		
		cdev_del(&c_set_gpio);
		device_destroy(cls_set,set_gpio);
		class_destroy(cls_set);
		unregister_chrdev_region(set_gpio,1);


		
		printk(KERN_INFO "Bye Linux\n\n");
	}

	module_init(mychar_init);
	module_exit(mychar_exit);

	MODULE_PARM_DESC(myint,"STUFF");
	MODULE_DESCRIPTION("ASSIGNMENT 1");
	MODULE_AUTHOR("PRATYUSH BHARATI");
	MODULE_LICENSE("GPL");
