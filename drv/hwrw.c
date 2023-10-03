/*
 * Copyright © 2010, Nuvoton Technology Corporation. All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 * 
 * Redistributions of source code must retain the above copyright 
 * notice, this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the 
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of Nuvoton Technology Corporation. or the names of 
 * contributors or licensors may be used to endorse or promote products derived 
 * from this software without specific prior written permission.
 * 
 * This software is provided "AS IS," without a warranty of any kind. 
 * ALL EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, 
 * INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED.
 * NUVOTON TECHNOLOGY CORPORATION. ("NUVOTON") AND ITS LICENSORS SHALL NOT BE LIABLE
 * FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING
 * OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.  IN NO EVENT WILL
 * SUN OR ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA,
 * OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR
 * PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF
 * LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE,
 * EVEN IF SUN HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 */

 


// FILE VERSION : 1.0.0


//#define DEBUG 1

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "hwrw.h"

MODULE_AUTHOR("Nuvoton Technology Corporation");
MODULE_DESCRIPTION("HW Read Write (I/O,Memory access) Driver");
MODULE_LICENSE("GPL");

/* ------------------------------------------------------------------------------- */
/*                           Macros                                                */
/* ------------------------------------------------------------------------------- */

#define HWRW_MINOR  112


/* ------------------------------------------------------------------------------- */
/*                           Global variables                                      */
/* ------------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------------- */
/*                           Driver functions                                      */
/* ------------------------------------------------------------------------------- */

int hwrw_ioctl (struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{

    GeneralParam_T *Parms;
    void *mmio;

    Parms = (GeneralParam_T *)arg;


    switch(cmd) {

	case HWRW_IOC_MEM_MAP:
            mmio = ioremap(*(unsigned int *)Parms->p_1, Parms->p_2);        // map virtual to physical
            printk(KERN_INFO "hwReWr: HWRW_IOC_MEM_MAP, mmio = %lx, P1 = %lx, P2 = %lx\n",
		   (unsigned long)mmio,
                   *(unsigned long *)Parms->p_1,   // address
                   Parms->p_2                     // size
                   );
	    *(unsigned long *)(Parms->p_1) = (unsigned long)mmio;
	    break;

	case HWRW_IOC_MEM_UNMAP:
            iounmap((void *)Parms->p_1);        // un map virtual to physical
            printk(KERN_INFO "hwReWr: HWRW_IOC_MEM_UNMAP, P1 = %x, P2 = %lx\n",
                   (unsigned int)Parms->p_1,   // address
                   Parms->p_2                     // size
                   );
	    break;

        case HWRW_IOC_MEM_READ_32:            
            *(unsigned long *)(Parms->p_2) = ioread32((void *)Parms->p_1);
            printk(KERN_INFO "hwReWr: HWRW_IOC_MEM_READ_32 %lx = %x\n",
                   Parms->p_1,                    //Read address
                   *(unsigned int *)(Parms->p_2)  // result
                   );            
            break;
        
        case HWRW_IOC_MEM_WRITE_32:            
            iowrite32(Parms->p_2, (void *)Parms->p_1);
            printk(KERN_INFO "hwReWr: HWRW_IOC_MEM_WRITE_32 %lx = %lx\n",
                   Parms->p_1,                    //Write address
                   Parms->p_2                     // Data
                   );            
            break;

        case HWRW_IOC_MEM_READ_16:
            *(unsigned short *)(Parms->p_2) = ioread16((void *)Parms->p_1);
            printk(KERN_INFO "hwReWr: HWRW_IOC_MEM_READ_16 %lx = %x\n",
                   Parms->p_1,                      //Read address
                   *(unsigned short *)(Parms->p_2)  // result
                   );
            break;
        
        case HWRW_IOC_MEM_WRITE_16:            
            iowrite16((unsigned short)Parms->p_2, (void *)Parms->p_1);
            printk(KERN_INFO "hwReWr: HWRW_IOC_MEM_WRITE_16 %lx = %x\n",
                   Parms->p_1,                    //Write address
                   (unsigned short)Parms->p_2     // Data
                   );
            break;

        case HWRW_IOC_MEM_READ_8:            
            *(unsigned char *)(Parms->p_2) = ioread8((void *)Parms->p_1);
            printk(KERN_INFO "hwReWr: HWRW_IOC_MEM_READ_8 %lx = %x\n",
                   Parms->p_1,                      //Read address
                   *(unsigned char *)(Parms->p_2)  // result
                   );
            break;
        
        case HWRW_IOC_MEM_WRITE_8:            
            iowrite8((unsigned char)Parms->p_2, (void *)Parms->p_1);
            printk(KERN_INFO "hwReWr: HWRW_IOC_MEM_WRITE_8 %lx = %x\n",
                   Parms->p_1,                   //Write address
                   (unsigned char)Parms->p_2     // Data
                   );
            break;

        case HWRW_IOC_IO_READ_32:
            *(unsigned long *)(Parms->p_2) = inl(Parms->p_1);
            printk(KERN_INFO "hwReWr: HWRW_IOC_IO_READ_32 %lx = %x\n",
                   Parms->p_1,                      //Read address
                   *(unsigned int *)(Parms->p_2)    // result
                   );      
            break;

        case HWRW_IOC_IO_WRITE_32:
            outl((unsigned long)Parms->p_2, Parms->p_1);
            printk(KERN_INFO "hwReWr: HWRW_IOC_IO_WRITE_32 %lx = %x\n",
                   Parms->p_1,                    //Write address
                   (unsigned int)Parms->p_2      // Data
                   );     
            break;

        case HWRW_IOC_IO_READ_16:
            *(unsigned short *)(Parms->p_2) = inw(Parms->p_1);
            printk(KERN_INFO "hwReWr: HWRW_IOC_IO_READ_16 %lx = %x\n",
                   Parms->p_1,                      //Read address
                   *(unsigned short *)(Parms->p_2)  // result
                   );      
            break;

        case HWRW_IOC_IO_WRITE_16:
            outw((unsigned short)Parms->p_2, Parms->p_1);
            printk(KERN_INFO "hwReWr: HWRW_IOC_IO_WRITE_16 %lx = %x\n",
                   Parms->p_1,                    //Write address
                   (unsigned short)Parms->p_2     // Data
                   );     
            break;

        case HWRW_IOC_IO_READ_8:
            *(unsigned char *)(Parms->p_2) = inb(Parms->p_1);
            printk(KERN_INFO "hwReWr: HWRW_IOC_IO_READ_8 %lx = %x\n",
                   Parms->p_1,                      //Read address
                   *(unsigned char *)(Parms->p_2)  // result
                   );      
            break;

        case HWRW_IOC_IO_WRITE_8:
            outb((unsigned char)Parms->p_2, Parms->p_1);
            printk(KERN_INFO "hwReWr: HWRW_IOC_IO_WRITE_8 %lx = %x\n",
                   Parms->p_1,                   //Write address
                   (unsigned char)Parms->p_2     // Data
                   );     
            break;

    }

    return 0;
}

const struct file_operations hwrw_fops = {
    .owner  = THIS_MODULE,
    .ioctl   = hwrw_ioctl,
};

static struct miscdevice hwrw_dev = {
    HWRW_MINOR,
    "hwrw",
    &hwrw_fops
};

int __init hwrw_init(void)
{
    int err = 0;
    
    printk(KERN_INFO "hwReWr: init\n");

    err = misc_register(&hwrw_dev);
    if (err) {
        goto fail;
    }
    printk(KERN_INFO "hwReWr: init Done\n");

    return 0;
   
fail:    
    return err;
  
}

void __exit hwrw_exit(void)
{
    printk(KERN_INFO "hwReWr: exit\n");
    misc_deregister( &hwrw_dev );
}

module_init(hwrw_init);
module_exit(hwrw_exit);
