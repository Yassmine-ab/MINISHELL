#include "ft_printf.h"

int	putchar_count(char c, int *count)
{
	*count += write(1, &c, 1);
	return (*count);
}

int	putstr_count(char *s, int *count)
{
	if (!s)
		s = "(null)";
	while (*s)
		putchar_count(*s++, count);
	return (*count);
}

int	putnbr_count(int n, int *count)
{
	long	nb;

	nb = n;
	if (nb < 0)
	{
		putchar_count('-', count);
		nb = -nb;
	}
	if (nb / 10)
		putnbr_count(nb / 10, count);
	putchar_count(nb % 10 + '0', count);
	return (*count);
}

int	putunbr_count(unsigned int n, int *count)
{
	if (n / 10)
		putunbr_count(n / 10, count);
	putchar_count(n % 10 + '0', count);
	return (*count);
}

int	puthex_count(unsigned long n, int *count, bool uppercase)
{
	const char	*base = "0123456789abcdef";
	const char	*base_upper = "0123456789ABCDEF";

	if (n / 16)
		puthex_count(n / 16, count, uppercase);
	if (uppercase == true)
		putchar_count(base_upper[n % 16], count);
	else
		putchar_count(base[n % 16], count);
	return (*count);
}
