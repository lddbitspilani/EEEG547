#include<linux/init.h>
#include<linux/module.h>
#include<linux/moduleparam.h>
#include<linux/version.h>
#include<linux/types.h>
#include<linux/jiffies.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/cdev.h>
#include<linux/kernel.h>
#include <linux/random.h>
#include <linux/uaccess.h>
#include <linux/delay.h> /* usleep_range */
#include <asm/io.h>

#define PERF_BASE 0xFE000000
#define GPIO_BASE (PERF_BASE + 0x200000)
#define BUF_LEN 80

#define CLR_LCD 0
#define SETUP_LCD 1
#define LSHFT_LCD 5
#define RSHFT_LCD 3

#define OUT_MODE 0
#define IN_MODE 1


//DEFINING LCD PINS
#define LCD_RS 22	
#define LCD_EN 17
#define LCD_D4 25
#define LCD_D5 24
#define LCD_D6 23
#define LCD_D7 18



static dev_t set_gpio;

static struct cdev c_set_gpio;


static struct class *cls_set;


static char *mystr = "assign1";
static int myint = 666;
static uint32_t* sel_reg1;
static uint32_t* sel_reg2;
static uint32_t* set_reg;
static uint32_t* clr_reg;


	//STEP 4.... callback functions
	
	void set_pin(int pin_num)
	{
		uint32_t reg_val = 1<<pin_num;
		printk(KERN_INFO "Setting GPIO%d", pin_num);
		writel(reg_val,set_reg);
	}
	
	void pin_mode_all(int i)
	{

		uint32_t read_reg_val1=readl(sel_reg1);
		uint32_t read_reg_val2=readl(sel_reg2);

		uint32_t to_write1 = (((0x9<<21)|(0x9<<24))|0xF00FFFFF)&read_reg_val1;
		uint32_t to_write2 = (((0x9<<15)|(0x9<<12)|(0x9<<9)|(0x9<<6))|0xFFF0000F)&read_reg_val2;
		
		printk(KERN_INFO "PINSEL REG1 val: %#010x", to_write1);
		writel(to_write1,sel_reg1);
		printk(KERN_INFO "PINSEL REG2 val: %#010x", to_write2);
		writel(to_write2,sel_reg2);
	}
	
	void clr_pin(int pin_num)
	{
		uint32_t reg_val = 1<<pin_num;
		printk(KERN_INFO "Clearing GPIO%d", pin_num);
		writel(reg_val,clr_reg);
	}
	
	void write_to_pin(int pin_num, uint8_t data)
	{
		if(data)
		set_pin(pin_num);
		
		else
		clr_pin(pin_num);
		
	}
	
	void write_to_databus(uint8_t c)
	{
		write_to_pin(LCD_D4,0x01&c);
		write_to_pin(LCD_D5,0x02&c);
		write_to_pin(LCD_D6,0x04&c);
		write_to_pin(LCD_D7,0x08&c);
		
	}
	
	void delay(uint32_t time)
	{
		int c = jiffies;
		while(jiffies<(c+time));
	}
	
	
	void Lcd_CmdWrite(uint8_t cmd)
	{
		// LcdDataBus = (cmd & 0xF0);     //Send higher nibble
		msleep(1);
		write_to_databus(cmd >> 4);
		// LCD_RS = 0;   // Send LOW -pulse on RS pin for selecting Command register
		write_to_pin(LCD_RS,0);
		// LCD_RW = 0;   // Send LOW pulse on RW pin for Write operation
		// LCD_EN = 1;   // Generate a High-to-low pulse on EN pin
		write_to_pin(LCD_EN,0);
		udelay(10);
		write_to_pin(LCD_EN,1);
		// msleep(1000);
		udelay(10);
		// LCD_EN = 0;
		write_to_pin(LCD_EN,0);

		// msleep(10000);
		udelay(100);

		// LcdDataBus = ((cmd<<4) & 0xF0); //Send Lower nibble
		write_to_databus((cmd & 0x0F));
		// LCD_RS = 0;   // Send LOW pulse on RS pin for selecting Command register
		write_to_pin(LCD_RS,0);
		// LCD_RW = 0;   // Send LOW pulse on RW pin for Write operation
		// LCD_EN = 1;   // Generate a High-to-low pulse on EN pin
		write_to_pin(LCD_EN,0);
		udelay(10);
		write_to_pin(LCD_EN,1);
		// msleep(1000);
		udelay(10);
		// LCD_EN = 0;
		write_to_pin(LCD_EN,0);		

		// msleep(10000);
		// msleep(1);
	}
	
		void Lcd_DataWrite(uint8_t cmd)
	{
		// LcdDataBus = (cmd & 0xF0);     //Send higher nibble
		msleep(1);
		write_to_databus(cmd >> 4);
		// LCD_RS = 0;   // Send HIGH -pulse on RS pin for selecting DATA register
		write_to_pin(LCD_RS,1);
		// LCD_RW = 0;   // Send LOW pulse on RW pin for Write operation
		// LCD_EN = 1;   // Generate a High-to-low pulse on EN pin
		write_to_pin(LCD_EN,0);
		udelay(10);
		write_to_pin(LCD_EN,1);
		// msleep(1000);
		udelay(10);
		// LCD_EN = 0;
		write_to_pin(LCD_EN,0);

		// msleep(10000);
		udelay(100);

		// LcdDataBus = ((cmd<<4) & 0xF0); //Send Lower nibble
		write_to_databus((cmd & 0x0F));
		// LCD_RS = 0;   // Send HIGH pulse on RS pin for selecting DATA register
		write_to_pin(LCD_RS,1);
		// LCD_RW = 0;   // Send LOW pulse on RW pin for Write operation
		// LCD_EN = 1;   // Generate a High-to-low pulse on EN pin
		write_to_pin(LCD_EN,0);
		udelay(10);
		write_to_pin(LCD_EN,1);
		// msleep(1000);
		udelay(10);
		// LCD_EN = 0;
		write_to_pin(LCD_EN,0);		

		// msleep(10000);
		// msleep(1);
	}

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

	static ssize_t set_pin_cb(struct file *f, char __user *buf, size_t len, loff_t *offp)
		{
			
			return 0;
		}

	static ssize_t device_write(struct file *file, const char __user * buffer, size_t length, loff_t * offset)
	{
		int i;

		char Message[BUF_LEN];
		int flag = 1;
		
		printk(KERN_INFO "device_write(%p,%s,%d)", file, buffer, length);
		

		for (i = 0; i < length && i < BUF_LEN; i++)
			get_user(Message[i], buffer + i);


		/*
		 * Again, return the number of input characters used
		 */
		// Lcd_DataWrite('H');
		// Lcd_DataWrite('e');
		// Lcd_DataWrite('l');
		// Lcd_DataWrite('l');
		// Lcd_DataWrite('o');
		// Lcd_DataWrite(' ');
		// Lcd_DataWrite('w');
		// Lcd_DataWrite('o');
		// Lcd_DataWrite('r');
		// Lcd_DataWrite('l');
		// Lcd_DataWrite('d');
		// Lcd_CmdWrite(0xC0);        // Move the cursor to beginning of second line
		for(i=0;i<16;i++)
			{
				if (Message[i]!=0&&flag)
				Lcd_DataWrite(Message[i]);
				else
				{flag = 0;
				// Lcd_DataWrite(' ');
				}
			}
		
		if(flag)
		{
			Lcd_CmdWrite(0xC0);        // Move the cursor to beginning of second line	
			for(i=16;i<33;i++)
				{
					if (Message[i]!=0&&flag)
					Lcd_DataWrite(Message[i]);
					else
					{
						// if(i==31&&flag)
						// Lcd_DataWrite(Message[i]);
					
						flag=0;
						// Lcd_DataWrite(' ');
					}
				}
		}
		// Lcd_CmdWrite(0x80);
		return i;
	}
	
	long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
	{
		int i;
		// char *temp;

		// char message1[80];
		
		// temp = (char *)ioctl_param;


		 
		// for (i = 0; i < BUF_LEN; i++)
			// get_user(message1[i], temp + i);
		 
		printk(KERN_INFO "IOCTL CALL: %d\nPARAM: %d",ioctl_num,ioctl_param);


		switch (ioctl_num) {
			case CLR_LCD:		
	
				Lcd_CmdWrite(0x01);        // Clear Display
				// msleep(1);
				Lcd_CmdWrite(0x80);        // Move the cursor to beginning of first line
				// msleep(1);

				 break;
				 
			case SETUP_LCD:
				Lcd_CmdWrite(0x33);        // Initialize Lcd in 4-bit mode
				// msleep(5);
				Lcd_CmdWrite(0x32);        // enable 5x7 mode for chars 
				// msleep(5);
				Lcd_CmdWrite(0x28);        // Function set
				// msleep(5);
				Lcd_CmdWrite(0x08);        // Display OFF, Cursor ON
				// msleep(5);				
				Lcd_CmdWrite(0x01);        // Move the cursor to beginning of first line	
				// msleep(5);
				Lcd_CmdWrite(0x06);        // Move the cursor to beginning of first line	
				// msleep(5);
				Lcd_CmdWrite(0x0C);        // Move the cursor to beginning of first line	
				// msleep(5);
				Lcd_CmdWrite(0x80);        // Move the cursor to beginning of first line	
				// msleep(5);
				
			
				break;
				
			
				 
			case RSHFT_LCD:	

				
				for(i=0;i<ioctl_param;i++)
				Lcd_CmdWrite(0x14);

				break;
				 
			case LSHFT_LCD:	

				
				for(i=0;i<ioctl_param;i++)
				Lcd_CmdWrite(0x10);

				break;
				
			case 7:					
				
				Lcd_CmdWrite(ioctl_param);

				break;
				 
			default: 
				printk(KERN_INFO "INVALID IOCTL CALL");
				break;
			

			}

		return 0;
	}





	static struct file_operations fops_set =
					{
						.owner = THIS_MODULE,
						.open = my_open,
						.release = my_close,
						.read = set_pin_cb,
						.write = device_write,
						.unlocked_ioctl = device_ioctl,
												
					};





	module_param(mystr, charp, S_IRUGO);
	module_param(myint, int, S_IRUGO);

	static int __init mychar_init(void)
	
	{

	

		// MAP HARDWARE ADDRESSES TO THE VIRTUAL PAGE TABLE
		if ((sel_reg1 = ioremap(GPIO_BASE+0x04,0X04))==NULL)
			{
				printk(KERN_INFO "IOREMAP FAIL");
				return -1;
			}
		
		if ((sel_reg2 = ioremap(GPIO_BASE+0x08,0X04))==NULL)
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
		pin_mode_all(0);	
		
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
		iounmap(sel_reg2);
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
