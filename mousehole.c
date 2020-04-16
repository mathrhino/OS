#include <linux/syscalls.h> 
#include <linux/module.h> 
#include <linux/proc_fs.h>  
#include <linux/seq_file.h> 
#include <linux/uaccess.h> 
#include <linux/kallsyms.h> 
#include <linux/init.h> 
#include <linux/kernel.h> 
#include <asm/unistd.h> 
#include <linux/cred.h>
#include<linux/sched.h>
#include<linux/string.h>
#include<linux/errno.h>
#include<linux/moduleparam.h> 
#include<linux/proc_fs.h> 
#include<asm/uaccess.h> 



MODULE_LICENSE("GPL");

char filepath[128] = { 0x0, } ;
int uid_num =-1;
int count =0;
void ** sctable ;
int proc_uid_num = -1 ;


asmlinkage int (*orig_sys_open)(const char __user * filename, int flags, umode_t mode) ;
//asmlinkage long (*orig_sys_kill)(pid_t pid, int sig);

asmlinkage int mousehole_sys_open(const char __user * filename, int flags, umode_t mode)
{
	char fname[256] ;
	int curr_id = current->cred->uid.val;

	printk("working_sys_open");
	copy_from_user(fname, filename, 256) ;
	printk("curr_id %d",curr_id);
	printk("uid_num %d", uid_num);
	if(filepath[0]!=0x0){
		if(strstr(fname,filepath)!=NULL&&uid_num == curr_id){
			
			printk("Error");
			return -1;
		}
	}
	return orig_sys_open(filename, flags, mode) ;
}
/*
asmlinkage long mousehole_sys_kill(pid_t pid, int sig){
	uid_t proc_id;
	struct task_struct *t;

	for_each_process(t){
		proc_id = t->real_cred->uid.val;
		
		if(proc_uid_num==proc_id&&pid==t->pid){
			//printk("Error");
			return -1;
		}
	}
	return orig_sys_kill(pid, sig);
}
*/

static
int mousehole_proc_open(struct inode *inode, struct file *file) {
	return 0 ;
}

static
int mousehole_proc_release(struct inode *inode, struct file *file) {
	return 0 ;
}

static
ssize_t mousehole_proc_read(struct file *file, char __user *ubuf, size_t size, loff_t *offset)
{
	char buf[256] ;
	ssize_t toread ;

	sprintf(buf, "%s:%d\n", filepath, count) ;

	toread = strlen(buf) >= *offset + size ? size : strlen(buf) - *offset ;

	if (copy_to_user(ubuf, buf + *offset, toread))
		return -EFAULT ;

	*offset = *offset + toread ;

	return toread ;
}

static
ssize_t mousehole_proc_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offset)
{
	int button;
	char buf[128] ;
	printk("working");	
	if (*offset != 0 || size > 128){
		return -EFAULT ;
	}
	if (copy_from_user(buf, ubuf, size)){
		return -EFAULT ;
	}
	if(buf[0]=='1'){
		sscanf(buf,"%d %d %s", &button, &uid_num, filepath);
		printk("working on sscanf");
		count=0;
		*offset=strlen(buf);
		return *offset;
	}	

	else if(buf[0]=='2'){
		sscanf(buf,"%d %d",&button,  &proc_uid_num) ;
	
	count = 0 ;
	*offset = strlen(buf) ;
	}
	printk("working_done_write");
	return *offset ;
}

static const struct file_operations mousehole_fops = {
	.owner = 	THIS_MODULE,
	.open = 	mousehole_proc_open,
	.read = 	mousehole_proc_read,
	.write = 	mousehole_proc_write,
	.llseek = 	seq_lseek,
	.release = 	mousehole_proc_release,
} ;

static
int __init mousehole_init(void) {
	unsigned int level ;
	pte_t * pte ;

	proc_create("mousehole", S_IRUGO | S_IWUGO, NULL, &mousehole_fops) ;

	sctable = (void *) kallsyms_lookup_name("sys_call_table") ;

	orig_sys_open = sctable[__NR_open] ;
//	orig_sys_kill = sctable[__NR_kill];
	pte = lookup_address((unsigned long) sctable, &level) ;
	if (pte->pte &~ _PAGE_RW)
		pte->pte |= _PAGE_RW ;

	sctable[__NR_open] = mousehole_sys_open;
//	sctable[__NR_kill] = mousehole_sys_kill;
	printk("working init");
	return 0;
}

static
void __exit mousehole_exit(void) {
	unsigned int level ;
	pte_t * pte ;
	remove_proc_entry("mousehole", NULL) ;

	sctable[__NR_open] = orig_sys_open;
	//sctable[__NR_kill] = orig_sys_kill;
	pte = lookup_address((unsigned long) sctable, &level) ;
	pte->pte = pte->pte &~ _PAGE_RW ;
	printk("Finish");
}

module_init(mousehole_init);
module_exit(mousehole_exit);
