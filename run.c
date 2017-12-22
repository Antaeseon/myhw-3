#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "run.h"
#include "util.h"
#define align4(x) (((((x)-1)>>2)<<2)+4)

void *base = 0;
// so hard
p_meta find_meta(p_meta *last, size_t size) {
  p_meta index = base;
  p_meta result = base;

  switch(fit_flag){
    case FIRST_FIT:
    {
      //FIRST FIT CODE
	p_meta p = base;
	while(p&& !(p->free && p->size >=size))
	{
		*last=p;
		p= p->next;
	}
	return p;
    }
    break;

    case BEST_FIT:
    {
      //BEST_FIT CODE
    }
    break;

    case WORST_FIT:
    {
      //WORST_FIT CODE
    }
    break;

  }
  return result;
}

void *m_malloc(size_t size) {
	p_meta p,last;
	size_t s;
	s=align4(size);
	if(base)
	{
		last =base;
		p = find_meta(&last,p);
		if(p)
		{
			if((p->size-s)>=(META_SIZE +4))
				split_block(p,s);
			p->free=0;
		}
		else
		{
			p=extend_heap(last,s);
			if(!p)
				return NULL;
		}
	}
	else
	{
		p = extend_heap(NULL,s);
		if(!p)
			return NULL;
		base = p;
	}
	return p->data;
}

void m_free(void *ptr) {
	p_meta p;
	if(valid_addr(p))
	{
		p=get_block(p);
		p->free=1;
		if(p->prev && p->prev->free)
			p=fusion(p->prev);
		if(p->next)
			fusion(p);
		else
		{		
			if(p->prev)
				p->prev->next=NULL;
			else
				base=NULL;
			brk(p);
		}
	}	
}

void* m_realloc(void* ptr, size_t size)
{
	size_t s;
	p_meta p,new;
	void *newp;
	if(!p)
		return (malloc(size));
	if(valid_addr(p))
	{
		s=align4(size);
		p=get_block(p);
		if(p->size>=s)
		{
			if(p->size -s>=(META_SIZE +4))
				split_block(p,s);

		}
		else
		{
			if(p->next && p->next->free&&(p->size+META_SIZE + p->next
			->size)>=s)
			{
				fusion(p);
				if(p->size-s<=(META_SIZE + 4))
					split_block(p,s);
			}
			else
			{
				newp = malloc(s);
				if(!newp)
					return NULL;
				new = get_block(newp);
				copy_block(p,new);
				free(p);
				return newp;
			}
		}
	return p;
	}
}




void split_block(p_meta p,size_t s)
{
	p_meta new;
	new = p->data+s;
	new -> size = p-> size -s - META_SIZE;
	new ->next = p->next;
	new -> free = 1;
	p-> size = s;
	p->next = new;
}

p_meta extend_heap(p_meta last,size_t s)
{
	p_meta p;
	p=sbrk(0);
	if(sbrk(META_SIZE +s)==(void*)-1)
		return NULL;
	p->size =s;
	p->next =NULL;
	if(last)
		last->next =p;
	p->free =0;
	return p;
}


p_meta get_block(void *p)
{
	char *tmp;
	tmp=p;
	return p=tmp-=META_SIZE;
}

void copy_block(p_meta src,p_meta dst)
{
	int *sdata,*ddata;
	size_t i;
	sdata= src->ptr;
	ddata = dst->ptr;
	for(i=0;i*4<src->size && i*4<dst->size; i++)
		ddata[i]=sdata[i];
}

p_meta fusion(p_meta p)
{
	if(p->next && p->next->free)
	{
		p->size += META_SIZE + p->next->size;
		p->next = p->next->next;
		if(p->next)
			p->next->prev=p;
	}
	return p;
}
	
int valid_addr(void *p)
{
	if(base)
	{
		if(p>base&&p<sbrk(0))
		{
			return (p==(get_block(p))->ptr);
		}
	}
	return 0;
}


