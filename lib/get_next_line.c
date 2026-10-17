#include "get_next_line.h"

static bool	fill_remainder(int fd, char **rem, char *buffer, size_t *rem_len)
{
	char	*tmp;
	int		bytes_read;

	bytes_read = 1;
	buffer[0] = '\0';
	while (bytes_read && !ft_strchr_gnl(buffer, '\n'))
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read == -1)
			return (false);
		buffer[bytes_read] = '\0';
		tmp = ft_strjoin_with_len(*rem, buffer, *rem_len, bytes_read);
		free(*rem);
		*rem = tmp;
		if (!*rem)
			return (false);
		*rem_len += bytes_read;
	}
	return (true);
}

static char	*extract_line(char **rem)
{
	char	*line;
	char	*tmp;
	size_t	len;

	len = 0;
	while ((*rem)[len] && (*rem)[len] != '\n')
		len++;
	if ((*rem)[len] == '\n')
		len++;
	line = malloc(sizeof(char) * (len + 1));
	if (!line)
		return (NULL);
	ft_memcpy_gnl(line, *rem, len);
	line[len] = '\0';
	tmp = ft_strdup_gnl((*rem) + len);
	free(*rem);
	*rem = tmp;
	if (!*rem || !**rem)
		(free(*rem), *rem = NULL);
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*rem;
	char		*buffer;
	size_t		rem_len;

	if (fd < 0 || BUFFER_SIZE < 1)
		return (NULL);
	if (!rem)
		rem = ft_strdup_gnl("");
	buffer = malloc(BUFFER_SIZE + 1);
	if (!rem || !buffer)
		return (free(buffer), free(rem), rem = NULL, NULL);
	rem_len = ft_strlen_gnl(rem);
	if (fill_remainder(fd, &rem, buffer, &rem_len) == false)
		return (free(buffer), free(rem), rem = NULL, NULL);
	free(buffer);
	if (!rem || !*rem)
		return (free(rem), rem = NULL, NULL);
	return (extract_line(&rem));
}
