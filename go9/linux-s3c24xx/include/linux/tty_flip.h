#ifndef _LINUX_TTY_FLIP_H
#define _LINUX_TTY_FLIP_H

#ifdef INCLUDE_INLINE_FUNCS
#define _INLINE_ extern
#else
#define _INLINE_ static __inline__
#endif

_INLINE_ void tty_insert_flip_char(struct tty_struct *tty,
				   unsigned char ch, char flag)
{
	static unsigned int req_info = 1;

	if (tty->flip.count < TTY_FLIPBUF_SIZE) {
		tty->flip.count++;
		*tty->flip.flag_buf_ptr++ = flag;
		*tty->flip.char_buf_ptr++ = ch;
		req_info = 1;
	}
	else 
	{
		if (req_info) {
			printk(KERN_ERR "TTY FLIP buffer overflow for: %s\n", tty->name);
			req_info = 0;
		}
	}

}

_INLINE_ void tty_schedule_flip(struct tty_struct *tty)
{
	schedule_delayed_work(&tty->flip.work, 1);
}

#undef _INLINE_


#endif /* _LINUX_TTY_FLIP_H */






