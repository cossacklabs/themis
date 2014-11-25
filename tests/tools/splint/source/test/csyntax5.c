typedef int fd_mask;

typedef	struct fd_set {
  fd_mask fds_bits[sizeof(fd_mask)];
} fd_set;


