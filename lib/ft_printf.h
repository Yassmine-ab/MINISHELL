#ifndef FT_PRINTF_H
# define FT_PRINTF_H

/* ================================ INCLUDES ================================ */

# include <unistd.h>
# include <stdarg.h>
# include <stdbool.h>

/* ================================ DEFINES ================================= */

# define NULL_PTR "(nil)"

/* =========================== FUNCTION PROTOTYPES ========================== */

/* --- Main function --- */

int	ft_printf(const char *str, ...);

/* --- Helper Functions --- */

int	putchar_count(char c, int *count);
int	putstr_count(char *s, int *count);
int	putnbr_count(int n, int *count);
int	putunbr_count(unsigned int n, int *count);
int	puthex_count(unsigned long n, int *count, bool uppercase);

#endif
