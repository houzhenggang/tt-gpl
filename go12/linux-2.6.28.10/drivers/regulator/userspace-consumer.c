/*
 * userspace-consumer.c
 *
 * Copyright 2009 CompuLab, Ltd.
 *
 * Author: Mike Rapoport <mike@compulab.co.il>
 *
 * Based of virtual consumer driver:
 *   Copyright 2008 Wolfson Microelectronics PLC.
 *   Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 */

#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/capability.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/userspace-consumer.h>

struct userspace_consumer_data {
	const char *name;

	struct mutex lock;
	bool enabled;

	int num_supplies;
	struct regulator_bulk_data *supplies;
};

static ssize_t reg_show_name(struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	struct userspace_consumer_data *data = dev_get_drvdata(dev);

	return sprintf(buf, "%s\n", data->name);
}

static ssize_t reg_show_state(struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	struct userspace_consumer_data *data = dev_get_drvdata(dev);

	if (data->enabled)
		return sprintf(buf, "enabled\n");

	return sprintf(buf, "disabled\n");
}

static ssize_t reg_set_state(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	struct userspace_consumer_data *data = dev_get_drvdata(dev);
	bool enabled;
	int ret;

	/*
	 * sysfs_streq() doesn't need the \n's, but we add them so the strings
	 * will be shared with show_state(), above.
	 */
	if (sysfs_streq(buf, "enabled\n") || sysfs_streq(buf, "1"))
		enabled = true;
	else if (sysfs_streq(buf, "disabled\n") || sysfs_streq(buf, "0"))
		enabled = false;
	else {
		dev_err(dev, "Configuring invalid mode\n");
		return count;
	}

	mutex_lock(&data->lock);
	if (enabled != data->enabled) {
		if (enabled)
			ret = regulator_bulk_enable(data->num_supplies,
						    data->supplies);
		else
			ret = regulator_bulk_disable(data->num_supplies,
						     data->supplies);

		if (ret == 0)
			data->enabled = enabled;
		else
			dev_err(dev, "Failed to configure state: %d\n", ret);
	}
	mutex_unlock(&data->lock);

	return count;
}

static ssize_t reg_force_disable(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct userspace_consumer_data *data = dev_get_drvdata(dev);
	int ret;

	/* Only allow the superuser to do this */
	if (!capable(CAP_SYS_BOOT))
		return -EPERM;

	mutex_lock(&data->lock);
	data->enabled = false;
	ret = regulator_bulk_force_disable(data->num_supplies, data->supplies);
	if (ret) {
		/* Regulators now in an indeterminate state */
		dev_err(dev, "Failed to configure state: %d\n", ret);
	}
	mutex_unlock(&data->lock);

	return count;
}

static DEVICE_ATTR(name, 0444, reg_show_name, NULL);
static DEVICE_ATTR(state, 0644, reg_show_state, reg_set_state);
static DEVICE_ATTR(force_disable, 0200, NULL, reg_force_disable);

static struct device_attribute *attributes[] = {
	&dev_attr_name,
	&dev_attr_state,
	&dev_attr_force_disable,
};

static int regulator_userspace_consumer_probe(struct platform_device *pdev)
{
	struct regulator_userspace_consumer_data *pdata;
	struct userspace_consumer_data *drvdata;
	int ret, i;

	pdata = pdev->dev.platform_data;
	if (!pdata)
		return -EINVAL;

	drvdata = kzalloc(sizeof(struct userspace_consumer_data), GFP_KERNEL);
	if (drvdata == NULL)
		return -ENOMEM;

	drvdata->name = pdata->name;
	drvdata->num_supplies = pdata->num_supplies;
	drvdata->supplies = pdata->supplies;

	mutex_init(&drvdata->lock);

	ret = regulator_bulk_get(&pdev->dev, drvdata->num_supplies,
				 drvdata->supplies);
	if (ret) {
		dev_err(&pdev->dev, "Failed to get supplies: %d\n", ret);
		goto err_alloc_supplies;
	}

	for (i = 0; i < ARRAY_SIZE(attributes); i++) {
		ret = device_create_file(&pdev->dev, attributes[i]);
		if (ret != 0)
			goto err_create_attrs;
	}

	if (pdata->init_on)
		ret = regulator_bulk_enable(drvdata->num_supplies,
					    drvdata->supplies);

	drvdata->enabled = pdata->init_on;

	if (ret) {
		dev_err(&pdev->dev, "Failed to set initial state: %d\n", ret);
		goto err_create_attrs;
	}

	platform_set_drvdata(pdev, drvdata);

	return 0;

err_create_attrs:
	for (i = 0; i < ARRAY_SIZE(attributes); i++)
		device_remove_file(&pdev->dev, attributes[i]);

	regulator_bulk_free(drvdata->num_supplies, drvdata->supplies);

err_alloc_supplies:
	kfree(drvdata);
	return ret;
}

static int regulator_userspace_consumer_remove(struct platform_device *pdev)
{
	struct userspace_consumer_data *data = platform_get_drvdata(pdev);
	int i;

	for (i = 0; i < ARRAY_SIZE(attributes); i++)
		device_remove_file(&pdev->dev, attributes[i]);

	if (data->enabled)
		regulator_bulk_disable(data->num_supplies, data->supplies);

	regulator_bulk_free(data->num_supplies, data->supplies);
	kfree(data);

	return 0;
}

static struct platform_driver regulator_userspace_consumer_driver = {
	.probe		= regulator_userspace_consumer_probe,
	.remove		= regulator_userspace_consumer_remove,
	.driver		= {
		.name		= "reg-us-consumer",
	},
};


static int __init regulator_userspace_consumer_init(void)
{
	return platform_driver_register(&regulator_userspace_consumer_driver);
}
module_init(regulator_userspace_consumer_init);

static void __exit regulator_userspace_consumer_exit(void)
{
	platform_driver_unregister(&regulator_userspace_consumer_driver);
}
module_exit(regulator_userspace_consumer_exit);

MODULE_AUTHOR("Mike Rapoport <mike@compulab.co.il>");
MODULE_DESCRIPTION("Userspace consumer for voltage and current regulators");
MODULE_LICENSE("GPL");
