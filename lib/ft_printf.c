#include "ft_printf.h"

static void	handle_ptr(void *ptr, int *count)
{
	if (!ptr)
		putstr_count(NULL_PTR, count);
	else
	{
		putstr_count("0x", count);
		puthex_count((unsigned long)ptr, count, false);
	}
}

static void	format_dispatcher(const char *str, va_list ap, int *count)
{
	if (*str == 'c')
		putchar_count((char)va_arg(ap, int), count);
	else if (*str == 's')
		putstr_count(va_arg(ap, char *), count);
	else if (*str == 'p')
		handle_ptr(va_arg(ap, void *), count);
	else if (*str == 'd' || *str == 'i')
		putnbr_count(va_arg(ap, int), count);
	else if (*str == 'u')
		putunbr_count(va_arg(ap, unsigned int), count);
	else if (*str == 'x')
		puthex_count((unsigned long)va_arg(ap, unsigned int), count, false);
	else if (*str == 'X')
		puthex_count((unsigned long)va_arg(ap, unsigned int), count, true);
	else if (*str == '%')
		putchar_count('%', count);
}

int	ft_printf(const char *str, ...)
{
	va_list	ap;
	int		count;

	if (!str)
		return (0);
	va_start(ap, str);
	count = 0;
	while (*str)
	{
		if (*str == '%')
		{
			if (str++)
				format_dispatcher(str, ap, &count);
		}
		else
			putchar_count(*str, &count);
		str++;
	}
	va_end(ap);
	return (count);
}
