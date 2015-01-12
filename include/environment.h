/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

/**************************************************************************
 *
 * The "environment" is stored as a list of '\0' terminated
 * "name=value" strings. The end of the list is marked by a double
 * '\0'. New entries are always added at the end. Deleting an entry
 * shifts the remaining entries to the front. Replacing an entry is a
 * combination of deleting the old value and adding the new one.
 *
 * The environment is preceeded by a 32 bit CRC over the data part.
 *
 **************************************************************************
 */

#if defined(CFG_ENV_IS_IN_FLASH)
# ifndef	CFG_ENV_ADDR
#  define	CFG_ENV_ADDR	(CFG_SYS_FLASH_BASE + CFG_ENV_OFFSET)
# endif
# ifndef	CFG_ENV_OFFSET
#  define	CFG_ENV_OFFSET (CFG_ENV_ADDR - CFG_SYS_FLASH_BASE)
# endif
# if !defined(CFG_ENV_ADDR_REDUND) && defined(CFG_ENV_OFFSET_REDUND)
#  define	CFG_ENV_ADDR_REDUND	\
		(CFG_SYS_FLASH_BASE + CFG_ENV_OFFSET_REDUND)
# endif
# if defined(CFG_ENV_SECT_SIZE) || defined(CFG_ENV_SIZE)
#  ifndef	CFG_ENV_SECT_SIZE
#   define	CFG_ENV_SECT_SIZE	CFG_ENV_SIZE
#  endif
#  ifndef	CFG_ENV_SIZE
#   define	CFG_ENV_SIZE	CFG_ENV_SECT_SIZE
#  endif
# else
#  error "Both CFG_ENV_SECT_SIZE and CFG_ENV_SIZE undefined"
# endif
# if defined(CFG_ENV_ADDR_REDUND) && !defined(CFG_ENV_SIZE_REDUND)
#  define CFG_ENV_SIZE_REDUND	CFG_ENV_SIZE
# endif
# if	(CFG_ENV_ADDR >= CFG_SYS_MONITOR_BASE) &&		\
	(CFG_ENV_ADDR + CFG_ENV_SIZE) <=			\
	(CFG_SYS_MONITOR_BASE + CFG_SYS_MONITOR_LEN)
#  define ENV_IS_EMBEDDED
# endif
# if defined(CFG_ENV_ADDR_REDUND) || defined(CFG_ENV_OFFSET_REDUND)
#  define CFG_SYS_REDUNDAND_ENVIRONMENT
# endif
# ifdef CFG_ENV_IS_EMBEDDED
#  error "do not define CFG_ENV_IS_EMBEDDED in your board config"
#  error "it is calculated automatically for you"
# endif
#endif	/* CFG_ENV_IS_IN_FLASH */

#if defined(CFG_ENV_IS_IN_MMC)
# ifdef CFG_ENV_OFFSET_REDUND
#  define CFG_SYS_REDUNDAND_ENVIRONMENT
# endif
#endif

#if defined(CFG_ENV_IS_IN_NAND)
# if defined(CFG_ENV_OFFSET_OOB)
#  ifdef CFG_ENV_OFFSET_REDUND
#   error "CFG_ENV_OFFSET_REDUND is not supported when CFG_ENV_OFFSET_OOB"
#   error "is set"
#  endif
extern unsigned long nand_env_oob_offset;
#  define CFG_ENV_OFFSET nand_env_oob_offset
# else
#  ifndef CFG_ENV_OFFSET
#   error "Need to define CFG_ENV_OFFSET when using CFG_ENV_IS_IN_NAND"
#  endif
#  ifdef CFG_ENV_OFFSET_REDUND
#   define CFG_SYS_REDUNDAND_ENVIRONMENT
#  endif
# endif /* CFG_ENV_OFFSET_OOB */
# ifndef CFG_ENV_SIZE
#  error "Need to define CFG_ENV_SIZE when using CFG_ENV_IS_IN_NAND"
# endif
#endif /* CFG_ENV_IS_IN_NAND */

#if defined(CFG_ENV_IS_IN_UBI)
# ifndef CFG_ENV_UBI_PART
#  error "Need to define CFG_ENV_UBI_PART when using CFG_ENV_IS_IN_UBI"
# endif
# ifndef CFG_ENV_UBI_VOLUME
#  error "Need to define CFG_ENV_UBI_VOLUME when using CFG_ENV_IS_IN_UBI"
# endif
# if defined(CFG_ENV_UBI_VOLUME_REDUND)
#  define CFG_SYS_REDUNDAND_ENVIRONMENT
# endif
# ifndef CFG_ENV_SIZE
#  error "Need to define CFG_ENV_SIZE when using CFG_ENV_IS_IN_UBI"
# endif
# ifndef CFG_CMD_UBI
#  error "Need to define CFG_CMD_UBI when using CFG_ENV_IS_IN_UBI"
# endif
#endif /* CFG_ENV_IS_IN_UBI */

/* Embedded env is only supported for some flash types */
#ifdef CFG_ENV_IS_EMBEDDED
# if	!defined(CFG_ENV_IS_IN_FLASH)	&& \
	!defined(CFG_ENV_IS_IN_NAND)		&& \
	!defined(CFG_ENV_IS_IN_ONENAND)	&& \
	!defined(CFG_ENV_IS_IN_SPI_FLASH)
#  error "CFG_ENV_IS_EMBEDDED not supported for your flash type"
# endif
#endif

/*
 * For the flash types where embedded env is supported, but it cannot be
 * calculated automatically (i.e. NAND), take the board opt-in.
 */
#if defined(CFG_ENV_IS_EMBEDDED) && !defined(ENV_IS_EMBEDDED)
# define ENV_IS_EMBEDDED
#endif

/* The build system likes to know if the env is embedded */
#ifdef DO_DEPS_ONLY
# ifdef ENV_IS_EMBEDDED
#  ifndef CFG_ENV_IS_EMBEDDED
#   define CFG_ENV_IS_EMBEDDED
#  endif
# endif
#endif

#include "compiler.h"

#ifdef CFG_SYS_REDUNDAND_ENVIRONMENT
# define ENV_HEADER_SIZE	(sizeof(uint32_t) + 1)

# define ACTIVE_FLAG   1
# define OBSOLETE_FLAG 0
#else
# define ENV_HEADER_SIZE	(sizeof(uint32_t))
#endif

#if defined(CFG_CMD_SAVEENV) && !defined(CFG_ENV_IS_NOWHERE)
extern char *env_name_spec;
#endif

#define ENV_SIZE (CFG_ENV_SIZE - ENV_HEADER_SIZE)

typedef struct environment_s {
	uint32_t	crc;		/* CRC32 over data bytes	*/
#ifdef CFG_SYS_REDUNDAND_ENVIRONMENT
	unsigned char	flags;		/* active/obsolete flags	*/
#endif
	unsigned char	data[ENV_SIZE]; /* Environment data		*/
} env_t;

#ifdef ENV_IS_EMBEDDED
extern env_t environment;
#endif /* ENV_IS_EMBEDDED */

extern const unsigned char default_environment[];
extern env_t *env_ptr;

extern void env_relocate_spec(void);
extern unsigned char env_get_char_spec(int);

#if defined(CFG_NEEDS_MANUAL_RELOC)
extern void env_reloc(void);
#endif

#ifndef DO_DEPS_ONLY

#include <env_attr.h>
#include <env_callback.h>
#include <env_flags.h>
#include <search.h>

extern struct hsearch_data env_htab;

/* Function that returns a character from the environment */
unsigned char env_get_char(int);

/* Function that returns a pointer to a value from the environment */
const unsigned char *env_get_addr(int);
unsigned char env_get_char_memory(int index);

/* Function that updates CRC of the enironment */
void env_crc_update(void);

/* Look up the variable from the default environment */
char *getenv_default(const char *name);

/* [re]set to the default environment */
void set_default_env(const char *s);

/* [re]set individual variables to their value in the default environment */
int set_default_vars(int nvars, char * const vars[]);

/* Import from binary representation into hash table */
int env_import(const char *buf, int check);

#endif /* DO_DEPS_ONLY */

#endif /* _ENVIRONMENT_H_ */
