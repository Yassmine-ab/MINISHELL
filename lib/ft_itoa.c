#include "libft.h"

static void	fill_tab(char *str, long n, size_t *n_len)
{
	if (n / 10)
		fill_tab(str, n / 10, n_len);
	str[(*n_len)++] = n % 10 + '0';
}

char	*ft_itoa(int n)
{
	if (n == 0)
		return (ft_strdup("0"));
	size_t (n_len) = 0;
	long (nb) = n;
	if (nb < 0)
		nb = -nb;
	while (nb)
	{
		nb /= 10;
		n_len++;
	}
	n_len += (n < 0);
	char *(str) = malloc(sizeof(char) * (n_len + 1));
	if (!str)
		return (NULL);
	nb = n;
	size_t (i) = 0;
	if (n < 0)
	{
		str[0] = '-';
		nb = -nb;
		i = 1;
	}
	str[n_len] = '\0';
	return (fill_tab(str, nb, &i), str);
}
