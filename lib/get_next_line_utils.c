#include "get_next_line.h"

char	*ft_strchr(const char *s, int c)
{
	if (!s)
		return (NULL);
	while (*s != (char)c)
	{
		if (*s == '\0')
			return (NULL);
		s++;
	}
	return ((char *)s);
}

size_t	ft_strlen(const char *s)
{
	size_t	len;

	len = 0;
	while (s[len])
		len++;
	return (len);
}

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned long		*dest_word;
	const unsigned long	*src_word;
	unsigned char		*dest_byte;
	const unsigned char	*src_byte;

	dest_word = (unsigned long *)dest;
	src_word = (const unsigned long *)src;
	while (n >= sizeof(unsigned long))
	{
		*dest_word++ = *src_word++;
		n -= sizeof(unsigned long);
	}
	dest_byte = (unsigned char *)dest_word;
	src_byte = (const unsigned char *)src_word;
	while (n--)
		*dest_byte++ = *src_byte++;
	return (dest);
}

char	*ft_strdup(const char *s)
{
	char	*str;
	size_t	len;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	str = malloc(sizeof(char) * (len + 1));
	if (!str)
		return (NULL);
	ft_memcpy(str, s, len);
	str[len] = '\0';
	return (str);
}

char	*ft_strjoin_with_len(const char *s1, const char *s2,
	size_t s1_len, size_t s2_len)
{
	char	*str;

	if (!s1 || !s2)
		return (NULL);
	str = malloc(sizeof(char) * (s1_len + s2_len + 1));
	if (!str)
		return (NULL);
	ft_memcpy(str, s1, s1_len);
	ft_memcpy(str + s1_len, s2, s2_len);
	str[s1_len + s2_len] = '\0';
	return (str);
}
