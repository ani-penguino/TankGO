/* * Device driver for the VGA video generator
 *
 * A Platform device implemented using the misc subsystem
 *
 * Stephen A. Edwards
 * Columbia University
 *
 * References:
 * Linux source: Documentation/driver-model/platform.txt
 *               drivers/misc/arm-charlcd.c
 * http://www.linuxforu.com/tag/linux-device-drivers/
 * http://free-electrons.com/docs/
 *
 * "make" to build
 * insmod vga_ball.ko
 *
 * Check code style with
 * checkpatch.pl --file --no-tree vga_ball.c
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "vga_ball.h"

#define DRIVER_NAME "vga_ball"

/* Device registers */
#define GAME_INFO_1(x) 		(x)  	// stage num->2, explosion num->2, map num->3
#define GAME_INFO_2(x) 		((x)+1) // player1score->4, player2score->4
#define GAME_INFO_3(x)          ((x)+2) 
#define TANK1LOC_X(x) 		((x)+3)
#define TANK1LOC_Y(x) 		((x)+4)
#define TANK2LOC_X(x) 		((x)+5)
#define TANK2LOC_Y(x) 		((x)+6)
#define TANK1_DIRC(x) 		((x)+7) // first 2 tank1 dirc, next 2 tank2 dirc
#define TANK2_DIRC(x)           ((x)+8)
 
#define BULLET1LOC_X(x)		((x)+9)
#define BULLET1LOC_Y(x)		((x)+10)
#define BULLET2LOC_X(x)		((x)+11)
#define BULLET2LOC_Y(x)		((x)+12)
#define BULLET1_DIRC(x)         ((x)+13)
#define BULLET2_DIRC(x)         ((x)+14)
#define EXPLO_LOC_X(x) 		((x)+15)
#define EXPLO_LOC_Y(x) 		((x)+16) // 7 bits only

/*
 * Information about our device
 */
struct vga_ball_dev {
	struct resource res; /* Resource: our registers */
	void __iomem *virtbase; /* Where registers can be accessed in memory */
        vga_ball_color_t background;
} dev;

/*
 * Write segments of a single digit
 * Assumes digit is in range and the device information has been set up
 */
static void write_background(vga_ball_color_t *background)
{
	iowrite8(background->game_info_1, 	GAME_INFO_1(dev.virtbase) );
	iowrite8(background->game_info_2, 	GAME_INFO_2(dev.virtbase) );
	iowrite8(background->game_info_3,       GAME_INFO_3(dev.virtbase) );
	iowrite8(background->tank1loc_x, 	TANK1LOC_X(dev.virtbase) );
	iowrite8(background->tank1loc_y, 	TANK1LOC_Y(dev.virtbase) );
	iowrite8(background->tank2loc_x, 	TANK2LOC_X(dev.virtbase) );
	iowrite8(background->tank2loc_y, 	TANK2LOC_Y(dev.virtbase) );
	iowrite8(background->tank1_dirc, 	TANK1_DIRC(dev.virtbase) );
	iowrite8(background->tank2_dirc,        TANK2_DIRC(dev.virtbase) );
	iowrite8(background->bullet1loc_x, 	BULLET1LOC_X(dev.virtbase) );
	iowrite8(background->bullet1loc_y, 	BULLET1LOC_Y(dev.virtbase) );
	iowrite8(background->bullet2loc_x, 	BULLET2LOC_X(dev.virtbase) );
	iowrite8(background->bullet2loc_y, 	BULLET2LOC_Y(dev.virtbase) );
	iowrite8(background->bullet1_dirc,      BULLET1_DIRC(dev.virtbase) );
	iowrite8(background->bullet2_dirc,      BULLET2_DIRC(dev.virtbase) );
	iowrite8(background->explo_loc_x, 	EXPLO_LOC_X(dev.virtbase) );
	iowrite8(background->explo_loc_y, 	EXPLO_LOC_Y(dev.virtbase) );
	dev.background = *background;
}

/*
 * Handle ioctl() calls from userspace:
 * Read or write the segments on single digits.
 * Note extensive error checking of arguments
 */
static long vga_ball_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	vga_ball_arg_t vla;

	switch (cmd) {
	case VGA_BALL_WRITE_BACKGROUND:
		if (copy_from_user(&vla, (vga_ball_arg_t *) arg,
				   sizeof(vga_ball_arg_t)))
			return -EACCES;
		write_background(&vla.background);
		break;

	case VGA_BALL_READ_BACKGROUND:
	  	vla.background = dev.background;
		if (copy_to_user((vga_ball_arg_t *) arg, &vla,
				 sizeof(vga_ball_arg_t)))
			return -EACCES;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

/* The operations our device knows how to do */
static const struct file_operations vga_ball_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl = vga_ball_ioctl,
};

/* Information about our device for the "misc" framework -- like a char dev */
static struct miscdevice vga_ball_misc_device = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= DRIVER_NAME,
	.fops		= &vga_ball_fops,
};

/*
 * Initialization code: get resources (registers) and display
 * a welcome message
 */
static int __init vga_ball_probe(struct platform_device *pdev)
{
        vga_ball_color_t beige = { 0xf9, 0xe4, 0xb7, 0x00, 0x00 }; //this place is different
	int ret;

	/* Register ourselves as a misc device: creates /dev/vga_ball */
	ret = misc_register(&vga_ball_misc_device);

	/* Get the address of our registers from the device tree */
	ret = of_address_to_resource(pdev->dev.of_node, 0, &dev.res);
	if (ret) {
		ret = -ENOENT;
		goto out_deregister;
	}

	/* Make sure we can use these registers */
	if (request_mem_region(dev.res.start, resource_size(&dev.res),
			       DRIVER_NAME) == NULL) {
		ret = -EBUSY;
		goto out_deregister;
	}

	/* Arrange access to our registers */
	dev.virtbase = of_iomap(pdev->dev.of_node, 0);
	if (dev.virtbase == NULL) {
		ret = -ENOMEM;
		goto out_release_mem_region;
	}
        
	/* Set an initial color */
        write_background(&beige);

	return 0;

out_release_mem_region:
	release_mem_region(dev.res.start, resource_size(&dev.res));
out_deregister:
	misc_deregister(&vga_ball_misc_device);
	return ret;
}

/* Clean-up code: release resources */
static int vga_ball_remove(struct platform_device *pdev)
{
	iounmap(dev.virtbase);
	release_mem_region(dev.res.start, resource_size(&dev.res));
	misc_deregister(&vga_ball_misc_device);
	return 0;
}

/* Which "compatible" string(s) to search for in the Device Tree */
#ifdef CONFIG_OF
static const struct of_device_id vga_ball_of_match[] = {
	{ .compatible = "csee4840,vga_ball-1.0" },
	{},
};
MODULE_DEVICE_TABLE(of, vga_ball_of_match);
#endif

/* Information for registering ourselves as a "platform" driver */
static struct platform_driver vga_ball_driver = {
	.driver	= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(vga_ball_of_match),
	},
	.remove	= __exit_p(vga_ball_remove),
};

/* Called when the module is loaded: set things up */
static int __init vga_ball_init(void)
{
	pr_info(DRIVER_NAME ": init\n");
	return platform_driver_probe(&vga_ball_driver, vga_ball_probe);
}

/* Calball when the module is unloaded: release resources */
static void __exit vga_ball_exit(void)
{
	platform_driver_unregister(&vga_ball_driver);
	pr_info(DRIVER_NAME ": exit\n");
}

module_init(vga_ball_init);
module_exit(vga_ball_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stephen A. Edwards, Columbia University");
MODULE_DESCRIPTION("VGA ball driver");
