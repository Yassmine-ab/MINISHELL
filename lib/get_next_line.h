#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

/* ================================ INCLUDES ================================ */

# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>

/* ================================ DEFINES ================================= */

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1000
# endif

/* ========================== FUNCTION PROTOTYPES =========================== */

/* --- Main function --- */

char	*get_next_line(int fd);

/* --- Helper Functions --- */

char	*ft_strchr_gnl(const char *string, int searchedChar);
char	*ft_strdup_gnl(const char *s1);
char	*ft_strjoin_with_len(const char *s1, const char *s2,
			size_t s1_len, size_t s2_len);
size_t	ft_strlen_gnl(const char *s);
void	*ft_memcpy_gnl(void *dest, const void *src, size_t n);

#endif
