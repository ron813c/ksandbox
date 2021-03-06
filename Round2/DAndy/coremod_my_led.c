
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/input.h>
#include <linux/gpio.h>
#include <linux/stat.h>
#include <linux/fs.h>

#define DRIVER_NAME			"x_led_drv"


static struct device *dev;

static const struct of_device_id my_drvr_match[];

static unsigned int gpioLED = 32+24;	/* GPIO1_24 */

/*
#define LEN_MSG 8
static char buf_msg[LEN_MSG + 1] = {0};
*/


static void get_platform_info(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct device *dev = &pdev->dev;
	const char *name;
	u32 val;

	if (np) {

	if (!of_property_read_string(np, "my_name", &name))
		dev_info(dev, "my name is %s\n", name);

	if (!of_property_read_u32(np, "tmp_var", &val))
		dev_info(dev, "tmp_var = %d\n", val);
	}
}



static ssize_t sys_led_on(struct class *class, struct class_attribute *attr, char *buf)
{	
	ssize_t i = 0;

	i += sprintf(buf, "LED is ON\n");
	
	dev_info(dev, "LED is ON\n");
	gpio_set_value(gpioLED, 1);
	return i;
}

static ssize_t sys_led_off(struct class *class, struct class_attribute *attr, char *buf)
{	
	ssize_t i = 0;

	i += sprintf(buf, "LED is OFF\n");

	dev_info(dev, "LED is OFF\n");
	gpio_set_value(gpioLED, 0);
	return i;
}

/* <linux/device.h>
* #define CLASS_ATTR(_name, _mode, _show, _store) \
* struct class_attribute class_attr_##_name = __ATTR(_name, _mode,
* _show, _store)
*/
CLASS_ATTR(myled_on, 0664, &sys_led_on, NULL);
CLASS_ATTR(myled_off, 0664, &sys_led_off, NULL);

static struct class *x_led_class;


static int x_led_init(struct platform_device *pdev)
{
	int res;
	const struct of_device_id *match;

	dev = &pdev->dev;

	match = of_match_device(of_match_ptr(my_drvr_match), dev);
	if (!match) {
		dev_err(dev, "failed of_match_device()\n");
		return -EINVAL;
	}

	dev_info(dev, "Hello, world!\n");

	get_platform_info(pdev);


	x_led_class = class_create(THIS_MODULE, "x_led_class");
	if (IS_ERR(x_led_class))
		dev_info(dev, "bad class create\n");

	res = class_create_file(x_led_class, &class_attr_myled_on);
	res = class_create_file(x_led_class, &class_attr_myled_off);

	dev_info(dev, "'x_led_drv' module initialized\n");
	return 0;
}



static int x_led_exit(struct platform_device *pdev)
{
		struct device *dev = &pdev->dev;

		class_remove_file(x_led_class, &class_attr_myled_on);
		class_remove_file(x_led_class, &class_attr_myled_off);
		class_destroy(x_led_class);
				
		dev_info(dev, "Goodbye, world!\n");
		return 0;
}




static const struct of_device_id my_drvr_match[] = {
	{ .compatible = "DAndy,coremod_my", },
	{ },
};
MODULE_DEVICE_TABLE(of, my_drvr_match);


static struct platform_driver my_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.of_match_table = my_drvr_match,
	},
	.probe = x_led_init,
	.remove = x_led_exit,
};
module_platform_driver(my_driver);



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Devyatov Andrey <devyatov.andey@litech.com>");
